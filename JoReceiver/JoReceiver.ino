#include <SPI.h>
#include <mcp_can.h>
#include <Servo.h>

MCP_CAN CAN(9); 

// Pins
const int EMS_LED     = 6;
const int LIGHT       = 10;
const int CURTAIN     = 11;
const int FAN_PIN     = 3;

// State Tracking
bool fanOn = false; 
int servoPos = 0;
int direction = 1;
unsigned long lastMoveTime = 0;
const int fanSpeed = 15; 

Servo myServo;

void setup() {
    Serial.begin(115200);
    pinMode(EMS_LED, OUTPUT);
    pinMode(LIGHT, OUTPUT);
    pinMode(CURTAIN, OUTPUT);
    myServo.attach(FAN_PIN);
    
    while (CAN_OK != CAN.begin(CAN_125KBPS)) {
        Serial.println("CAN init fail, retrying...");
        delay(100);
    }
    Serial.println("System Ready.");
}

void loop() {

    // --- 2. COMMS CHECK: CAN Bus ---
    if (CAN_MSGAVAIL == CAN.checkReceive()) {
        unsigned char len = 0;
        unsigned char buf[8];
        CAN.readMsgBuf(&len, buf);
        unsigned int rxId = CAN.getCanId();

        if (rxId == 0x00) { // Emergency
            digitalWrite(EMS_LED, buf[0]);
        } 
        else if (rxId == 0x01) { // Light
            digitalWrite(LIGHT, buf[0]);
        } 
        else if (rxId == 0x02) { // Fan
            if (buf[0] == 0x01) fanOn = true;
            else if (buf[0] == 0x00) {
                fanOn = false; // Only turn off if temp is also safe
            }
        }
        else if (rxId == 0x03) { // Curtain
            digitalWrite(CURTAIN, LOW);
        }
    }

    // --- 3. EXECUTION: Fan Movement ---
    if (fanOn) {
        if (millis() - lastMoveTime > fanSpeed) {
            lastMoveTime = millis();
            servoPos += direction;
            myServo.write(servoPos);
        }
    } else {
        // Optional: Return fan to parked position when off
        if (servoPos != 0) {
            servoPos = 0;
            myServo.write(servoPos);
        }
    }
}