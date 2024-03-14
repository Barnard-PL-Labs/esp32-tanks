## Packet Structure

### Sent Commands

Sent commands are in JSON format, where the `function` field can be:

- [`motor`](#motor): To move the tank
- [`light`](#light): To adjust the headlight
- [`camera`](#camera): To control the camera

Each of these functions have their own required fields:

#### `motor` <a name="motor"></a>

- `direction`: `l`/`r`/`f`/`b` to denote the direction the tank should go (respectively, left, right, forward, backwards)
- `value`: degrees for l,r and milliseconds for f,b

Example:

```json
{   
    "function": "motor",
    "direction": "f",
    "value": "1000"
}
```

#### `light` <a name="light"></a>

- `value`: range from 0 = off to 255 = full brightness
  
Example:

```json
{   
    "function": "light",
    "value": "50"
}
```

#### `camera` (TBD) <a name="camera"></a>

TBD



Note: Commands should be entered via the serial in the form: `[function] <required fields>`, where the required fields follow the order specified above. 

### Responses

Responses are also in JSON format, with the fields:

- `status`: `success` or `error` 
- `message`: for a detailed message of the command executed

Example:

```json
{
    "status": "success",
    "message": "Motor command f1000 executed"
}
```
