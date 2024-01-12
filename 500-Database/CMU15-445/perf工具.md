### Commands

```bash
sudo perf record -g -a /path/to/your/application
```

运行一个程序，并对其采样，生成相关数据 `perf.data`。`-g` 表明记录函数调用关系，`-a` 为路径名，还有一个可选参数 `-e`，表明需要监控的事件。

perf 有一个监控事件列表，包含 cpu clock，cache miss，page fault 等各种事件。可以用

```bash
sudo perf list
```

查看。默认为 cpu clock。

生成 `perf.data` 后，执行

```bash
sudo perf report -g
```

查看报告。