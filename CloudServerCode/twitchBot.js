// You might need to install npm and the axios library  via terminal or command depending on if using mac or windows
// Also might need to install Flask-Cors to handle error with communicating 
// via pip install flask-cors

const tmi = require('tmi.js');
const axios = require('axios');
// Valid commands to pass to the flask server 
const commands = {
    "!forward": { function: "f", value: "" },
    "!right": { function: "r", value: "" },
    "!left": { function: "l",  value: "" },
    "!back": { function: "b", value: "" },
    "!light": { function: "c", value: ""},
    "!blink": { function: "h", value: "" },
};

// Set up the bot
const client = new tmi.Client({
    identity: {
        username: 'MyUserName', // Needs a valid twitch username/account, can be one set up specifically for the bot apparently
        password: 'MyPassword'  // You can generate a token using 
                                                          // the Twitch Chat OAuth Password Generator: https://twitchapps.com/tmi/
    },
    channels: [ 'MyChannel' ] // Whichever channel is being used, 
                                // and not the full address, but just the username like 
                                // shroud or your own personal username 
});

client.connect();

client.on('message', (channel, tags, message, self) => {
    // Ignore echoed messages
    if(self) return;
    const commandParts = message.split(' '); // split message
    const commandKey = commandParts[0].toLowerCase(); // get command
    const commandValue = parseInt(commandParts[1]); // get value

    if(commandKey in commands && !isNaN(commandValue)) {
        const command = commands[commandKey];
        command.value = commandValue.toString(); // Set the value in the command object
        console.log("sending")
        axios.post('http://localhost:8080/command', command)
        .then(response => {
            // Handle response
            console.log(response.data);
        })
        .catch(error => {
            // Handle error
            console.error(error);
        });
    }
});