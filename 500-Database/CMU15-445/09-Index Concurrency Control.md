## 1 Index Concurrency Control

之前一直假设我们讨论的数据结构都是在单线程情况下的。但是 DBMS 会允许多线程安全的访问数据结构，这样可以充分利用多个 CPU 从而隐藏 IO 操作的迟钝。

并发控制协议（concurrency control protocol）：DBMS 用于确保共享对象上的并发操作产生**正确结果**的方法。

协议的正确性包括：
- 逻辑正确性（Logical Correctness）：这意味着线程能够读取它应该读取的值，例如，一个线程应该能够读到之前写入的值。
- 物理正确性（Physical Correctness）：这意味着对象的内部表示是正确的，例如，数据结构中没有指针会导致线程读取无效的内存。

在本课程中，我们只关心强制执行物理正确性。我们将在后续课程中重新讨论逻辑正确性。

## 2 Locks vs. Latches

Locks：锁是一种更高级别的、逻辑上的原语，用于保护数据库中的内容（例如元组、表、数据库）免受其他事务的影响。事务将在其整个持续时间内持有锁。数据库系统可以在运行查询时向用户公开正在持有的锁。锁需要能够回滚更改。

Latches：闩锁是用于保护 DBMS 的内部数据结构（例如数据结构、内存区域）免受其他线程干扰的低级别保护原语。闩锁只在进行操作的持续时间内保持。闩锁不需要具备回滚更改的能力。闩锁有两种模式：

- 读（READ）：允许多个线程同时读取同一个对象。即使另一个线程已经获取了读闩锁，其他线程仍然可以获取读闩锁。
- 写（WRITE）：只允许一个线程访问对象。另外一个线程不能以任何模式获得写闩锁。持有写入闩锁的线程还会阻止其他线程获取读闩锁。

![](pics/Pasted%20image%2020230908140720.png)

## 3 Latch Implementations

实现闩锁的底层原语通常是通过现代 CPU 提供的原子比较与交换（compare-and-swap, CAS）指令来实现的。使用 CAS 指令，线程可以检查内存的内容，以查看它是否具有某个特定值。如果是这个值，那么 CPU 将旧值与新值进行交换。否则，内存的值将保持不变。
  
在数据库管理系统中实现闩锁有几种方法。每种方法在工程复杂性和运行时性能方面都有不同的权衡。这些 test-and-set 步骤是以原子方式执行的（即，在 test-and-set 步骤之间没有其他线程可以更新该值）。

### Blocking OS Mutex

一种可能的闩锁实现是使用操作系统内置的互斥锁（mutex）。Linux 提供了 futex（fast user-space mutex），它由以下两部分组成：(1) 用户空间中的自旋闩锁（spin latch）和 (2) 操作系统级别的互斥锁。
- 如果 DBMS 能够获取用户空间的自旋闩锁，那么闩锁就被设置。尽管 futex 包含两个内部闩锁，但对于 DBMS 来说，它看起来像一个单一的闩锁。
- 如果 DBMS 无法获取用户空间的自旋闩锁，那么它将陷入内核，并尝试获取一个更昂贵的互斥锁。如果 DBMS 无法获取这个第二个互斥锁，那么线程将通知操作系统它被阻塞在互斥锁上，然后被取消调度（descheduled）。

操作系统的互斥锁（OS mutex）通常不是在 DBMS 内部的最佳选择，因为它由操作系统管理，具有较大的开销。

- 例子：`std::mutex`
- 优点：使用简单，DBMS 中不需要引入额外的代码。
- 缺点：由于操作系统调度，成本昂贵且不可扩展（每次 lock/unlock 调用大约 25 ns）。

### Test-and-Set Spin Latch (TAS)

自旋锁可以被 DBMS 控制，相比操作系统的互斥锁更加有效。自旋锁本质上是内存中的一个位置，线程尝试更新它（例如，将一个布尔值设置为 true）。

线程执行 CAS（比较并交换）操作，试图更新内存位置。如果无法获取锁，DBMS 可以控制接下来的操作。它可以选择再次尝试（例如，使用 while 循环）或允许操作系统取消调度线程。因此，这种方法相比操作系统的互斥锁，DBMS 有了更多的控制权，而当无法获得锁的时候再将控制权转移给操作系统。

