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
// Channel hopping configuration
int currentChannel = 1; // start on channel 1
const unsigned long HOP_NO_PACKET_MS = 1000;   // stay on channel while packets come <1s apart
const unsigned long HOP_INTERVAL_MS = 200;     // when no packets, hop every 200 ms
unsigned long lastHopMillis = 0;
volatile unsigned long lastPacketSeenMillis = 0;
volatile bool channelLocked = false; // locked to channel when packets seen

// Function to simulate a click with given force
void IRAM_ATTR doClick() { clickLength = CLICK_DURATION; }

// Callback function that will be executed when packets are received
void IRAM_ATTR promiscuousCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
    if (buf == NULL) return;

    // Record packet time and lock to current channel while packets arrive frequently
    lastPacketSeenMillis = millis();
    channelLocked = true;

    // Request a click (ISR-safe)
    doClick();
}

void setup() {
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
    // Ensure radio is on the starting channel
    esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
}

void loop() {
    // Main loop will be implemented later
    unsigned long now = millis();

    // Channel locking logic:
    // - If we have seen a packet recently (channelLocked==true) we remain on channel
    //   until HOP_NO_PACKET_MS elapses with no new packet.
    // - If unlocked, we hop periodically every HOP_INTERVAL_MS to the next channel.
    if (channelLocked) {
        if ((now - lastPacketSeenMillis) >= HOP_NO_PACKET_MS) {
            channelLocked = false;
            lastHopMillis = now;
        }
    } else {
        if ((now - lastHopMillis) >= HOP_INTERVAL_MS) {
            lastHopMillis = now;
            currentChannel++;
            if (currentChannel > 13) currentChannel = 1; // channels 1..13
            esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
        }
    }

    // LED click playback (driven by ISR via clickLength)
    if (clickLength > 0) {
        digitalWrite(LED_PIN, HIGH);
        clickLength--;
    } else {
        digitalWrite(LED_PIN, LOW);
    }

    delayMicroseconds(1);
}