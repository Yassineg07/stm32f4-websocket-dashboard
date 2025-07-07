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
const server = app.listen(port, () => {
    console.log(`Dashboard running at http://localhost:${port}`);
});

// Initialize WebSocket server
const wss = new WebSocket.Server({ server });

// Connect to MQTT Broker (replace with your broker IP)
const mqttClient = mqtt.connect('mqtt://192.168.1.29'); 

// Store connected WebSocket clients
const clients = new Set();

// WebSocket Connection Handler
wss.on('connection', (ws) => {
    console.log('New WebSocket client connected');
    clients.add(ws);

    // Send initial connection message
    ws.send(JSON.stringify({
        type: 'connection',
        status: 'connected'
    }));

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
});

mqttClient.on('message', (topic, message) => {
    if (topic === 'led/status') {
        // Broadcast LED state to all WebSocket clients
        const ledState = message.toString();
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