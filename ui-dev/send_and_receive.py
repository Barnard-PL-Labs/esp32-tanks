'''
Built on the previous example arduino_echo.py, this script sends a message to the ESP32 and waits for an echo message. 
The echo message is printed to the console. The script takes one argument, which is the message to send to the ESP32. 
The message is sent to the ESP32, and the echo message is received and printed to the console. 
The serial port is closed after the echo message is received.
'''

import serial
import time
import sys

def get_valid_port():
    while True:
        # Case 1: Windows
        print("-" * 50)
        print("Windows: COM9, COM10, etc.")
        # Case 2: Linux
        print("Linux: /dev/ttyUSB0, /dev/ttyUSB1, etc.")
        # Case 3: Mac
        print("Mac: /dev/cu.usbmodem14101, /dev/cu.usbserial-54F40080041, etc.")
        # Case sensitive
        print("Note: The port name is case-sensitive.")
        print("-" * 50)
        # Prompt the user to enter the serial port
        port = input("Enter the serial port: ")
        try:
            # Attempt to open the serial port
            with serial.Serial(port, baudrate=115200, timeout=1) as ser:
                print(f"Serial port {port} is valid.")
                print("-" * 50)
                return port
        except serial.SerialException as e:
            print(f"SerialException: {e}. Please try again.")
        except ValueError as e:
            print(f"ValueError: {e}. Please check the port name and try again.")

def main():

    try:
        # Obtain a valid port through user input
        port = get_valid_port()
        baud_rate = 115200

        ser = serial.Serial(port=port, baudrate=baud_rate, timeout=1)
        time.sleep(2) # Wait for the connection to initialize

        # Check for message argument
        if len(sys.argv) != 2:
            print("Usage: python send_and_receive.py '<message>'")
            sys.exit(1)

        # Send the message
        message = sys.argv[1] + '\n' # Add newline character to ensure ESP32 recognizes end of message
        ser.write(message.encode())

        # Receive and print the echo message
        echo_message = ser.readline().decode('utf-8', errors='replace').strip()
        print(f"Received echo: {echo_message}")

        # Close the serial connection
        ser.close()
    except serial.SerialException as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    main()
