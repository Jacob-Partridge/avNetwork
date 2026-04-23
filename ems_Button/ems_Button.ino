#include <SPI.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>

MCP_CAN CAN(9);   // CS pin for MCP2515

// ---------------------------
// Pin setup
// ---------------------------
const int EMERGENCY_BUTTON_PIN = 2;

// ---------------------------
// CAN setup
// ---------------------------
const unsigned int EMERGENCY_CAN_ID = 0x080;   // highest priority among your commands
const byte EMERGENCY_ALERT_CMD = 0x01;

// For button state tracking
int lastEmergencyButtonState = HIGH;

void setup()
{
    Serial.begin(115200);

    pinMode(EMERGENCY_BUTTON_PIN, INPUT_PULLUP);

    while (CAN_OK != CAN.begin(CAN_125KBPS))
    {
        Serial.println("CAN init fail, retrying...");
        delay(100);
    }

    Serial.println("CAN init ok!");
    Serial.println("Emergency sender ready.");
}

void loop()
{
    int emergencyButtonState = digitalRead(EMERGENCY_BUTTON_PIN);

    // Send only when button is first pressed
    if (emergencyButtonState == LOW && lastEmergencyButtonState == HIGH)
    {
        sendEmergencyAlert();
        Serial.println("Sent: EMERGENCY ALERT");
        delay(200);   // simple debounce
    }

    lastEmergencyButtonState = emergencyButtonState;
}

void sendEmergencyAlert()
{
    byte data[1];
    data[0] = EMERGENCY_ALERT_CMD;

    byte sendResult = CAN.sendMsgBuf(EMERGENCY_CAN_ID, 0, 1, data);

    if (sendResult == CAN_OK)
    {
        Serial.print("Message sent on CAN ID 0x");
        Serial.print(EMERGENCY_CAN_ID, HEX);
        Serial.print(" | Data: ");
        Serial.println(data[0], HEX);
    }
    else
    {
        Serial.println("Error sending emergency message.");
    }
}