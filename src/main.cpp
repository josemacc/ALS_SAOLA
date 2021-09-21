/* WiFi scan Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/event_groups.h"
#include "sdkconfig.h"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_pm.h>
#include "esp_log.h"
#include "LCD_ST7032.h"
#include "LCD_ST7032.cpp"
/*
* Variables
*
*/
const char* ssid = "MOXA";
const char* password = "";
#define CONFIG_ESP32_WIFI_DEBUG_LOG_ENABLE =0 
LCD_ST7032 lcd;
#if CONFIG_EXAMPLE_POWER_SAVE_MIN_MODEM
#define DEFAULT_PS_MODE WIFI_PS_MIN_MODEM
#elif CONFIG_EXAMPLE_POWER_SAVE_MAX_MODEM
#define DEFAULT_PS_MODE WIFI_PS_MAX_MODEM
#elif CONFIG_EXAMPLE_POWER_SAVE_NONE
#define DEFAULT_PS_MODE WIFI_PS_NONE
#else
#define DEFAULT_PS_MODE WIFI_PS_NONE
#endif /*CONFIG_POWER_SAVE_MODEM*/
/*
*  Funciones
*
*/

void scrollText(int column, int row, String message, int delayTime, int lcdColumns) {
  for (int i = 0; i < lcdColumns; i++) {
    message = " " + message;
  }
  message = message + " ";
  for (int pos = 0; pos < message.length(); pos++) {
    lcd.setCursor(column, row);
    lcd.print(message.substring(pos, pos + lcdColumns));
    delay(delayTime);
  }
}

/*void wifiScan() {
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    }
    Serial.println("");
}*/

#if !CONFIG_AUTOSTART_ARDUINO
void arduinoTask(void *pvParameter) {
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    Serial.begin(115200);
    delay(100);

    while(1) {
        wifiScan();

        // Wait a bit before scanning again
        delay(5000);
    }
}

void app_main()
{
    // initialize arduino library before we start the tasks
    initArduino();

    xTaskCreate(&arduinoTask, "arduino_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
}
#else

void setup() {
  esp_log_level_set("wifi", ESP_LOG_WARN);
  



  lcd.begin();
  lcd.setcontrast(30); 
  Serial.begin(115200);
  esp_pm_config_esp32_t PmConfig;
  PmConfig.max_freq_mhz = 240; //MHz
  PmConfig.min_freq_mhz = 80; //MHz
  PmConfig.light_sleep_enable = true;
  esp_err_t err = esp_pm_configure(&PmConfig);
  if(err == ESP_OK){
    Serial.println("ESP_OK");
  }else if (err== ESP_ERR_INVALID_ARG){
    Serial.println("ESP_ERR_INVALID_ARG");
  }else if (err == ESP_ERR_NOT_SUPPORTED){
    Serial.println("ESP_ERR_NOT_SUPPORTED");
  }
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
  Serial.println("Conectando a wifi");
  unsigned long nw= millis();
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  esp_wifi_set_ps(WIFI_PS_MIN_MODEM); //Setting power save mode on wifi
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //print_wifi_state(WIFI_FLAG);
  
  Serial.println("\nConectado a wifi, tiempo de conexión:" + String(millis()-nw) + " ms");
  Serial.print("\n\nConnected, IP address: ");
  Serial.println(WiFi.localIP());
  Serial.flush();

}
unsigned long tm = millis();
void loop() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Seconds="+String((millis()-tm)/1000));
    //Serial.print("\n\nConnected, IP address: ");
    //Serial.print("Seconds="+String((millis()-tm)/1000));
    //Serial.flush();
    // Wait a bit before scanning again
    delay(1000);
}
#endif 
