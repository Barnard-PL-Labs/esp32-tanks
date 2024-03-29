from flask import Flask, request, Response
import requests
import serial

# Enter the serial port that the coordinator is connected to. EG 'COM1' or "/dev/tty...".
# Make sure the serial port isn't being actively read in Arduino IDE
serial_port = 'YOUR_SERIAL_PORT'

mac_addresses = {  # Where we should store all the tank mac addresses permanently
    "48:27:E2:91:A5:BC": "tank_1"
}

tank_addresses = {  # Maps the IP to the tank's name after it connects
    "tank_1": None,
    "tank_2": None,
    "tank_3": None,
    "tank_4": None
}

connected_tanks = []  # A list for easy tracking of the tank names that are connected

baud_rate = 115200

app = Flask(__name__)


#  Reads the coordinator's serial messages. It's looking for a message in the form of "mac_address*ip_address" that
#  the coordinator forwards from the tank. The tank lights should blink twice when it first connects. It will
#  instruct you to hard code your tank's mac address if it isn't already included in mac_addresses. Do that, please.
def connect_tank():
    ser = serial.Serial(serial_port, baud_rate)
    print("[Server] Waiting for tank connection...")
    while True:
        line = ser.readline().decode().strip()  # This serial reader is a blocking function

        if line:
            print("[Coordinator] " + line)
            if '*' in line:
                lines = line.split('*')
                mac = lines[0]
                ip = lines[1]

                if mac in mac_addresses:
                    print(f"[Server] connecting to {mac_addresses[mac]}.")

                    # make headlight blink twice when it connects
                    url = f'http://{ip}/blink'
                    data = {'num': 2}
                    response = requests.post(url, data=data)

                    tank_addresses[mac_addresses[mac]] = ip
                    connected_tanks.append(mac_addresses[mac])
                    break
                else:
                    print(f"[Server] Please assign {mac} to a tank in mac_addresses.")
                    return


#  Send a command to all connected tanks. e.g. "f 1000"
def send_commands(command):
    for tank in connected_tanks:
        url = f'http://{tank_addresses[tank]}/command'
        data = {'command': command}

        response = requests.post(url, data=data)

        print("[Server] Status Code:", response.status_code)
        print("[Server] Response Text:", response.text)


#  For typing commands manually. Mainly for testing.
def input_commands():
    print("[Server] Enter tank command. Enter \"stop\" to end.")

    while True:
        command = input().lower()
        if command == "stop":
            break
        send_commands(command)


if __name__ == '__main__':
    connect_tank()
    input_commands()

    # start a flask server
    app.run(debug=True)
