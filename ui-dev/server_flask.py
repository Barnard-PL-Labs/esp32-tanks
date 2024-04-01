from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/')
def index():
    return "Hello, World!"

# communicate with node js
@app.route('/receive_message', methods=['POST'])
def receive_message():
    # Extract the message sent from Node.js
    data = request.json
    print(data)
    message = data['message']
    
    print(f"Received message: {message}")
    
    # For now, just send back a confirmation
    return jsonify({"status": "Message received", "message": message})


def forward():
    print("Forward")
    return
    
def backward():
    print("Backward")
    return

def left():
    print("Left")
    return

def right():
    print("Right")
    return

def stop():
    print("Stop")
    return


if __name__ == "__main__":
    app.run(host='localhost', port=80)