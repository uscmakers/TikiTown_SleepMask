from __future__ import print_function
import sys
import re
from time import sleep, time
from struct import *
from serial import Serial
import os
import math
import binascii
import threading

command = -1
i = -1

def code(port):
	################# Old Code #######################
	#x = raw_input("Enter (s) to start or (q) quit: ")
	#while(x == 's'):
		#port.write(x);
		#line = port.readline();
		#print(line)
		#x = raw_input("Enter a number (0 - 9): ")
	##################################################
        global command
        while(command !=0):
                packed_data = ""
                command = input("Enter command:\n\t0:quit\n\t1:start\n\t2:wake\n\t3:stop")
                if (command == 0):
                        break;
                elif (command == 1):
                        NS = input("Enter number of samples: ")
                        SD = input("Enter sampling delay: ")
                        PD = input("Enter pulse delay: ")
                        packed_data = pack("=BHHH", command, NS, SD, PD)
                        #print(binascii.hexlify(packed_data))
                elif (command == 2):
                        brightness = input("Enter brightness: ")
                        ramp_up = input("Enter ramp up: ")
                        packed_data = pack("=BBH", command, brightness, ramp_up)
                        #print(binascii.hexlify("=BB", packed_data))
                elif (command == 3):
                        packed_data = pack("=B", command)
                        #print(binascii.hexlify("=B", packed_data))
                else:
                        print("Unauthorized command")
                port.write(packed_data)


def script(port):
	global command
	while(command != 0):
		line = port.read(1)
		if (line != "" and command != 0):
			#i = int(line)
			#i = unpack("=B", line)
			i = ord(line)
			if (i == 1):
				print("USER IS IN LIGHT SLEEP")
				packed_data = pack("=BBH", 2, 255, 5000)
				port.write(packed_data)
				#exit()
                	#print(line[:-1])
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
        #code(conn)
	t1 = threading.Thread(target=script, args=(conn,))
	t2 = threading.Thread(target=code, args=(conn,))
	t1.start()
	t2.start()
	#print("Code complete!")

if __name__ == "__main__":
	main()