- 例子：`std::atomic<T>`
- 优点：Latch/unlatch 非常有效，因为只需要单个指令就可以 lock/unlock。
- 缺点：可拓展性差，也不友好于缓存，在多线程环境中，CAS 指令将在不同线程中执行多次。这些浪费的指令会在高竞争环境中积累起来；尽管线程并未执行有用的工作，但它们会让操作系统认为线程很忙碌。这会导致缓存一致性问题，因为线程会轮询其他 CPU 上的缓存行。

### Reader-Writer Latches

Mutexs 和 Spin Latchs 不区分读取和写入操作（即，它们不支持不同的模式）。DBMS 需要一种方式来允许并发的读取，因为如果应用程序有大量读取操作，它将有更好的性能，读操作可以共享资源而不用等待。

读写锁（Reader-Writer Latch）允许锁以读取或写入模式来使用。它跟踪每种模式下有多少线程持有锁以及哪些线程正在等待获取锁。读写锁使用前面两种锁之一作为原语，并通过额外的逻辑来处理读写队列，这些队列用于处理每种模式下锁的请求。不同的数据库管理系统可以有不同的策略来处理这些队列。

- 例子：`std::shared_mutex`
- 优点：运行并发的读；
- 缺点：DBMS 必须管理读/写队列以避免饥饿。由于额外的元数据，存储开销比自旋锁更大。

## 4 Hash Table Latching

在静态哈希表中支持并发访问比较容易，因为线程访问数据结构的方式有限。例如，当从一个槽移动到下一个槽时，所有线程都按相同的方向移动（即从上到下）。线程一次只能访问一个页面或槽。因此，在这种情况下，死锁是不可能的，因为没有两个线程会竞争获取对方持有的锁。当我们需要调整表的大小时，只需对整个表获取全局锁定即可。

在动态哈希方案（例如，可扩展哈希）中进行锁定是一种更复杂的方案，因为有更多的共享状态需要更新，但一般的方法是相同的。

有两种方法在哈希表中上锁，它们在锁的颗粒度上有所不同：  
- 页面锁定（Page Latches）：每个页面都有自己的读写锁，用于保护整个页面内容。线程在访问页面之前会获取读锁或写锁。这降低了并行性，因为潜在地只有一个线程可以同时访问一个页面，但对于单个线程来说，访问页面中的多个槽位将会很快，因为它只需要获取一个锁。
- 槽位锁定（Slot Latches）：每个槽位都有自己的锁。这增加了并行性，因为两个线程可以访问同一页面上的不同槽位。但这会增加访问表的存储和计算开销，因为线程必须为它们访问的每个槽位获取一个锁，每个槽位都必须存储用于锁定的数据。DBMS 可以使用单一模式的锁（例如，自旋锁）来降低元数据和计算开销，以换取一些并行性。

也可以直接使用比较并交换（CAS）指令创建一种不需要锁定的线性探测哈希表。在槽位上插入数据可以通过尝试使用 CAS 将一个特殊的 null 值与要插入的元组进行比较并交换。如果这一步失败，我们可以探测下一个槽位，一直重试，直到成功为止。这种方法可以避免锁定，但需要谨慎处理并发操作以确保数据一致性。

## 5 B+Tree Latching

如果运行多个线程同时读或写 B+ 树，需要防止以下两个问题：
1. 多个线程尝试同时修改节点的内容。
2. 一个线程在遍历树的同时，另一个线程在拆分/合并节点。

下面是 B+ 树多线程的一个例子：

假设线程 T1 要删除 44：

![](pics/Pasted%20image%2020230908145714.png)

删除 44 之后会导致节点 I 为空，需要重新平衡一下，这里是像兄弟节点 H 借一个值：

![](pics/Pasted%20image%2020230908145818.png)

而此时如果有另外一个线程 T2 查找 41 这个值：

![](pics/Pasted%20image%2020230908145903.png)

当遍历到 H 节点的时候，假设 T1 开始向兄弟节点借用一个值，会将 41 移动到节点 I：

![](pics/Pasted%20image%2020230908145927.png)

