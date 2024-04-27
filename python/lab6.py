#!/usr/bin/env python3

from datetime import datetime
import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import collections
import re

# Open serial port
ser = serial.Serial('/dev/cu.usbmodem145303', 115200)

# Store tuples of (timestamp, motor_position)
data = collections.deque(maxlen=500)
motor_pattern = re.compile(r"Motor_position = (\d+)")
target_pattern = re.compile(r"Target_position = (\d+)")

target_position = None  # Initialize target position

def animate(i):
    global target_position
    # If there's data available in the serial buffer
    while ser.in_waiting:
        try:
            line = ser.readline().decode('utf-8').strip()
            motor_match = motor_pattern.search(line)
            target_match = target_pattern.search(line)
            
            if motor_match:
                position = int(motor_match.group(1))
                timestamp = datetime.now().timestamp() * 1000  # Convert to milliseconds
                data.append((timestamp, position))
            
            if target_match:
                # Update target position
                target_position = int(target_match.group(1))
                
        except ValueError:
            continue

    ax.clear()
    if data:
        timestamps, positions = zip(*data)
        ax.plot(timestamps, positions, label='Motor Position')
        if target_position is not None:
            ax.axhline(y=target_position, color='r', label='Target Position')
        
        latest_time = data[-1][0]  # Latest timestamp in data
        ax.set_xlim(latest_time - 1000, latest_time)

    # Set fixed y-axis range
    ax.set_ylim(0, 1200)
    
    plt.xlabel('Timestamp (ms)')
    plt.ylabel('Motor Position')
    plt.legend()

# Set up plot to call animate() function periodically
fig, ax = plt.subplots()
ani = FuncAnimation(fig, animate, interval=10)
plt.tight_layout()
plt.show()
ser.close()