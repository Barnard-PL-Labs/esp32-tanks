
const tmi = require('tmi.js');
const WebSocket = require('ws');

const ws = new WebSocket('ws://45.55.200.204:5001/ws/twitch/overall'); // URL of the websocket, either the VM's IP or your own
// Conect to websocket
ws.on('open', function open() {
    console.log('Connected to WebSocket server as Twitch bot');
});
// Shouldn't be receiving anything but still track it in case
ws.on('message', function incoming(data) {
    console.log(data);  // Log any messages received from the server
});

ws.on('close', function close() {
    console.log('Disconnected from WebSocket server');
});
// Valid commands to pass to the web socket server 
const commands = {
    "!forward": { function: "f", tankID: "", value: "" },
    "!right": { function: "r", tankID: "", value: "" },
    "!left": { function: "l",  tankID: "", value: "" },
    "!back": { function: "b", tankID: "", value: "" },
    "!light": { function: "c", tankID: "", value: "" },
    "!blink": { function: "h", tankID: "", value: "" },
};

// Sets up the bot
const client = new tmi.Client({
    identity: {
        username: 'MyUsername', // Needs a valid twitch username/account, can be one set up specifically for the bot apparently
        password: 'MyPassword'  // You can generate a token using 
                                                          // the Twitch Chat OAuth Password Generator: https://twitchapps.com/tmi/
    },
    channels: [ 'MyChannel' ] // Whichever channel is being used, 
                                // and not the full address, but just the username like 
                                // shroud or your own personal username 
});

client.connect();

client.on('message', (channel, tags, message, self) => {
    if(self) return; // Ignore messages from the bot itself
    const commandParts = message.split(' ');
    const commandKey = commandParts[0].toLowerCase();
    const tankToRun = commandParts[1];
    const commandValue = parseInt(commandParts[2]);

    if(commandKey in commands && !isNaN(commandValue)) {
        const command = commands[commandKey];
        command.tankID = tankToRun;
        command.value = commandValue.toString();

        if (ws.readyState === WebSocket.OPEN) {
            ws.send(JSON.stringify(command));
        } else {
            console.error('WebSocket is not open');
        }
    }
});