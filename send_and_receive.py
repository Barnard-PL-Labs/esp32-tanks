import serial
import time
import sys

def get_valid_port():
    while True:
        # Prompt the user to enter the serial port        
        port = input("Enter the serial port (e.g., COM9 (Windows), /dev/ttyUSB0 (Linux), /dev/cu.usbmodem14101 (Mac)): ")
        try:
            # Attempt to open the serial port
            with serial.Serial(port, baudrate=9600, timeout=1) as ser:
                print(f"Serial port {port} is valid.")
                return port
        except serial.SerialException as e:
            print(f"SerialException: {e}. Please try again.")
        except ValueError as e:
            print(f"ValueError: {e}. Please check the port name and try again.")

def main():

    try:
        # Obtain a valid port through user input
        port = get_valid_port()
        baud_rate = 9600

        ser = serial.Serial(port=port, baudrate=baud_rate, timeout=1)
        time.sleep(2) # Wait for the connection to initialize

        # Check for message argument
        if len(sys.argv) != 2:
            print("Usage: python send_and_receive.py '<message>'")
            sys.exit(1)

        # Send the message
        message = sys.argv[1] + '\n'
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
