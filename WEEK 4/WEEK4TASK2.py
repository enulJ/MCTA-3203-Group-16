import serial
import time

# === CONFIG ===
RFID_PORT = "COM12"    # RFID reader COM port
ARDUINO_PORT = "COM7"  # Arduino COM port
BAUD_RATE = 9600       # Common baud rate for both

def read_rfid_uid(ser):
    """
    Reads a single UID packet from the RFID-IDR-232N.
    The reader sends 12 bytes: [0x01][10 ASCII UID][0x02]
    """
    if ser.in_waiting >= 12:
        data = ser.read(12)
        if data[0] == 0x01 and data[-1] == 0x02:
            uid = data[1:11].decode('ascii', errors='ignore')
            return uid.strip()
    return None

def main():
    try:
        # Open RFID reader serial
        ser = serial.Serial(RFID_PORT, BAUD_RATE, timeout=1)
        time.sleep(1)

        # Open Arduino serial
        arduino = serial.Serial(ARDUINO_PORT, BAUD_RATE, timeout=1)
        time.sleep(2)  # Allow Arduino to reset
        print(f"[+] Connected to RFID reader on {RFID_PORT}")
        print(f"[+] Connected to Arduino on {ARDUINO_PORT}")
        print("[System] Waiting for RFID tag...")

        while True:
            uid = read_rfid_uid(ser)
            if uid:
                print(f"[RFID] Tag detected: {uid}")
                arduino.write((uid + '\n').encode())
                print("[→] UID sent to Arduino. Waiting for response...")

                # Wait up to 10 seconds for Arduino reply
                response = ""
                start_time = time.time()
                while time.time() - start_time < 10:
                    if arduino.in_waiting > 0:
                        response = arduino.readline().decode(errors='ignore').strip()
                        if response:
                            print(f"[Arduino Response] {response}")
                            break
                    time.sleep(0.1)

                # Evaluate response
                if response == "A":
                    print("[✅] Motion detected — Access GRANTED.")
                elif response == "D":
                    print("[❌] Access DENIED (no motion or unauthorized UID).")
                else:
                    print("[!] No valid response from Arduino.")

                print("[System] Ready for next RFID tag...\n")
                time.sleep(1)  # Small delay before next read

    except KeyboardInterrupt:
        print("\n[!] Stopping program...")
    except Exception as e:
        print(f"[!] Error: {e}")
    finally:
        try:
            ser.close()
        except:
            pass
        try:
            arduino.close()
        except:
            pass

if __name__ == "__main__":
    main()
