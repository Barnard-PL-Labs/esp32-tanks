from flask import Flask, request, Response, jsonify
import requests
# Need to install flask CORs
from flask_cors import CORS

# Save commands from the twitchBot here, to be sent to the tanks
readyCommands = []

app = Flask(__name__)
# Enable CORS for all routes and default options
# This CORS might be overkill and possible to tone it down not sure
CORS(app, resources={r"/*": {"origins": "*"}})


@app.route('/command', methods=['POST'])  # Receive commands from the twitchBot
def receive_command():
    # extract json message, parse command, send it
    data = request.json
    # check value is greater than 0 (negative values result in unexpected tank behavior)
    if int(data['value']) < 0:
        return jsonify({"status": "failed", "message": "invalid value"}), 400

    # extract function and construct command based on that function and value
    command = data['function'] + " " + str(data['value'])

    print(f"Received command: {command}")
    readyCommands.append(command)
    return jsonify({"status": "success", "message": "Command processed"}), 200

# End point for tanks to "listen" to and take commands from
# Currently just pops first and sends it


@app.route('/get-command', methods=['GET'])
def send_command():
    print("request received from tank")
    if readyCommands:
        # Pop the first command from the list to send it to the ESP32
        command = readyCommands.pop(0)
        return jsonify({"command": command}), 200
    else:
        # If there are no commands, return a 'no command' status or similar
        return jsonify({"status": "no command"}), 204


if __name__ == '__main__':
    # start a flask server
    # host needs to be '0.0.0.0' in order to not just be local to the vm
    app.run(debug=True, host='0.0.0.0', port=8080)
