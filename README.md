# plant-healthcheck-wifi
ESP-8266 WiFi module source code. It receives data from Serial and sends it to an HTTP server.

# Upstream commands
| Command    | Description |
| ----------- | ----------- |
|  `0#ap_name!passowrd`     |  Sets up a WiFi configuration interface  (it has a default value if you only send `0`)    |
| `1`| Connects to the latest configured WiFi Access Point. You should not send this command manually after a succesful configuration. |
| `2#http://target_url!{"json":"data"}`| Sends a POST request to the given URL |
| `9` | Idle state. Does not hosts an AP and does not connects to a WiFi AP |

# Downstream commands
| Command    | Description |
| ----------- | ----------- |
|       |        |

# Upload to ESP-01
 
I am using an another Arduino UNO to flash the module.
Computer -> Arduino UNO -> ESP-01

- Check the arduino, upload an empty script. **Do not open the serial.**

## Wiring

![ESP-01 pinout](https://github.com/sylank/plant-healthcheck-wifi/blob/master/media/FM2OIKDJVMNNXEX.png)

| ESP-01       | Arduino UNO programmer |
| ----------- | ----------- |
| TX      | TX       |
| GND   | GND        |
| CH_PD   | 3.3v        |
| GPIO2   | - (nowhere)        |
| RST   | RST        |
| GPIO0   | GND (during upload)        |
| VCC   | 3.3v        |
| RX   | RX        |

## Upload process
**Used ESP-8266 Board manager version 2.5.2**

![ESP-01 pinout](https://github.com/sylank/plant-healthcheck-wifi/blob/master/media/programmer.png)

1. Write the code
1. Connect the GPIO0 with the Arduino UNO's GND
1. Press the upload
1. When the IDE wants to connect to the Arduino press the reset for ~1 sec. (It will turn on the flash mode on the ESP-01)
1. When upload is done, disconnect the GPIO0 from the GND
1. Press the Arduino's reset button 
1. It should send serial texts and it should work
