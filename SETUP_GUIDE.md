# Quick Setup Guide

## Before You Start

1. **Update WiFi Credentials** in `Core/Inc/esp8266.h`:
   ```c
   #define WIFI_SSID           "YourActualWiFiName"
   #define WIFI_PASSWORD       "YourActualWiFiPassword"
   ```

2. **Update MQTT Broker IP** in `Core/Inc/esp8266.h`:
   ```c
   #define MQTT_BROKER_IP      "192.168.1.29"  // Replace with your computer's IP
   ```

3. **Install MQTT Broker** (if not already installed):
   ```bash
   # On Windows (using Chocolatey)
   choco install mosquitto
   
   # Or download from: https://mosquitto.org/download/
   ```

## Step-by-Step Setup

### Step 1: Hardware Connections
```
STM32F407VG          ESP8266
-----------          -------
PA2 (USART2_TX) ---> RX
PA3 (USART2_RX) <--- TX
3.3V            ---> VCC
GND             ---> GND
3.3V            ---> CH_PD
3.3V            ---> RST (optional)

LED Connection:
PC9 ---> LED ---> 220Ω Resistor ---> GND
```

### Step 2: Start MQTT Broker
```bash
# Open Command Prompt as Administrator
mosquitto -p 1883 -v
```

### Step 3: Start WebSocket Server
```bash
# Open another Command Prompt
cd websocket
npm install express ws mqtt
node server.js
```

### Step 4: Flash STM32 Firmware
1. Build the project in STM32CubeIDE
2. Flash to STM32F407VG board
3. Reset the board

### Step 5: Test the System
1. Open browser to `http://localhost:3000`
2. Click ON/OFF buttons to control LED
3. Check MQTT broker logs for messages

## Expected Behavior

1. **STM32 Startup**: LED should blink briefly during initialization
2. **WiFi Connection**: ESP8266 connects to your WiFi network
3. **MQTT Connection**: STM32 connects to MQTT broker
4. **Web Dashboard**: Shows connection status and LED control buttons
5. **LED Control**: Clicking buttons should immediately change LED state

## Common Issues & Solutions

### ESP8266 Not Responding
- Check power supply (3.3V, adequate current)
- Verify UART connections (TX/RX crossed)
- Ensure CH_PD is pulled high to 3.3V

### WiFi Connection Failed
- Double-check SSID and password
- Ensure WiFi network is 2.4GHz (ESP8266 doesn't support 5GHz)
- Check if network has MAC address filtering

### MQTT Connection Failed
- Verify MQTT broker is running
- Check IP address configuration
- Ensure port 1883 is not blocked by firewall

### Web Dashboard Not Working
- Check if Node.js server is running
- Verify WebSocket connection in browser console
- Ensure MQTT broker is accessible

## Testing Commands

### Test MQTT Communication
```bash
# Subscribe to LED status
mosquitto_sub -h localhost -t "led/status"

# Publish LED control command
mosquitto_pub -h localhost -t "led/control" -m "ON"
mosquitto_pub -h localhost -t "led/control" -m "OFF"
```

### Monitor ESP8266 AT Commands
Connect a USB-to-Serial adapter to monitor ESP8266 communication:
- Baud rate: 115200
- Data bits: 8
- Stop bits: 1
- Parity: None
