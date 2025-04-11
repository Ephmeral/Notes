
## ASAN 介绍

ASAN，全称 AddressSanitizer，也即地址消毒技术。可以用来检测内存问题，例如缓冲区溢出或对悬空指针的非法访问等。根据谷歌的工程师介绍 ASAN 已经在 chromium 项目上检测出了 300 多个潜在的未知 bug，而且在使用 ASAN 作为内存错误检测工具对程序性能损耗也是及其可观的。根据检测结果显示可能导致性能降低 2 倍左右，比 Valgrind（官方给的数据大概是降低 10-50 倍）快了一个数量级。而且相比于 Valgrind 只能检查到堆内存的越界访问和悬空指针的访问，ASAN 不仅可以检测到堆内存的越界和悬空指针的访问，还能检测到栈和全局对象的越界访问。这也是 ASAN 在众多内存检测工具的比较上出类拔萃的重要原因，基本上现在 C/C++ 项目都会使用 ASAN 来保证产品质量，尤其是大项目中更为需要。

## ASAN 使用

为了使用 ASan，需要在编译时加上 -fsanitize=address 标志。例如，如果使用的是 g++，可以这样编译程序：

```shell
g++ -fsanitize=address -g your_program.cpp -o your_program
```

此后，当我们运行程序时，如果出现了内存错误，ASan 就会立即报告这个错误。

## ASAN 原理

ASAN 工具主要包括两个模块：1）**编译时插桩模块**；2）**运行时库**。

### 编译时插桩模块

使用 ASAN 相关编译参数进行编译，代码的所有内存访问（读写）操作会被编译器修改，访问内存前，判断即将被访问的内存是否为中毒状态，如果为中毒状态则输出报错信息，否则正常访问内存。

对于读操作

```c
... = *address;
```

替换为

```c
if (IsPoisoned(address)) {
    ReportError(address, kAccessSize, kIsWrite);
}
... = *address;
```

对于写操作

```c
*address = ...;
```

替换为

```c
if (IsPoisoned(address)) {
    ReportError(address, kAccessSize, kIsWrite);
}
*address = ...;
```

函数 IsPoisoned 和函数 ReportError 的实现需要注重代码执行效率和对编译目标的大小影响。

编译时会被插桩的函数可通过下载对应版本的 GCC 代码，查看函数 InitializeAsanInterceptors 获得。

```text
memmove
memset
memcpy
strcat
strchr
strcpy
strlen
wcslen
strncat
strncpy
strdup
strnlen
index
atoi
atol
strtol
atoll
strtoll
longjmp
sigaction
bsd_signal
signal
swapcontext
_longjmp
siglongjmp
__cxa_throw
pthread_create
pthread_join
__cxa_atexit
fork
```

### 运行时库

运行时库（libasan.so）接管函数 malloc 和函数 free，接管对函数 malloc 分配的内存前后产生影响，其前后被 ASAN 所用，称为红区（redzone），redzone 被标记为中毒状态，在使用函数 free 释放内存时，所释放的内存被隔离开来（暂时不会被分配出去），并被标记为中毒状态。

## 5 ASAN 防护[缓冲区溢出](https://zhida.zhihu.com/search?q=%E7%BC%93%E5%86%B2%E5%8C%BA%E6%BA%A2%E5%87%BA&zhida_source=entity&is_preview=1)的基本步骤

1. 在被保护的全局变量、堆、栈前后创建 redzone，并将 redzone 标记为中毒状态。
2. 将缓冲区和 redzone 每 8 字节对应 1 字节的映射方式建立影子内存区（影子内存区使用函数 MemToShadow 获取）。
3. 出现对 redzone 的访问（读写执行）行为时，由于 redzone [对应的影子](https://zhida.zhihu.com/search?q=%E5%AF%B9%E5%BA%94%E7%9A%84%E5%BD%B1%E5%AD%90&zhida_source=entity&is_preview=1)内存区被标记为中毒状态触发报错。
4. 报错信息包含发生错误的进程号、错误类型、出错的源文件名、行号、函数调用关系、影子内存状态。其中影子内存状态信息中出错的部分用中括号标识出来。
5. 中毒状态：内存对应的 shadow 区标记该内存不能访问的状态。

## 6 ASAN 内存泄漏检测原理

1. ASAN 接管内存申请接口（用户使用的内存全部由 ASAN 管理）。
2. 在进程退出时触发 ASAN 内存泄漏检测（可通过复位、重启等可使进程正常退出的方法触发 ASAN 对内存泄漏的检测）。
3. 触发 ASAN 内存泄漏检测后，ASAN 遍历当前所有已分配给用户但没有释放的堆内存，扫描这些内存是否被某个指针（可能是全局变量、局部变量或者是堆内存里面的指针）引用，没有被引用则表示发生了内存泄漏。
4. 输出所有泄漏的内存信息，包含内存大小和内存申请的调用栈信息等。

## 7 ASAN 内存泄漏误报

1. 结构体非 4 [字节对齐](https://zhida.zhihu.com/search?q=%E5%AD%97%E8%8A%82%E5%AF%B9%E9%BD%90&zhida_source=entity&is_preview=1)：报错提示结构体 A 内存泄漏，A 内存的指针存放在结构体 B 中，A 内存指针在结构体 B 中的偏移量非 4 的整数倍，由于 ASAN 扫描内存时是按照 4 字节偏移进行，从而扫描不到 A 内存指针导致误报。解决方法：对非4字节对齐的结构体进行整改。
2. 信号栈内存：该内存是在信号处理函数执行时做栈内存用的，其指针会保存在内核中，所以在用户态的 ASAN 扫描不到，产生误报；
3. 内存指针偏移后保存：
4. 存在ASAN未监控的内存接口：
5. 越界太离谱，越界访问的地址不在 buffer 的 redzone 内：
6. 对于memcpy的dest和src是在同一个malloc的内存块中时，内存重叠的情况无法检测到。
7. ASAN对于overflow的检测依赖于安全区，而安全区总归是有大小的。它可能是64bytes，128bytes或者其他什么值，但不管怎么样终归是有限的。如果某次踩踏跨过了安全区，踩踏到另一片可寻址的内存区域，ASAN同样不会报错。这是ASAN的另一种漏检。
8. ASAN对于UseAfterFree的检测依赖于隔离区，而隔离时间是非永久的。也就意味着已经free的区域过一段时间后又会重新被分配给其他人。当它被重新分配给其他人后，原先的持有者再次访问此块区域将不会报错。因为这一块区域的shadow memory不再是0xfd。所以这算是ASAN漏检的一种情况。

## 参考资料

- [关于 ASAN - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/512578904)