from flask import Flask, request, Response, jsonify, send_from_directory
import requests
from flask_cors import CORS

# Save commands from the twitchBot here, to be sent to the tanks
readyCommands = []
maxCommands = 10

# List to keep track of tanks sending GET requests
roverIDs = []

app = Flask(__name__)
# Enable CORS for all routes and default options
# This CORS might be overkill and possible to tone it down not sure
CORS(app, resources={r"/*": {"origins": "*"}})

# Receives the IP of the rovers as they come online so it knows where to send the commands
@app.route('/receive_ip', methods=['POST'])
def receive_ip():
    ip_data = request.json 
    ip_address = ip_data.get('ip_address', 'No IP provided')
    print("Received IP Address from ESP32:", ip_address)
    roverIDs.append("http://" + ip_address + "/command")
    print(roverIDs)
    return jsonify({"message": "IP received successfully"}), 200

@app.route('/twitch_command', methods=['POST'])  # Receive commands from the twitchBot
def receive_command():
    # Extract json message, parse command, send it
    data = request.json

    function = data['function']
    value = data['value']
    tankID = data['tankID']
    print(function, value, tankID)
    # Check value is greater than 0 (negative values result in unexpected tank behavior)
    if int(value) < 0:
        return jsonify({"status": "failed", "message": "invalid value"}), 400
    
    # Extract function and construct command based on that function and value
    command = {'function': function, 'value': value}
    # Store the results of sending the commands to all the tanks
    results = []
    if tankID.lower() == "all":
        for rover in roverIDs:
            success, message = send_command(rover, command)
            results.append((rover, success, message))
        
        failed = False
        for r, worked, m in results:
            if not worked:
                failed = True
                break
        if failed:
            return jsonify({"status": "error", "results": results}), 500
        else:
            return jsonify({"status": "success", "results": results}), 200
    else:
        if len(roverIDs) < int(tankID):
            return jsonify({"status": "error"}), 404
        else:
            # For now I'm just assuming we have the tanks in the stream in the same order as 
            # they turn on and are connected to the server
            success, message = send_command(roverIDs[int(tankID)-1], command) 
            if not success:
                    return jsonify({"status": "error", "results": results}), 500
            else:
                return jsonify({"status": "success", "results": results}), 200
    
    # Ensure command list doesn't get too long
    if len(readyCommands) >= maxCommands:
        readyCommands.pop(0) # remove oldest command

    return jsonify({"status": "success", "message": "Command processed"}), 200
# Sends command to esp32
def send_command(url, command):
    headers = {'Content-Type': 'application/json'}
    # The json has to be sent twice for the esp32 to receive it
    # This is true for both plain text commands and json, I'm not sure why 
    # The first time the server will send back "This URI does not exist" and the second time it will work
    # So I just send it twice here 
    response = requests.post(url, headers=headers, json=command, timeout=5)
    response = requests.post(url, headers=headers, json=command, timeout=5)
    if response.status_code == 200:
        return True, response.text
    else:
        return False, f"Failed with status code {response.status_code}"



if __name__ == '__main__':
    # start a flask server
    # host needs to be '0.0.0.0' in order to not just be local to the vm
    app.run(host='0.0.0.0', port=5001, use_reloader=False, debug=True)


