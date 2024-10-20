import serial as ser
import time

def main():
    s = ser.Serial('COM3', baudrate=9600, bytesize=ser.EIGHTBITS,
                   parity=ser.PARITY_NONE, stopbits=ser.STOPBITS_ONE,
                   timeout=1)  # timeout of 1 sec so that the read and write operations are blocking,s

    # clear buffers
    s.reset_input_buffer()
    s.reset_output_buffer()

    menu = ["                                 Menu             ",
            "1. Blink RGB LED, color by color with delay of X[ms]",
            "2. Counting up on to LCD screen with delay of X[ms]",
            "3. Circular tone series via Buzzer with delay of X[ms]",
            "4. Get delay time X[ms]:",
            "5. Potentiometer 3-digit value [v] onto LCD",
            "6. Clear LCD screen",
            "7. On each PB1 pressed, send a Massage 'I love my Negev'",
            "8. Show menu",
            "9. Sleep"]

    for i in menu:
        print(i)

    enableTX = True

    pos4 = 0
    while 1:
#        while (s.in_waiting > 0):
#            lineByte = s.read_until(terminator='\n')
#            print(lineByte.decode("ascii"))
#            time.sleep(0.25)
        while s.out_waiting > 0 or enableTX :
            while True:
                inChar = input("Enter char:")
                if inChar is not '':
                    break
            bytesChar = bytes(inChar , 'ascii')
            s.write(bytesChar)
            if inChar[0] == '8':
                while s.in_waiting <= 0:
                    time.sleep(0.25)  # delay for accurate read/write operations on both ends
                readChar = s.read(1)
                if readChar == b'8':
                    for i in menu:
                        print(i)
            if inChar[0] == '4':
                while True:
                    inStr = input("Enter time in ms:")
                    if inStr is not '':
                        break
                byteStr = bytes(inStr + '\n' , 'ascii')
                s.write(byteStr)
                time.sleep(0.25)  # delay for accurate read/write operations on both ends
            if inChar[0] == '7':
                while s.in_waiting <= 0:
                    time.sleep(0.25)  # delay for accurate read/write operations on both ends
                readChar = s.read_until()
                print(readChar.decode("ascii"))
                # while 1:
                #     while s.in_waiting <= 0:
                #         readChar = s.read(1)
                #         if readChar == b'\n':
                #             break
                #         print(readChar.decode("ascii"),end='')
                time.sleep(0.25)


if __name__ == '__main__':
    main()