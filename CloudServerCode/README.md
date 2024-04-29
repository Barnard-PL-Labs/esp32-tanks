# ESP32 as the server

This portion of the code is for when you believe peer-to-peer will be enabled on the WiFi you're running the code on, so everything can communicate with each other. The esp32 itself acts as the server, based on the "CameraWebServer" example so the user also gets the functionality that comes with that. 

## Features

* **IP Registration**: Automatically registers and tracks the IP addresses of ESP32 devices as they connect to the network.
* **Command Processing**: Receives and processes commands from Twitch, distributing them to the appropriate ESP32 devices based on the command details.

## Setup and Configuration

### Requirements
* Flask
* Requests
* Flask-CORS

These can be installed using `pip`:
```shell
pip install flask requests flask-cors
```

### Running the Server
* To start the server, execute the following command from your terminal:
```shell
python server.py
```

### API Endpoints

#### `/receive_ip` (POST)
Receives and stores the IP address of a newly connected ESP32 device. Each device should call this endpoint upon initialization.


#### `/twitch_command` (POST)
Processes commands from Twitch or other command sources and sends them to the appropriate ESP32 devices.

## Command Handling Logic
Commands are either sent to a specific device, identified by tankID, or broadcast to all devices if tankID is set to "all". If a command fails to be sent to any device, the server responds with a 500 status code indicating an error.

# Web Page for Streaming
- The rovers are acting as the servers, so they're reached at https://roverIP:81/stream. This is where one would direct OBS or ffmpeg to take the stream from.
- Also note these servers can only handle one client at a time, so if they're open on a browser and then you try to stream it using OBS or ffmpeg, it won't happen until you close the browser. 

# Implementation Details

- **Device Management**: Uses a list roverIDs to manage the active connections by storing their URLs.
- **Command Sending**: Includes a function send_command that handles the actual sending of the commands to the ESP32 devices. This function sends the command twice due to issues with the first attempt being unrecognized.
