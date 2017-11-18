from __future__ import print_function
import sys
import re
from time import sleep, time
from struct import *
from serial import Serial
import os
import math
import binascii

def code(port):
	################# Old Code #######################
	#x = raw_input("Enter (s) to start or (q) quit: ")
	#while(x == 's'):
		#port.write(x);
		#line = port.readline();
		#print(line)
		#x = raw_input("Enter a number (0 - 9): ")
	##################################################

	packed_data = ""
	cmd = input("Enter command (1, 2, or 3): ")
	if (cmd == 1):
		IF = input("Enter intersampling frequency: ")
		IT = input("Enter intersampling time: ")
		SF = input("Enter sampling frequency: ")
		packed_data = pack("=BBHB", cmd, IF, IT, SF)
		#print(binascii.hexlify(packed_data))
	elif (cmd == 2):
		brightness = input("Enter brightness: ")
		ramp_up = input("Enter ramp up: ")
		packed_data = pack("=BBB", cmd, brightness, ramp_up)
		#print(binascii.hexlify("=BB", packed_data))
	elif (cmd == 3):
		packed_data = pack("=B", cmd)
		#print(binascii.hexlify("=B", packed_data))
	else:
		print("Unauthorized command")
	port.write(packed_data)
	line = port.readline()
	print(line)

def connect():
	try:
		conn = Serial('/dev/tty.usbmodem1411', dsrdtr=0, rtscts=0, timeout=1)
	except IOError:
		print("Error opening serial port.", file=sys.stderr)
		sys.exit(2)

	return conn


def main():
    print("Connecting...")
    conn = connect()
    print("Connected!")
    
    print("Running code...")
    code(conn)
    print("Code complete!")

if __name__ == "__main__":
    main()
