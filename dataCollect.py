__author__ = 'YutongGu'
import bluetooth
from threading import Thread
import struct
import time
#!/usr/bin/python
# -*- coding: utf-8 -*-

quit = False
datalist = []
starttime = time.time()


class bluetoothTalker:
    def __init__(self):
        self.connected=False
        self.partnerName=''
        self.partnerAddress=''
        self.port=1
        self.passkey = "1234" 
        self.sock=bluetooth.BluetoothSocket(bluetooth.RFCOMM)
        pass

    def connect(self):
        if(self.connected==False):
            self.promptToConnect()
        else:
            print('Already connected to'+self.partnerName+'-'+self.partnerAddress)

    def send(self, val):
        try:
            print('sent '+ val)
        except:
            print('sent'+ str(val))
        self.sock.send(val)

    def recv(self):
        rxdata = ""
        data = ""
        while "\n" not in rxdata:
            try:
                rxdata = str(self.sock.recv(1024).decode("utf-8"))
                data += rxdata
            except Exception as e:
                print(str(e))
                return None

        return data

    def quit(self):
        if(self.connected==True):
            self.sock.close()

    def promptToConnect(self):
        self.nearby_devices = bluetooth.discover_devices(lookup_names=True)
        numdevices = 0
        i = 0
        while(self.connected==False):
            numdevices = len(self.nearby_devices);
            print("found %d devices" % numdevices)
            if numdevices == 0:
                break

            for addr, name in self.nearby_devices:
                print("%d:  %s - %s" % (i, addr, name))
                i = i+1
            try:
                self.partnerName=self.nearby_devices[int(input("Please select a number device: "))][1]
            except:
                print("Input failed")
                self.partnerName='None'
            self.partnerAddress=''

            if self.partnerName == 'back' or self.partnerName == 'None':
                break

            for bdaddr in self.nearby_devices:
                if( self.partnerName == bluetooth.lookup_name( bdaddr[0] )):
                    self.partnerAddress = bdaddr[0]

                    # kill any "bluetooth-agent" process that is already running
                    # subprocess.call("kill -9 `pidof bluetooth-agent`",shell=True)

                    # Start a new "bluetooth-agent" process where XXXX is the passkey
                    # status = subprocess.call("bluetooth-agent " + self.passkey + " &",shell=True)

                    self.sock.connect((self.partnerAddress,self.port))
                    print('Connected')
                    self.connected=True
                    break
                else:
                    print('Device not found: please try again')

btTalker= bluetoothTalker()

def _recieve_thread():
    global quit
    global datalist
    global btTalker
    
    while(not quit):
        recieveval = btTalker.recv()
        print(str(recieveval))
        if recieveval != None:
            try:
                datalist.append((round(time.time()-starttime, 2),float(recieveval)))
            except Exception as e:
                print("Recieved nothing")


def main():
    global quit
    global datalist 
    global btTalker
    global starttime

    t = Thread(target=_recieve_thread)
    userinput=''
    userinput2=''
    while userinput!='quit':
        try:
            userinput=input('Input: ')
        except Exception as e:
            print(str(e))
            userinput='None'
        if userinput=='connect':
            try:
                btTalker.connect()
                threadstarted = True
                t.start()
            except:
                print("connection failed")
        if userinput=='start':
            if(btTalker.connected):
                userinput2 = ''

                while(userinput2!='back' and userinput2!='None'):
                    try:
                        userinput2=input('sampling delay: ')

                        if(userinput2!='back'):
                            returnval = struct.pack("=BH", 1, int(userinput2))
                            btTalker.send(returnval)
                            starttime = time.time()
                            userinput2 = 'back'
                    except Exception as e:
                        print(str(e))
                        userinput2='None'
            else:
                print('Device is not connected. Please connect then try again')
        if userinput=='stop':
            returnval = struct.pack("=B", 3)
            btTalker.send(returnval)
        if userinput=='data':
            print(datalist)
        if userinput=='save':
            try:
                file_name = input("File name:")
                file_name = file_name + ".csv"
                file = open(file_name, 'w')
                for data in datalist:
                    file.write(str(data[0])+','+str(data[1])+'\n')
                file.close()
            except Exception as e:
                print(str(e))
        if userinput == 'None':
            break
    quit = True

if __name__ == '__main__':
    main()