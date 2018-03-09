import serial    
ser = serial.Serial('/dev/ttyUSB0','9600')    

def check(data):
    temp = int(data[3:5],16)
    for i in [5,7,9,11]:
        temp = temp^int(data[i:i+2],16)
    if temp:
        return 0
    else:
        return 1
    
data = ''
while 1:  
    n = ser.inWaiting()  
    str = ser.read(n)  
    if str:  
        data += str
        if len(data) == 13:
            if data[1:3] == '0x' and check(data):
                if data[0] == '0':    
                    print int(data[1:11],16),data
                else:
                    print '-{}'.format(int(data[1:11],16)),data
            data = ''

