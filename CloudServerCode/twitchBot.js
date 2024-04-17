
const tmi = require('tmi.js');
const axios = require('axios');
// Valid commands to pass to the flask server 
const commands = {
    "!forward": { function: "f", tankID: "", value: "" },
    "!right": { function: "r", tankID: "", value: "" },
    "!left": { function: "l",  tankID: "", value: "" },
    "!back": { function: "b", tankID: "", value: "" },
    "!light": { function: "c", tankID: "", value: "" },
    "!blink": { function: "h", tankID: "", value: "" },
};

// Set up the bot
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
    // Ignore echoed messages
    if(self) return;
    const commandParts = message.split(' '); // split message
    const commandKey = commandParts[0].toLowerCase(); // get command
    const tankToRun = commandParts[1]; // get tankID to run command on
    const commandValue= parseInt(commandParts[2]); // get value
    

    if(commandKey in commands && !isNaN(commandValue)) {
        const command = commands[commandKey];
        command.value = commandValue.toString(); // Set the value in the command object
        command.tankID = tankToRun
        console.log("sending")
        axios.post('http://45.55.200.204:5001/command', command)
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