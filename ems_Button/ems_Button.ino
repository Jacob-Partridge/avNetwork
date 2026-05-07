#include <SPI.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>

MCP_CAN CAN(9);   // CS pin for MCP2515

// ---------------------------
// Pin setup
// ---------------------------
const int EMERGENCY_ON_PIN  = 3;
const int EMERGENCY_OFF_PIN = 6;

// ---------------------------
// CAN setup
// ---------------------------
const unsigned int EMERGENCY_CAN_ID = 0x00;   // Highest priority
const byte EMERGENCY_ON_CMD  = 0x01;
const byte EMERGENCY_OFF_CMD = 0x00;

// For button state tracking
int lastOnState  = HIGH;
int lastOffState = HIGH;

void setup()
{
    Serial.begin(115200);

    pinMode(EMERGENCY_ON_PIN, INPUT_PULLUP);
    pinMode(EMERGENCY_OFF_PIN, INPUT_PULLUP);

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
    // Read current states
    int onState  = digitalRead(EMERGENCY_ON_PIN);
    int offState = digitalRead(EMERGENCY_OFF_PIN);

    // Check if "ON" button was just pressed
    if (onState == LOW && lastOnState == HIGH)
    {
        sendEmergencyCommand(EMERGENCY_ON_CMD);
        Serial.println("Sent: EMERGENCY ON");
        delay(200); // Debounce
    }

    // Check if "OFF" button was just pressed
    if (offState == LOW && lastOffState == HIGH)
    {
        sendEmergencyCommand(EMERGENCY_OFF_CMD);
        Serial.println("Sent: EMERGENCY OFF");
        delay(200); // Debounce
    }

    // Update last states for next loop
    lastOnState = onState;
    lastOffState = offState;
}

void sendEmergencyCommand(byte command)
{
    byte data[1];
    data[0] = command;

    // Send message: ID, Standard Frame (0), Data Length (1), Data Array
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