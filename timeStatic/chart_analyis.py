import pandas as pd
import matplotlib.pyplot as plt

# 假设数据保存在data.txt文件中，每行一条睡眠时间数据
# 读取数据并解析成DataFrame
with open('data.txt', 'r') as f:
    data = [line.strip().split() for line in f]

df = pd.DataFrame(data, columns=['device', 'sleep_date', 'start_time', 'end_time', 'duration', 'tag'])

# 解析日期和时间并计算睡眠时长
df['sleep_date'] = pd.to_datetime(df['sleep_date'])
df['start_time'] = pd.to_datetime(df['start_time'])
df['end_time'] = pd.to_datetime(df['end_time'])
df['duration'] = (df['end_time'] - df['start_time']).dt.total_seconds() / 3600

# 将日期和睡眠时长进行累积计算
daily_sleep = df.groupby('sleep_date')['duration'].sum()
weekly_sleep = df.groupby(pd.Grouper(key='sleep_date', freq='W-MON'))['duration'].sum()
monthly_sleep = df.groupby(pd.Grouper(key='sleep_date', freq='M'))['duration'].sum()

# 绘制条形图
plt.figure(figsize=(12, 6))

# 绘制每天睡眠时间条形图
plt.subplot(1, 3, 1)
daily_sleep.plot(kind='bar', color='skyblue')
plt.xlabel('Date')
plt.ylabel('Sleep Time (hours)')
plt.title('Daily Sleep Time')

# 绘制每周睡眠时间条形图
plt.subplot(1, 3, 2)
weekly_sleep.plot(kind='bar', color='lightgreen')
plt.xlabel('Week')
plt.ylabel('Sleep Time (hours)')
plt.title('Weekly Sleep Time')

# 绘制每月睡眠时间条形图
plt.subplot(1, 3, 3)
monthly_sleep.plot(kind='bar', color='lightcoral')
plt.xlabel('Month')
plt.ylabel('Sleep Time (hours)')
plt.title('Monthly Sleep Time')

plt.tight_layout()
plt.savefig('my_plot.png')
