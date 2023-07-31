from datetime import timedelta, datetime

time_dict = {'S1': 27480, 'S2': 12900, 'S3': 0, 'S4': 6900, 'S5': 3600, 'S6': 0, 'S7': 31080}
types = {'S1':'娱乐时间', 'S2':'学习成长', 'S3':'日常通勤', 'S4':'日常生活', 'S5':'人际交往', 'S6':'工作', 'S7':'睡眠'}

time_summary = {}
for time_type, duration_seconds in time_dict.items():
    duration = timedelta(seconds=duration_seconds)
    # hours, minutes = divmod(duration_seconds, 3600)[0], divmod(duration_seconds, 60)[1]
    hours, remainder = divmod(duration_seconds, 3600)
    print("remainder = ", remainder)
    minutes, seconds = divmod(remainder, 60)
    time_summary[types[time_type]] = datetime.strptime(f"{hours:02d}:{minutes:02d}", "%H:%M").strftime("%H:%M")

print(time_summary)
