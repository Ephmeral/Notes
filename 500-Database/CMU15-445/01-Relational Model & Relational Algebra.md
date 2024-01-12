# 01-Relational Model & Relational Algebra
## 1 Databases

A **database** is an organized collection of inter-related data that models some aspect of the real-world (e.g., modeling the students in a class or a digital music store).

A database management system (DBMS) is the software that manages a database.

数据库是一组相互关联数据的有组织的集合，这些数据往往是对现实世界的某些方面进行建模。

考虑一个对数字音乐商店（例如 Spotify）进行建模的数据库。 让数据库保存有关艺术家以及这些艺术家发行的专辑的信息。

## 2 Flat File Strawman

数据库用 CSV 文件存储，我们在应用程序中自行管理这些文件：
- 每个实体将存储在其自己的文件中；
- 应用程序每次想要读取或更新记录时都必须解析文件；

需要用 2 个文件来存储数据库，一个用于艺术家，一个用于专辑。

每个实体都有自己的一组属性，因此在每个文件中，不同的记录由换行符分隔，而记录中的每个相应属性由逗号分隔。

例如：艺术家可以具有姓名、年份和国家/地区属性，而专辑则具有姓名、艺术家和年份属性。

下面是两个文件的例子：

Artist(name, year, country):
```csv
"Wu Tang Clan",1992,"USA"
"Notorious BIG",1992,"USA"
"Ice Cube",1989,"USA"
```

Album(name, artist, year):
```csv
"Enter the Wu-Tang","Wu-Tang Clan",1993
"St.Ides Mix Tape","Wu-Tang Clan",1994
"Liquid Swords","GZA",1990
```

当需要查询 GZA 那一年发布专辑，我们可能会写出下面的代码：

```
for line in file.readlines():
	record = parse(line)
	if record[0] == "GZA":
		print(int(record[1]))
```

这样查询会有很多缺陷：
- 数据质量：
	- 如何确保在每一个专辑条例中艺术家字段是相同的？
	- 如果将不合法的字符串写到专辑的年份上，怎么办？
	- 如何处理一个专辑的多个艺术家？
	- 如果删除艺术家之后，专辑字段应该怎么办？
- 实现：
	- 如何查询特定的记录？
	- 如果我们想创建一个新的应用，但是使用相同的数据库，怎么办？
	- 如果两个线程同时写入相同的文件？
- 一致性：
	- 当我们程序更新记录的时候，机器崩溃了，怎么办？
	- 如果我们想在多台机器上复制数据库以获得高可用性怎么办？

## 3 Database Management System

DBMS 是一种允许应用程序在数据库中存储和分析信息的软件。

通用 DBMS 旨在允许根据某种数据模型定义、创建、查询、更新和管理数据库。

数据模型（Data model）是用于描述数据库中的数据的概念的集合。

示例：关系型（最常见）、NoSQL（键/值、图形）、数组/矩阵/向量

模式（Schema）是基于数据模型的特定数据集合的描述。

### Early DBMSs

数据库应用程序很难构建和维护，因为逻辑层和物理层之间存在紧密耦合。  

逻辑层描述数据库具有哪些实体和属性，而物理层描述这些实体和属性的存储方式。 早期，物理层是在应用程序代码中定义的，因此如果我们想要更改应用程序正在使用的物理层，我们必须更改所有代码以匹配新的物理层。

## 4 Relational Model

Ted Codd 注意到人们每次想要更改物理层时都会重写 DBMS，因此他在 1970 年提出了关系模型（Relational model）来避免这种情况。  

关系模型定义了基于关系的数据库抽象，以避免维护开销。 它有三个关键点：  
• 以简单的数据结构（关系）存储数据库。  
• 通过高级语言访问数据，DBMS 找出最佳执行策略。  
• 物理存储留给 DBMS 实现。

关系数据模型定义了三个概念：  
• **结构（Structure）**：关系及其内容的定义。 这是关系所具有的属性以及这些属性可以持有的值。  
• **完整性（Integrity）**：确保数据库的内容满足约束。 一个示例约束是年份属性的任何值都必须是数字。  
• **操作（Manipulation）**：如何访问和修改数据库的内容。

关系（relation）是一个无序集合，包含表示实体的属性的关系。 由于关系是无序的，因此 DBMS 可以按照自己想要的任何方式存储它们，从而实现优化。  

元组（tuple）是关系中的一组属性值（也称为其域）。 最初，值必须是原子或标量，但现在值也可以是列表或嵌套数据结构。 每个属性都可以是特殊值 NULL，这意味着对于给定元组，该属性未定义。  

具有 n 个属性的关系称为 n 元关系。

### Keys

关系的主键（primary key）唯一标识单个元组。 如果您未定义内部主键，某些 DBMS 会自动创建内部主键。 许多 DBMS 都支持自动生成的键，因此应用程序不必手动递增键，但某些 DBMS 仍然需要主键。

