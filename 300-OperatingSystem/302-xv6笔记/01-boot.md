#! https://zhuanlan.zhihu.com/p/641859088
# xv6 内核启动
## Qemu 启动

xv6 使用的是 qemu-system-riscv64 模拟器来模拟一台 64 位 RISC-V 架构的计算机，它包含 CPU 、物理内存以及若干 I/O 外设。

执行 `make qemu` 命令的时候，我们的计算机会使用 riscv 编译、链接工具，先对 xv6 的源代码进行编译和链接，然后执行下面的语句：

```shell
# 来自Makefile文件
qemu-system-riscv64 -machine virt -bios none -kernel kernel/kernel -m 128M -smp 3 -nographic -global virtio-mmio.force-legacy=false -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0
```

这条语句相当于启动一个 Qemu 模拟的计算机，下面是一些命令参数的简单介绍：

- `-machine virt` 表示将模拟的 64 位 RISC-V 计算机设置为名为 `virt` 的虚拟计算机；
-  `-bios none` 表示没有初始化引导加载程序（bootloader）；
- `-kernel kernel/kernel`: 指定内核镜像文件的路径，即 `kernel/kernel`；
- `-m 128M`: 设置虚拟机的内存大小为 128MB；
- `-smp 3`: 启动 3 个 CPU 核心，模拟多核系统；
- `-nographic`: 禁用图形显示，以纯文本模式运行 QEMU；
- `-global virtio-mmio.force-legacy=false`: 禁用 virtio-mmio 设备的旧版兼容性；
- `-drive file=fs.img,if=none,format=raw,id=x0`: 指定虚拟机中的驱动器配置；
- `file=fs.img` 表示虚拟机使用名为 fs.img 的磁盘镜像文件作为驱动器，`format=raw` 表示镜像文件格式为原始格式，`id=x0` 给该驱动器指定了一个 ID 为 x0；
- `-device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0`: 添加一个 virtio 块设备，将之前指定的驱动器 x0 连接到该设备上，并将其连接到 virtio-mmio 总线上；

后面驱动稍微有点复杂，主要理解前面几个命令参数就好；

## Qemu 启动过程

