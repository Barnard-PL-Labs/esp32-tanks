import sys
import serial
from time import sleep

# Check if a message was provided
if len(sys.argv) != 2:
    print("Usage: python script.py 'message'")
    sys.exit(1)

message = sys.argv[1] + '\n'  # Add newline character to ensure ESP32 recognizes end of message

try:
    # Replace '/dev/ttyUSB0' with your ESP32's serial port
    with serial.Serial('/dev/cu.usbserial-54F40080041', 115200, timeout=1) as ser:
        sleep(2)  # Wait for serial connection to initialize
        ser.write(message.encode())  # Send message to ESP32
        echo = ser.readline().decode().strip()  # Read echoed message
        print(f"Echoed: {echo}")
except serial.SerialException as e:
    print(f"Error: {e}")
