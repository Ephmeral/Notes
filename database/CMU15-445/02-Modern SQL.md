# 02-Modern SQL
## 1 Relational Languages

Edgar Codd 在 20 世纪 70 年代初发表了一篇关于关系模型的重要论文。 最初，他只定义了 DBMS 如何在关系模型 DBMS 上执行查询的数学符号。  

用户只需使用声明性语言（即 SQL）指定他们想要的结果。 DBMS 负责确定产生该答案的最有效计划。  

关系代数基于 sets（无序、无重复）。 SQL 基于 bags（无序，允许重复）

## 2 SQL History

关系数据库的声明式查询语言。 它最初是在 20 世纪 70 年代作为 IBM System R 项目的一部分开发的。 IBM 最初将其称为“SEQUEL”（结构化英语查询语言）。 该名称在 20 世纪 80 年代改为“SQL”（结构化查询语言）。

该语言由不同类别的命令组成：

1. 数据操作语言 (DML)：SELECT、INSERT、UPDATE 和 DELETE 语句。
2. 数据定义语言 (DDL)：表、索引、视图和其他对象的 Schema 定义。
3. 数据控制语言 (DCL)：安全、访问控制。

SQL 并不是一种死语言。 它每隔几年就会更新一次新功能。 SQL-92 是 DBMS 必须支持的最低要求，才能声称它们支持 SQL。 每个供应商都在一定程度上遵循该标准，但有许多专有的扩展。  

SQL 标准每个新版本发布的一些主要更新如下所示。

• SQL:1999 Regular expressions, Triggers  
• SQL:2003 XML, Windows, Sequences  
• SQL:2008 Truncation, Fancy sorting  
• SQL:2011 Temporal DBs, Pipelined DML  
• SQL:2016 JSON, Polymorphic tables

## 3 Joins

组合一个或多个表中的列并生成一个新表。 用于表达涉及跨多个表的数据的查询。

图 1：用于讲座的示例数据库

```sql
CREATE TABLE student (
	sid INT PRIMARY KEY,
	name VARCHAR(16),
	login VARCHAR(32) UNIQUE,
	age SMALLINT,
	gpa FLOAT
);
CREATE TABLE course (
	cid VARCHAR(32) PRIMARY KEY,
	name VARCHAR(32) NOT NULL
);
CREATE TABLE enrolled (
	sid INT REFERENCES student (sid),
	cid VARCHAR(32) REFERENCES course (cid),
	grade CHAR(1)
);
```

示例：哪些学生在 15-721 中获得了 A？

```sql
SELECT s.name
FROM enrolled AS e, student AS s
WHERE e.grade = 'A' AND e.cid = '15-721'
AND e.sid = s.sid;
```

## 4 Aggregates

聚合函数接受一包元组作为其输入，然后生成一个标量值作为其输出。 聚合函数（几乎）只能在 SELECT 输出列表中使用。

• AVG(COL)：COL 中值的平均值  
• MIN(COL)：COL 中的最小值  
• MAX(COL)：COL 中的最大值  
• COUNT(COL)：关系中元组的数量

示例：获取使用“@cs”登录的学生人数。 以下三个查询是等效的：

```sql
SELECT COUNT(*) FROM student WHERE login LIKE '%@cs';
SELECT COUNT(login) FROM student WHERE login LIKE '%@cs';
SELECT COUNT(1) FROM student WHERE login LIKE '%@cs';
```

问题：
- 上面 3 条语句中是否有业界规范具体使用哪条？
	- 没有规定，就像不同的 C/C++ 代码一样，不同的人写的代码风格不一致；
- 上面 3 条语句效率是否有区别？
	- 对于一个数据库来说，应该能够判断出上面做的事情是相同的，可能最终执行的过程都是一样的，所以性能上并没有太大差异；

单个 SELECT 语句可以包含多个聚合：

示例：使用“@cs”登录获取学生人数及其平均 GPA。

```sql
SELECT AVG(gpa), COUNT(sid)
	FROM student WHERE login LIKE '%@cs';
```

