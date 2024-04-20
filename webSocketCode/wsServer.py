import asyncio
import websockets
import base64
from aiohttp import web
import json

# Dictionary to store connections by device ID
connections = {}

async def websocket_handler(websocket, path):
    print(f"Received a WebSocket connection on path: {path}")
    path_parts = path.strip('/').split('/')

    # Check if there are enough parts in the path
    if len(path_parts) < 2:
        print("Invalid path format. At least 'role' is required.")
        return

    role = path_parts[1]  # This should be 'device', 'view', or 'twitch'

    # Check if a device_id is required
    if role in ['device', 'view'] and len(path_parts) < 3:
        print("Invalid path format for device or view. Device ID is missing.")
        return

    device_id = path_parts[2] if len(path_parts) > 2 else None

    print(f"Role: {role}, Device ID: {device_id}")

    if role == 'twitch':
        await handle_twitch(websocket)
    elif role == 'device':
        if device_id not in connections:
            connections[device_id] = {'devices': set(), 'viewers': set()}
        connections[device_id]['devices'].add(websocket)
        try:
            async for message in websocket:
                if isinstance(message, bytes):
                    base64_encoded = base64.b64encode(message).decode('utf-8')
                    await broadcast(base64_encoded, device_id, 'viewers')
                else:
                    await websocket.send("Only binary data supported.")
        finally:
            connections[device_id]['devices'].remove(websocket)
            if not connections[device_id]['devices'] and not connections[device_id]['viewers']:
                del connections[device_id]
    elif role == 'view':
        if device_id not in connections:
            connections[device_id] = {'devices': set(), 'viewers': set()}
        connections[device_id]['viewers'].add(websocket)
        try:
            while True:
                await websocket.wait_closed()
        finally:
            connections[device_id]['viewers'].remove(websocket)
            if not connections[device_id]['devices'] and not connections[device_id]['viewers']:
                del connections[device_id]

async def handle_twitch(websocket):
    print('Twitch connection initialized')
    try:
        async for message in websocket:
            data = json.loads(message)
            print(f"Command received from Twitch: {data}")
            if data.get('tankID') == "all":
                # If the command is for all devices, iterate over all device IDs
                print("Broadcasting command to all devices")
                for key in connections:
                    await broadcast(json.dumps(data), key, 'devices')
            # Otherwise only send the command to the correct tank
            else:
                device_id = "device" + data.get('tankID')
                if device_id is None:
                    print("No device ID specified in the message")
                    continue  # Skip this command if no device ID is provided

                elif device_id in connections:
                    # Convert the entire command back to JSON and send it to the designated device
                    await broadcast(json.dumps(data), device_id, 'devices')
                else:
                    print(f"No active connections found for device ID {device_id}")
    except websockets.exceptions.ConnectionClosed:
        print("Twitch connection closed")

async def broadcast(message, device_id, target):
    """Broadcasts a message to all connected devices or viewers based on target."""
    if device_id in connections:
        recipients = connections[device_id][target]
        for websocket in recipients:
            await websocket.send(message)


#  HTML page that connects to the WebSocket server and displays the stream
routes = web.RouteTableDef()

@routes.get('/stream/{device_id}')
async def stream_page(request):
    device_id = request.match_info['device_id']
    return web.Response(text=f"""
    <!DOCTYPE html>
    <html>
    <head>
        <title>Video Stream {device_id}</title>
        <style>
            img {{
                width: 100%;
                height: auto;
            }}
        </style>
    </head>
    <body>
    <!-- Image element for displaying the video stream -->
    <img id="videoFrame" src="" alt="Video stream from {device_id}">
    <script>
    document.addEventListener('DOMContentLoaded', function() {{
        var device_id = "{device_id}"; // Correctly pass Python variable
        var ws = new WebSocket('ws://' + window.location.hostname + ':5001/ws/view/' + device_id);
        ws.onmessage = function(event) {{
            document.getElementById('videoFrame').src = 'data:image/jpeg;base64,' + event.data;
        }};
        ws.onopen = function() {{
            console.log('WebSocket connection established with device ' + device_id);
        }};
        ws.onerror = function(error) {{
            console.error('WebSocket error:', error);
        }};
        ws.onclose = function() {{
            console.log('WebSocket connection closed');
        }};
    }});
    </script>
    </body>
    </html>
    """, content_type='text/html')

# Main function to start the web server and WebSocket server
async def main():
    app = web.Application()
    app.add_routes(routes)
    runner = web.AppRunner(app)
    await runner.setup()
    # The port to view the stream is 8080, or whatever this is set to 
    site = web.TCPSite(runner, '0.0.0.0', 8080)
    await site.start()

    # Start WebSocket server
    # The port the devices send things to is 5001, or whatever this is set to  
    async with websockets.serve(websocket_handler, "0.0.0.0", 5001):
        await asyncio.Future()  # Run indefinitely

if __name__ == "__main__":
    asyncio.run(main())
