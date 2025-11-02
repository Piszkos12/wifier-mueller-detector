#include <Arduino.h>
#include <WiFi.h>
#include <driver/rtc_io.h>
#include <esp_bt.h>
#include <esp_wifi.h>

// Define the built-in LED pin
const int LED_PIN = 2;  // Built-in LED on ESP32
const unsigned int CLICK_DURATION =
    10;  // Duration of the click in microseconds
volatile unsigned long lastPacketTime = 0;
volatile unsigned int clickLength = 0;

// Function to simulate a click with given force
void IRAM_ATTR doClick(uint8_t force) { clickLength = CLICK_DURATION; }

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
    if (clickLength > 0) {
        digitalWrite(LED_PIN, HIGH);
        clickLength--;
    } else {
        digitalWrite(LED_PIN, LOW);
    }

    delayMicroseconds(1);
}