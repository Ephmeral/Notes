import matplotlib.pyplot as plt

# 假设你有一些时间数据和相应的数值数据
time_data = ['2022-05-18', '2022-05-19', '2022-05-20', '2022-05-21']
values = [10, 15, 8, 12]

# 将时间数据转换为索引
x = range(len(time_data))

# 绘制折线图
plt.plot(x, values, marker='o')

# 添加垂直时间块
plt.axvspan(x[1], x[2], color='gray', alpha=0.3)

# 设置X轴标签为时间数据
plt.xticks(x, time_data, rotation=45)

plt.xlabel('Date')
plt.ylabel('Values')
plt.title('Line Plot with Time Block')

plt.tight_layout()
plt.savefig('my_plot.png')