这个时候线程 T2 访问的数据为空，这样的结果并不正确。

![](pics/Pasted%20image%2020230908150028.png)

### Latch Crabbing/Coupling

Latch crabbing/coupling 是一种协议，允许多个线程同时访问/修改 B+ 树。其基本思想如下：
1. 获取父节点的锁。
2. 获取子节点的锁。
3. 如果子节点被视为“安全”，则释放父节点的锁定。这里的“安全”节点是指在更新时不会发生分裂、合并或重新分配的节点。

节点是否**安全**取决于操作是插入还是删除，例如对于一个满节点来说，删除的时候是安全的，但是插入的时候会导致分裂，不是安全的。

读锁不需要考虑安全的问题，读操作并不会改变树的结构，所以不需要等待或处理类似分裂或合并的情况。

### Basic Latch Crabbing Protocol

基础的螃蟹锁协议（Basic Latch Crabbing Protocol）：
- **Search**：从根节点开始一直往下遍历 B+ 树，不断的对子节点上读锁（R latch）同时释放父节点的锁。
- **Insert/Delete**：从根节点开始向下遍历 B+ 树，根据需要获取写锁（W latch）。一旦子节点被锁定，检查它是否是“安全”的。如果子节点被认为是“安全”的，那么释放所有其祖先节点上的锁。

从正确性的角度来看，释放锁的顺序并不重要。然而，从性能的角度来看，最好释放位于树的高层的锁，因为它们会阻塞对更多叶节点的访问。这可以减少锁定竞争，提高并发性，从而改善性能。

下面是几个基本的例子：

#### 示例 1：Find 38

从根节点 A 开始，获取 A 的读锁：

![](pics/Pasted%20image%2020230908151128.png)

接着往下访问子节点 B，这个时候因为 B 是安全的，所以释放 A 的锁：

![](pics/Pasted%20image%2020230908151201.png)

访问节点 D 的时候，需要上读锁，同样的释放父节点 B 的锁：

![](pics/Pasted%20image%2020230908151321.png)

继续访问 H 节点，上锁，释放 D 的锁，最终找到 38，返回。

![](pics/Pasted%20image%2020230908151350.png)

#### 示例 2：Delete 38

从根节点 A 开始，上写锁：

![](pics/Pasted%20image%2020230908151452.png)

然后往下遍历访问节点 B，也上写锁，这个时候发现 B 并不是安全的，因为 B 只有一个值，可能会发生合并，所以不能释放父节点 A 的锁：

![](pics/Pasted%20image%2020230908151506.png)

继续往下遍历，访问节点 D，上写锁，这个时候发现 D 不会和 C 发生合并操作，对于节点 D 来说是安全的，所以需要释放 D 所有祖先节点的锁。

![](pics/Pasted%20image%2020230908151726.png)

释放完 D 祖先节点的锁之后如下：

![](pics/Pasted%20image%2020230908151957.png)

继续往下访问节点 H，H 也是安全的，所以需要释放 D 的锁，最终将 38 删除：

![](pics/Pasted%20image%2020230908152054.png)

#### 示例 3：Insert 45

假设现在要插入 45 ，先获取根节点 A 的写锁，然后往下遍历获取 B 的写锁，这里发现 B 还有空间，所以 B 是安全的，释放 A 的锁：

![](pics/Pasted%20image%2020230908152252.png)

继续往下遍历，访问 D，上写锁，此时 D 可能溢出，所以不能释放父节点 B 的锁：

![](pics/Pasted%20image%2020230908152413.png)

再往下遍历，访问节点 I，发现节点 I 有空间插入新的元素 45，所以直接释放 I 所有祖先节点的锁，最终 45 插入到 B+ 树中：

![](pics/Pasted%20image%2020230908152455.png)

#### 示例 4：Insert 25

前面遍历部分就不赘述了，和上面示例差不多，现在访问到节点 C，获取 C 的写锁：

![](pics/Pasted%20image%2020230908152653.png)

继续往下遍历，访问节点 F，此时发现如果插入一个元素之后，节点 F 会分裂，所以 F 并不安全，F 的父节点的锁也不需要释放。

![](pics/Pasted%20image%2020230908152738.png)

