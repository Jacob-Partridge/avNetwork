#include <SPI.h>
#include <mcp_can.h>
#include <Servo.h>

// CAN Chip Select pin
MCP_CAN CAN(9); 

// Pins
const int FAN_CURTAIN_PIN = 3; // The physical pin for the Servo
const int EMS_LED         = 6;
const int LIGHT           = 10;

// State Tracking
bool fanOn = false; 
bool curtainOn = false;
int servoPos = 0;
int direction = 1;
unsigned long lastMoveTime = 0;
const int fanSpeed = 15; // Lower is faster

Servo myServo;

void setup() {
    Serial.begin(115200);
    
    pinMode(EMS_LED, OUTPUT);
    pinMode(LIGHT, OUTPUT);
    digitalWrite(LIGHT, 0);
    digitalWrite(EMS_LED, 0);
    
    // Attach the servo to the correct pin defined above
    myServo.attach(FAN_CURTAIN_PIN);
    myServo.write(0); // Start at home position
    
    while (CAN_OK != CAN.begin(CAN_125KBPS)) {
        Serial.println("CAN init fail, retrying...");
        delay(100);
    }
    Serial.println("System Ready.");
}

void loop() {

    // --- 1. COMMS CHECK: CAN Bus ---
    if (CAN_MSGAVAIL == CAN.checkReceive()) {
        unsigned char len = 0;
        unsigned char buf[8];
        CAN.readMsgBuf(&len, buf);
        unsigned int rxId = CAN.getCanId();

        Serial.print(rxId);
        Serial.println(buf[0]);

        if (rxId == 0x00) { // Emergency
            if (buf[0] == 1 || buf[0] == 0x01) {
                digitalWrite(EMS_LED, HIGH);
                Serial.println("EMS turned ON");
            } 
            else if (buf[0] == 0 || buf[0] == 0x00) {
                digitalWrite(EMS_LED, LOW);
                Serial.println("EMS turned OFF");
            }
        } 
        else if (rxId == 0x01) { // Light ID
            if (buf[0] == 1 || buf[0] == 0x01) {
                digitalWrite(LIGHT, HIGH);
                Serial.println("LIGHT turned ON");
            } 
            else if (buf[0] == 0 || buf[0] == 0x00) {
                digitalWrite(LIGHT, LOW);
                Serial.println("LIGHT turned OFF");
            }
        }
        else if (rxId == 0x02) { // Fan
            fanOn = (buf[0] == 0x01);
        }
        else if (rxId == 0x03) { // Curtain
            curtainOn = (buf[0] == 0x01);
        }
    }

    // --- 2. EXECUTION: Dynamic Fan/Curtain Movement ---
    
    int minPos = 0;
    int maxPos = 0;
    bool active = true;

    // Set boundaries based on your requirements
    if (fanOn && curtainOn) {
        minPos = 0;
        maxPos = 180;
    } 
    else if (fanOn) {
        minPos = 90;
        maxPos = 180;
    } 
    else if (curtainOn) {
        minPos = 0;
        maxPos = 90;
    } 
    else {
        active = false; // Neither is true
    }

    if (active) {
        // Non-blocking timer
        if (millis() - lastMoveTime > fanSpeed) {
            lastMoveTime = millis();
            
            servoPos += direction;

            // Check boundaries and reverse direction
            if (servoPos >= maxPos) {
                servoPos = maxPos;
                direction = -1; // Move backward
            } 
            else if (servoPos <= minPos) {
                servoPos = minPos;
                direction = 1;  // Move forward
            }

            myServo.write(servoPos);
        }
    } 
    else {
        // Return to 0 if everything is OFF
        if (servoPos != 0) {
            servoPos = 0;
            myServo.write(servoPos);
            direction = 1; // Reset direction for next activation
        }
    }
}