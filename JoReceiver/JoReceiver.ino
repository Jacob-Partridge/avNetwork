#include <SPI.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>

MCP_CAN CAN(9);   // CS pin for MCP2515

// Simplified CAN IDs
const unsigned int EMERGENCY_CAN_ID = 0x00;
const unsigned int LIGHT_CAN_ID     = 0x01;
const unsigned int FAN_CAN_ID       = 0x02;
const unsigned int CURTAIN_CAN_ID   = 0x03;

const unsigned int EMS_LED          = 9;
const unsigned int LIGHT            = 10;
const unsigned int CURTAIN          = 11;
const unsigned int FAN              = 6;
void setup()
{
    Serial.begin(115200);
    pinMode(6, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);
    pinMode(11, OUTPUT);
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
            if (buf[0] == 0x01)
                digitalWrite(EMS_LED, HIGH);
            
            else if (buf[0] == 0x00)
                digitalWrite(EMS_LED, LOW);
        }

        else if (rxId == LIGHT_CAN_ID)
        {
            if (len > 0 && buf[0] == 0x01)
                digitalWrite(LIGHT, HIGH);

            else if (len > 0 && buf[0] == 0x00)
                digitalWrite(LIGHT, LOW);
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
                digitalWrite(CURTAIN, HIGH);
            }
            else if (len > 0 && buf[0] == 0x00)
            {
                digitalWrite(CURTAIN, LOW);
            }
        }
        else
        {
            Serial.println("Unknown message received.");
        }

        Serial.println();
    }
}