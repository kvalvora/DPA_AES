"""
   Trace Collection
   Vernam Lab - WPI
   Authors: Abraham Fernandez-Rubio and Kewal Vora   
   Based on Digilent, Inc. Analog Discovery 2 python SDK examples.

   Requires:                       
       Python 2.7, numpy, matplotlib
       python-dateutil, pyparsing, pyserial
"""
from ctypes import *
from dwfconstants import *
import math
import time
import matplotlib.pyplot as plt
import sys

import serial
import time
import uuid 

import csv
from tqdm import *

def init_serial():
    global ser          #Must be declared in Each Function
    ser = serial.Serial()
    ser.baudrate = 9600
    ser.port = 'COM4'
    #ser.port = '/dev/ttyACM0' 			#If Using Linux
    #ser.port = '/dev/tty.usbmodem1423' #If Using Mac
    
	#Specify the TimeOut in seconds
    ser.timeout = 15
    ser.open()  
    
    
if sys.platform.startswith("win"):
    dwf = cdll.dwf
elif sys.platform.startswith("darwin"):
    dwf = cdll.LoadLibrary("/Library/Frameworks/dwf.framework/dwf")
else:
    dwf = cdll.LoadLibrary("libdwf.so")

    
ser = 0
init_serial()    
    
#declare ctype variables
hdwf = c_int()
sts = c_byte()


# PARAMETERS
num_traces = 5000
# file_name = time.strftime('%Y%m%d-%H%M%S') + '-ptxt.csv'
# f1 = open(file_name,'wb')
f2=open("power_traces.csv","wb")
f1=open("plaintext_list.csv","wb")


sample_list = list()
for l in range(num_traces):
    sample_list.append( (c_double*8192)() )

#print DWF version
version = create_string_buffer(16)
dwf.FDwfGetVersion(version)
print "DWF Version: "+version.value

#open device
print "Opening first device"
dwf.FDwfDeviceOpen(c_int(-1), byref(hdwf))

if hdwf.value == hdwfNone.value:
    szError = create_string_buffer(512)
    dwf.FDwfGetLastErrorMsg(szError);
    print "failed to open device\n"+szError.value
    quit()

print "Preparing to capture traces..."

#set up acquisition
dwf.FDwfAnalogInFrequencySet(hdwf, c_double(100000000.0))
dwf.FDwfAnalogInBufferSizeSet(hdwf, c_int(8192)) 

# Enable analog in channels
dwf.FDwfAnalogInChannelEnableSet(hdwf, c_int(0), c_bool(True))
dwf.FDwfAnalogInChannelEnableSet(hdwf, c_int(1), c_bool(True))

# Configure the range of each channel
dwf.FDwfAnalogInChannelRangeSet(hdwf, c_int(0), c_double(5))
dwf.FDwfAnalogInChannelRangeSet(hdwf, c_int(1), c_double(0.05))

#set up trigger
dwf.FDwfAnalogInTriggerAutoTimeoutSet(hdwf, c_double(0)) #disable auto trigger
dwf.FDwfAnalogInTriggerSourceSet(hdwf, trigsrcDetectorAnalogIn) #one of the analog in channels
dwf.FDwfAnalogInTriggerTypeSet(hdwf, trigtypeEdge)
dwf.FDwfAnalogInTriggerChannelSet(hdwf, c_int(0)) # first channel
dwf.FDwfAnalogInTriggerLevelSet(hdwf, c_double(1.5)) # 1.5V
dwf.FDwfAnalogInTriggerConditionSet(hdwf, trigcondFallingNegative) 
dwf.FDwfAnalogInTriggerPositionSet(hdwf, c_double(0.000043)) #0.000015

# wait at least 2 seconds with Analog Discovery for the offset to stabilize, before the first reading after device open or offset/range change
time.sleep(2)
print "Starting repeated acquisitions...\n"

writer1 = csv.writer(f1)
writer2 = csv.writer(f2)

# begin acquisition
dwf.FDwfAnalogInConfigure(hdwf, c_bool(False), c_bool(True))

for i in tqdm(range(num_traces)):
    # new acquisition is started automatically after done state 

    #Send hello to Nucleo
    ser.write('hello' + '\r')  
    serstr = ser.readline() 
    serstr = serstr.rstrip() 
    serstr = serstr.replace('\r','')
    
    while True:
        dwf.FDwfAnalogInStatus(hdwf, c_int(1), byref(sts))        
        if sts.value == DwfStateDone.value :
            dwf.FDwfAnalogInStatusData(hdwf, 1, sample_list[i], 8192)            
            break
        
    writer1.writerow(serstr.split(' '))

    
power_list = list()
for l in range(num_traces):
    power_list.append([0.0]*len(sample_list[0]))
    power_list[l]=sample_list[l]
    writer2.writerow(power_list[l])


#plt.plot(power_list[0])
# plt.plot(power_list[0],'c',power_list[1],'r',power_list[2],'k')
#plt.show()

print 'Collection finished.\n\n'
ser.close()
f1.close()
f2.close()    
dwf.FDwfDeviceCloseAll()


