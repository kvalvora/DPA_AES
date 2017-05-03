"""
   DWF Python Example 5
   Author:  Digilent, Inc.
   Revision: 10/17/2013

   Requires:                       
       Python 2.7, numpy, matplotlib
       python-dateutil, pyparsing
"""
import math

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
    
ser = 0
init_serial()    
    
# PARAMETERS
num_traces = 100
f1=open("plaintext_list_LeCroy.csv","wb")
writer1 = csv.writer(f1)

for i in tqdm(range(num_traces)):
    # new acquisition is started automatically after done state 

    #Send hello to Nucleo
    ser.write('hello' + '\r')  
    serstr = ser.readline() 
    serstr = serstr.rstrip() 
    serstr = serstr.replace('\r','')
    writer1.writerow(serstr.split(' '))
    time.sleep(0.000480)

print 'Collection finished.\n\n'
ser.close()
f1.close()

