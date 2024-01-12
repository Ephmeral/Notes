## 1 Database Workloads

数据库的工作负载需要考虑两个方面：读写分布和操作的复杂性

![](pics/Pasted%20image%2020230830122305.png)

OLTP(Online Transaction Processing)：OLTP 工作负载的特点是快速、运行时间短的操作、一次对单个实体进行操作的简单查询以及重复操作。 OLTP 工作负载通常处理的写入多于读取。

```sql
SELECT P.*, R.*
  FROM pages AS P
 INNER JOIN revisions AS R
    ON P.latest = R.revID
 WHERE P.pageID = ?;
 
UPDATE useracct
   SET lastLogin = NOW(),
       hostname = ?
 WHERE userID = ?
 
 INSERT INTO revisions
 VALUES (?,?...,?)
```

OLAP(Online Analytical Processing) ：OLAP 工作负载的特点是长时间运行、复杂的查询、读取数据库中大部分内容。 在 OLAP 工作负载中，数据库系统通过对 OLTP 端收集到的数据进行分析并得出新的数据。

```sql
SELECT COUNT(U.lastLogin)
       EXTRACT (month FROM U.lastLogin) AS month
  FROM useracct AS U
 WHERE U.hostname LIKE '%.gov'
 GROUP BY EXTRACT(month FROM U.lastLogin);
```

HTAP(Hybrid Transaction + Analytical Processing)：最近流行的一种新型工作负载是 HTAP，它尝试在同一个数据库上同时执行 OLTP 和 OLAP。

## 2 Storage Models

关系模型并没有明确指出 DBMS 必须存储一个 tuple 的所有信息在同一个页面中。实际的存储方式可以根据实际场景优化，如 OLTP 和 OLAP。

常见的两种数据存储模型 (Data Storage Model)：
- N-Ary Storage Model (NSM) 行存储
- Decomposition Storage Model (DSM) 列存储

### N-Ary Storage Model (NSM)

在 n-ary 存储模型中，DBMS 将单个元组的所有属性连续存储在单个页面中。 此方法非常适合 OLTP 工作负载，其中请求插入较多且事务往往仅操作单个实体。 它是理想的，因为只需要一次获取就能够获取单个元组的所有属性。

![](pics/Pasted%20image%2020230830124051.png)

DBMS 针对一些常用 attributes 建立 Index，如例子中的 userID，一个查询语句通过 Index 找到相应的 tuples，返回查询结果，流程如下：

![](pics/Pasted%20image%2020230830124153.png)

但是对于一个 OLAP 查询，尽管只涉及到 hostname 和 lastLogin 这两个属性，但是也需要读取 tuple 中所有的属性，就产生了很多冗余数据：

![](pics/Pasted%20image%2020230830124248.png)

NSM 优点：  
• 快速插入、更新和删除。  
• 适合需要整个元组的查询。  

缺点：  
• 不适合扫描表的大部分和/或属性的子集。

### Decomposition Storage Model (DSM)

在分解存储模型中，DBMS 将所有元组的单个属性（列）连续存储在数据块中。 因此，它也被称为**列存储**。 该模型非常适合大量只读查询的 OLAP 工作负载，这些查询对表的属性子集进行扫描。

![](pics/Pasted%20image%2020230830124822.png)

这个时候对于 OLAP 查询来说，就不需要读取不相关的属性了，减少内存/IO 的开销：

![](pics/Pasted%20image%2020230830124907.png)

因为每个 tuple 的各个属性是存放的不同页面上的，所以需要将这些属性重新组合成一个 tuple，有两种解决方法：
- 固定长度偏移量 (Fixed-length Offsets)：列中每个值长度都是相同的，给定列的值会和另外一列有相同偏移量的值进行匹配，组合成一条 tuple。（常用）
- 嵌入元祖 ID(Embedded Tuple Ids)：在列中的每一个值都存储 tuple id，另外系统还将存储一个映射，告诉它如何跳转到具有该 id 的每个属性。 但是，这种方法会导致较大的存储开销。

