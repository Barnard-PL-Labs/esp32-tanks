# Websocket Version

Use this code if your WiFi doesn't allow peer-to-peer, or you believe it doesn't.
In this version, instead of the rover acting as a server based on the given "CameraWebServer", the rover acts as a client to a web socket server hosted either locally on your own machine or on a cloud VM. 


## Server Responsibilities

The server manages:

WebSocket Connections: Handles WebSocket connections on different paths to distinguish between devices, viewers, and Twitch commands.
Message Handling: Routes messages appropriately based on the role and the content of the message (e.g., device streams, viewer requests, or Twitch commands).
Broadcasting: Supports broadcasting messages either to specific devices or to all connected viewers, depending on the command received.

# Server Setup

## Requirements
Python 3.6+
Libraries: websockets, aiohttp, base64, json
Install the necessary Python packages

### Running the Server
To start the server, run the following command from your terminal:

python server.py

This command launches the WebSocket server that listens for device connections on port 5001 and serves the streaming page on port 8080.

## Detailed Functionality

### WebSocket Handler
The websocket_handler function manages incoming WebSocket connections. It distinguishes between three types of roles based on the URL path:

device: Handles incoming streams from devices. Stores connections and broadcasts streams to all registered viewers.
view: Manages viewer connections who want to watch device streams.
twitch: Processes commands from Twitch, broadcasting them to devices as needed.

### Handle Twitch
The handle_twitch function processes incoming commands from Twitch, interpreting and forwarding them as necessary to either specific devices or all devices.

### Broadcast Function
The broadcast function sends messages to appropriate recipients based on the role (device or viewer). It ensures that all connected clients receive relevant updates or commands.

## Web Page for Streaming

The server also serves an HTML page that connects to the WebSocket server for displaying the stream from a specific device. The route /stream/{device_id} serves this page, dynamically filling in the device ID to connect to the correct WebSocket stream. It is based on the IP on which the server is running, for example if it's local it might be 
"http://192.168.1.41:8080/stream/device1"

## HTML Template
Ensure that the streamPage.html file is located in the templates directory relative to the server script. This file should be properly formatted to connect to the WebSocket server using JavaScript embedded within the HTML.