外键（foreign key）指定一个关系中的属性必须映射到另一关系中的元组。

我们就可以利用第三张表，ArtistAlbum(artist_id, album_id)，来解决专辑与艺术家的 1 对多的关系问题：  

![](pics/Pasted%20image%2020230824202125.png)

## 5 Data Manipulation Languages (DMLs)

从数据库存储和检索信息的方法主要有两种：  
• **过程（Procedural）**：查询指定 DBMS 应使用的（高级）策略来根据集合/包查找所需要的结果。 例如，使用 for 循环扫描所有记录并统计有多少条记录来检索表中的记录数。  
• **非过程 Non-Procedural（声明性）**：查询仅需要指定查询什么数据，而不关注如何查找它。例如，使用 SQL `select count(*) from Artist` 来统计表中有多少条记录。

## 6 Relational Algebra

关系代数（Relational Algebra）是一组检索和操作关系中元组的基本运算。 每个运算符将一个或多个关系作为输入，并输出一个新的关系。 要编写查询，我们可以将这些运算符“链接”在一起以创建更复杂的操作。

### Select
选择（Select）接受一个关系并输出该关系中满足选择谓词的元组子集。 谓词的作用就像一个过滤器，我们可以使用连词和析取来组合多个谓词。

语法： $σ_{predicate}(R)$.  
示例： $σ_{a\_id=’a2’}(R)$  
SQL: `SELECT * FROM R WHERE a_id = 'a2'`

![](pics/Pasted%20image%2020230824204709.png)

### Projection

投影（Projection）接受一个关系并输出一个与仅包含指定属性的元组的关系。 您可以重新排列输入关系中属性的顺序以及操作值。

语法： $π_{A1,A2,. . . ,An}(R)$.  
示例： $π_{{b\_id - 100}, a_{id}}(σ_{a\_id=’a2’}R))$  
SQL: `SELECT b_id-100, a_id FROM R WHERE a_id = 'a2'`

![](pics/Pasted%20image%2020230824204722.png)

### Union

并集（Union）接受两个关系并输出一个关系，该关系包含至少一个输入关系中出现的所有元组。 注意：两个输入关系必须具有完全相同的属性。

语法： (R ∪ S).  
SQL: (SELECT * FROM R) UNION ALL (SELECT * FROM S)

![](pics/Pasted%20image%2020230824204735.png)

### Intersection

交集（Intersection）接受两个关系并输出一个包含出现在两个输入关系中的所有元组的关系。 注意：两个输入关系必须具有完全相同的属性。

语法： (R ∩ S).  
SQL: (SELECT * FROM R) INTERSECT (SELECT * FROM S)

![](pics/Pasted%20image%2020230824204759.png)

### Difference

差集（Difference）接受两个关系并输出一个关系，该关系包含出现在第一个关系中但不出现在第二个关系中的所有元组。 注意：两个输入关系必须具有完全相同的属性。

语法： (R − S).  
SQL: (SELECT * FROM R) EXCEPT (SELECT * FROM S)

![](pics/Pasted%20image%2020230824204818.png)

### Product

Product（又称笛卡尔积） 接受两个关系并输出一个关系，其中包含输入关系中元组的所有可能组合。

语法： (R × S).  
SQL: (SELECT * FROM R) CROSS JOIN (SELECT * FROM S)，或者更简单的 SELECT * FROM R, S

![](pics/Pasted%20image%2020230824204833.png)

### Join

连接（Join）接受两个关系并输出一个关系，该关系包含作为两个元组组合的所有元组，其中对于两个关系共享的每个属性，两个元组的该属性的值是相同的。

语法： (R ⨝ S).  
SQL: SELECT * FROM R JOIN S USING (ATTRIBUTE1, ATTRIBUTE2...)

![](pics/Pasted%20image%2020230824204849.png)

其他的关系代数：  
- Rename (ρ)  
- Assignment (R←S)  
- Duplicate Elimination (δ)  
- Aggregation (γ)  
- Sorting (τ)  
- Division (R÷S)

### 观察

关系代数是一种过程语言，因为它定义了如何计算查询的高级步骤。 例如， $σ_{b\_id=102}(R ⨝ S)$ 表示首先进行 R 和 S 的连接，然后进行选择， 而 $R ⨝ (σ_{b\_id=102}(S))$ 将首先对 S 进行选择，然后进行连接。 这两个语句实际上会产生相同的答案，但如果 S 中的 10 亿元组中只有 1 个 b_id=102 的元组，那么 $R ⨝ (σ_{b\_id=102}(S))$ 将明显快于 $σ_{b\_id=102}(R ⨝ S)$。

更好的方法是说出您想要的结果（从 R 和 S 中检索连接元组，其中 b_id 等于 102），然后让 DBMS 决定计算查询时要采取的步骤。 SQL 正是这样做的，它是在关系模型数据库上编写查询的事实上的标准。