import csv
from bs4 import BeautifulSoup
import re
from datetime import datetime, timedelta

types = {'S0':'计划总结', 'S1':'娱乐时间', 'S2':'学习成长', 'S3':'交通出行', 
            'S4':'日常生活', 'S5':'人际交往', 'S6':'工作学业', 'S7':'睡眠'}

class Activity:
    def __init__(self, activity_type, duration, percent):
        self.activity_type = activity_type
        self.duration = duration
        self.percent = percent
    
    def __cmp__(self, other):  
        return cmp(self.activity_type, other.activity_type)

    def __lt__(self, other):
        if self.activity_type == other.activity_type:
            return self.duration < other.duration
        return self.activity_type < other.activity_type

class TimeEntries:
    def __init__(self, activity_type, time_from, time_to, duration, comment, tags):
        self.activity_type = activity_type
        self.time_from = time_from
        self.time_to = time_to
        self.duration = duration
        self.comment = comment
        self.tags = tags
    def __cmp__(self, other):  
        return cmp(self.activity_type, other.activity_type)
    def __lt__(self, other):
        if self.activity_type == other.activity_type:
            return self.time_from < other.time_from
        return self.activity_type < other.activity_type

def classify_time(activities):
    time_summary = {'S0': 0, 'S1': 0, 'S2': 0, 'S3': 0, 'S4': 0, 'S5': 0, 'S6': 0, 'S7': 0}
    # 假设times是包含所有时间记录的列表，每个记录是一个字符串，格式为"时间分类:时间长度"，例如"S1:01:30"。
    for activity in activities:
        time_type = activity.activity_type[:2]
        idx = activity.duration.index(":")
        duration_hours = int(activity.duration[:idx])
        duration_minutes = int(activity.duration[idx + 1:])
        # 将时间长度转换为timedelta对象，并累加到对应时间分类的总时间中
        duration = timedelta(hours=duration_hours, minutes=duration_minutes)
        time_summary[time_type] += int(duration.total_seconds())
    # print(time_summary)
    # types = {'S1':'娱乐时间', 'S2':'学习成长', 'S3':'日常通勤', 'S4':'日常生活', 'S5':'人际交往', 'S6':'工作', 'S7':'睡眠'}

    result = {}
    for time_type, seconds in time_summary.items():
        duration = timedelta(seconds=seconds)
        hours, remainder = divmod(seconds, 3600)
        minutes, seconds = divmod(remainder, 60)
        # result[types[time_type]] = datetime.strptime(f"{hours:03d}:{minutes:03d}", "%H:%M").strftime("%H:%M")
        result[time_type + ":" + types[time_type]] = str(hours) + ":" + f"{minutes:02d}"
    # print(result)
    print("| 分类 | 时间 |\n| --- | --- |")
    for key, val in result.items():
        print("| " + key + " | " + val + " |")

def print_sleep(entities):
    print("Sleep time")
    entities = reversed(entities)
    for entity in entities:
        if entity.activity_type == "S7:睡眠":
            sleep_time = entity.time_from
            wakeup_time = entity.time_to
            
            # print("Sleep time:" + sleep_time[-5:] + " Wakeup time:" + wakeup_time[-5:])
            print("日期：" + sleep_time[:-5])
            print("- 起床：" + wakeup_time[-5:] + "\n- 就寝：" + sleep_time[-5:])
            # print(entity.activity_type, entity.time_from, entity.time_to, entity.duration, entity.comment)


def opencsv():
    with open('report.csv', 'r') as file:
        reader = csv.reader(file)
        for row in reader:
            # 处理每一行数据
            print(row)

def openTime():
    # 创建一个字典来存储每个活动类别的总持续时间
    activities = []
    entities = []
    with open('report.csv', 'r') as file:
        reader = csv.DictReader(file)
        flags = True
        for row in reader:
            # 遇见空行，说明要处理下一个字段的数据
            if row == {'活动类别': '活动类别', '持续时间': '持续时间', '开始时间': '%', '结束时间': None, '备注': None, '标签': None}:
                flags = False
                continue
            if flags:
                activity_type = row['活动类别']
                duration = row['持续时间']
                time_from = row['开始时间']
                time_to = row['结束时间']
                comment = row['备注']
                tags = row['标签']
                entity = TimeEntries(activity_type, time_from, time_to, duration, comment, tags)
                entities.append(entity)
            else:
                activity_type = row['活动类别']
                duration = row['持续时间']
                percent = row['开始时间'] # 实际上这里应该是 %
                activity = Activity(activity_type, duration, percent)
                activities.append(activity)

    # 进行后续的处理
    for activity in activities:
        print(activity.activity_type, activity.duration, activity.percent)
    print("=============================================================================")
    for entity in entities:
        print(entity.activity_type, entity.time_from, entity.time_to, entity.duration, entity.comment)

    print("=============================================================================")
    
    print_sleep(entities) # 打印睡眠时间
    print("=============================================================================")
    classify_time(activities) # 打印活动时间分类统计

if __name__ == "__main__":
    # opencsv()
    openTime()