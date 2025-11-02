import serial
import matplotlib.pyplot as plt
from collections import deque

# === SERIAL SETUP ===
ser = serial.Serial('COM7', 9600, timeout=1)

# === PLOT SETUP ===
plt.ion()
fig, ax = plt.subplots()

x_data = deque(maxlen=100)
y_data = deque(maxlen=100)

plot_line, = ax.plot([], [], 'ro-')

ax.set_xlim(-20000, 20000)
ax.set_ylim(-20000, 20000)
ax.set_xlabel('X-axis (Accel)')
ax.set_ylabel('Y-axis (Accel)')
ax.set_title('Real-time Hand Gesture Plot')

gesture = "None"   # Default gesture

print("Reading data... (Ctrl+C to stop)")

while True:
    try:
        line = ser.readline().decode('utf-8').strip()
        if not line:
            continue

        # Check if line contains gesture text
        if line.startswith("Detected Gesture: "):
            gesture = line.split(": ")[1]
            print(f"Gesture Detected: {gesture}")

        # Check if it's raw accelerometer data
        elif ',' in line:
            parts = line.split(',')
            if len(parts) == 2:
                ax_val = int(parts[0])
                ay_val = int(parts[1])

                x_data.append(ax_val)
                y_data.append(ay_val)

                plot_line.set_xdata(x_data)
                plot_line.set_ydata(y_data)

                ax.relim()
                ax.autoscale_view()

                plt.draw()
                plt.pause(0.01)

    except KeyboardInterrupt:
        print("Exiting...")
        break

    except Exception as e:
        print(f"Error: {e}")