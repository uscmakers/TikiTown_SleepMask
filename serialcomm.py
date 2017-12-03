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

def code(port):
        global command
	global time
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
			user_time = raw_time.split(":")
			time_arr = [int(user_time[0]), int(user_time[1]), int(user_time[2])]
			if (datetime.datetime.now().time().hour > time_arr[0]):
				days = monthrange(datetime.date.year, datetime.date.month)
				if (datetime.date.day + 1 > days):
					if (datetime.date.month > 12):
						time = datetime.datetime(datetime.date.year + 1, 1, 1, time_arr[0], time_arr[1], time_arr[2], 0, None)
					else:
						time = datetime.datetime(datetime.date.year, datetime.date.month + 1, 1, time_arr[0], time_arr[1], time_arr[2], 0, None)
				else:
					time = datetime.datetime(datetime.date.year, datetime.date.month, datetime.date.day + 1, time_arr[0], time_arr[1], time_arr[2], 0, None)
			else:
				time = datetime.datetime(datetime.date.year, datetime.date.month, datetime.date.day, time_arr[0], time_arr[1], time_arr[2], 0, None)
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
	while(command != 0):
		line = port.read(1)
		if (line != "" and command != 0):
			i = ord(line)
			if (i == 1):
				print("USER IS IN LIGHT SLEEP")
				packed_data = pack("=BBH", 2, 255, 5000)
				if (time < datetime.datetime.now()):
					port.write(packed_data)
				#exit()
			print(line, end='')

def connect():
	try:
		conn = Serial('/dev/cu.usbmodemFA131', dsrdtr=0, rtscts=0, timeout=1) #tty.usbmodem1411
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
