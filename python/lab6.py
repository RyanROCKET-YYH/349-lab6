#!/usr/bin/env python3

from datetime import datetime
import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import collections
import re

# Open serial port
ser = serial.Serial('/dev/cu.usbmodem145303', 115200)

data = collections.deque(maxlen=500)
pattern = re.compile(r"Motor_position = (\d+)")

def animate(i):
    # If there's data available in the serial buffer
    while ser.in_waiting:
        try:
            line = ser.readline().decode('utf-8').strip()
            match = pattern.search(line)
            if match:
                # Convert the first group to integer and append to data
                position = int(match.group(1))
                data.append(position)
        except ValueError:
            continue

    ax.clear()
    ax.plot(data)
    plt.title('Motor Position Over Time')
    plt.ylabel('Motor Position')

# Set up plot to call animate() function periodically
fig, ax = plt.subplots()
ani = FuncAnimation(fig, animate, interval=100)
plt.tight_layout()
plt.show()

def main():
    # Log activity to a file
    # Open log file
    log = open('./lab6log.txt', 'w')

    # Set up plot to call animate() function periodically
    fig, ax = plt.subplots()
    ani = FuncAnimation(fig, animate, fargs=(log,), interval=100)
    plt.tight_layout()
    plt.show()

    # Cleanup
    log.close()
    ser.close()

if __name__ == '__main__':
    main()
