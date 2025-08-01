
## valgrind 概述

Valgrind 是运行在 Linux 上一套基于仿真技术的程序调试和分析工具，它包含一个内核──一个软件合成的 CPU，和一系列的小工具，每个工具都可以完成一项任务──调试，分析，或测试等。Valgrind 可以检测内存泄漏和内存违例，还可以分析 cache 的使用等

不管是使用哪个工具，valgrind 在开始之前总会先取得对你的程序的控制权，从可执行关联库里读取调试信息。然后在 valgrind 核心提供的虚拟 CPU 上运行程序，valgrind 会根据选择的工具来处理代码，该工具会向代码中加入检测代码，并把这些代码作为最终代码返回给 valgrind 核心，最后 valgrind 核心运行这些代码。

valgrind 提供多种内存检测方法，用于检测不同的数据，满足不同的使用需求，可使用的工具如下：

1. cachegrind 是一个缓冲模拟器。它可以用来标出你的程序每一行执行的指令数和导致的缓冲不命中数。
2. callgrind 在 cachegrind 基础上添加调用追踪。它可以用来得到调用的次数以及每次函数调用的开销。作为对 cachegrind 的补充，callgrind 可以分别标注各个线程，以及程序反汇编输出的每条指令的执行次数以及缓存未命中数。
3. helgrind 能够发现程序中潜在的条件竞争。
4. lackey 是一个示例程序，以其为模版可以创建你自己的工具。在程序结束后，它打印出一些基本的关于程序执行统计数据。
5. massif 是一个堆剖析器，它测量你的程序使用了多少堆内存。
6. memcheck 是一个细粒度的的内存检查器。
7. none 没有任何功能。它一般用于 Valgrind 的调试和基准测试。

## Memcheck 工具介绍

Memcheck 是 valgrind 应用最广泛的工具，能够发现开发中绝大多数内存错误使用情况。此工具主要可检查以下错误
（1） 使用未初始化的内存 (Use of uninitialised memory)
（2） 使用已经释放了的内存 (Reading/writing memory after it has been free’d)
（3） 使用超过 malloc 分配的内存空间 (Reading/writing off the end of malloc’d blocks)
（4） 对堆栈的非法访问 (Reading/writing inappropriate areas on the stack)
（5） 申请的空间是否有释放 (Memory leaks – where pointers to malloc’d blocks are lost forever)
（6） malloc/free/new/delete 申请和释放内存的匹配 (Mismatched use of malloc/new/new [] vs free/delete/delete [])
（7） src 和 dst 的重叠 (Overlapping src and dst pointers in memcpy() and related functions)

## memcheck 简单使用

memcheck 工具的使用方式如下:

```shell
valgrind --tool=memcheck ./a.out
```

从上面的命令可以清楚的看到, 主要的命令是 valgrind，而我们想使用的工具是通过 '-tool' 选项来指定的. 上面的‘a.out’指的是我们想使用 memcheck 运行的可执行文件.

_**工程实践注意：**_

需要测试运行中的内存泄漏问题，可以使用如下命令：

```shell
valgrind --log-file=valgrind.log --tool=memcheck --leak-check=full \
--show-leak-kinds=all ./your_app arg1 arg2
```

--log-file 报告文件名。如果没有指定，输出到 stderr。 --tool=memcheck 指定 Valgrind 使用的工具。Valgrind 是一个工具集，包括 Memcheck、Cachegrind、Callgrind 等多个工具。memcheck 是缺省项。 --leak-check 指定如何报告内存泄漏（memcheck 能检查多种内存使用错误，内存泄漏是其中常见的一种），可选值有:

- no 不报告
- summary 显示简要信息，有多少个内存泄漏。summary 是缺省值。
- yes 和 full 显示每个泄漏的内存在哪里分配。

show-leak-kinds 指定显示内存泄漏的类型的组合。类型包括 definite, indirect, possible,reachable。也可以指定 all 或 none。缺省值是 definite,possible。 运行一段时间后想停止进程不要 kill 掉，需要 ctrl + c 来结束，输出的 log 会在上述命令中的 valgrind.log 中。

## memcheck 原理

Memcheck 实现了一个仿真的 CPU，被监控的程序被这个仿真 CPU 解释执行，该仿真 CPU 可以在所有的内存读写指令发生时，检测地址的合法性和读操作的合法性。

![](pics/Pasted%20image%2020240424143518.png)

Memcheck 能够检测出内存问题，关键在于其建立了两个全局表。
（1）Valid-Value 表：
对于进程的整个地址空间中的每一个字节 (byte)，都有与之对应的 8 个 bits；对于 CPU 的每个寄存器，也有一个与之对应的 bit 向量。这些 bits 负责记录该字节或者寄存器值是否具有有效的、已初始化的值。
（2）Valid-Address 表
对于进程整个地址空间中的每一个字节 (byte)，还有与之对应的 1 个 bit，负责记录该地址是否能够被读写。

检测原理：
当要读写内存中某个字节时，首先检查这个字节对应的 A bit。如果该 A bit 显示该位置是无效位置，memcheck 则报告读写错误。

内核（core）类似于一个虚拟的 CPU 环境，这样当内存中的某个字节被加载到真实的 CPU 中时，该字节对应的 V bit 也被加载到虚拟的 CPU 环境中。一旦寄存器中的值，被用来产生内存地址，或者该值能够影响程序输出，则 memcheck 会检查对应的 V bits，如果该值尚未初始化，则会报告使用未初始化内存错误。

简单来说：
（1）如何知道那些地址是合法的（内存已分配）？
维护一张合法地址表（Valid-address (A) bits），当前所有可以合法读写（已分配）的地址在其中有对应的表项。该表通过以下措施维护：
① 全局数据 (data, bss section)–在程序启动的时候标记为合法地址
② 局部变量–监控 sp(stack pointer) 的变化，动态维护
③动态分配的内存–截获 分配/释放 内存的调用：malloc, calloc, realloc, valloc, memalign, free, new, new[], delete and delete[]
④ 系统调用–截获 mmap 映射的地址
⑤ 其他–可以显示知会 memcheck 某地字段是合法的

（2）如何知道某内存是否已经被赋值？
① 维护一张合法值表（Valid-value (V) bits），指示对应的 bit 是否已经被赋值。因为虚拟 CPU 可以捕获所有对内存的写指令，所以这张表很容易维护。

## 参考资料   

- [valgrind基本功能介绍、基础使用方法说明_valgrind教程-CSDN博客](https://blog.csdn.net/weixin_45518728/article/details/119865117)
- [Linux 性能分析valgrind（一）之memcheck使用 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/92074597)

