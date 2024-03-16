## Twitch Chat Bot for ESP32 
### Team Members:
- Han (hanvitC)
- Seongho (seongholee4)
- Nira (niranair01)
- Evan (esk2188)

### Description
- Listens for commands from Twitch Chat and sends them to an ESP32 via serial port.

### Example Workflow:
1. User sends a command from Twitch Chat.
2. The bot listens for the command and sends it to the ESP32 via serial port.
3. The ESP32 receives the command and performs the action.

## How to use:

#### 0. Clone the repository
```bash
git clone <repository link>
```

#### 1. Make sure ESP32 is connected to the computer
- Connect the ESP32 to the computer using a USB cable. 
- The baud rate is set to 115200. You can modify the baud rate in the code if needed.

#### 2. Run the following command to install the required packages.
- On terminal, make sure you are in the directory of the repository.
```bash
npm install tmi.js serialport @serialport/parser-readline
```
- tmi.js is a library that allows you to interact with the Twitch chat and events.
- serialport is a library that allows you to interact with the serial port.
- @serialport/parser-readline is a library that allows you to read data from the serial port line by line.

If you have package.json file, you can simply run:
```bash
npm install
```

#### 3. Verify installation:
```bash
npm list serialport
npm list @serialport/parser-readline
npm list tmi.js
```

#### 4. Provide your Twitch credentials
- Open the `twitchBot.js` file and provide your Twitch username, channel name, and OAuth token.

**You can get the OAuth token from [here](https://twitchapps.com/tmi/)**.

**Replace the following with your credentials:**
```javascript
const opts = {
        identity: {
            username: '<username>', // account username or bot name
            password: 'oauth:<token>' // Get this from https://twitchapps.com/tmi/
        },
        channels: ['<channel name>']
        // The channel your bot will be connected to.
        // e.g. https://www.twitch.tv/seongholee4
};
```

#### 5. Run the following command to start the bot.
```bash
node twitchBot.js
```

#### 6. Send Commands from Twitch Chat
- Once the bot is running, you can send commands from Twitch Chat to control the ESP32. For example, you can send the following commands:
*Commands are case-insensitive*

Commands for controlling the robot:
- `!forward` to move the robot forward
- `!right` to move the robot right
- `!left` to move the robot left
- `!backward` to move the robot backward
- `!stop` to stop the robot

For Testing purposes, the following commands are available:
  - `!dice` to roll a dice and sends the result to the chat
  - `!ledon` to display "LED ON" on the ESP32 display
  - `!ledoff` to display "LED OFF" on the ESP32 display



## TODO:
- Edit commands' functionality for ESP32
e.g. !forward, !right, !left, !backward, !stop only writes to the serial port.
- Add more commands/features for controlling the ESP32 if needed.
- Communicatable via WiFi connection.
- Test the bot with the ESP32 with other teams.
- Check if it works online.
- Check if it works with Linux and Mac OS.

## References:
- [Twitch Chat Bot Tutorial](https://dev.twitch.tv/docs/irc/get-started/)
- [tmi.js: JavaScript Package for Twitch Chat](https://tmijs.com/)
- [tmi.js Documentation](https://www.npmjs.com/package/tmi.js)


## Resolved Github Issues:
1.	Send messages to arduino [#2](https://github.com/Barnard-PL-Labs/esp32-tanks/issues/2)
2.	Add Cross-OS compatibility with user-prompted port entry [#11](https://github.com/Barnard-PL-Labs/esp32-tanks/issues/11)
3.	Implement a way to send commands to the ESP32 from the chat [#12](https://github.com/Barnard-PL-Labs/esp32-tanks/issues/12)
