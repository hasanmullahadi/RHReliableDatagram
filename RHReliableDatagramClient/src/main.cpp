#include <Arduino.h>

#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>

// Define LoRa hardware pins
#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 16
#define RF95_FREQ 433.0  // Frequency in MHz (match with your hardware specs)

// Define addresses for client-server communication
#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Create LoRa driver instance with specified pins
RH_RF95 rf95(LORA_SS, LORA_DIO0);  // CS and interrupt pins
RHReliableDatagram manager(rf95, CLIENT_ADDRESS);  // My address

void setup() {
    Serial.begin(9600);
    delay(1000);

    // Initialize LoRa module
    pinMode(LORA_RST, OUTPUT);
    digitalWrite(LORA_RST, HIGH);  // Ensure LoRa is not in reset mode
    delay(10);
    digitalWrite(LORA_RST, LOW);   // Reset LoRa module
    delay(10);
    digitalWrite(LORA_RST, HIGH);  // Take LoRa out of reset
    delay(10);

    if (!rf95.init()) {
        Serial.println("LoRa init failed. Check your connections.");
        while (1);  // Halt
    }

    // Set frequency and power output
    rf95.setFrequency(RF95_FREQ);
    rf95.setTxPower(13, false);  // Set power output (13 dBm is a common value)

    
    if (!manager.init())
        Serial.println("init failed");

    Serial.println("LoRa initialized with reliable communication.");
}

void loop() {
    // Message to send to the server
    const char* message = "Hello from Client!";
    if (manager.sendtoWait((uint8_t*)message, strlen(message), SERVER_ADDRESS)) {
        Serial.println("Message sent to server.");

        // Wait for acknowledgment
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);
        if (manager.recvfromAckTimeout(buf, &len, 2000)) {  // 2-second timeout
            Serial.print("Acknowledgment received: ");
            Serial.println((char*)buf);
        } else {
            Serial.println("No acknowledgment from server.");
        }
    } else {
        Serial.println("Failed to send message to server.");
    }

    delay(5000);  // Send a message every 5 seconds
}