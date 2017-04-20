import serial
import time
import uuid 
#Can be Downloaded from this Link
#https://pypi.python.org/pypi/pyserial

#Function to Initialize the Serial Port
def init_serial():
    #COMNUM = 4          #Enter Your COM Port Number Here.
    global ser          #Must be declared in Each Function
    ser = serial.Serial()
    ser.baudrate = 57600
    #ser.port = COMNUM - 1   #COM Port Name Start from 0
    ser.port = 'COM7'
    #ser.port = '/dev/ttyACM0' #If Using Linux
    #ser.port = '/dev/tty.usbmodem1423' 
    #Specify the TimeOut in seconds, so that SerialPort
    #Doesn't hangs
    ser.timeout = 100
    ser.open()          #Opens SerialPort

    # print port open or closed
    #if ser.isOpen():
    #    print 'Open: ' + ser.portstr


	
ser = 0
init_serial()

    
for seq_num in range(10):       
 

    rand_pat = uuid.uuid4().hex
    print 'Triggering pulse...\n\r'  #nibbles from 18 to 32 seem to be different from every call   
       
    #Send something to Nucleo
    ser.write(rand_pat[18:32] + '\r')
    serstr = ser.readline() 

    while ( serstr.find("byebye") == -1):    
        serstr = ser.readline()  #Read from Serial Port
        serstr = serstr.rstrip("\n\r")
        print serstr
    
    print 'Pulse done.'
    #time.sleep(1)

ser.close()
