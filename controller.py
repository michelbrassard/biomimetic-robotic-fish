# https://shallowsky.com/blog/programming/python-read-characters.html

import serial
import sys, os
import termios, fcntl
import select

fd = sys.stdin.fileno()
newattr = termios.tcgetattr(fd)
newattr[3] = newattr[3] & ~termios.ICANON
newattr[3] = newattr[3] & ~termios.ECHO
termios.tcsetattr(fd, termios.TCSANOW, newattr)

oldterm = termios.tcgetattr(fd)
oldflags = fcntl.fcntl(fd, fcntl.F_GETFL)
fcntl.fcntl(fd, fcntl.F_SETFL, oldflags | os.O_NONBLOCK)

def write_serial(port, baudrate):
    with serial.Serial(port, baudrate, timeout = 1) as ser:
        try:
            print("Control the fish")
            while True:
                select.select([sys.stdin], [], [])
                character = sys.stdin.read()
                if character == 'q':
                    break
                print("-", character)
                ser.write(bytes(character, 'ascii'))
            termios.tcsetattr(fd, termios.TCSAFLUSH, oldterm)
            fcntl.fcntl(fd, fcntl.F_SETFL, oldflags)
        except serial.SerialException as e:
            print(e)
        except KeyboardInterrupt:
            print("Program stopped!")

if __name__ == '__main__':
    write_serial('/dev/cu.usbserial-0001', 9600)