import serial #instal pyserial if this cant be found
import warnings #will send out problems if issues connecting to Arduino
import serial.tools.list_ports #lists the ports on the computer
import time #to use sleep function

'''
'' initArduino creates a serial connection with the arduino
'' please coneect the arduino that YOU want to connect to this python code FIRST
'' to the computer before connecting other arduinos
'''
def initArduino():
    arduino_ports = [
        p.device
        for p in serial.tools.list_ports.comports()
        if 'Arduino' in p.description
    ]

    if not arduino_ports:
        raise IOError("No Arduino is connected")
    if len(arduino_ports) > 1:
        warnings.warn('More than one Arduino found -- using the first port')

    global ser
    ser = serial.Serial(arduino_ports[0])

'''
'' Below function will continue to add values from commandline
'''
def addValuesFromCommandLine():
    exit = 0;
    while(~exit):
        commandinput = input('Enter Value or press \'q\' to quit\n')
        if (commandinput == 'q'):
            exit = ~exit
        else:
            ser.write(str.encode(commandinput))
        print('\n')

'''
'' Will add cordinates to the arduino
'' CURRENT DESIGN addValue('XY') where X is 1-8 and Y 1-8 related to cordinates
'''
def addValue(cordinates):
    #print(cordinates)
    ser.write(str.encode(cordinates))
    time.sleep(1)

def exitserial():
    ser.close()

def main():
    initArduino()
    time.sleep(3) #IMPORTANT DELAY FOR serial connection to stablize
    #addValue('11')
    #addValue('00')
    #addValue('11')
    #addValue('11')
    #addValue('11')
    addValuesFromCommandLine()
    exitserial()

main()


'''for Kathleen:
1) import the top statements
2) run initArduino() with time.sleep(3) of delay before calling
3) various combinations of addValue('XY') depending on what is said
4) X will be turning ON Led of number X and Y will be turning off LED of
   number y
5) ie addValue('12') will turn on led 1 and turn off led 2.

'''
