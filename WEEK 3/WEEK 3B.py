import serial

import matplotlib.pyplot as plt

import numpy as np

import time

# --- CONFIGURATION ---

SERIAL_PORT = 'COM7'  # ⚠️ Change this if needed (e.g., 'COM3', '/dev/ttyACM0')

BAUD_RATE = 9600

# --- SERIAL INITIALIZATION ---

ser = None

try:

    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

    print(f"✅ Connected to {SERIAL_PORT} at {BAUD_RATE} baud.")

    time.sleep(2)  # Allow Arduino to reset

    ser.flushInput()  # Clear any leftover data

except serial.SerialException as e:

    print("\n--- SERIAL ERROR ---")

    print(f"Error: {e}")

    print("Check:")

    print("1️⃣ Correct port name (Tools → Port in Arduino IDE)")

    print("2️⃣ Arduino Serial Monitor is CLOSED")

    print("3️⃣ Baud rate matches Serial.begin() in Arduino code")

    print("--------------------\n")

    exit()

# --- PLOTTING INITIALIZATION ---

plt.ion()

fig, ax = plt.subplots(figsize=(10, 6))

fig.canvas.manager.set_window_title('Real-Time Potentiometer Data')

ax.set_title('Real-Time Potentiometer Value (0–1023)')

ax.set_xlabel('Time (s)')

ax.set_ylabel('Potentiometer Value')

ax.set_ylim(0, 1023)

ax.grid(True)

xpoints = []

ypoints = []

print("\n--- Starting Data Acquisition ---")

print("Adjust the potentiometer now. Press Ctrl+C to stop.\n")

# --- MAIN LOOP ---

try:

    while True:

        try:

            line = ser.readline().decode(errors='ignore').strip()

            if not line:
                continue

            pot_value = int(line)  # Convert to integer

            print("POT Value:", pot_value)

            xpoints.append(time.time())

            ypoints.append(pot_value)

            # Keep only the last 100 readings

            if len(xpoints) > 100:
                xpoints = xpoints[-100:]

                ypoints = ypoints[-100:]

            # --- PLOT UPDATE ---

            ax.clear()

            ax.plot(xpoints, ypoints, 'b-', linewidth=2)

            ax.set_title('Real-Time Potentiometer Value (0–1023)')

            ax.set_xlabel('Time (s)')

            ax.set_ylabel('Potentiometer Value')

            ax.set_ylim(0, 1023)

            ax.grid(True)

            fig.canvas.draw()

            fig.canvas.flush_events()

            plt.pause(0.05)

        except ValueError:

            print(f"Invalid pot value received: {line}")

except KeyboardInterrupt:

    print("\n\n--- Ctrl+C pressed ---")

    if ser and ser.is_open:
        print("Sending 'q' to Arduino to stop...")

        ser.write(b'q')

        time.sleep(0.5)

finally:

    if ser and ser.is_open:
        ser.close()

        print("Serial connection closed.")

    plt.ioff()

    if ypoints:

        plt.show()

    else:

        print("No data collected to plot.")
