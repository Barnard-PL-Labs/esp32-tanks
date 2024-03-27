from flask import Flask, request
import socket
import threading
import cv2
from ControlFunctions import *
app = Flask(__name__)

def displayStream(tank_ip):
    url = f"http://{tank_ip}:81/stream"
    cap = cv2.VideoCapture(url)

    while True:
        ret, frame = cap.read()

        if not ret:
            break

        if frame[0][0][2] > 100 and (frame[0][0][0] < 100 and frame[0][0][1] < 100): # This is how we 
            print("Red pixel detected")

        cv2.imshow("Rover Stream", frame)
        key = cv2.waitKey(1)

        if key == 27:
            break

    cap.release()
    cv2.destroyAllWindows()
    return
    return

@app.route('/register')
def begin_stream():
    tank_address = request.args.to_dict()['addr'] # This processes the request args from the tank
    print(f"Registered: {tank_address}", flush=True)

    reader = threading.Thread(target=displayStream, args=(tank_address,)) # We want to display the stream in a separate thread
    reader.start()

    return "Success", 200 # This is the response to the tank

if __name__ == "__main__":
    private_ip_addr = socket.gethostbyname(socket.gethostname())
    print(f"\n=======================\nServer at:\n{private_ip_addr}\nEnsure that serverIP in the ESP32 code is set to this IP address\n=======================\n")
    app.run(host=private_ip_addr, port=80)