from flask import Flask, request
app = Flask(__name__)

@app.route('/')
def log_connection():
    print(f"Connection established! from {request.remote_addr}")
    return 0