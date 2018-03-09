import serial 
import numpy as np   
ser = serial.Serial('/dev/ttyUSB0','9600')    
Scales = [10,50,200,500]

def revise(ur):
    f = open('config.py','r')
    lines = f.readlines()
    print lines
    f = open('config.py','w')
    lines[0] = 'unit = ' + str(ur) + '\n'
    f.writelines(lines)

def w_regression(scale):
    r=0
    w = []
    data = ''
    print 'Please put {}g on the platform and press enter'.format(scale)
    raw_input()
    print 'calculating...'
    while len(w) < 21:
        n = ser.inWaiting()  
        str = ser.read(n)  
        if str:  
            data += str
            if len(data) == 13 and data[1:3] == '0x' and check(data):
                if data[0] == '0':    
                    w.append(int(data[1:11],16))
                else:
                    w.append(int(data[1:11],16)*-1)
        data = ''
    r = np.mean(w)/scale
    return r

def check(data):
    temp = int(data[3:5],16)
    for i in [5,7,9,11]:
        temp = temp^int(data[i:i+2],16)
    if temp:
        return 0
    else:
        return 1

if __name__ == '__main__':
    r = []
    for scale in Scales:
        r.append(w_regression(scale))
    unit = np.mean(r)
    revise(unit)
    print 'done!'

