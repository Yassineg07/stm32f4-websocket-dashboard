/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : esp8266.c
  * @brief          : ESP8266 WiFi Module Driver Implementation
  ******************************************************************************
  * @attention
  *
  * ESP8266 AT Command Driver for STM32F4xx
  * Provides functions for WiFi connectivity and MQTT communication
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "esp8266.h"
#include "main.h"

/* Private variables ---------------------------------------------------------*/
static char esp8266_buffer[ESP8266_BUFFER_SIZE];
static uint16_t esp8266_buffer_index = 0;
static bool mqtt_subscribed = false;

/* External variables --------------------------------------------------------*/
// All external variables are declared in main.h

/* Private function prototypes -----------------------------------------------*/
static void ESP8266_Delay(uint32_t ms);
static ESP8266_Status_t ESP8266_WaitForResponse(const char* expected, uint32_t timeout);
static void ESP8266_UART_Transmit(const char* data);
static void ESP8266_ProcessBuffer(void);

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize ESP8266 module
  * @retval ESP8266_Status_t
  */
ESP8266_Status_t ESP8266_Init(void)
{
    ESP8266_ClearBuffer();
    
    // Start UART interrupt receiving
    HAL_UART_Receive_IT(&huart2, uart_rx_buffer, 1);
    
    // Reset ESP8266 to ensure clean state
    ESP8266_UART_Transmit("AT+RST\r\n");

    HAL_Delay(5000);

    // Disconnect from any existing WiFi connection
    ESP8266_SendCommand("AT+CWQAP\r\n", "OK", 1000);

    // Simple initialization - just test basic communication
    if (ESP8266_SendCommand("AT\r\n", "OK", 1000) != ESP8266_OK) {
        return ESP8266_ERROR;
    }

    // Disable echo first to reduce noise
    if (ESP8266_SendCommand("ATE0\r\n", "OK", 1000) != ESP8266_OK) {
        return ESP8266_ERROR;
    }
    

    // Set WiFi mode to station mode
    if (ESP8266_SendCommand("AT+CWMODE=1\r\n", "OK", 1000) != ESP8266_OK) {
        return ESP8266_ERROR;
    }
    
    return ESP8266_OK;
}


/**
  * @brief  Connect to WiFi network
  * @param  ssid: WiFi network name
  * @param  password: WiFi password
  * @retval ESP8266_Status_t
  */
