
# oneM2M Arduino-ESP32 App

[oneM2M](https://www.onem2m.org/) based example of communication between the device and the oneM2M TinyIoT server on the same network using ESP32 boards  

## Overview
- **Key Feature**: Sensor data collection, data transmission between oneM2M TinyIoT server 
- **Used Board**: ESP32 (ESP32 Dev Module(Chip: ESP-WROOM-32))
- [ESP32 dev module image example]<img src="./images/esp32 dev module image.jpg">
- [ESP32 dev module pin map image example, from lastminuteengineers]<img src="./images/ESP32-Pinout.webp">
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
4. After refactoring it by referring to the network configuration section below then click upload button(if can't upload with error code, please press 'boot' button on your ESP32 board)

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
[ESP32 circuit example]<img src="./images/esp32 circuit example.png">

## Network Configuration
**Some settings are required for the network**
1. Add Inbound Rule for Port 3000 in Firewall(by "New-NetFirewallRule -DisplayName "Allow Port 3000" -Direction Inbound -LocalPort 3000 -Protocol TCP -Action Allow" in PowerShell)
2. Setting the Port Proxy Using the netsh Command(by "netsh interface portproxy add v4tov4 listenaddress=0.0.0.0 listenport=3000 connectaddress=<WSL_interface_IP> connectport=3000" in PowerShell)
   -Here, wsl_interface_ip corresponds to the ip of the square in the picture below
   -[wsl ip addr example image]
3. In Arduino IDE, before execute you need to set wifi ssid, password, server_ip addresss
   - ssid and password correspond to the hotspot or iptime router Wi-Fi network you want to connect to
   - The server ip corresponds to the windows host ip on the pc (or laptop) where the server is running(verify by ipconfig in PowerShell)
   - [Arduino code section that you may modify]
   - [windows powershell ipconfig example image]

