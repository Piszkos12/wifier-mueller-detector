#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_bt.h>

const int LED_PIN = 2;
const unsigned int CLICK_DURATION = 10;
volatile unsigned int clickLength = 0;

// Channel hopping parameters
int currentChannel = 1;
const unsigned long HOP_NO_PACKET_MS = 1000;   // Lock timeout
const unsigned long HOP_INTERVAL_MS = 200;     // Hop interval
unsigned long lastHopMillis = 0;
volatile unsigned long lastPacketSeenMillis = 0;
volatile bool channelLocked = false;

void IRAM_ATTR doClick() { 
    clickLength = CLICK_DURATION; 
}

void IRAM_ATTR promiscuousCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
    lastPacketSeenMillis = millis();
    channelLocked = true;
    doClick();
}

void setup() {
    esp_bt_controller_disable();
    pinMode(LED_PIN, OUTPUT);
    
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    esp_wifi_set_ps(WIFI_PS_MIN_MODEM);
    delay(100);

    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&promiscuousCallback);
    esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
}

void loop() {
    unsigned long now = millis();

    if (channelLocked) {
        if ((now - lastPacketSeenMillis) >= HOP_NO_PACKET_MS) {
            channelLocked = false;
            lastHopMillis = now;
        }
    } else {
        if ((now - lastHopMillis) >= HOP_INTERVAL_MS) {
            lastHopMillis = now;
            currentChannel = (currentChannel % 13) + 1;
            esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
        }
    }

    if (clickLength > 0) {
        digitalWrite(LED_PIN, HIGH);
        clickLength--;
    } else {
        digitalWrite(LED_PIN, LOW);
    }

    delayMicroseconds(1);
}