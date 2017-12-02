import datetime
from time import sleep

def main():
	raw_time = raw_input("Enter time to wake up (XX:XX): ")
	user_time = raw_time.split(":")
	user_time[0] = int(user_time[0])
	user_time[1] = int(user_time[1])

	no_alarm = 1
	while(no_alarm):
		hour = datetime.datetime.now().time().hour
		min = datetime.datetime.now().time().minute
		
		if (hour > 12):
			hour -= 12

		print("Curr:", hour, min, "Set:", user_time[0], user_time[1])

		if (hour == user_time[0] and min == user_time[1]):
			print("Alarm sounds!")
			no_alarm = 0
		else:
			print("Alarm still going!")
			sleep(1)

if __name__ == "__main__":
        main()
