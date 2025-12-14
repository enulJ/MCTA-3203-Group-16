import serial
import time
import matplotlib.pyplot as plt
from collections import deque

# ===== SERIAL CONNECTION =====
arduino_port = 'COM7'      # CHANGE THIS
baud_rate = 9600
ser = serial.Serial(arduino_port, baud_rate)
time.sleep(2)

# ===== DATA STORAGE =====
max_points = 60
r_data = deque(maxlen=max_points)
g_data = deque(maxlen=max_points)
b_data = deque(maxlen=max_points)

# ===== PLOT SETUP =====
plt.ion()
fig, ax = plt.subplots()

line_r, = ax.plot([], [], label="Red")
line_g, = ax.plot([], [], label="Green")
line_b, = ax.plot([], [], label="Blue")

ax.set_title("Real-Time RGB Color Sensor Plot")
ax.set_xlabel("Sample Number")
ax.set_ylabel("Frequency Output")
ax.legend()
ax.grid(True)

print("Connected. Reading RGB data...")

# ===== MAIN LOOP =====
while True:
    try:
        line = ser.readline().decode().strip()

        if line:
            print("Received:", line)

            parts = line.replace("=", "").split()
            r = int(parts[1])
            g = int(parts[3])
            b = int(parts[5])

            r_data.append(r)
            g_data.append(g)
            b_data.append(b)

            x = range(len(r_data))

            line_r.set_data(x, r_data)
            line_g.set_data(x, g_data)
            line_b.set_data(x, b_data)

            ax.relim()
            ax.autoscale_view()

            plt.pause(0.05)

    except KeyboardInterrupt:
        print("Stopped by user")
        break

ser.close()
plt.ioff()
plt.show()
