# Twitch plays Rovers

This is a repo containing code which lets someone set up a sort of spin-off of "Twitch plays pokemon", but with ESP32 rovers!

## ESP32

## Networking

There are 3 components to the networking: 

1. **ESP32(s)**: The microcontrollers that drive the rovers.
2. **Server**: `server.py` that facilitates communication between the users' chats from a streaming channel and the ESP32s.
3. **Twitch ChatBot.js**: `twitchBot.js` that monitors and interprets commands from a streaming Twitch channel and communicates with server(s) via API requests.

There are two versions of networking setups based on the WiFi capabilities:

* The first is for scenarios where peer-to-peer is possible on WiFi, and does not require a work-around. The second is where peer-to-peer is not possible on WiFi, and one has to use a Virtual Machine in order to have all elements communicate with each other. For more on the first option, see `localServerCode`. For the VM version, see `webSocketCode`.

## Streaming to Twitch

* The ESP32 sends out either jpeg or mjpeg depending on which implentation is being used, and Twitch only accepts h264.

* In both implementations, it is assumed some sort of existing software is being used to encode and stream, such as OBS, ffmpeg, etc. 

