/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : esp8266.h
  * @brief          : ESP8266 WiFi Module Driver Header
  ******************************************************************************
  * @attention
  *
  * ESP8266 AT Command Driver for STM32F4xx
  * Provides functions for WiFi connectivity and MQTT communication
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __ESP8266_H
#define __ESP8266_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

/* Exported types ------------------------------------------------------------*/
typedef enum {
    ESP8266_OK = 0,
    ESP8266_ERROR = 1,
    ESP8266_TIMEOUT = 2
} ESP8266_Status_t;

/* Exported constants --------------------------------------------------------*/
#define ESP8266_BUFFER_SIZE         512

/* WiFi Configuration */
#define WIFI_SSID                   "Your_WiFi_Network"
#define WIFI_PASSWORD               "Your_WiFi_Password"

/* MQTT Configuration */
#define MQTT_BROKER_IP              "192.168.1.XXX" //YOUR BROKER ID
#define MQTT_BROKER_PORT            1883
#define MQTT_CLIENT_ID              "STM32_LED_Controller"
#define MQTT_TOPIC_LED_CONTROL      "led/control"
#define MQTT_TOPIC_LED_STATUS       "led/status"

/* Exported variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart2;

/* Exported function prototypes ---------------------------------------------*/
ESP8266_Status_t ESP8266_Init(void);
ESP8266_Status_t ESP8266_ConnectWiFi(const char* ssid, const char* password);
ESP8266_Status_t ESP8266_ConnectMQTT(const char* broker_ip, uint16_t port, const char* client_id);
ESP8266_Status_t ESP8266_SubscribeMQTT(const char* topic);
ESP8266_Status_t ESP8266_PublishMQTT(const char* topic, const char* message);
ESP8266_Status_t ESP8266_SendCommand(const char* command, const char* expected_response, uint32_t timeout);
ESP8266_Status_t ESP8266_ProcessReceivedData(void);
void ESP8266_ClearBuffer(void);
void ESP8266_UART_RxCallback(uint8_t data);

/* Callback function prototypes */
void ESP8266_OnMQTTMessageReceived(const char* topic, const char* message);

#ifdef __cplusplus
}
#endif

#endif /* __ESP8266_H */
