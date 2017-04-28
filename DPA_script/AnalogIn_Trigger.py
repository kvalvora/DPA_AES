"""
   DWF Python Example 5
   Author:  Digilent, Inc.
   Revision: 10/17/2013

   Requires:                       
       Python 2.7, numpy, matplotlib
       python-dateutil, pyparsing
"""
from ctypes import *
from dwfconstants import *
import math
import time
import matplotlib.pyplot as plt
import sys
import serial

#setup serial
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

#write in a file
f1 = None
ser = 0
init_serial()

file_name = time.strftime('%Y%m%d-%H%M%S') + '-ptxt_list.txt'
f1 = open(file_name,'w')


if sys.platform.startswith("win"):
    dwf = cdll.dwf
elif sys.platform.startswith("darwin"):
    dwf = cdll.LoadLibrary("/Library/Frameworks/dwf.framework/dwf")
else:
    dwf = cdll.LoadLibrary("libdwf.so")

#declare ctype variables
hdwf = c_int()
sts = c_byte()
rgdSamples = (c_double*2048)()

pnt, l = [0.0]*len((c_double*2048)()),10 ;
rgpy = [[0.0000 for x in pnt] for y in range(l)] 
#rgpy=[0.0]*len(rgdSamples)


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

print "Preparing to read sample..."

#set up acquisition
dwf.FDwfAnalogInFrequencySet(hdwf, c_double(50000000.0))
dwf.FDwfAnalogInBufferSizeSet(hdwf, c_int(2048)) 
dwf.FDwfAnalogInChannelEnableSet(hdwf, c_int(0), c_bool(True))
dwf.FDwfAnalogInChannelRangeSet(hdwf, c_int(0), c_double(5))

#set up trigger
dwf.FDwfAnalogInTriggerAutoTimeoutSet(hdwf, c_double(0)) #disable auto trigger
dwf.FDwfAnalogInTriggerSourceSet(hdwf, trigsrcDetectorAnalogIn) #one of the analog in channels
dwf.FDwfAnalogInTriggerTypeSet(hdwf, trigtypeEdge)
dwf.FDwfAnalogInTriggerChannelSet(hdwf, c_int(0)) # first channel
dwf.FDwfAnalogInTriggerLevelSet(hdwf, c_double(1.5)) # 1.5V
dwf.FDwfAnalogInTriggerConditionSet(hdwf, trigcondRisingPositive) 
dwf.FDwfAnalogInTriggerPositionSet(hdwf, c_double(0.0001)) #0.000015
# wait at least 2 seconds with Analog Discovery for the offset to stabilize, before the first reading after device open or offset/range change
time.sleep(2)

print "   starting repeated acquisitions"

# begin acquisition
dwf.FDwfAnalogInConfigure(hdwf, c_bool(False), c_bool(True))

f2=open("power_traces.csv","w")


    
for i in range(10):

    serstr = ser.readline()
    serstr = serstr.rstrip("\n\r")
    
    #print serstr
    f1.write('%s\n' % serstr)
    while True:
        dwf.FDwfAnalogInStatus(hdwf, c_int(0), byref(sts))
        if sts.value == DwfStateDone.value :
            break
        time.sleep(0.001)

    dwf.FDwfAnalogInStatusData(hdwf, 1, rgdSamples, 2048)
    point = 0
            
    for v in rgdSamples:
        f2.write("%s\n" % v)
        rgpy[i][point]=v
        point += 1

print '\nTrace collection finished.\n\n'    
dwf.FDwfDeviceCloseAll()
f1.close()
f2.close()
ser.close()

time = list(range(8192))        


plt.plot(rgpy[0])        
#plt.plot(rgpy[0],'c',rgpy[1],'b',rgpy[2],'r',rgpy[3],'g',rgpy[4],'k',rgpy[5],'y',rgpy[6],'m')
plt.show()


