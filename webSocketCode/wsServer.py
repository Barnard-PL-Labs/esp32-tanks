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


    role = path_parts[1]  # This should be 'device', 'view', or 'twitch'
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
# Sends either stream or twitch commands depending on what's passed in 
async def broadcast(message, device_id, target):
    if device_id in connections:
        recipients = connections[device_id][target]
        for websocket in recipients:
            await websocket.send(message)


# HTML page that connects to the WebSocket server and displays the stream
routes = web.RouteTableDef()
# Uses the HTML page in templates to stream
@routes.get('/stream/{device_id}')
async def stream_page(request):
    device_id = request.match_info['device_id']
    # Load the HTML template, if having issues look at where your terminal currently is
    with open('templates/streamPage.html', 'r') as file:
        html_content = file.read()

    html_content = html_content.replace('{{ device_id }}', device_id)

    # Return the modified HTML content
    return web.Response(text=html_content, content_type='text/html')


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
