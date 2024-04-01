
// Import the required libraries 
// npm install tmi.js node-fetch
import tmi from 'tmi.js';
import fetch from 'node-fetch';

// The commands that the bot can listen for and send to the ESP32
const commands = {"!forward": "forward", "!right": "right", "!left": "left", "!backward": "backward", "!stop": "stop", "!ledon": "ON", "!ledoff": "OFF"};

// Define your Twitch client setup and event handlers as functions
function connectTwitchBot() {
    // Define configuration options
    const opts = {
        identity: {
            username: 'username', // account username or bot name
            password: 'oauth:{}' // Get this from https://twitchapps.com/tmi/
        },
        channels: ['username']
        // The channel your bot will be connected to
        // e.g. https://www.twitch.tv/username
    };

    // Create a client with the configuration
    const client = new tmi.client(opts);

    const sendMessageToFlask = async (message) => {
        try {
            const response = await fetch('http://localhost:80/receive_message', {
                method: 'POST',
                body: JSON.stringify({ message }),
                headers: { 'Content-Type': 'application/json' },
            });

            const data = await response.json();
        
            console.log('Message sent to Flask and received response:', data);
        } catch (error) {
            console.error('Failed to send message to Flask:', error);
        }
    };


    // Called every time the bot connects to Twitch chat
    const onMessageHandler = (target, context, msg, self) => {
        if (self) { return; } // Ignore messages from the bot

        console.log(`Received message: ${msg}`);

        const command = msg.trim();

        if (commands[command]) {
            console.log(`* Executing ${command} command`);
            client.say(target, `Your command was: ${command}`);
            sendMessageToFlask(commands[command]);
        } else if (command.startsWith('!')) {
            client.say(target, `Unknown command: ${command}`);
            console.log(`* Unknown command ${command}`);
        } else {
            console.log(`* User message was not a command: ${command}`);
        }

    };

    // Called every time the bot connects to Twitch chat
    const onConnectedHandler = (addr, port) => {
        console.log(`* Connected to ${addr}:${port}`);
    };

    client.on('message', onMessageHandler);
    client.on('connected', onConnectedHandler);

    console.log("Connecting to the Twitch chat...")
    // Connect the client
    client.connect();
}

connectTwitchBot();