![](pics/Pasted%20image%2020230830125809.png)

列存储的优缺点：
- 优点：
	- 能够减少 IO 使用量，因为 DBMS 只读取它需要的数据。
	- 能够更好的查询处理和数据压缩（稍后会详细介绍）
- 缺点：
	- 因为 tuple 的每个属性是分割的，单个查询、插入、更新和删除速度很慢。

## 3 Database Compression

DBMS 在执行查询过程的时候，从磁盘中获取数据，往往 IO 是主要的瓶颈。DBMS 可以压缩数据，这样每次 IO 操作就可以读取更多的数据。这样的代价是：压缩和解压缩需要更大的计算开销。

内存 DBMS 更加复杂，因为查询的时候不必从磁盘获取数据。内存比磁盘快得多，但压缩数据库会减少 DRAM 需求和 CPU 处理开销。 必须在速度与压缩比之间取得平衡：
- 压缩数据库可以减少 DRAM 需求；
- 查询的时候会可能减少 CPU 的开销；

如果数据集完全是随机的，则无法执行压缩。 然而，在现实世界中数据集有一些特性：
- 数据集的属性值往往具有高度倾斜的分布（例如，Brown 语料库的 Zipfian 分布）；
- 数据集往往在同一元组的属性之间具有高度相关性（例如，邮政编码到城市、订单日期到发货日期）；

（这里可以或许可以类比考虑 Huffman 编码？就是根据单词或字母出现的频率进行压缩的）

基于以上的讨论，对数据库进行压缩需要满足以下特点：  
- 必须生成固定长度的值。 唯一的例外是存储在 separate pools 中的 var-length 数据。 这是因为 DBMS 应遵循按字对齐，并能够使用偏移量访问数据；
- 允许 DBMS 在查询执行期间尽可能推迟解压缩（也称之为 late materialization）；
- 必须是**无损**方案，因为人们不喜欢丢失数据。 任何类型的有损压缩都必须在应用程序级别执行；

**压缩粒度 (Compression Granularity)**

在向 DBMS 压缩之前，需要决定要压缩哪种数据。 该决定就确定压缩方案是否可用。 压缩粒度有四个级别：

- 块级 (Block Level)：压缩同一个表的元组块。  
- 元组级别 (Tuple Level)：压缩整个元组的内容（仅限 NSM）。  
- 属性级别 (Attribute Level)：将单个属性值压缩到一个元组内。 可以针对同一元组的多个属性。  
- 列级 (Columnar Level)：压缩一个或多个属性的多个值存放在多个元祖中（仅限 DSM）。 这允许更复杂的压缩方案。

## 4 Naive Compression

数据库管理系统（DBMS）使用通用的压缩算法（例如，gzip、LZO、LZ4、Snappy、Brotli、Oracle OZIP、Zstd）来压缩数据。虽然 DBMS 可以使用多种压缩算法，但工程师通常会选择那些在快速压缩/解压缩方面提供较低压缩比的算法。
  
一个使用朴素压缩（naive compression）的例子就是 MySQL InnoDB。数据库管理系统会压缩磁盘页面，将它们填充到 2 的幂次方 KB 大小，并将它们存储到缓冲池中。然而，每当数据库管理系统尝试读取数据时，缓冲池中的压缩数据都必须进行解压缩处理。

![](pics/Pasted%20image%2020230830134335.png)

由于访问数据需要对已经压缩的数据进行解压缩，这限制了压缩方案的适用范围。如果目标是将整个表压缩成一个巨大的块，使用朴素的压缩方案将是不可能的，因为每次访问都需要对整个表进行压缩/解压缩。因此，对于 MySQL 来说，由于压缩的范围限制，它会将表分成较小的块。

