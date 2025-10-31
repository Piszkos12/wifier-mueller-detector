#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_bt.h>
#include <driver/rtc_io.h>

// Define the built-in LED pin
const int LED_PIN = 2;  // Built-in LED on ESP32
volatile unsigned long lastPacketTime = 0;

// Function to simulate a click with given force
void IRAM_ATTR doClick(uint8_t force) {
    digitalWrite(LED_PIN, HIGH);
    ets_delay_us(50);  // Shorter flash duration, using esp-idf delay
    digitalWrite(LED_PIN, LOW);
    
    // Only print debug info if needed
    if (force > 50) {  // Only log stronger signals
        Serial.print("Click force: ");
        Serial.println(force);
    }
}

// Callback function that will be executed when packets are received
void promiscuousCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
    if (buf == NULL) return;

    // Cast the received buffer to wifi_promiscuous_pkt_t to get RSSI
    const wifi_promiscuous_pkt_t* packet = (wifi_promiscuous_pkt_t*)buf;

    // We only process the RSSI value, ignoring all other packet data
    int8_t rssi = packet->rx_ctrl.rssi;

    // TODO: We'll use this RSSI value to trigger sound based on signal strength
    Serial.println(rssi);  // Debug output, will be replaced with sound logic

    // Map RSSI (dBm) to 0-100% where -100 dBm => 0% and -30 dBm => 100%
    const int RSSI_MIN = -100;
    const int RSSI_MAX = -30;
    int percent = map((int)rssi, RSSI_MIN, RSSI_MAX, 0, 100);
    percent = constrain(percent, 0, 100);
    uint8_t packetForce = (uint8_t)percent;

    doClick(packetForce);
}

void setup() {
    // Initialize Serial for debugging
    Serial.begin(115200);
    
    // Disable Bluetooth to save power
    esp_bt_controller_disable();
    
    // Configure LED pin
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);  // Start with LED off

    // Initialize WiFi in Station mode with reduced power
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    esp_wifi_set_ps(WIFI_PS_MIN_MODEM);  // Set power save mode
    
    // Wait a bit for WiFi settings to stabilize
    delay(100);

    // Configure Promiscuous Mode
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&promiscuousCallback);

    Serial.println("Promiscuous mode enabled");
}

void loop() {
    // Main loop will be implemented later
    delay(10);
}