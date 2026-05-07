#include <SPI.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>

MCP_CAN CAN(9);   // CS pin for MCP2515

// ---------------------------
// Pin setup
// ---------------------------
const int EMERGENCY_ON_BUTTON_PIN  = 3;
const int EMERGENCY_OFF_BUTTON_PIN = 4;

// ---------------------------
// CAN setup
// ---------------------------
const unsigned int EMERGENCY_CAN_ID = 0x00;

const byte EMERGENCY_ALERT_ON_CMD  = 0x01;
const byte EMERGENCY_ALERT_OFF_CMD = 0x00;

// For button state tracking
int lastEmergencyOnButtonState  = HIGH;
int lastEmergencyOffButtonState = HIGH;

void setup()
{
    Serial.begin(115200);

    pinMode(EMERGENCY_ON_BUTTON_PIN, INPUT_PULLUP);
    pinMode(EMERGENCY_OFF_BUTTON_PIN, INPUT_PULLUP);

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
    int emergencyOnButtonState  = digitalRead(EMERGENCY_ON_BUTTON_PIN);
    int emergencyOffButtonState = digitalRead(EMERGENCY_OFF_BUTTON_PIN);

    // Send emergency ON
    if (emergencyOnButtonState == LOW && lastEmergencyOnButtonState == HIGH)
    {
        sendEmergencyCommand(EMERGENCY_ALERT_ON_CMD);
        Serial.println("Sent: EMERGENCY ON");
        delay(200);
    }

    // Send emergency OFF
    if (emergencyOffButtonState == LOW && lastEmergencyOffButtonState == HIGH)
    {
        sendEmergencyCommand(EMERGENCY_ALERT_OFF_CMD);
        Serial.println("Sent: EMERGENCY OFF");
        delay(200);
    }

    lastEmergencyOnButtonState = emergencyOnButtonState;
    lastEmergencyOffButtonState = emergencyOffButtonState;
}

void sendEmergencyCommand(byte command)
{
    byte data[1];
    data[0] = command;

    byte sendResult = CAN.sendMsgBuf(EMERGENCY_CAN_ID, 0, 1, data);

    if (sendResult == CAN_OK)
    {
        Serial.print("Message sent on CAN ID ");
        Serial.print(EMERGENCY_CAN_ID);
        Serial.print(" | Data: ");
        Serial.println(data[0]);
    }
    else
    {
        Serial.println("Error sending emergency message.");
    }
}
