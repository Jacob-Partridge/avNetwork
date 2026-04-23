#include <SPI.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>

MCP_CAN CAN(9);   // CS pin for MCP2515

// Simplified CAN IDs
const unsigned int EMERGENCY_CAN_ID = 0;
const unsigned int LIGHT_CAN_ID     = 1;
const unsigned int FAN_CAN_ID       = 2;
const unsigned int CURTAIN_CAN_ID   = 3;

void setup()
{
    Serial.begin(115200);

    while (CAN_OK != CAN.begin(CAN_125KBPS))
    {
        Serial.println("CAN init fail, retrying...");
        delay(100);
    }

    Serial.println("CAN init ok!");
    Serial.println("Receiver ready.");
}

void loop()
{
    if (CAN_MSGAVAIL == CAN.checkReceive())
    {
        unsigned char len = 0;
        unsigned char buf[8];

        CAN.readMsgBuf(&len, buf);
        unsigned int rxId = CAN.getCanId();

        Serial.print("Received CAN ID: ");
        Serial.print(rxId);
        Serial.print(" | Data: ");

        for (int i = 0; i < len; i++)
        {
            Serial.print(buf[i]);
            Serial.print(" ");
        }
        Serial.println();

        if (rxId == EMERGENCY_CAN_ID)
        {
            if (len > 0 && buf[0] == 0x01)
            {
                Serial.println("EMERGENCY ALERT RECEIVED");
            }
        }
        else if (rxId == LIGHT_CAN_ID)
        {
            if (len > 0 && buf[0] == 0x01)
            {
                Serial.println("LIGHT ON");
            }
            else if (len > 0 && buf[0] == 0x00)
            {
                Serial.println("LIGHT OFF");
            }
        }
        else if (rxId == FAN_CAN_ID)
        {
            if (len > 0 && buf[0] == 0x01)
            {
                Serial.println("FAN ON");
            }
            else if (len > 0 && buf[0] == 0x00)
            {
                Serial.println("FAN OFF");
            }
        }
        else if (rxId == CURTAIN_CAN_ID)
        {
            if (len > 0 && buf[0] == 0x01)
            {
                Serial.println("CURTAIN OPEN");
            }
            else if (len > 0 && buf[0] == 0x00)
            {
                Serial.println("CURTAIN CLOSE");
            }
        }
        else
        {
            Serial.println("Unknown message received.");
        }

        Serial.println();
    }
}