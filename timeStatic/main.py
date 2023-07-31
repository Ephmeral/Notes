from filecmp import cmp
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
    def __init__(self, activity_type, time_from, time_to, duration, comment):
        self.activity_type = activity_type
        self.time_from = time_from
        self.time_to = time_to
        self.duration = duration
        self.comment = comment
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
    

if __name__ == "__main__":
    with open("report.html") as fp:
        soup = BeautifulSoup(fp, "html.parser")
        tables = soup.find_all("table")
        activities = []
        entities = []
        for idx, table in enumerate(tables):
            if idx == 0:
                for row in table.tbody.find_all("tr"):
                    cells = row.find_all("td")
                    activity_type = cells[0].text.strip()
                    time_from = cells[1].text.strip()
                    time_to = cells[2].text.strip()
                    duration = cells[3].text.strip()
                    comment = cells[4].text.strip()
                    entity = TimeEntries(activity_type, time_from, time_to, duration, comment)
                    entities.append(entity)
            else:
                for row in table.tbody.find_all("tr"):
                    cells = row.find_all("td")
                    activity_type = cells[0].text.strip()
                    duration = cells[1].text.strip()
                    percent = cells[2].text.strip()
                    activity = Activity(activity_type, duration, percent)
                    activities.append(activity)

        activities = sorted(activities)
        entities = sorted(entities)

        # 进行后续的处理
        for activity in activities:
            print(activity.activity_type, activity.duration, activity.percent)
        for entity in entities:
            print(entity.activity_type, entity.time_from, entity.time_to, entity.duration, entity.comment)
    classify_time(activities)
    with open("result.txt", "w") as fp:
        for activity in activities:
            fp.write(f"{activity.activity_type}, {activity.duration}, {activity.percent}\n")
        
        fp.write("\n")

        for entity in entities:
            fp.write(f"{entity.activity_type}, {entity.time_from}, {entity.time_to}, {entity.duration}, {entity.comment}\n")
    print("====================================================")
    print_sleep(entities)