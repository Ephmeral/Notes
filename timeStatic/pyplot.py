import matplotlib.pyplot as plt

class TimeRecord:  
    def __init__(self, start_time, end_time):  
        self.start_time = start_time  
        self.end_time = end_time

data = [  
    TimeRecord("2022-01-01 00:00:00", "2022-01-01 23:59:59"),  
    TimeRecord("2022-01-02 00:00:00", "2022-01-02 23:59:59"),  
    TimeRecord("2022-01-03 00:00:00", "2022-01-03 23:59:59"),  
    TimeRecord("2022-01-04 00:00:00", "2022-01-04 23:59:59"),  
    TimeRecord("2022-01-05 00:00:00", "2022-01-05 23:59:59"),  
    TimeRecord("2022-01-06 00:00:00", "2022-01-06 23:59:59"),  
]

# 将数据转换成列表，方便进行统计分析  
data_list = []  
for record in data:  
    data_list.append((record.start_time, record.end_time))

# 计算平均值  
mean_sleep_time = sum(data_list) / len(data_list)  
mean_wake_time = sum(data_list) / len(data_list)

# 输出平均值  
print("睡眠时间平均值:", mean_sleep_time)  
print("起床时间平均值:", mean_wake_time)

# 创建 pyplot 对象  
plt = plt.pyplot()

# 绘制折线图  
plt.plot(data_list[1], data_list[0])  
plt.xlabel("日期")  
plt.ylabel("睡眠时间")  
plt.title("睡眠时间")  
plt.show()

# 绘制起床时间折线图  
plt.plot(data_list[3], data_list[2])  
plt.xlabel("日期")  
plt.ylabel("起床时间")  
plt.title("起床时间")  
plt.show()  
