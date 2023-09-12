# MIT6.S081/6.1810 相关资料推荐

这个文件夹主要是对MIT6.S081/6.1810课程的一些总结，下面是我推荐的一些资料，我自己对xv6部分代码做了小结，关于lab并没有写太多内容，网上这部分的博客也挺多的。

## 官网
官网：https://pdos.csail.mit.edu/6.828/2023/schedule.html

官方推荐的书籍：xv-book

注：每年的lab差别不大，22或者23年的都可以，20年的第三个页表的lab会难一点（有兴趣的可以看看之前的课程）

## 推荐一些资料
- http://xv6.dgs.zone/ 
  - 这份资料是对xv6-boook和课程lab进行中文翻译的，不过翻译的版本是20的，除了页表lab变了，其他变化不大，另外20年多了个lazy allocation lab，21年之后这个lab就被删除了。
- https://mit-public-courses-cn-translatio.gitbook.io/mit6-s081/
  - 这份资料是20年视频的中文翻译，强烈推荐！！
- [一起来学MIT6.S081呀～ ](https://tarplkpqsm.feishu.cn/docs/doccnBFsXFMsAr1oXEVsaT9E3Jg#Hwd9gu)
  - 这份资料是B站up主[阿苏EEer](https://space.bilibili.com/16765968)创建的群聊，群里小伙伴们的一些踩坑记录，做lab的时候遇见一些问题的时候可以再这个里面找一找。
- 同样的推荐一下B站up主[阿苏EEer](https://space.bilibili.com/16765968)的专栏，对各个lab进行详细解说。
- 最后推荐一下我这里的一些代码分析（2333）。

## 一点点心得
刷这门课之前最好对OS有个大致的了解，至少大概需要知道进程、内存、文件系统这些，刷lab之前建议将课程先看看，可以看上面翻译的中文版，上课的内容对后面调试、理解lab都是很有帮助的。刷lab的时候，实验手册的提示很重要，大部分内容跟着实验手册的提示都能做出来。刷lab的时候最好独立完成，遇见问题多尝试用gdb进行调试，一般来说只要大概看懂xv6的代码，跟着实验手册走多多少少都会有一点思路，可能具体代码实现的时候有点问题，这时候调试是很重要的。

关于RISC-V：如果单纯是做lab的话，RISC-V不需要了解特别的深入，但是如果想深入分析一些xv6的代码，包括内核启动、中断异常、系统调用等等这些内容，还是需要简单学习一下RISC-V的，这里有一本[中文手册](http://staff.ustc.edu.cn/~llxx/cod/reference_books/RISC-V-Reader-Chinese-v2p12017.pdf)可以看一下。如果想学习RISC-V的话，推荐[《RISC-V体系结构编程与实践》](https://book.douban.com/subject/36240082/)。