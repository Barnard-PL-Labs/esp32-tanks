# the websocket code is way down below, commented out

from flask import Flask, request, Response, jsonify, send_from_directory
import threading
import time
from flask_cors import CORS
import base64

# Save commands from the twitchBot here, to be sent to the tanks
readyCommands = []
maxCommands = 10

# List to keep track of tanks sending GET requests
tankIDs = []

app = Flask(__name__)
# Enable CORS for all routes and default options
# This CORS might be overkill and possible to tone it down not sure
CORS(app, resources={r"/*": {"origins": "*"}})
latest_frame = None
lock = threading.Lock()


@app.route('/command', methods=['POST'])  # Receive commands from the twitchBot
def receive_command():
    # Extract json message, parse command, send it
    data = request.json

    function = data['function']
    value = data['value']
    tankID = data['tankID']

    # Check value is greater than 0 (negative values result in unexpected tank behavior)
    if int(value) < 0:
        return jsonify({"status": "failed", "message": "invalid value"}), 400
    
    # Extract function and construct command based on that function and value
    command = f"{function} {value}"
    print(f"Received command: {command}")

    # Check tankID to figure out on which tanks to run command
    if tankID == 'all':
        # sets it to all tanks that have sent a GET
        tanksToRun = tankIDs
    elif tankID not in tankIDs:
        return jsonify({"status": "failed", "message": "invalid tank ID"}), 400
    else:
        tanksToRun = [tankID]

    # Ensure command list doesn't get too long
    if len(readyCommands) >= maxCommands:
        readyCommands.pop(0) # remove oldest command
    
    # Adds command and the list of tanks to run it
    readyCommands.append({'command': command, 'tanksToRun': tanksToRun.copy()})
    print(readyCommands)
    return jsonify({"status": "success", "message": "Command processed"}), 200

# End point for tanks to "listen" to and take commands from
# Currently just pops first and sends it

@app.route('/get-command', methods=['GET'])
def send_command():
    tankID = request.args.get('tankID')
    if not tankID:
        return jsonify({"status": "failed", "message": "tankID is required"}), 400
    # If it's a new tank, add it to server's list
    if tankID not in tankIDs:
        tankIDs.append(tankID)
    # Iterate through the commands to see which command to run
    for command in readyCommands:
        if tankID in command["tanksToRun"]:
            command["tanksToRun"].remove(tankID)
            # If no more tanks to run on, remove it from command list
            if len(command["tanksToRun"]) == 0:
                readyCommands.remove(command)
            return jsonify({"command": command["command"]}), 200
    # If there are no commands, return no command status
    return jsonify({"status": "no command"}), 204

@app.route('/upload', methods=['POST'])
def upload():
    global latest_frame
    image_file = request.data
    if image_file:
        with lock:
            latest_frame = image_file
        return "Frame received", 200
    return "No frame received", 400

def generate_frames():
    while True:
        with lock:
            if latest_frame:
                yield (b'--frame\r\n'
                       b'Content-Type: image/jpeg\r\n\r\n' + latest_frame + b'\r\n')
        time.sleep(0.1)  # Needs to be some delay or it goes kind of crazy 

@app.route('/video')
# This is the url the stream is actually on
# i.e., on the VM, http://45.55.200.204:5001/video
def video_feed():
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')
if __name__ == '__main__':
    # start a flask server
    # host needs to be '0.0.0.0' in order to not just be local to the vm
    app.run(host='0.0.0.0', port=5001, use_reloader=False, debug=True)


# import asyncio
# import websockets
# import base64

# async def echo(websocket, path):
#     async for message in websocket:
#         if isinstance(message, bytes):
#             # Assuming the message is a JPEG image
#             print(f"Received binary data of length {len(message)}")
#             base64_encoded = base64.b64encode(message).decode('utf-8')
#             await websocket.send(base64_encoded)  # Send base64 encoded data to html page (in theory)
                # it doesn't seem to send properly, or at all. not sure why 
                # I also will push index.html which is the html page that is supposed to display
                # the stuff gotten from the esp32
#         else:
#             print("Received non-binary message")
#             await websocket.send("Only binary data supported.")
# async def main():
#     async with websockets.serve(echo, "0.0.0.0", 5001):
#         await asyncio.Future()  # Run forever

# if __name__ == "__main__":
#     asyncio.run(main())