一些聚合函数（例如 COUNT、SUM、AVG）支持 DISTINCT 关键字。示例：获取使用“@cs”登录名的学生的数量，并且学生是不重复的。

```sql
SELECT COUNT(DISTINCT login)
	FROM student WHERE login LIKE '%@cs';
```

输出聚合之外的其他列是未定义行为（下面未定义 e.cid）。 例如：获取每门课程学生的平均 GPA。

```sql
SELECT AVG(s.gpa), e.cid
	FROM enrolled AS e, student AS s
	WHERE e.sid = s.sid;
```

SELECT 输出子句中的非聚合值必须出现在 GROUP BY 子句中。上面的正确写法如下：

```sql
SELECT AVG(s.gpa), e.cid
	FROM enrolled AS e, student AS s
	WHERE e.sid = s.sid
	GROUP BY e.cid;
```

GROUP BY ：将元组投影到子集中并针对每个子集计算聚合。

![](pics/Pasted%20image%2020230826150835.png)

如果想根据聚合的结果再进一步过滤的话，比如过滤出 GPA 平均值大于 3.9 的数据，可能会想到这样写：

```sql
SELECT AVG(s.gpa) AS avg_gpa, e.cid
	FROM enrolled AS e, student AS s
	WHERE e.sid = s.sid
	AND avg_gpa > 3.9
	GROUP BY e.cid
```

实际上上面 SQL 语句是错误的，where 语句后面不能直接使用聚合运算的结果，正确是使用 HAVING 语句。HAVING 子句根据聚合计算过滤输出结果。 这使得 HAVING 的行为类似于 GROUP BY 的 WHERE 子句。

```sql
SELECT AVG(s.gpa) AS avg_gpa, e.cid
	FROM enrolled AS e, student AS s
	WHERE e.sid = s.sid
	GROUP BY e.cid
	HAVING avg_gpa > 3.9;
```

上述查询语法被许多主要数据库系统支持，但不符合 SQL 标准。 为了使查询符合标准，我们必须在 HAVING 子句的主体中重复使用 AVG(S.GPA)。

```sql
SELECT AVG(s.gpa), e.cid
	FROM enrolled AS e, student AS s
	WHERE e.sid = s.sid
	GROUP BY e.cid
	HAVING AVG(s.gpa) > 3.9;
```

## 5 String Operations

SQL 标准规定字符串区分大小写且仅使用单引号。 但是在不同的数据库中具体还是有所差异的，例如 MySQL 就不区分大小写，可以使用单引号也可以使用双引号。

![](pics/Pasted%20image%2020230826153729.png)

例如判断两个字符串忽略大小写后是否相等：

```sql
/* SQL-92 */
WHERE UPPER(name) = UPPER('KaNyE')

/* MySQL */
WHERE name = "KaNyE"
```

### Pattern Matching

模式匹配：LIKE 关键字用于谓词中的字符串匹配。
- `%` 匹配任何字符串（包括空字符串）；
- `_` 匹配任何一个字符；

```sql
SELECT * FROM enrolled AS e
 WHERE e.cid LIKE '15-%';

SELECT * FROM student AS s
 WHERE s.login LIKE '%@c_';
```

### String Functions

SQL-92 定义了一些字符串函数，大部分的数据库系统都有他们自己的函数。

这些字符串函数可以用在谓词或输出结果上，例如：

```sql
SELECT SUBSTRING(name, 0, 5) AS abbrv_name
  FROM student WHERE sid = 53688;

SELECT * FROM student AS s
 WHERE UPPER(e.name) LIKE 'KAN%';
```

### Concatenation

SQL-92 使用两个竖线（“||”）将两个或多个字符串连接在一起形成一个字符串。但是其他的 DBMS 可能使用其他的（如 + 号，或 CONCAT 函数）：

```sql
/* SQL-92 */
SELECT name FROM student WHERE login = LOWER(name) || '@cs'

/* MSSQL */
SELECT name FROM student WHERE login = LOWER(name) + '@cs'

/* MySQL */
SELECT name FROM student WHERE login = CONCAT(LOWER(name), '@cs')
```

## 6 Date and Time

获取当前的时间：

