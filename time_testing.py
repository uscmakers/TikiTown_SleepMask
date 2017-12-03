import datetime

def main():
	raw_time = raw_input("Enter time to wake up (XX:XX:XX): ")
	user_time = raw_time.split(":")
	user_time[0] = int(user_time[0])
	user_time[1] = int(user_time[1])
	user_time[2] = int(user_time[2])

	threshold_sec = user_time[2] - 30
	if (threshold_sec < 0):
		threshold_min = user_time[1] - 1
		if (threshold_min < 0):
			threshold_hour = user_time[0] - 1
			if (threshold_hour < 0):
				user_time[0] = 23
				user_time[1] = 60 + threshold_min
				user_time[2] = 60 + threshold_sec
			else:
				user_time[0] = threshold_hour
				user_time[1] = 60 + threshold_min
				user_time[2] = 60 + threshold_sec
		else:
			user_time[1] = threshold_min
			user_time[2] = 60 + threshold_sec
	else:
		user_time[2] = threshold_sec
	
	
	time = datetime.datetime(datetime.datetime.now().year, datetime.datetime.now().month, datetime.datetime.now().day, user_time[0], user_time[1], user_time[2], 0, None)
	print time

if __name__ == "__main__":
        main()
