/* 
Listens for commands from Twitch Chat and sends them to an ESP32 via serial port.
*/
// Import the required libraries 
// npm install tmi.js serialport @serialport/parser-readline
const tmi = require('tmi.js');
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');
const readline = require('readline').createInterface({
    input: process.stdin,
    output: process.stdout
});

// Define your Twitch client setup and event handlers as functions
function setupTwitchClient(port) {
    // Define configuration options
    const opts = {
        identity: {
            username: '<username>', // account username or bot name
            password: 'oauth:<token>' // Get this from https://twitchapps.com/tmi/
        },
        channels: ['<username>']
        // The channel your bot will be connected to
        // e.g. https://www.twitch.tv/seongholee4
    };

    // Create a client with the configuration
    const client = new tmi.client(opts);

    // Called every time the bot connects to Twitch chat
    const onMessageHandler = (target, context, msg, self) => {
        if (self) { return; } // Ignore messages from the bot

        console.log(`Received message: ${msg}`);

        const command = msg.trim();

        // If the command is known, let's send it to the ESP32
        client.say(target, `Your command was: ${command}`);
        if (command === '!dice') {
            const num = rollDice();
            client.say(target, `You rolled a ${num}`);
        } else if (command === '!forward') {
            console.log(`* Executing ${command} command`);
            port.write('forward\n'); // Sends 'forward' command to the ESP32
        } else if (command === '!right') {
            console.log(`* Executing ${command} command`);
            port.write('right\n'); // Sends 'right' command to the ESP32
        } else if (command === '!left') {
            console.log(`* Executing ${command} command`);
            port.write('left\n'); // Sends 'left' command to the ESP32
        } else if (command === '!backward') {
            console.log(`* Executing ${command} command`);
            port.write('backward\n'); // Sends 'backward' command to the ESP32
        } else if (command === '!stop') {
            console.log(`* Executing ${command} command`);
            port.write('stop\n'); // Sends 'stop' command to the ESP32
        } else if (command === '!ledon') {
            // Testing purpose only
            // Displays "LED ON" on the ESP32 display
            console.log(`* Executing ${command} command`);
            client.say(target, `Turning LED on`);
            port.write('ON\n'); // Sends 'ON' command to the ESP32
        } else if (command === '!ledoff') {
            // Testing purpose only
            // Displays "LED OFF" on the ESP32 display
            console.log(`* Executing ${command} command`);
            client.say(target, `Turning LED off`);
            port.write('OFF\n'); // Sends 'OFF' command to the ESP32
        } else {
            console.log(`* Unknown command ${command}`);
        }
    };

    // Called every time the bot connects to Twitch chat
    const onConnectedHandler = (addr, port) => {
        console.log(`* Connected to ${addr}:${port}`);
    };

    client.on('message', onMessageHandler);
    client.on('connected', onConnectedHandler);

    // Connect the client
    client.connect();
}


// Ask user for the serial port path
async function getValidPort() {
    // List all available serial ports
    const ports = await SerialPort.list();
    ports.forEach((port, index) => {
        console.log(`${index}: ${port.path} (${port.manufacturer || 'Unknown manufacturer'})`);
    });

    return new Promise((resolve, reject) => {
        readline.question('Enter the index of the serial port: ', (index) => {
            const selectedPortInfo = ports[parseInt(index, 10)];
            if (!selectedPortInfo) {
                reject(new Error('Invalid selection. Please restart the program and select a valid index.'));
            } else {
                resolve(selectedPortInfo.path);
            }
        });
    });
}

// Initialize the serial port connection
async function initSerialPort() {
    try {
        const portPath = await getValidPort();
        const port = new SerialPort({ path: portPath, baudRate: 115200 });
        const parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));

        parser.on('data', data => {
            console.log(`Received from serial port: ${data}`);
            // Handle received data from the serial port
        });

        console.log(`Serial port ${portPath} is open.`);

        // After the serial port is successfully opened, set up and connect the Twitch client
        setupTwitchClient(port);
    } catch (error) {
        console.error(`Failed to open serial port: ${error}`);
    } finally {
        readline.close(); // Ensure readline interface is closed
    }
}

initSerialPort();

// Called when the "dice" command is issued on Twitch chat
function rollDice() {
    const sides = 6;
    return Math.floor(Math.random() * sides) + 1;
}