```sql
SELECT NOW();
```

其他的一些操作 Date 和 Time 的函数就不记录了，不同 DBMS 之前的差异很大，具体需要使用的时候可以再查询。

## 7 Output Redirection

如果想将查询的结果存放到另外一张表上，以便后续查询的时候可以访问这些数据，而不是直接将查询结果返回给客户端（如终端），可以考虑使用重定向：

- 新表：将查询的结果永久的存放在新表中

```sql
SELECT DISTINCT cid INTO CourseIds FROM enrolled;
```

- 已有的表：将查询的输出存储到数据库中已存在的表中。 查询结果必须与目标表具有相同数量和相同类型的列，但查询输出中的列名称不必匹配。

```sql
INSERT INTO CourseIds (SELECT DISTINCT cid FROM enrolled);
```

插入其他表的时候，不同的 DBMS 发生错误后的行为也不一致，有的会直接撤销插入的数据，而有的则不会。

## 8 Output Control
### ORDER BY
SQL 查询结果是无序的，可以使用 ORDER BY 语句对元祖进行排序。

语法：`ORDER BY <column*> [ASC|DESC]`
- 其中 column 可以是一个或多个；
- ASC 表示升序排序，DESC 表示降序排序；

```sql
SELECT sid, grade FROM enrolled WHERE cid = '15-721'
	ORDER BY grade;
```

默认情况是 ASC 是升序，想要降序可以使用 DESC：

```sql
SELECT sid, grade FROM enrolled WHERE cid = '15-721'
	ORDER BY grade DESC;
```

可以使用多个 ORDER BY 来进行更复杂的排序：

```sql
SELECT sid, grade FROM enrolled WHERE cid = '15-721'
	ORDER BY grade DESC, sid ASC;
/* 根据成绩降序，成绩相同的根据sid升序 */
```

ORDER BY 子句中还可以使用任意表达式：

```sql
SELECT sid FROM enrolled WHERE cid = '15-721'
	ORDER BY UPPER(grade) DESC, sid + 1 ASC;
```

### LIMIT
默认情况下，DBMS 将返回查询生成的所有元组。 我们可以使用 LIMIT 子句来限制结果元组的数量：

```sql
SELECT sid, name FROM student WHERE login LIKE '%@cs'
	LIMIT 10;
```

我们还可以提供一个偏移量来返回结果中的一个范围：

```sql
SELECT sid, name FROM student WHERE login LIKE '%@cs'
LIMIT 20 OFFSET 10;
```

## 9 Nested Queries

嵌套查询（Nested Queries）指的是一个查询中包含其他的查询。嵌套查询通常很难优化。

外部查询（Outer Query）的范围包含在内部查询（Inner Query) 中，即内部查询可以访问外部查询的属性，但反之则不然。

```sql
SELECT name FROM student /* Outer Query */
WHERE sid IN (SELECT sid FROM enrolled) /* Inner Query */
```

内部查询几乎可以出现在查询的任何部分：

1. SELECT 输出目标：

```sql
SELECT (SELECT 1) AS one FROM student;
```

2. FROM 语句：

```sql
SELECT name
	FROM student AS s, (SELECT sid FROM enrolled) AS e
	WHERE s.sid = e.sid;
```

3. WHERE 语句：

```sql
SELECT name FROM student
	WHERE sid IN ( SELECT sid FROM enrolled );
```

示例 1：获取注册 ’15-445‘ 这门课的所有学生的姓名

```sql
SELECT name FROM student
	WHERE sid IN (
		SELECT sid FROM enrolled
		WHERE cid = '15-445'
	);
```

注：上面的 sid 具有不同的范围（类似于编程语言中的作用域），具体取决于是在哪条语句中，对于内部查询 sid 指的是在 enrolled 表课程 cid 等于’15-445’的学生，而外部查询中 sid 指的是在 student 表中参加了‘15-445’课程的学生。

示例 2：查找至少注册了一门课程的学生中 ID 最高的记录

使用 while 嵌套查询：
```sql
SELECT sid, name FROM student 
WHERE sid IN (
	SELECT MAX(sid) FROM enrolled
	ORDER BY sid DESC 
	LIMIT 1
)
```

