# STM32 ESP8266 MQTT LED Controller

A professional STM32F4 embedded system that controls an LED via MQTT messages through an ESP8266 WiFi module, featuring a modern web dashboard for real-time LED control. **The ESP8266 driver is general-purpose and can be easily integrated into any STM32 project.**

## 🚀 Features

- **STM32F4 Microcontroller**: Robust embedded system with efficient DMA-based UART communication
- **ESP8266 WiFi Module**: Reliable WiFi connectivity with AT command interface
- **General-Purpose Driver**: Reusable ESP8266 driver with no project-specific dependencies
- **MQTT Protocol**: Lightweight messaging for IoT applications
- **Modern Web Dashboard**: Real-time LED control with animated toggle switch
- **DMA Implementation**: Interrupt-free, efficient data processing
- **Configurable UART**: Flexible driver supporting multiple UART peripherals
- **Clean Architecture**: Driver separated from application logic for maximum reusability

## 📋 Table of Contents

- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Architecture](#architecture)
- [Installation](#installation)
- [Configuration](#configuration)
- [Usage](#usage)
- [API Reference](#api-reference)
- [Web Dashboard](#web-dashboard)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)

## 🛠️ Hardware Requirements

### STM32F4 Development Board
- **Microcontroller**: STM32F407VGT6 (or compatible STM32F4 series)
- **RAM**: 192KB minimum
- **Flash**: 1MB minimum
- **GPIO**: For LED control (PD15 used in this implementation)
- **UART**: For ESP8266 communication (UART2 used by default)
- **DMA**: For efficient UART data transfer

### ESP8266 WiFi Module
- **Module**: ESP8266 (ESP-01, ESP-12E, or similar)
- **Firmware**: AT command firmware v2.0 or later
- **Power**: 3.3V supply (stable, adequate current)
- **Connections**:
  - VCC → 3.3V
  - GND → Ground
  - TX → STM32 UART2 RX (PA3)
  - RX → STM32 UART2 TX (PA2)
  - CH_PD → 3.3V (pull-up)

### Additional Components
- **LED**: Connected to PD15 (with appropriate current limiting resistor)
- **MQTT Broker**: Mosquitto or any MQTT broker
- **Network**: WiFi router/access point

## 💻 Software Requirements

### Development Environment
- **STM32CubeIDE**: v1.14.1 or later
- **STM32CubeMX**: For peripheral configuration
- **HAL Library**: STM32F4xx HAL drivers

### Runtime Environment
- **Node.js**: v16.0+ (for web dashboard server)
- **MQTT Broker**: Mosquitto or similar
- **Web Browser**: Modern browser with WebSocket support

## 🏗️ Architecture

```
┌─────────────────┐    UART/DMA     ┌─────────────────┐    WiFi     ┌─────────────────┐
│   STM32F407     │ ◄─────────────► │    ESP8266      │ ◄─────────► │   WiFi Router   │
│                 │                 │                 │             │                 │
│ • Main Logic    │                 │ • AT Commands   │             │ • DHCP Server   │
│ • DMA Handler   │                 │ • WiFi Stack    │             │ • NAT           │
│ • LED Control   │                 │ • TCP/IP Stack  │             │                 │
│ • MQTT Client   │                 │                 │             │                 │
└─────────────────┘                 └─────────────────┘             └─────────────────┘
         │                                   │                               │
         │                                   │                               │
         │                              MQTT over TCP                        │
         │                                   │                               │
         │                                   └───────────────────────────────┘
         │                                                                   │
         │                                                                   │
         │                                                                   ▼
         │                                                          ┌─────────────────┐
         │                                                          │   MQTT Broker   │
         │                                                          │                 │
         │                                                          │ • Mosquitto     │
         │                                                          │ • Topic Routing │
         │                                                          │ • Message Queue │
         │                                                          └─────────────────┘
         │                                                                   │
         │                                                                   │
         │                                                              WebSocket
         │                                                                   │
         │                                                                   ▼
         │                                                          ┌─────────────────┐
         │                                                          │   Web Server    │
         │                                                          │                 │
         │                                                          │ • Node.js       │
         │                                                          │ • WebSocket     │
         │                                                          │ • Static Files  │
         │                                                          └─────────────────┘
         │                                                                   │
         │                                                                   │
         │                                                               HTTP/WS
         │                                                                   │
         │                                                                   ▼
         │                                                          ┌─────────────────┐
         │                                                          │  Web Dashboard  │
         │                                                          │                 │
         │                                                          │ • HTML/CSS/JS   │
         │                                                          │ • Toggle Switch │
         │                                                          │ • Real-time UI  │
         │                                                          └─────────────────┘
         │
         ▼
┌─────────────────┐
│      LED        │
│                 │
│ • Visual Status │
│ • ON/OFF State  │
└─────────────────┘
```

## 📥 Installation

### 1. Clone the Repository
```bash
git clone https://github.com/yourusername/stm32-esp8266-mqtt-led-controller.git
cd stm32-esp8266-mqtt-led-controller
```

### 2. STM32 Project Setup
1. Open STM32CubeIDE
2. Import the project folder
3. Configure your target STM32F4 board
4. Build and flash the firmware

### 3. Web Dashboard Setup
```bash
cd websocket
npm install
```

### 4. MQTT Broker Setup
#### Using Mosquitto (Ubuntu/Debian):
```bash
sudo apt-get install mosquitto mosquitto-clients
sudo systemctl start mosquitto
sudo systemctl enable mosquitto
```

#### Using Docker:
```bash
docker run -it -p 1883:1883 eclipse-mosquitto
```

## ⚙️ Configuration

### 1. WiFi Configuration
Edit `Core/Src/main.c`:
```c
/* WiFi Configuration */
#define WIFI_SSID                   "Your_WiFi_SSID"
#define WIFI_PASSWORD               "Your_WiFi_Password"
```

### 2. MQTT Configuration
Edit `Core/Src/main.c`:
```c
/* MQTT Configuration */
#define MQTT_BROKER_IP              "192.168.1.100"  // Your MQTT broker IP
#define MQTT_BROKER_PORT            1883
#define MQTT_CLIENT_ID              "STM32_LED_Controller"
#define MQTT_TOPIC_LED_CONTROL      "led/control"
#define MQTT_TOPIC_LED_STATUS       "led/status"
```

### 3. UART Configuration
The driver supports any UART peripheral. In `main.c`:
```c
// Initialize ESP8266 with your chosen UART
ESP8266_Init(&huart2);  // or &huart1, &huart3, etc.
```

### 4. Web Server Configuration
Edit `websocket/server.js`:
```javascript
const MQTT_BROKER_IP = '192.168.1.100';  // Your MQTT broker IP
const WEB_SERVER_PORT = 3000;
```

## 🎮 Usage

### 1. Start the System
1. **Flash STM32**: Build and flash the firmware to your STM32F4 board
2. **Power On**: Ensure both STM32 and ESP8266 are powered
3. **Start MQTT Broker**: Ensure your MQTT broker is running
4. **Start Web Server**:
   ```bash
   cd websocket
   node server.js
   ```

### 2. Access Web Dashboard
Open your browser and navigate to:
- **Local**: `http://localhost:3000`
- **Network**: `http://[your-pc-ip]:3000`

### 3. Control LED
- Use the animated toggle switch on the web dashboard
- Or send MQTT messages directly:
  ```bash
  # Turn LED ON
  mosquitto_pub -h localhost -t led/control -m "on"
  
  # Turn LED OFF
  mosquitto_pub -h localhost -t led/control -m "off"
  ```

### 4. Monitor Status
- LED status is published to `led/status` topic
- Real-time updates appear on the web dashboard
- Console logs show all MQTT traffic

## 📚 API Reference

### ESP8266 Driver Functions

#### `ESP8266_Init(UART_HandleTypeDef* huart)`
Initialize ESP8266 module with specified UART peripheral.
- **Parameters**: `huart` - Pointer to UART handle
- **Returns**: `ESP8266_Status_t` - Success/Error status
- **Example**:
  ```c
  if (ESP8266_Init(&huart2) == ESP8266_OK) {
      // Initialization successful
  }
  ```

#### `ESP8266_ConnectWiFi(const char* ssid, const char* password)`
Connect to WiFi network.
- **Parameters**: 
  - `ssid` - WiFi network name
  - `password` - WiFi password
- **Returns**: `ESP8266_Status_t`
- **Example**:
  ```c
  ESP8266_ConnectWiFi("MyWiFi", "MyPassword");
  ```

#### `ESP8266_ConnectMQTT(const char* broker_ip, uint16_t port, const char* client_id)`
Connect to MQTT broker.
- **Parameters**:
  - `broker_ip` - MQTT broker IP address
  - `port` - MQTT broker port (usually 1883)
  - `client_id` - Unique client identifier
- **Returns**: `ESP8266_Status_t`

#### `ESP8266_SubscribeMQTT(const char* topic)`
Subscribe to MQTT topic.
- **Parameters**: `topic` - MQTT topic string
- **Returns**: `ESP8266_Status_t`

#### `ESP8266_PublishMQTT(const char* topic, const char* message)`
Publish message to MQTT topic.
- **Parameters**:
  - `topic` - MQTT topic string
  - `message` - Message content
- **Returns**: `ESP8266_Status_t`

#### `ESP8266_ProcessDMAData(void)`
Process incoming DMA data (call regularly in main loop).
- **Parameters**: None
- **Returns**: None

### AT Command Definitions

All AT commands are defined in `esp8266.h` for easy modification:

```c
#define AT_CMD_TEST                 "AT\r\n"
#define AT_CMD_RESET                "AT+RST\r\n"
#define AT_CMD_ECHO_OFF             "ATE0\r\n"
#define AT_CMD_WIFI_MODE_STA        "AT+CWMODE=1\r\n"
#define AT_CMD_WIFI_CONNECT         "AT+CWJAP=\"%s\",\"%s\"\r\n"
#define AT_CMD_MQTT_USER_CFG        "AT+MQTTUSERCFG=0,1,\"%s\",\"\",\"\",0,0,\"\"\r\n"
#define AT_CMD_MQTT_CONNECT         "AT+MQTTCONN=0,\"%s\",%d,1\r\n"
#define AT_CMD_MQTT_SUBSCRIBE       "AT+MQTTSUB=0,\"%s\",1\r\n"
#define AT_CMD_MQTT_PUBLISH         "AT+MQTTPUB=0,\"%s\",\"%s\",1,0\r\n"
```

## 🌐 Web Dashboard

The web dashboard provides a modern, responsive interface for LED control:

### Features
- **Animated Toggle Switch**: Smooth transitions and visual feedback
- **Real-time Status**: Live LED state updates via WebSocket
- **Connection Status**: Visual indicator for system connectivity
- **Responsive Design**: Works on desktop and mobile devices
- **Professional UI**: Clean, modern styling with CSS animations

### Technical Details
- **Frontend**: HTML5, CSS3, Vanilla JavaScript
- **Backend**: Node.js with WebSocket server
- **Communication**: WebSocket for real-time updates
- **MQTT Integration**: Seamless bridge between web and MQTT

## 🔧 Driver Portability

The ESP8266 driver (`Core/Src/esp8266.c` and `Core/Inc/esp8266.h`) is designed to be **completely general-purpose** and can be easily integrated into any STM32 project:

### Driver Features
- **Zero project dependencies**: No hardcoded WiFi/MQTT settings
- **Configurable UART**: Works with any UART peripheral
- **Clean API**: Simple function interface for WiFi and MQTT operations
- **Callback-based**: Uses weak functions for easy customization
- **STM32 HAL compatible**: Works with any STM32 series using HAL

### Integration Steps
1. **Copy driver files** to your project:
   - `Core/Src/esp8266.c`
   - `Core/Inc/esp8266.h`

2. **Add your configuration** in `main.c`:
   ```c
   #define WIFI_SSID                   "Your_Network"
   #define WIFI_PASSWORD               "Your_Password"
   #define MQTT_BROKER_IP              "192.168.1.100"
   #define MQTT_BROKER_PORT            1883
   #define MQTT_CLIENT_ID              "Your_Client_ID"
   ```

3. **Initialize and use**:
   ```c
   ESP8266_Init(&huart2);  // Your UART handle
   ESP8266_ConnectWiFi(WIFI_SSID, WIFI_PASSWORD);
   ESP8266_ConnectMQTT(MQTT_BROKER_IP, MQTT_BROKER_PORT, MQTT_CLIENT_ID);
   ```

4. **Implement callback** for MQTT messages:
   ```c
   void ESP8266_OnMQTTMessageReceived(const char* topic, const char* message) {
       // Your custom message handling logic
   }
   ```

## 🔧 Troubleshooting

### Common Issues

#### 1. ESP8266 Not Responding
- **Symptoms**: `ESP8266_Init()` returns `ESP8266_ERROR`
- **Solutions**:
  - Check power supply (stable 3.3V)
  - Verify UART connections (TX↔RX, RX↔TX)
  - Ensure CH_PD is pulled high
  - Check AT firmware version

#### 2. WiFi Connection Failed
- **Symptoms**: `ESP8266_ConnectWiFi()` returns `ESP8266_ERROR`
- **Solutions**:
  - Verify SSID and password
  - Check WiFi signal strength
  - Ensure 2.4GHz network (ESP8266 doesn't support 5GHz)
  - Check router settings (WPA2 recommended)

#### 3. MQTT Connection Issues
- **Symptoms**: `ESP8266_ConnectMQTT()` returns `ESP8266_ERROR`
- **Solutions**:
  - Verify broker IP address and port
  - Check broker is running and accessible
  - Ensure firewall allows MQTT traffic
  - Check client ID uniqueness

#### 4. Web Dashboard Not Loading
- **Symptoms**: Browser shows connection error
- **Solutions**:
  - Verify Node.js server is running
  - Check port 3000 is available
  - Ensure MQTT broker is accessible
  - Check browser console for errors

### Debug Tools

#### 1. STM32 Debug
```c
// Add to main.c for debugging
void Debug_Print_Buffer(void) {
    // Set breakpoint and examine esp8266_dma_buffer
    volatile uint8_t debug_buffer[32];
    memcpy((void*)debug_buffer, esp8266_dma_buffer, 32);
    __NOP(); // Breakpoint here
}
```

#### 2. MQTT Debug
```bash
# Monitor all MQTT traffic
mosquitto_sub -h localhost -v -t '#'

# Test LED control manually
mosquitto_pub -h localhost -t led/control -m "on"
mosquitto_pub -h localhost -t led/control -m "off"
```

#### 3. Web Server Debug
```javascript
// In server.js, enable verbose logging
console.log('MQTT Message:', topic, message.toString());
console.log('WebSocket clients:', clients.size);
```

## 🤝 Contributing

We welcome contributions! Please follow these steps:

1. **Fork the Repository**
2. **Create Feature Branch**: `git checkout -b feature/amazing-feature`
3. **Commit Changes**: `git commit -m 'Add amazing feature'`
4. **Push to Branch**: `git push origin feature/amazing-feature`
5. **Open Pull Request**

### Development Guidelines
- Follow existing code style and formatting
- Add comments for complex logic
- Test thoroughly on hardware
- Update documentation as needed
- Ensure compatibility with STM32F4 series

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **STMicroelectronics** for STM32 HAL libraries
- **Espressif Systems** for ESP8266 AT firmware
- **Eclipse Mosquitto** for MQTT broker
- **Node.js Community** for excellent WebSocket libraries

## 📧 Contact

For questions, issues, or contributions:
- **GitHub Issues**: [Project Issues](https://github.com/yourusername/stm32-esp8266-mqtt-led-controller/issues)
- **Email**: your.email@example.com
- **Documentation**: [Wiki](https://github.com/yourusername/stm32-esp8266-mqtt-led-controller/wiki)

---

**Made with ❤️ for the STM32 and IoT community**
