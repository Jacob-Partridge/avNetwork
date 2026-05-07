#include <SPI.h>
#include <mcp_can.h>
#include <DHT.h>

// CAN Setup
MCP_CAN CAN(9); 
const unsigned int FAN_CAN_ID = 0x02;
const unsigned int CURTAIN_CAN_ID = 0x03;

// DHT Setup
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
const float TEMP_THRESHOLD_F = 75.0;

// Curtain Button Pins
const int CURTAIN_ON_PIN = 5;
const int CURTAIN_OFF_PIN = 7;

// Timing variables (to replace delay)
unsigned long lastTempCheck = 0;
const unsigned long tempInterval = 2000; 

// Button State Tracking
int lastOnState = HIGH;
int lastOffState = HIGH;

void setup() {
    Serial.begin(115200);
    dht.begin();

    // Initialize Buttons
    pinMode(CURTAIN_ON_PIN, INPUT_PULLUP);
    pinMode(CURTAIN_OFF_PIN, INPUT_PULLUP);

    while (CAN_OK != CAN.begin(CAN_125KBPS)) {
        Serial.println("CAN init fail, retrying...");
        delay(100);
    }

    Serial.println("Combined DHT11 & Curtain Sender Ready.");
}

void loop() {
    // --- 1. CURTAIN BUTTON LOGIC (Checked constantly) ---
    int currentOnRead = digitalRead(CURTAIN_ON_PIN);
    int currentOffRead = digitalRead(CURTAIN_OFF_PIN);

    // If ON button pressed
    if (currentOnRead == LOW && lastOnState == HIGH) {
        sendCANMessage(CURTAIN_CAN_ID, 0x01);
        Serial.println("Sent: CURTAIN ON (0x03)");
        delay(50); // Small debounce
    }

    // If OFF button pressed
    if (currentOffRead == LOW && lastOffState == HIGH) {
        sendCANMessage(CURTAIN_CAN_ID, 0x00);
        Serial.println("Sent: CURTAIN OFF (0x03)");
        delay(50); // Small debounce
    }

    lastOnState = currentOnRead;
    lastOffState = currentOffRead;


    // --- 2. TEMPERATURE LOGIC (Checked every 2 seconds) ---
    if (millis() - lastTempCheck >= tempInterval) {
        lastTempCheck = millis();

        float tempF = dht.readTemperature(true);

        if (isnan(tempF)) {
            Serial.println("Failed to read from DHT11.");
        } else {
            byte fanState = (tempF >= TEMP_THRESHOLD_F) ? 1 : 0;
            sendCANMessage(FAN_CAN_ID, fanState);

            Serial.print("Temp: ");
            Serial.print(tempF);
            Serial.print(" F | Sent Fan State: ");
            Serial.println(fanState);
        }
    }
}

// Universal function to send 1-byte CAN messages
void sendCANMessage(unsigned int id, byte value) {
    byte data[1];
    data[0] = value;
    
    byte result = CAN.sendMsgBuf(id, 0, 1, data);
    
    if (result != CAN_OK) {
        Serial.print("Error sending to ID 0x");
        Serial.println(id, HEX);
    }
}