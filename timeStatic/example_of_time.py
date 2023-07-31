import datetime

# 定义一个列表，包含多个时间字符串
time_strings = ['08:38', '03:54', '03:35', '02:27', '01:20', '01:17', '01:00', '00:35']

# 初始化时间总和为 0
total_time = datetime.timedelta()

# 遍历时间字符串列表，并将每个时间字符串转换为 datetime.time 对象
for time_string in time_strings:
    # 使用 strptime() 方法将时间字符串转换为 datetime.time 对象
    time_obj = datetime.datetime.strptime(time_string, '%H:%M').time()
    # 将时间对象转换为 timedelta 对象，并将其加入到时间总和中
    total_time += datetime.timedelta(hours=time_obj.hour, minutes=time_obj.minute)

# 计算总时间的小时数和分钟数
total_hours = total_time.seconds // 3600
total_minutes = (total_time.seconds % 3600) // 60

# 将小时数和分钟数格式化为字符串，并拼接成 hh:mm 的格式
total_time_string = f'{total_hours:02d}:{total_minutes:02d}'

# 打印时间总和
print(total_time_string)
