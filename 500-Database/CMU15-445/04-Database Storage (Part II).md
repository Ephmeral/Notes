## 1 Log-Structured Storage

上节提到用 Slotted Pages 方法存储每个数据记录 tuple，这种方法的缺点是：
- 碎片：删除元组可能会在页面中留下间隙。
- 无用的磁盘 I/O：由于非易失性存储的面向块的性质，需要读取整个块才能获取元组。
- 随机磁盘 I/O：磁盘读取器可能必须跳转到 20 个不同的位置才能更新 20 个不同的元组，这可能非常慢。

如果我们正在开发一个只允许创建新数据而不允许覆盖的系统怎么办？ 日志结构存储模型（log-structured storage model）遵循这一假设并解决了上面列出的一些问题。

**日志结构存储（Log-Structured Storage）**：DBMS 仅存储日志记录，而不存储元组。

- 将数据库修改方式（PUT 和 DELETE）的记录存储到文件中：
	- 每个日志记录都包含元组的唯一标识符；  
	- Put 记录包含元组内容；
	- DELETE 将元组标记为已删除；
	- 当应用程序对数据库进行更改时，DBMS 将日志记录追加到文件末尾，而不检查以前的日志记录；

![](pics/Pasted%20image%2020230829134845.png)

当内存中页面写满的时候，DBMS 将其写回磁盘中，并从下一页开始填充记录：
- 所有磁盘写入都是连续的；
- 磁盘页面是不可变的；

为了读取记录，DBMS 从最新到最旧，向后扫描日志文件并“重新创建”元组。 维护一个将元组 ID 映射到最新日志记录的索引：
- 如果日志记录在内存中，则只需读取它即可；
- 如果日志记录在磁盘页上，则检索它；
- 我们将在两周内讨论索引；

![](pics/Pasted%20image%2020230829135449.png)

日志会一直增长，DBMS 需要定期压缩页面以减少空间浪费。

![](pics/Pasted%20image%2020230829135627.png)

压缩页面后，DBMS 确实需要维护页面内记录的时间顺序。每个元组 id 保证在页面中最多出现一次。DBMS 可以根据 id 顺序对页面进行排序，这些称为排序字符串表（SSTables），它们可以使元组搜索非常快。  

![](pics/Pasted%20image%2020230829140031.png)

压缩通过删除不必要的记录将较大的日志文件合并为较小的文件。压缩的问题是 DBMS 最终会出现写放大，即它一遍又一遍地重写相同的数据。

![](pics/Pasted%20image%2020230829140127.png)

## 2 Data Representation

tuple 中的数据本质上只是字节数组。 DBMS 负责了解如何解释这些字节为属性类型和值。 DBMS 的目录包含有关表的模式信息（schema information），系统使用这些信息来确定元组的布局。

数据表示方案（data representation scheme）是 DBMS 如何存储这些字节来表示一个值。

有五种高级数据类型可以存储在 tuple 中：
- integers 整数
- variable-precision numbers 可变精度数字
- fixed-point precision numbers 定长精度数字
- variable length values 可变长度
- dates/times 日期/时间

| 类型 | 实现 |
| --- | --- |
| INTEGER/BIGINT/SMALLINT/TINYINT | C/C++ Representation |
| FLOAT/REAL vs. NUMERIC/DECIMAL | IEEE-754 Standard / Fixed-point Decimals |
| VARCHAR/VARBINARY/TEXT/BLOB | Header with length, followed by data bytes |
| TIME/DATE/TIMESTAMP | 32/64-bit integer of (micro) seconds since Unix epoch |

### Integers

大多数 DBMS 使用 IEEE-754 标准指定的原生的 C/C++ 类型来存储整数。 这些值是固定长度的。 示例：INTEGER、BIGINT、SMALLINT、TINYINT。


### Variable Precision Numbers

这些是不精确、精度可变的数值类型，使用 IEEE-754 标准指定的原生的 C/C++ 类型。 这些值也是固定长度的。  

可变精度数的运算比任意精度数的运算速度更快，因为 CPU 可以直接对其执行指令。 然而，由于某些数字无法精确表示，因此在执行计算时可能会出现舍入误差。

示例：FLOAT、REAL。

在 C/C++ 中 float 精度可能是有误差的，比如下面的代码：

