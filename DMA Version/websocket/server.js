const express = require('express');
const WebSocket = require('ws');
const mqtt = require('mqtt');
const path = require('path');

// Initialize Express for serving HTML
const app = express();
const port = 3000;

// Serve static files from 'public' folder
app.use(express.static(path.join(__dirname, 'public')));

// Start HTTP server
const server = app.listen(port, '0.0.0.0', () => {
    console.log(`Dashboard running at:`);
    console.log(`- Local: http://localhost:${port}`);
    console.log(`- Network: http://YOUR_PC_IP:${port}`);
    console.log(`Access from any device on your WiFi network!`);
});

// Initialize WebSocket server
const wss = new WebSocket.Server({ server });

// Connect to MQTT Broker (replace with your broker IP)
const mqttClient = mqtt.connect('mqtt://YOUR_PC_IP'); 

// Store connected WebSocket clients
const clients = new Set();

// Store last known LED state
let lastLedState = 'unknown';

// WebSocket Connection Handler
wss.on('connection', (ws) => {
    console.log('New WebSocket client connected');
    clients.add(ws);

    // Send initial connection message with current LED state
    ws.send(JSON.stringify({
        type: 'connection',
        status: 'connected'
    }));

    // Send current LED state immediately after connection
    if (lastLedState !== 'unknown') {
        ws.send(JSON.stringify({
            type: 'led_state',
            state: lastLedState
        }));
    } else {
        // Request current status from STM32 if we don't know the state
        mqttClient.publish('led/status_request', 'get_status');
        console.log('Requested current LED status from STM32');
    }

    // Handle incoming messages from browser
    ws.on('message', (message) => {
        try {
            const data = JSON.parse(message);
            
            if (data.type === 'control') {
                // Forward LED command to MQTT
                mqttClient.publish('led/control', data.state);
                console.log(`LED command: ${data.state}`);
            }
        } catch (error) {
            console.error('Invalid WebSocket message:', error);
        }
    });

    // Handle client disconnection
    ws.on('close', () => {
        clients.delete(ws);
        console.log('WebSocket client disconnected');
    });
});

// MQTT Message Handler (for receiving STM32 updates)
mqttClient.on('connect', () => {
    console.log('Connected to MQTT broker');
    mqttClient.subscribe('led/status'); // Subscribe to LED updates
    mqttClient.subscribe('led/status_request'); // Subscribe to status requests (optional)
});

mqttClient.on('message', (topic, message) => {
    if (topic === 'led/status') {
        // Update stored LED state
        const ledState = message.toString();
        lastLedState = ledState;
        
        console.log(`LED status update: ${ledState}`); // Debug log
        
        // Broadcast LED state to all WebSocket clients
        clients.forEach((client) => {
            if (client.readyState === WebSocket.OPEN) {
                client.send(JSON.stringify({
                    type: 'led_state',
                    state: ledState
                }));
            }
        });
    }
});