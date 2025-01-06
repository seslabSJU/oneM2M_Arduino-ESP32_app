
# oneM2M Arduino-ESP32 App

[oneM2M](https://www.onem2m.org/) based example of communication between the device and the oneM2M TinyIoT server on the same network using ESP32 boards  

## Overview
- **Key Feature**: Sensor data collection, data transmission between oneM2M TinyIoT server 
- **Used Board**: ESP32 (ESP32 Dev Module(Chip: ESP-WROOM-32))
- **Development Environment**: Arduino IDE

## Prepartion
1. Arduino IDE(at least 1.8.x over) install
2. In Arduino IDE, top menu -> file -> preference -> additional board manager select
3. enter URL (https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json)
4. clone this project or download

## How to execute
1. In Arduino IDE by `File > Open`, open `TinyIoT_Demo_esp32/TinyIoT_Demo_esp32.ino` 
2. **Tools > Board** select `ESP32 Dev Module`  
3. **Tools > Port** select USB port  
4. click upload button(if can't upload with error code, please press 'boot' button on your ESP32 board)

## Code Configurations
- **Network Configuration**: Attempt to connect wifi according to the WiFi SSID, Password entered
- **Sensor Configuration**: After the song goes off and the PIR sensor initializes for 30 seconds, then the song goes off again
- **oneM2M Structure Setting**: If Aduido is not looking up the AE of it, the process of creating the AE and then creating the CNTs of each sensor below it
- **Sensor Detection**: If the PIR detects movement for a certain period of time or longer, the LED and Buzzer operate
- **Server Communication**: Send the sensor's value to the server when the PIR operates for a certain period of time or longer, when the LED and Buzzer operate, and when the detection of the PIR ends

## Libararies
- **WiFi.h**: ESP32 nested wifi functions   
- **pitches.h**: Header file used on the scale of the song that signals the initialization of the PIR sensor
- **ArduinoJson**: Required for json parsing and creation

## Circuit Configuration
-The picture below is an example of a circuit configuration in esp32
![ESP32 circuit example]<./images/esp32 circuit example.png>