另一个问题是这些朴素的方案也没有考虑到数据的高层含义或语义。该算法既不考虑数据的结构，也不考虑查询计划如何访问数据。因此，这失去了利用延迟实例化的机会，因为数据库管理系统将无法确定何时能够延迟解压缩数据。

## 5 Columnar Compression

理想情况下，我们想让 DBMS 直接在压缩的数据上操作，而不用先解压数据。

![](pics/Pasted%20image%2020230830135203.png)

### Run-Length Encoding (RLE)

RLE 将单个列中相同值的连续出现压缩成三元组：

- 属性的值
- 列段中的起始位置
- 连续出现的元素数量

下面是个示例：

![](pics/Pasted%20image%2020230830135434.png)

DBMS 应该在压缩之前对列进行智能排序，以最大限度地提高压缩机会。这样可以聚集重复的属性，从而增加压缩比率。

上面的例子中会发生 F,M 出现的次序是乱的，如果排序一下可以进一步压缩：

![](pics/Pasted%20image%2020230830135748.png)

### Bit-Packing Encoding

当属性的值始终小于所声明的最大大小时，可以将它们存储为较小的数据类型。

![](pics/Pasted%20image%2020230830135858.png)

### Mostly Encoding

Bit-packing 的变种方法使用特殊标记来指示何时一个值超过了最大大小，然后维护一个查找表来存储这些值。  

![](pics/Pasted%20image%2020230830135937.png)

### Bitmap Encoding

DBMS 为特定属性的每个唯一值存储一个单独的位图，其中向量中的偏移对应一个元组。位图中的第 i 个位置对应于表中的第 i 个元组，用于指示该值是否存在。位图通常被分段成块，以避免分配大块连续内存。

![](pics/Pasted%20image%2020230830140249.png)

这种方法只有在值的基数较低时才实际可行，因为位图的大小与属性值的基数成线性关系。如果值的基数很高，那么位图可能会比原始数据集还要大。下面是个示例：

假设我们有 100 万个元祖，美国有 43,000 个邮政编码。每当应用程序插入一个新的元组，DBMS 都必须扩展 43,000 个不同的位图，这就导致位图数据比原数据还要大：
- 10000000 × 32-bits = 40 MB
- 10000000 × 43000 = 53.75 GB

```sql
CREATE TABLE customer_dim (
	id INT PRIMARY KEY,
	name VARCHAR(32),
	email VARCHAR(64),
	address VARCHAR(64),
	zip_code INT /* 邮政编码 */
);
```

### Delta Encoding

不存储精确的值，而是记录同一列中相邻值之间的差异。基准值可以内联存储或存储在单独的查找表中。我们还可以对存储的增量使用 RLE 来获得更好的压缩比率。

![](pics/Pasted%20image%2020230830140944.png)

### Incremental Encoding

这是一种增量编码的类型，其中记录了常见的前缀或后缀及其长度，以避免重复。这在排序数据中效果最佳。

![](pics/Pasted%20image%2020230830141156.png)

### Dictionary Compression
  
最常见的数据库压缩方案是字典编码。数据库管理系统将值中的频繁模式替换为较小的代码。然后只存储这些代码以及一个将这些代码映射到其原始值的数据结构（即字典）。字典压缩方案需要支持快速的编码/解码，以及范围查询。

![](pics/Pasted%20image%2020230830141409.png)

**Encoding and Decoding:** 字典需要决定如何对数据进行编码（将未压缩的值转换为其压缩形式）/解码（将压缩的值转换回其原始形式）。不可能使用哈希函数来实现这一点。

编码后的值还需要支持与原始值相同的排序顺序。这确保了在压缩数据上运行的压缩查询返回的结果与在原始数据上运行的未压缩查询一致。这种保持顺序的属性允许直接在代码上执行操作。

例如：使用通配符查询名字是 'And%' 的时候，在字典中要能正确的转为查询 10-20 的名字。

![](pics/Pasted%20image%2020230830141649.png)  
![](pics/Pasted%20image%2020230830141702.png)