最后分裂的结果如下：

![](pics/Pasted%20image%2020230908153334.png)

#### Observation

通过观察上面的步骤，能发现每次更新的第一步都是获取 B+ 树根节点的锁：

![](pics/Pasted%20image%2020230908153501.png)

每次都在根节点上获取写锁可能会在高并发情况下成为瓶颈。这是因为多个线程同时尝试获取根节点的写锁时会发生竞争，导致性能下降。

### Improved Latch Crabbing Protocol

前面提到如果每次更新操作都对根节点上写锁的话，会降低并发度。Basic Latch Crabbing Protocol 是一种悲观锁，它会假设每次写操作都可能发生分裂或合并。但是大部分更新操作并不会发生分裂或合并。

所以我们可以乐观的假设更新操作不会发生分裂或合并，这样的话在遍历树的时候通过上读锁，来提高并发度。当发现有节点需要分裂或合并的时候，再采用之前悲观的策略。

增强的螃蟹锁协议（Improved Latch Crabbing Protocol）：
- **Search**：和之前一样。
- **Insert/Delete**：从根节点往下遍历 B+ 树，每次都是上读锁，到叶子节点的时候，给叶子节点上写锁。如果叶子节点需要合并或分裂（即不安全），释放所有的锁，重新采用先前的策略（即每次上写锁）。

下面是两个示例：

还是之前的示例 2：Delete 38

从根节点 A 开始，上读锁：

![](pics/Pasted%20image%2020230908154918.png)

继续往下遍历到 B 节点，上读锁，同时释放父节点 A 的锁：

![](pics/Pasted%20image%2020230908154927.png)

继续遍历到节点 D，上读锁，是否父节点 B 的锁：

![](pics/Pasted%20image%2020230908155033.png)

最后遍历到叶子节点 H，上写锁，发现不会发生合并，这个时候是安全的，是否父节点的锁，并删除 38：

![](pics/Pasted%20image%2020230908155113.png)

对于之前的示例 4：Insert 25

到叶子节点的时候，发生会发生分裂，这个时候需要重新从根节点开始，采用悲观锁的策略重新遍历一次。

![](pics/Pasted%20image%2020230908155214.png)

### Leaf Node Scans

前面螃蟹锁协议中的线程都是以“自顶向下”的方式获取锁。这意味着线程只能从当前节点下方的节点获取锁。如果所需的锁不可用，线程必须等待直到它变为可用。因此，在这种情况下，不会发生死锁。

然而，叶节点扫描（Leaf Node Scans）可能会受到死锁的影响，可能会有线程同时尝试在两个不同的方向上获取排他锁（例如，线程 1 尝试删除，线程 2 进行叶节点扫描）。而 B+ 树的索引并不支持死锁检测或死锁避免。

解决这种方法只能通过编码约定。叶节点兄弟锁获取协议必须支持“无等待”模式。也就是说，B+ 树代码必须处理锁获取失败情况。由于上锁意味着要持有（相对）短暂的时间，如果一个线程尝试获取叶节点上的锁定，但该锁不可用，那么它应该迅速中止其操作（释放它持有的任何锁），然后重新启动操作。

下面是两个简单的示例：

![](pics/Pasted%20image%2020230908161952.png)  
![](pics/Pasted%20image%2020230908162003.png)  
![](pics/Pasted%20image%2020230908162011.png)  
![](pics/Pasted%20image%2020230908162026.png)  
![](pics/Pasted%20image%2020230908162036.png)

多个线程并发的读，可以同时持有读锁，所以并不会发生任何的问题。

下面是一个读操作，一个写操作：

![](pics/Pasted%20image%2020230908162119.png)  
![](pics/Pasted%20image%2020230908162130.png)  
![](pics/Pasted%20image%2020230908162139.png)  
![](pics/Pasted%20image%2020230908162149.png)  
![](pics/Pasted%20image%2020230908162158.png)

T2 线程并不知道 T1 线程正在做什么操作，T2 也无法获取写锁，所以这个时候会将 T1 终止，并重新进行查找。如果有两个线程都在写的话，上面这种情况会出现死锁，因为都在等待获取对方的锁。

