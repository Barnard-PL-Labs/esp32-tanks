from flask import Flask, request
# from flask_cors import CORS
import cv2
from ControlFunctions import *
app = Flask(__name__)

# CORS(app)

def displayStream(ip):
    url = "http://" + ip + ":81/stream"
    cap = cv2.VideoCapture(url)

    while True:
        ret, frame = cap.read()

        if not ret:
            break

        if frame[0][0][2] > 100 and (frame[0][0][0] < 100 and frame[0][0][1] < 100): # B G R
            print("Red pixel detected")

        cv2.imshow("Rover Stream", frame)
        key = cv2.waitKey(1)

        if key == 27:
            break

    cap.release()
    cv2.destroyAllWindows()

@app.route('/register')
def begin_stream():
    print(f"Connection established! from {request.remote_addr}")
    displayStream(request.remote_addr)


if __name__ == "__main__":
    app.run(host='127.0.0.1', port=80) # This is super insecure, but it's just a demo