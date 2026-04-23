#include <SPI.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>

MCP_CAN CAN(9);   // CS pin for MCP2515

// ---------------------------
// Pin setup
// ---------------------------
const int OPEN_BUTTON_PIN  = 2;
const int CLOSE_BUTTON_PIN = 3;

// ---------------------------
// CAN setup
// ---------------------------
const unsigned int CURTAIN_CAN_ID = 3;

// Data values for curtain commands
const byte CURTAIN_OPEN_CMD  = 0x01;
const byte CURTAIN_CLOSE_CMD = 0x00;

// For button state tracking
int lastOpenButtonState  = HIGH;
int lastCloseButtonState = HIGH;

void setup()
{
    Serial.begin(115200);

    pinMode(OPEN_BUTTON_PIN, INPUT_PULLUP);
    pinMode(CLOSE_BUTTON_PIN, INPUT_PULLUP);

    // Initialize CAN
    while (CAN_OK != CAN.begin(CAN_125KBPS))
    {
        Serial.println("CAN init fail, retrying...");
        delay(100);
    }

    Serial.println("CAN init ok!");
    Serial.println("Curtain sender ready.");
}

void loop()
{
    int openButtonState  = digitalRead(OPEN_BUTTON_PIN);
    int closeButtonState = digitalRead(CLOSE_BUTTON_PIN);

    // Detect button press for OPEN
    if (openButtonState == LOW && lastOpenButtonState == HIGH)
    {
        sendCurtainCommand(CURTAIN_OPEN_CMD);
        Serial.println("Sent: CURTAIN OPEN");
        delay(200);  // simple debounce
    }

    // Detect button press for CLOSE
    if (closeButtonState == LOW && lastCloseButtonState == HIGH)
    {
        sendCurtainCommand(CURTAIN_CLOSE_CMD);
        Serial.println("Sent: CURTAIN CLOSE");
        delay(200);  // simple debounce
    }

    lastOpenButtonState = openButtonState;
    lastCloseButtonState = closeButtonState;
}

void sendCurtainCommand(byte command)
{
    byte data[1];
    data[0] = command;

    byte sendResult = CAN.sendMsgBuf(CURTAIN_CAN_ID, 0, 1, data);

    if (sendResult == CAN_OK)
    {
        Serial.print("Message sent on CAN ID 0x");
        Serial.print(CURTAIN_CAN_ID, HEX);
        Serial.print(" | Data: ");
        Serial.println(data[0], HEX);
    }
    else
    {
        Serial.println("Error sending message.");
    }
}