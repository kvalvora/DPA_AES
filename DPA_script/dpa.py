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

if sys.platform.startswith("win"):
    dwf = cdll.dwf
elif sys.platform.startswith("darwin"):
    dwf = cdll.LoadLibrary("/Library/Frameworks/dwf.framework/dwf")
else:
    dwf = cdll.LoadLibrary("libdwf.so")

#declare ctype variables
hdwf = c_int()
sts = c_byte()

sample_list = list()
for l in range(3):
    sample_list.append( (c_double*2048)() )

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
dwf.FDwfAnalogInFrequencySet(hdwf, c_double(100000000.0))
dwf.FDwfAnalogInBufferSizeSet(hdwf, c_int(2048)) 
dwf.FDwfAnalogInChannelEnableSet(hdwf, c_int(0), c_bool(True))
dwf.FDwfAnalogInChannelRangeSet(hdwf, c_int(0), c_double(5))

#set up trigger
dwf.FDwfAnalogInTriggerAutoTimeoutSet(hdwf, c_double(0)) #disable auto trigger
dwf.FDwfAnalogInTriggerSourceSet(hdwf, trigsrcDetectorAnalogIn) #one of the analog in channels
dwf.FDwfAnalogInTriggerTypeSet(hdwf, trigtypeEdge)
dwf.FDwfAnalogInTriggerChannelSet(hdwf, c_int(0)) # first channel
dwf.FDwfAnalogInTriggerLevelSet(hdwf, c_double(1.5)) # 1.5V
dwf.FDwfAnalogInTriggerConditionSet(hdwf, trigcondFallingNegative) 
dwf.FDwfAnalogInTriggerPositionSet(hdwf, c_double(0.000011)) #0.000015

# wait at least 2 seconds with Analog Discovery for the offset to stabilize, before the first reading after device open or offset/range change
time.sleep(2)

print "   starting repeated acquisitions"

f2=open("power_traces.csv","w")


# begin acquisition
dwf.FDwfAnalogInConfigure(hdwf, c_bool(False), c_bool(True))

for i in range(3):
    # new acquisition is started automatically after done state 

    while True:
        dwf.FDwfAnalogInStatus(hdwf, c_int(1), byref(sts))
        if sts.value == DwfStateDone.value :
            dwf.FDwfAnalogInStatusData(hdwf, 1, sample_list[i], 2048)
            break
    

    
rgpy=[0.0]*len(sample_list[0])

power_list = list()
for l in range(3):
    print len(sample_list[l])
    power_list.append([0.0]*len(sample_list[0]))

    for pnt in range(0,len(power_list[l])):
        power_list[l][pnt]=sample_list[l][pnt]
        f2.write("%f\n" % sample_list[l][pnt])



#plt.plot(power_list[0])
plt.plot(power_list[0],'c',power_list[1],'r',power_list[2],'k')
plt.show()
f2.close()    
dwf.FDwfDeviceCloseAll()


