#!/usr/bin/env python3

from datetime import datetime
import serial
import matplotlib.pyplot as plt


def main():
    # Open serial port
    ser = serial.Serial('/dev/ttyACM0', 115200)

    # Send some AT commands
    ser.write(b'+++')
    ser.write(b'AT+HELLO=349\n')
    ser.write(b'AT+PASSCODE=1234\n')
    ser.write(b'AT+RESUME\n')

    # Log activity to a file
    log = open('./lab6log.txt', 'w')
    while (True):
        line = ser.readline()
        print(line)
        timestr = datetime.now().strftime('%m/%d/%Y, %H:%M:%S')
        if line.startswith(b'Unlock'):
            log.write('Unlocked ' + timestr + '\n')
        elif line.startswith(b'Lock'):
            log.write('Locked ' + timestr + '\n')

    # Cleanup
    log.close()
    ser.close()


# data = []
# try:
#     while True:
#         if ser.in_waiting > 0:
#             line = ser.readline().decode('utf-8').strip()
#             print(line)
#             if line.startswith('Encoder Position:'):
#                 value = int(line.split(':')[1].strip())
#                 data.append(value)
#                 plt.plot(data)
#                 plt.pause(0.05)
# finally:
#     ser.close()
#     plt.show()

if __name__ == '__main__':
    main()