使用连接：

```sql
SELECT student.sid, name
	FROM student
	JOIN (SELECT MAX(sid) AS sid
		FROM enrolled) AS max_e
	ON student.sid = max_e.sid;
```

**嵌套查询结果表达式**：  
• ALL：必须满足子查询中所有行的表达式。  
• ANY：必须满足子查询中至少一行的表达式。  
• IN：相当于 =ANY()。  
• EXISTS：至少返回一行。

示例 3：在所有课程中，找到没有学生注册的课程

```sql
SELECT * FROM course
	WHERE NOT EXISTS (
		SELECT * FROM enrolled
		WHERE course.cid = enrolled.cid
	);
```

## 10 Window Functions

窗口函数（Window Functions）在一组相关的元组上执行“滑动”计算。 类似于聚合，但元组不会分组为单个输出的元组。  

窗口函数可以是我们上面讨论的任何聚合函数。 还有一些特殊的窗口函数：
- **ROW_NUMBER**：当前行号；
- **RANK**：当前行的顺序位置；

语法形式是：

```sql
SELECT ... FUNC-NAME(...) OVER (...) 
FROM tableName
```

其中：
- FUNC-NAME 可以是聚合函数或特殊的窗口函数；
- OVER 指定在计算窗口函数时如何将元组分组在一起；

例如：使用 PARTITION BY 指定组

```sql
SELECT cid, sid, ROW_NUMBER() OVER (PARTITION BY cid)
FROM enrolled ORDER BY cid;
```

![](pics/Pasted%20image%2020230826173636.png)

可以看到根据 cid 进行分组，row_number 是每个分组当中的行号。

可以在 OVER 中使用 ORDER BY 来对分组中的数据排序：

```sql
SELECT *, ROW_NUMBER() OVER (ORDER BY cid)
FROM enrolled ORDER BY cid;
```

**区别**：DBMS 在窗口函数排序之后计算 RANK，而在排序之前计算 ROW_NUMBER。

示例：查找每门课程成绩第二高的学生。

```sql
SELECT * FROM (
	SELECT *, RANK() OVER (PARTITION BY cid
		ORDER BY grade ASC) AS rank
	  FROM enrolled) AS ranking
  WHERE ranking.rank = 2;
```

其中：
- 子查询会根据 cid 进行分组，然后根据 grade 进行排序；
- 外部查询的 WHERE 中 ranking.rank 都是对应的别名；

## 11 Common Table Expressions

在编写更复杂的查询时，通用表表达式 (CTE) 是窗口或嵌套查询的替代方案。 它们提供了一种在较大查询中为用户编写辅助语句的方法。 CTE 可以被视为仅限于单个查询的临时表。 WITH 子句将内部查询的输出绑定到具有该名称的临时结果。

示例：生成一个名为 cteName 的 CTE，其中包含单个元组，且单个属性设置为“1”。 选择此 CTE 中的所有来自 CTE.cteName 的属性。

```sql
WITH cteName AS (
	SELECT 1
)
SELECT * FROM cteName;
```

我们可以在 AS 之前，给输出列绑定名称：

```sql
WITH cteName (col1, col2) AS (
	SELECT 1, 2
)
SELECT col1 + col2 FROM cteName;
```

一条简单的查询语句可以包好多条 CTE 声明：

```sql
WITH cte1 (col1) AS (SELECT 1), cte2 (col2) AS (SELECT 2)
SELECT * FROM cte1, cte2;
```

在 WITH 之后添加 RECURSIVE 关键字允许 CTE 引用自身。 这使得能够在 SQL 查询中实现递归。 通过递归 CTE，SQL 可以证明是图灵完备的，这意味着它的计算表达能力与更通用的编程语言一样（虽然有点麻烦）。

示例：打印从 1 到 10 的数字序列

```sql
WITH RECURSIVE cteSource (counter) AS (
	( SELECT 1 )
	UNION
	( SELECT counter + 1 FROM cteSource
		WHERE counter < 10 )
)
SELECT * FROM cteSource;
```