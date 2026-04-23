#include <SPI.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>

MCP_CAN CAN(9);   // CS pin for MCP2515

// ---------------------------
// Pin setup
// ---------------------------
const int LIGHT_ON_BUTTON_PIN  = 2;
const int LIGHT_OFF_BUTTON_PIN = 3;

// ---------------------------
// CAN setup
// ---------------------------
const unsigned int LIGHT_CAN_ID = 1;

// Data values for light commands
const byte LIGHT_ON_CMD  = 0x01;
const byte LIGHT_OFF_CMD = 0x00;

// For button state tracking
int lastLightOnButtonState  = HIGH;
int lastLightOffButtonState = HIGH;

void setup()
{
    Serial.begin(115200);

    pinMode(LIGHT_ON_BUTTON_PIN, INPUT_PULLUP);
    pinMode(LIGHT_OFF_BUTTON_PIN, INPUT_PULLUP);

    while (CAN_OK != CAN.begin(CAN_125KBPS))
    {
        Serial.println("CAN init fail, retrying...");
        delay(100);
    }

    Serial.println("CAN init ok!");
    Serial.println("Light sender ready.");
}

void loop()
{
    int lightOnButtonState  = digitalRead(LIGHT_ON_BUTTON_PIN);
    int lightOffButtonState = digitalRead(LIGHT_OFF_BUTTON_PIN);

    if (lightOnButtonState == LOW && lastLightOnButtonState == HIGH)
    {
        sendLightCommand(LIGHT_ON_CMD);
        Serial.println("Sent: LIGHT ON");
        delay(200);
    }

    if (lightOffButtonState == LOW && lastLightOffButtonState == HIGH)
    {
        sendLightCommand(LIGHT_OFF_CMD);
        Serial.println("Sent: LIGHT OFF");
        delay(200);
    }

    lastLightOnButtonState = lightOnButtonState;
    lastLightOffButtonState = lightOffButtonState;
}

void sendLightCommand(byte command)
{
    byte data[1];
    data[0] = command;

    byte sendResult = CAN.sendMsgBuf(LIGHT_CAN_ID, 0, 1, data);

    if (sendResult == CAN_OK)
    {
        Serial.print("Message sent on CAN ID 0x");
        Serial.print(LIGHT_CAN_ID, HEX);
        Serial.print(" | Data: ");
        Serial.println(data[0], HEX);
    }
    else
    {
        Serial.println("Error sending message.");
    }
}