```c
#include <stdio.h>
int main(int argc, char* argv[]) {
	float x = 0.1;
	float y = 0.2;
	printf("x+y = %f\n", x+y);
	printf("0.3 = %f\n", 0.3);
}
```

输出：

```c
x+y = 0.300000
0.3 = 0.300000
```

但是如果取更多的位数，比如将上面代码中 %f -> %.20f 即表示需要取二十位小数，精度会有变化：

```c
x+y = 0.30000001192092895508
0.3 = 0.29999999999999998890
```

### Fixed-Point Precision Numbers

如果想要任意精度和小数位的数据类型的话，可以使用定长精度数字。它们通常以精确的、可变长度的二进制表示形式（几乎像字符串）存储，并带有附加元数据，这些元数据将告诉系统数据的长度和小数点应该在哪里等信息。 当舍入误差不可接受时使用这些数据类型，但 DBMS 会付出性能损失才能获得这种准确性。  

示例：NUMERIC、DECIMAL。

这种数据类型有不同的实现方式，例如 Postgres 的 NUMERIC：

```c
typedef unsigned char NumericDigit;
typedef struct {
    int ndigits;           // # of Digits
    int weight;            // Weight of 1st Digit
    int scale;             // Scale Factor
    int sign;              // Positive/Negative/NaN
    NumericDigit * digits; // Digit Storage
} numeric;
```

而 MySQL 的 NUMERIC 数据结构则是：

```c
typedef int32 decimal_digit_t;
struct decimal_t {
	int intg; // # of Digits Before Point
	int frac; // # of Digits After Point
	int len;   // Length (Bytes)
	bool sign; // Positive/Negative
	decimal_digit_t *buf; // Digit Storage
};
```

### Variable-Length Data

这些表示任意长度的数据类型。 它们通常与一个标头一起存储，该标头跟踪字符串的长度，以便轻松跳转到下一个值。 它还可能包含数据的校验和。  

#### Large Values

大多数 DBMS 不允许元组超过单个页面的大小。那些确实需要超过单个页面大小的 tuple，会将数据存储在专门的溢出 (overflow) 页面上，并且元组包含对该页面的引用。 这些溢出页可以包含指向其他溢出页的指针，直到可以存储所有数据。  
- Postgres: TOAST (>2KB)
- MySQL: Overflow (>½ size of page)
- SQL Server: Overflow (> size of page)

![](pics/Pasted%20image%2020230829143230.png)

#### EXTERNAL VALUES TO RAGE

有些系统允许您将这些大值存储在外部文件中，然后 tuple 将包含指向该文件的指针。 例如，如果数据库存储照片信息，DBMS 可以将照片存储在外部文件中，而不是让它们占用 DBMS 中的大量空间。 这样做的一个缺点是 DBMS 无法操作该文件的内容。 因此，不存在持久性或交易保护。

![](pics/Pasted%20image%2020230829143431.png)

示例：VARCHAR、VARBINARY、TEXT、BLOB。

### Dates and Times

不同系统的日期/时间表示方式有所不同。 通常，这些被表示为自 Unix 纪元以来的某个单位时间（微/毫秒）秒。  

示例：TIME、DATE、TIMESTAMP。

## System Catalogs

DBMS 会维护一些内部的 catalogs 来告诉数据库这些元数据是如何解释的。元数据告诉数据库有那些表、列，以及它们的数据类型等等：
- Tables, columns, indexes, views
- Users, permissions
- Internal statistics

大多数 DBMS 以用于表的格式将其 catalogs 存储在数据库内部。可以通过特殊的代码来访问这些元数据。

示例 1：列出当前数据库中的所有表格

```sql
/* SQL-92 */
SELECT *
  FROM INFORMATION_SCHEMA.TABLES
 WHERE table_catalog = '<db name>';

/* Postgres */ 
\d;

/* MySQL */ 
SHOW TABLES;

/* SQLite */ 
.tables
```

示例 2：在 student 表格中列出所有表格

```sql
/* SQL-92 */
SELECT *
  FROM INFORMATION_SCHEMA.TABLES
 WHERE table_catalog = 'student';

/* Postgres */ 
\d student;

/* MySQL */ 
DESCRIBE student;

/* SQLite */ 
.schema student
```