This program reads weight data using 80C52RC MCU and transfer it to computer by serial

How to install dependencies:

1.C code compiler for 51 MCU

sudo apt-get install sdcc

2.python serial

sudo pip install pyserial

How to use:

1.compile serial_weight.c 

sdcc serial_weight.c

Then you will find many files created
serial_weight.ihx is the target file that should be written into 51 MCU

2.Write .ihx to MCU

sudo python stcflash.py serial_weight.ihx

unplug vcc pin and plug it again to make MCU restart
so that the program can be written in MCU

3.Receive data

sudo python readserial.py

4.modify weight coefficient

sudo python unit_regression.py

follow the guid and the result would be saved as config.py

5.Reset MCU

sudo python writeserial.py

it will send '1' to serial. MCU recieve '1' and reset

'1' could be modified arbitrarily, as long as it corresponds to
that in MCU program.

Introduction of each file:

1.config.py

weight coefficient.

2.readserial.py

python program to read data from serial

run ls /dev/ttyUSB* to check serial and modify it in program
the default serial would be /dev/ttyUSB0

3.serial_weight.c

51 MCU read weight data from hx711

4.serial_weight_tem.c

51 MCU read weight data from hx711 and temperature data from DS18B20

5.stcflash.py

write .ihx file into 51 MCU

6.unit_regression.py

modify weight coefficient

7.writeserial.py

python program to send data to serial

