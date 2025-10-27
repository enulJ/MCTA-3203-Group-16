import serial
import time

# -- IMPORTANT: Replace 'COMx' with your Arduino's serial port (e.g., 'COM3' on Windows, '/dev/ttyACM0' on Linux, or '/dev/cu.usbmodemXXXX' on Mac)
# Ensure the baud rate matches the Arduino sketch (9600)
SERIAL_PORT = 'COM3'
BAUD_RATE = 9600
THRESHOLD = 511  # Half of the maximum 10-bit value (1023 / 2 = 511.5)

try:
    # Initialize the serial connection
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    time.sleep(2)  # Wait for the serial connection to initialize
    print(f"Serial connection established on {SERIAL_PORT} at {BAUD_RATE} baud.")

    while True:
        # Read a line from the Arduino
        if ser.in_waiting > 0:
            try:
                # Read the line, decode it to a string, and remove whitespace/newline characters
                line = ser.readline().decode('utf-8').strip()

                # Try to convert the reading to an integer
                pot_value = int(line)

                print("Potentiometer Value:", pot_value)

                # Control Logic: Check if the value exceeds the threshold
                if pot_value > THRESHOLD:
                    # Send '1' to Arduino to turn the LED ON
                    ser.write(b'1')
                    print("--> LED command: ON")
                else:
                    # Send '0' to Arduino to turn the LED OFF
                    ser.write(b'0')
                    print("--> LED command: OFF")

            except ValueError:
                # Handle cases where the line read is not a valid integer
                # This can happen if the serial data is interrupted or corrupted
                print(f"Skipping non-integer reading: {line}")

except serial.SerialException as e:
    print(
        f"ERROR: Could not open serial port {SERIAL_PORT}. Please check the port name and ensure the Arduino IDE Serial Monitor/Plotter is closed.")
    print(f"Details: {e}")

except KeyboardInterrupt:
    print("\nKeyboard interrupt detected. Closing serial connection.")
finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()
        print("Serial connection closed.")