以下内容参考自：[rCore-Tutorial-Book-v3](http://rcore-os.cn/rCore-Tutorial-Book-v3/chapter1/3first-instruction-in-kernel1.html#id5)

Qemu 模拟的 `virt` 硬件平台上，物理内存的起始地址为  `0x80000000` ，物理内存的默认大小为 128MiB，即区间 `[0x80000000,0x88000000)`。上面启动 Qemu 的时候，我们设置了内核的镜像文件地址，所以在执行 Qemu 第一条指令的时候，会将 `kernel/kernel` 镜像文件加载到物理内存 `0x80000000` 上。`0x80000000` 这个位置是 Qemu 本身设置的，一般是固定好的，除非修改 Qemu 源代码。

Qemu 启动过程一般分为三个阶段：第一个阶段由固化在 Qemu 内的一小段汇编程序负责；第二个阶段由 bootloader 负责；第三个阶段则由内核镜像负责。

xv6 这里没有使用 bootloader，所以只有第一和第三个阶段：
- 第一阶段：将必要的文件载入到 Qemu 物理内存之后，Qemu CPU 的程序计数器（PC, Program Counter）会被初始化为 `0x1000` ，因此 Qemu 实际执行的第一条指令位于物理地址 `0x1000` ，接下来它将执行寥寥数条指令并跳转到物理地址 `0x80000000` 对应的指令处并进入第二阶段。

如果想看下 `0x1000` 这里执行了哪些指令，可以通过 gdb 调试来查看，首先开启两个终端（也可以用 tmux），一个终端先运行 `make qemu-gdb CPUS=1`（为了方便调试，只开启了一个 CPU），另外一个终端运行 `gdb-multiarch`，然后就可以看见 gdb 停留在 `0x1000` 的位置；

![Image](https://pic4.zhimg.com/80/v2-8f8b89350acdc66ed9c0e2bfb4d09481.png)

接下来 gdb 中运行 `layout asm` 命令，就可以查看汇编指令了，可以看到这里只有简单的几条命令；

![Image](https://pic4.zhimg.com/80/v2-88d29f1a657af9331e2950f47e391128.png)

```asm
0x1000 auipc t0,0x0      # auipc是将立即数0x0左移12位后，并与PC（程序计数器）的高20位相加，生成一个32位的有符号立即数。然后存放到t0寄存器中，实际执行完t0存的值就是0x1000
0x1004 addi  a2,t0,40    # a2 = t0 + 40，即a2的值为 0x1028
0x1008 csrr  a0,mhartid  # 将mhartid寄存器的值存放到a0中，mhartid是线程的ID，这里只有一个CPU即为0
0x100c ld    a1,32(t0)   # 下面解释
0x1010 ld    t0,24(t0)   # 下面解释
0x1014 jr t0             # 无条件跳转到t0寄存器存储的地址位置上
```

上面汇编代码中，ld 是将 t0 存放的地址 + 32 之后得到的位置，加载一个双字（8 字节）的数据到 a1 寄存器中，t0 是 `0x1000 + 32` 之后为 `0x1020`，如果仔细看下面的汇编代码的话，会发现 `0x1020` 和 `0x1022` 位置组成的双字为 `0x87000000`（小端序），同理 `ld t0,24(t0)` 这条指令执行完毕后 t0 寄存器的值为 `0x80000000`，然后跳转到这个地址，此时进入了内核的第一条指令。

![Image](https://pic4.zhimg.com/80/v2-ec5a833e639a5e2cb09d1a585100cefd.png)

- 第三阶段：前面分析了，Qemu 第一阶段完成后，会跳转到 `0x80000000` 物理地址上，所以我们需要将内核镜像预先加载到 Qemu 物理内存以地址 `0x80000000` 开头的区域上。一旦 CPU 开始执行内核的第一条指令，证明计算机的控制权已经被移交给我们的内核；

## 内核第一条指令

内核的第一条指令可以在 `kernel/entry.S` 文件中查看，如下：

```asm
        # qemu -kernel loads the kernel at 0x80000000
        # and causes each hart (i.e. CPU) to jump there.
        # kernel.ld causes the following code to
        # be placed at 0x80000000.
.section .text
.global _entry
_entry:
        # set up a stack for C.
        # stack0 is declared in start.c,
        # with a 4096-byte stack per CPU.
        # sp = stack0 + (hartid * 4096)
        la sp, stack0    # 将stack0的地址加载到sp寄存器中，sp为栈指针，此时内核有了栈
        li a0, 1024*4    # 设置a0的值为1024*4，即设置每个CPU的栈大小为4096
		csrr a1, mhartid # 当前CPU的ID加载到寄存器a1中，用来计算每个CPU栈偏移量
        addi a1, a1, 1   # 将a1寄存器的值加1，为每个CPU的hartid添加一个偏移量，以避免栈之间的冲突
        mul a0, a0, a1   # 将a0寄存器和a1寄存器的值相乘，计算每个CPU的栈偏移量
        add sp, sp, a0   # 将栈指针sp加上栈偏移量a0，计算出每个CPU的最终栈地址
        # jump to start() in start.c
        call start       # 调用名为start的函数
spin:
        j spin           # 跳转到spin符号上，这里是一个无限循环
```

start 函数可以在 `kernel/start.c` 文件中查看，在 gpt 帮助下大概加了些注释，想要弄懂这些命令的作用，可能需要仔细看下 RISCV 手册（个人感觉对于学习 os 来说，这部分可以先跳过）：

```c
// entry.S jumps here in machine mode on stack0.
void
start()
{
  // set M Previous Privilege mode to Supervisor, for mret.
  unsigned long x = r_mstatus(); // 读取mstatus寄存器的值，并将其存储在变量x中
  x &= ~MSTATUS_MPP_MASK; //将x的MPP字段清零，以准备设置新的特权模式
  x |= MSTATUS_MPP_S;   // 将x的MPP字段设置为Supervisor模式，即将特权模式设置为Supervisor模式
  w_mstatus(x); // 将更新后的x值写回mstatus寄存器，从而设置了新的特权模式
 
  // set M Exception Program Counter to main, for mret.
  // requires gcc -mcmodel=medany
  // 将mepc寄存器设置为main函数的地址，以便在执行mret指令后返回到main函数
  w_mepc((uint64)main); 

  // disable paging for now.
  // 将satp寄存器设置为0，禁用分页机制，因此当前操作系统运行在物理内存的平坦地址空间中
  w_satp(0); 

  // delegate all interrupts and exceptions to supervisor mode.
  // 将medeleg寄存器设置为0xffff，将所有的异常（exceptions）委托给Supervisor模式处理
  w_medeleg(0xffff);
  // 将mideleg寄存器设置为0xffff，将所有的中断（interrupts）委托给Supervisor模式处理
  w_mideleg(0xffff);
  // 设置sie寄存器的值，使得Supervisor模式开启外部中断、时钟中断和软件中断
  w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);

  // configure Physical Memory Protection to give supervisor mode
  // access to all of physical memory.
  // 将pmpaddr0寄存器设置为0x3fffffffffffff，以便Supervisor模式可以访问物理内存的所有地址空间
  w_pmpaddr0(0x3fffffffffffffull);
  // 将pmpcfg0寄存器设置为0xf，配置物理内存保护，使得Supervisor模式可以完全访问所有的物理内存
  w_pmpcfg0(0xf);

  // ask for clock interrupts.
  // 初始化时钟中断，以便操作系统可以使用时钟来进行时间片轮转调度
  timerinit();

  // keep each CPU's hartid in its tp register, for cpuid().
  int id = r_mhartid(); // 获取当前CPU的hartid
  w_tp(id);  // 将变量id的值写入tp寄存器，以便每个CPU可以通过tp寄存器访问其唯一的标识符

  // switch to supervisor mode and jump to main().
  // 执行mret指令，将控制流从机器模式切换回Supervisor模式，并跳转到main函数开始执行操作系统的主要功能
  asm volatile("mret");
}
```

在 Machine 模式下设置好相关内容后，此时切换回 Supervisor 模式下，并跳转到 main 函数，然后 xv6 操作系统就开始运行了，下面是 main 函数内容：

```c
// start() jumps here in supervisor mode on all CPUs.
void
main()
{
  if(cpuid() == 0){
    consoleinit();
    printfinit();
    printf("\n");
    printf("xv6 kernel is booting\n");
    printf("\n");
    kinit();         // physical page allocator
    kvminit();       // create kernel page table
    kvminithart();   // turn on paging
    procinit();      // process table
    trapinit();      // trap vectors
    trapinithart();  // install kernel trap vector
    plicinit();      // set up interrupt controller
    plicinithart();  // ask PLIC for device interrupts
    binit();         // buffer cache
    iinit();         // inode table
    fileinit();      // file table
    virtio_disk_init(); // emulated hard disk
    userinit();      // first user process
    __sync_synchronize();
    started = 1;
  } else {
    while(started == 0)
      ;
    __sync_synchronize();
    printf("hart %d starting\n", cpuid());
    kvminithart();    // turn on paging
    trapinithart();   // install kernel trap vector
    plicinithart();   // ask PLIC for device interrupts
  }

  scheduler();        
}
```

可以看到做了一系列的初始化内容，这里不详细展开了，后续会逐步分析 xv6 的代码。

## 参考资料

- [内核第一条指令（基础篇） - rCore-Tutorial-Book-v3 3.6.0-alpha.1 文档 (rcore-os.cn)](http://rcore-os.cn/rCore-Tutorial-Book-v3/chapter1/3first-instruction-in-kernel1.html#)
- [MIT6.S081操作系统实验——操作系统是如何在qemu虚拟机中启动的？ - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/548099601)

