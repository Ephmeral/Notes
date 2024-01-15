## 锁相关命令

```mysql
// 全局锁
flush tables with read lock 
// 解全局锁
unlock tables

// 表级锁，对 user 表上读锁；
lock tables user read;
// 表级锁，对 user 表上写锁；
lock tables user write;
// 解锁
unlock tables
```

## 查看索引

如何查看 sql 命令是否用到索引

怎么判断一个 sql 语句性能问题


## 隔离级别

写一段 MySQL RR 隔离级别下的死锁代码