ESP8266_Status_t ESP8266_ConnectWiFi(const char* ssid, const char* password)
{
    char command[128];
    snprintf(command, sizeof(command), "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
    
    return ESP8266_SendCommand(command, "OK", 5000);
}

/**
  * @brief  Connect to MQTT broker
  * @param  broker_ip: MQTT broker IP address
  * @param  port: MQTT broker port
  * @param  client_id: MQTT client ID
  * @retval ESP8266_Status_t
  */
ESP8266_Status_t ESP8266_ConnectMQTT(const char* broker_ip, uint16_t port, const char* client_id)
{
    char command[256];
    
    // Set MQTT user config
    snprintf(command, sizeof(command), "AT+MQTTUSERCFG=0,1,\"%s\",\"\",\"\",0,0,\"\"\r\n", client_id);
    if (ESP8266_SendCommand(command, "OK", 2000) != ESP8266_OK) {
        return ESP8266_ERROR;
    }
    
    // Connect to MQTT broker
    snprintf(command, sizeof(command), "AT+MQTTCONN=0,\"%s\",%d,1\r\n", broker_ip, port);
    return ESP8266_SendCommand(command, "OK", 5000);
}

/**
  * @brief  Subscribe to MQTT topic
  * @param  topic: MQTT topic to subscribe
  * @retval ESP8266_Status_t
  */
ESP8266_Status_t ESP8266_SubscribeMQTT(const char* topic)
{
    char command[128];
    snprintf(command, sizeof(command), "AT+MQTTSUB=0,\"%s\",1\r\n", topic);
    ESP8266_Status_t status = ESP8266_SendCommand(command, "OK", 2000);
    
    if (status == ESP8266_OK) {
        mqtt_subscribed = true;
    }
    
    return status;
}

/**
  * @brief  Publish message to MQTT topic
  * @param  topic: MQTT topic
  * @param  message: Message to publish
  * @retval ESP8266_Status_t
  */
ESP8266_Status_t ESP8266_PublishMQTT(const char* topic, const char* message)
{
    char command[256];
    snprintf(command, sizeof(command), "AT+MQTTPUB=0,\"%s\",\"%s\",1,0\r\n", topic, message);
    return ESP8266_SendCommand(command, "OK", 1000);
}

/**
  * @brief  Send AT command and wait for response
  * @param  command: AT command to send
  * @param  expected_response: Expected response
  * @param  timeout: Timeout in milliseconds
  * @retval ESP8266_Status_t
  */
ESP8266_Status_t ESP8266_SendCommand(const char* command, const char* expected_response, uint32_t timeout)
{
    ESP8266_ClearBuffer();
    
    // Ensure UART RX interrupt is active
    if (huart2.RxState == HAL_UART_STATE_READY) {
        HAL_UART_Receive_IT(&huart2, uart_rx_buffer, 1);
    }
    
    ESP8266_UART_Transmit(command);
    return ESP8266_WaitForResponse(expected_response, timeout);
}

/**
  * @brief  Process received data from ESP8266
  * @retval ESP8266_Status_t
  */
ESP8266_Status_t ESP8266_ProcessReceivedData(void)
{
    // Only process MQTT messages if we're subscribed
    if (mqtt_subscribed) {
        ESP8266_ProcessBuffer();
    }
    return ESP8266_OK;
}

/**
  * @brief  Clear ESP8266 buffer
  * @retval None
  */
void ESP8266_ClearBuffer(void)
{
    memset(esp8266_buffer, 0, ESP8266_BUFFER_SIZE);
    esp8266_buffer_index = 0;
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Delay function
  * @param  ms: Delay in milliseconds
  * @retval None
  */
static void ESP8266_Delay(uint32_t ms)
{
    HAL_Delay(ms);
}

/**
  * @brief  Wait for expected response
  * @param  expected: Expected response string
  * @param  timeout: Timeout in milliseconds
  * @retval ESP8266_Status_t
  */
static ESP8266_Status_t ESP8266_WaitForResponse(const char* expected, uint32_t timeout)
{
    uint32_t start_time = HAL_GetTick();
    
    while ((HAL_GetTick() - start_time) < timeout) {
        // Check if we got the expected response
        if (strstr(esp8266_buffer, expected) != NULL) {
            return ESP8266_OK;
        }
        
        // Check for error responses
        if (strstr(esp8266_buffer, "ERROR") != NULL || 
            strstr(esp8266_buffer, "FAIL") != NULL) {
            return ESP8266_ERROR;
        }
        
        // Shorter delay to be more responsive
        ESP8266_Delay(10);
    }
    
    return ESP8266_TIMEOUT;
}

/**
  * @brief  Transmit data via UART
  * @param  data: Data to transmit
  * @retval None
  */
static void ESP8266_UART_Transmit(const char* data)
{
    // Use blocking transmit to ensure data is sent completely
    // before moving to the next command
    HAL_UART_Transmit(&huart2, (uint8_t*)data, strlen(data), 1000);
    
    // Small delay to ensure ESP8266 processes the command
    ESP8266_Delay(100);
}

/**
  * @brief  Process received buffer for MQTT messages
  * @retval None
  */
static void ESP8266_ProcessBuffer(void)
{
    // Check for MQTT message reception: +MQTTSUBRECV:0,"led/control",2,on
    char* mqtt_msg = strstr(esp8266_buffer, "+MQTTSUBRECV:");
    if (mqtt_msg != NULL) {
        // First, find the message content (after the last comma) BEFORE modifying the string
        char* msg_start = strrchr(mqtt_msg, ',');
        if (msg_start != NULL) {
            msg_start++; // Skip the comma
            
            // Now find the topic (first quoted string)
            char* topic_start = strchr(mqtt_msg, '"');
            if (topic_start != NULL) {
                topic_start++; // Skip the opening quote
                char* topic_end = strchr(topic_start, '"');
                if (topic_end != NULL) {
                    // Temporarily null terminate topic for copying
                    char saved_char = *topic_end;
                    *topic_end = '\0';
                    
                    // Remove any trailing whitespace/newlines from message
                    char* msg_end = msg_start + strlen(msg_start) - 1;
                    while (msg_end > msg_start && (*msg_end == '\r' || *msg_end == '\n' || *msg_end == ' ')) {
                        *msg_end = '\0';
                        msg_end--;
                    }
                    
                    // Make local copies to avoid buffer clearing issues
                    char topic_copy[64];
                    char message_copy[32];
                    strncpy(topic_copy, topic_start, sizeof(topic_copy) - 1);
                    topic_copy[sizeof(topic_copy) - 1] = '\0';
                    strncpy(message_copy, msg_start, sizeof(message_copy) - 1);
                    message_copy[sizeof(message_copy) - 1] = '\0';
                    
                    // Restore the original character (good practice)
                    *topic_end = saved_char;
                    

                    // Clear buffer BEFORE calling callback to prevent issues
                    //ESP8266_ClearBuffer();

                    // Call callback function with safe copies
                    ESP8266_OnMQTTMessageReceived(topic_copy, message_copy);
                    return; // Exit early since we've processed the message
                }
            }
        }
        
        // If we get here, parsing failed - still clear buffer
        ESP8266_ClearBuffer();
    }
}

/**
  * @brief  UART receive callback (to be called from interrupt)
  * @param  data: Received byte
  * @retval None
  */
void ESP8266_UART_RxCallback(uint8_t data)
{
    if (esp8266_buffer_index < ESP8266_BUFFER_SIZE - 1) {
        esp8266_buffer[esp8266_buffer_index++] = data;
        esp8266_buffer[esp8266_buffer_index] = '\0';
        
        // Process MQTT messages when we receive a complete line
        if (data == '\n' && mqtt_subscribed) {
            ESP8266_ProcessReceivedData();
        }
        // Also clear buffer periodically for AT command responses to prevent overflow
        else if (data == '\n' && !mqtt_subscribed) {
            // For regular AT commands, clear buffer after each line to prevent accumulation
            if (esp8266_buffer_index > 100) { // Keep some recent data for AT responses
                ESP8266_ClearBuffer();
            }
        }
    } else {
        // Buffer overflow - clear and start fresh
        ESP8266_ClearBuffer();
        esp8266_buffer[esp8266_buffer_index++] = data;
        esp8266_buffer[esp8266_buffer_index] = '\0';
    }
}

/**
  * @brief  Weak callback function for MQTT message reception
  * @param  topic: MQTT topic
  * @param  message: MQTT message
  * @retval None
  */
__weak void ESP8266_OnMQTTMessageReceived(const char* topic, const char* message)
{
    // This function should be implemented in main.c
    UNUSED(topic);
    UNUSED(message);
}


