from __future__ import print_function
import sys
import re
from time import sleep, time
from struct import pack
from serial import Serial
import os
import math
import threading
import datetime
from calendar import monthrange

command = -1
i = -1
time = datetime.datetime.now()
time_init = datetime.datetime.now()
flag = 0

def code(port):
	global command
	global time
	global flag
	global time_init
	while(command !=0):
		packed_data = ""
		command = input("Enter command:\n\t0:quit\n\t1:start\n\t2:wake\n\t3:stop")
		if (command == 0):
			break;
		elif (command == 1):
			NS = input("Enter number of samples: ")
			SD = input("Enter sampling delay: ")
			PD = input("Enter pulse delay: ")

			raw_time = raw_input("Enter time to wake up (XX:XX:XX): ")
			flag = 1
			user_time = raw_time.split(":")

			user_time[0] = int(user_time[0])
			user_time[1] = int(user_time[1])
			user_time[2] = int(user_time[2])
			
			year = datetime.datetime.now().year
			month = datetime.datetime.now().month
			day = datetime.datetime.now().day
			
			time_init = datetime.datetime(year, month, day, user_time[0], user_time[1], user_time[2], 0, None)
			
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
			time = datetime.datetime(year, month, day, user_time[0], user_time[1], user_time[2], 0, None)
			packed_data = pack("=BHHH", command, NS, SD, PD)
		elif (command == 2):
			brightness = input("Enter brightness: ")
			ramp_up = input("Enter ramp up: ")
			packed_data = pack("=BBH", command, brightness, ramp_up)
		elif (command == 3):
			packed_data = pack("=B", command)
		else:
			print("Unauthorized command")
		port.write(packed_data)

def script(port):
	global command
	global time
	global flag
	global time_init
	packed_data = pack("=BBH", 2, 255, 5000)
	while(command != 0):
		line = port.read(1)
		print(line, end='')
		if (line != "" and command != 0):
			sleep_mode = ord(line)
			if (sleep_mode == 1):
				print("USER IS IN LIGHT SLEEP")
				if (time < datetime.datetime.now()):
					port.write(packed_data)
					flag = 0
		if (flag == 1 and time_init <= datetime.datetime.now()):
                        flag = 0
			port.write(packed_data)
			#print(line, end='')

def connect():
	try:
		conn = Serial('/dev/cu.usbserial-AH05K3YE', dsrdtr=0, rtscts=0, timeout=1) #cu.usbmodemFA131
	except IOError:
		print("Error opening serial port.", file=sys.stderr)
		sys.exit(2)

	return conn


def main():
	print("Connecting...")
	conn = connect()
	print("Connected!")

	print("Running code...")
	t1 = threading.Thread(target=script, args=(conn,))
	t2 = threading.Thread(target=code, args=(conn,))
	t1.start()
	t2.start()

if __name__ == "__main__":
	main()
