# STM32 LED Control via WebSocket Dashboard

This project implements an LED control system using STM32F407VG microcontroller with ESP8266 WiFi module. The system allows controlling an LED through a web dashboard using WebSocket communication and MQTT protocol.

## System Architecture

```
[Web Dashboard] <--WebSocket--> [Node.js Server] <--MQTT--> [STM32+ESP8266]
```

## Hardware Requirements

- STM32F407VG Discovery Board
- ESP8266 WiFi Module (ESP-01 or similar)
- LED connected to pin PD15
- UART connection between STM32 and ESP8266:
  - STM32 USART2 (PA2/PA3) connected to ESP8266 RX/TX
  - ESP8266 VCC to 3.3V
  - ESP8266 GND to GND

## Software Components

### 1. STM32 Firmware
- **main.c**: Main application logic
- **esp8266.h/esp8266.c**: ESP8266 driver with AT command interface
- **GPIO Configuration**: PD15 as output for LED control
- **UART Configuration**: USART2 at 115200 baud for ESP8266 communication

### 2. Node.js WebSocket Server
- **server.js**: WebSocket server with MQTT client
- **public/index.html**: Web dashboard interface
- Handles WebSocket connections from browsers
- Forwards LED commands to MQTT broker
- Receives LED status updates from STM32

## Configuration

### 1. WiFi Settings
Update in `esp8266.h`:
```c
#define WIFI_SSID           "YOUR-WIFI-NAME"
#define WIFI_PASSWORD       "YOUR-WiFi-Password"
```

### 2. MQTT Broker Settings
Update in both `esp8266.h` and `server.js`:
```c
#define MQTT_BROKER_IP      "192.168.1.100"  // Your server IP
#define MQTT_BROKER_PORT    1883
```

### 3. MQTT Topics
- Control Topic: `led/control` (receives ON/OFF commands)
- Status Topic: `led/status` (publishes LED state)

## Setup Instructions

### 1. Hardware Setup
1. Connect ESP8266 to STM32 USART2 (PA2/PA3)
2. An integrated blue LED is already connected to PD15 pin
3. Power ESP8266 with 3.3V (ensure stable power supply)

### 2. Software Setup
1. **STM32 Configuration**:
   - Update WiFi credentials in `esp8266.h`
   - Update MQTT broker IP to match your server
   - Compile and flash the firmware

2. **Node.js Server Setup**:
   ```bash
   cd websocket
   npm install express ws mqtt
   node server.js
   ```

3. **Web Dashboard**:
   - Open browser to `http://localhost:3000`
   - Use ON/OFF buttons to control LED

## Protocol Details

### MQTT Messages
- **LED Control**: Topic `led/control`, Message `ON` or `OFF`
- **LED Status**: Topic `led/status`, Message `LED: ON` or `LED: OFF`

### ESP8266 AT Commands Used
- `AT+RST`: Reset module
- `AT+CWMODE=1`: Set station mode
- `AT+CWJAP`: Connect to WiFi
- `AT+MQTTUSERCFG`: Configure MQTT client
- `AT+MQTTCONN`: Connect to MQTT broker
- `AT+MQTTSUB`: Subscribe to topic
- `AT+MQTTPUB`: Publish message

## Troubleshooting

### Common Issues
1. **ESP8266 not responding**: Check power supply and wiring
2. **WiFi connection failed**: Verify SSID and password
3. **MQTT connection failed**: Check broker IP and port
4. **LED not controlling**: Verify GPIO configuration and wiring

### Debug Tips
- Use UART terminal to monitor ESP8266 AT commands
- Check MQTT broker logs for connection status
- Verify WebSocket connection in browser console
- Use oscilloscope to check UART communication

## Function Reference

### ESP8266 Driver Functions
- `ESP8266_Init()`: Initialize ESP8266 module
- `ESP8266_ConnectWiFi()`: Connect to WiFi network
- `ESP8266_ConnectMQTT()`: Connect to MQTT broker
- `ESP8266_SubscribeMQTT()`: Subscribe to MQTT topic
- `ESP8266_PublishMQTT()`: Publish MQTT message

### Main Application Functions
- `LED_Control()`: Control LED state and publish status
- `ESP8266_OnMQTTMessageReceived()`: Handle incoming MQTT messages
- `HAL_UART_RxCpltCallback()`: Process received UART data

## License

This project is provided as-is for educational and development purposes.
