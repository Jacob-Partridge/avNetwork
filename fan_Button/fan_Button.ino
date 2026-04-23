#include <SPI.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>

MCP_CAN CAN(9);   // CS pin for MCP2515

// ---------------------------
// Pin setup
// ---------------------------
const int FAN_ON_BUTTON_PIN  = 2;
const int FAN_OFF_BUTTON_PIN = 3;

// ---------------------------
// CAN setup
// ---------------------------
const unsigned int FAN_CAN_ID = 2;

// Data values for fan commands
const byte FAN_ON_CMD  = 0x01;
const byte FAN_OFF_CMD = 0x00;

// For button state tracking
int lastFanOnButtonState  = HIGH;
int lastFanOffButtonState = HIGH;

void setup()
{
    Serial.begin(115200);

    pinMode(FAN_ON_BUTTON_PIN, INPUT_PULLUP);
    pinMode(FAN_OFF_BUTTON_PIN, INPUT_PULLUP);

    while (CAN_OK != CAN.begin(CAN_125KBPS))
    {
        Serial.println("CAN init fail, retrying...");
        delay(100);
    }

    Serial.println("CAN init ok!");
    Serial.println("Fan sender ready.");
}

void loop()
{
    int fanOnButtonState  = digitalRead(FAN_ON_BUTTON_PIN);
    int fanOffButtonState = digitalRead(FAN_OFF_BUTTON_PIN);

    if (fanOnButtonState == LOW && lastFanOnButtonState == HIGH)
    {
        sendFanCommand(FAN_ON_CMD);
        Serial.println("Sent: FAN ON");
        delay(200);
    }

    if (fanOffButtonState == LOW && lastFanOffButtonState == HIGH)
    {
        sendFanCommand(FAN_OFF_CMD);
        Serial.println("Sent: FAN OFF");
        delay(200);
    }

    lastFanOnButtonState = fanOnButtonState;
    lastFanOffButtonState = fanOffButtonState;
}

void sendFanCommand(byte command)
{
    byte data[1];
    data[0] = command;

    byte sendResult = CAN.sendMsgBuf(FAN_CAN_ID, 0, 1, data);

    if (sendResult == CAN_OK)
    {
        Serial.print("Message sent on CAN ID 0x");
        Serial.print(FAN_CAN_ID, HEX);
        Serial.print(" | Data: ");
        Serial.println(data[0], HEX);
    }
    else
    {
        Serial.println("Error sending message.");
    }
}