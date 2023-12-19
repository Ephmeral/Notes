import csv
from bs4 import BeautifulSoup
import re
from datetime import datetime, timedelta

types = {'S0': '计划总结', 'S1': '娱乐时间', 'S2': '学习成长', 'S3': '交通出行',
         'S4': '日常生活', 'S5': '人际交往', 'S6': '工作学业', 'S7': '睡眠'}


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


# 对于每个小的分类，记录为:[类别，总时长，备注，标签]
class MiniTimeLog:
    def __init__(self, type, time, comment, tags):
        self.type = type
        self.time = time
        self.comment = comment
        self.tags = tags
        self.commentList = []
        if len(comment) != 0 and len(tags) != 0:
            self.commentList.append(f"{comment}\n")  # 加入新对象的标签/备注

    def __cmp__(self, other):
        return cmp(self.type, other.type) and cmp(self.tags, other.tags)

    def __lt__(self, other):
        if self.type == other.type:
            return self.time < other.time
        return self.type < other.type

    def addTime(self, time, comment, tags):
        self.time += time
        if len(comment) != 0 and len(tags) != 0:
            self.commentList.append(f"{comment}\n")

    def printTimeLog(self):
        # print("time = " + str(self.time))
        print("- " + self.type[3:] + " **" + self.getTime(self.time) + "**")
        if len(self.commentList) != 0:
            for com in self.commentList:
                print("    - {0} {1}".format(self.tags, com), end="")

    def getTime(self, time) -> str:
        hh = time // 3600
        mm = (time % 3600) // 60
        if hh == 0:
            return str(mm) + "min"
        if mm == 0:
            return str(hh) + "h"
        return str(hh) + "h" + str(mm) + "min"

# 将各个小活动分类合并为一个大类
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


def calTimeOfEntity(time_from, time_to) -> int:
    # 将字符串解析为datetime对象
    time1 = datetime.strptime(time_from, "%Y-%m-%d %H:%M")
    time2 = datetime.strptime(time_to, "%Y-%m-%d %H:%M")

    # 计算时间差
    time_difference = time2 - time1

    # 输出时间差
    # print("Time Difference:", time_difference)
    # print("Days:", time_difference.days)
    # print("Seconds:", time_difference.seconds)
    return time_difference.seconds


def time_entity_analysis(entities):
    # 对于每个具体的时间段，将备注信息提取出来，用来生成日志
    # 具体规则：
    ## 1. 对于每个条例来说，将type和备注相同的，时间进行合并，
    ### 比如S2:OS xv6 和 xv6 就可以合并
    ### 而S2:OS 操作系统导论 就不能和 xv6 合并
    ### 对于无备注、无标签的，统一合并起来
    ## 2. 输出的结果是：[分类, 总时长, 备注+标签]
    ### 对于第三个，使用 #tag + comment 的形式
    ### 如果有多个组合以 \n 结尾合并起来

    timeDict = {}
    for entity in entities:
        key = entity.activity_type + "-" + entity.tags
        time = calTimeOfEntity(entity.time_from, entity.time_to)
        if key in timeDict:
            timeDict[key].addTime(time, entity.comment, entity.tags)
        else:
            timeDict[key] = MiniTimeLog(entity.activity_type, time, entity.comment, entity.tags)

    for key, timeLog in timeDict.items():
        timeLog.printTimeLog()

    # TODO 根据时间段，画出类似于APP上的日历界面
    # TODO 根据标签进行分析
    # TODO 对比日/周/月等时间情况


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
    with open('/home/silas/MyCSNotes/timeStatic/report.csv', 'r') as file:
        reader = csv.DictReader(file)
        flags = True
        for row in reader:
            # 遇见空行，说明要处理下一个字段的数据
            if row == {'活动类别': '活动类别', '持续时间': '持续时间', '开始时间': '%', '结束时间': None, '备注': None,
                       '标签': None}:
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
                percent = row['开始时间']  # 实际上这里应该是 %
                activity = Activity(activity_type, duration, percent)
                activities.append(activity)

    activities = sorted(activities)
    entities = sorted(entities)

    # 进行后续的处理
    for activity in activities:
        print(activity.activity_type, activity.duration, activity.percent)
    print("=============================================================================")
    for entity in entities:
        print(entity.activity_type, entity.time_from,
              entity.time_to, entity.duration, entity.comment, entity.tags)

    print("=============================================================================")

    print_sleep(entities)  # 打印睡眠时间
    print("=============================================================================")
    classify_time(activities)  # 打印活动时间分类统计

    print("=============================================================================")
    time_entity_analysis(entities)  # 按小类统计时间，并显示备注/标签等


if __name__ == "__main__":
    # opencsv()
    openTime()