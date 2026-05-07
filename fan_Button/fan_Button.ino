#include <SPI.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <DHT.h>
#include <DHT_U.h>

MCP_CAN CAN(9);

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const unsigned int FAN_CAN_ID = 2;
const float TEMP_THRESHOLD_F = 78.0;

void setup()
{
    Serial.begin(115200);
    dht.begin();

    while (CAN_OK != CAN.begin(CAN_125KBPS))
    {
        Serial.println("CAN init fail, retrying...");
        delay(100);
    }

    Serial.println("DHT11 continuous CAN sender ready.");
}

void loop()
{
    float tempF = dht.readTemperature(true);

    if (isnan(tempF))
    {
        Serial.println("Failed to read from DHT11.");
        delay(2000);
        return;
    }

    byte fanState;

    if (tempF >= TEMP_THRESHOLD_F)
    {
        fanState = 1;
    }
    else
    {
        fanState = 0;
    }

    sendFanState(fanState);

    Serial.print("Temp: ");
    Serial.print(tempF);
    Serial.print(" F | Sent fan state: ");
    Serial.println(fanState);

    delay(2000);
}

void sendFanState(byte state)
{
    byte data[1];
    data[0] = state;

    byte result = CAN.sendMsgBuf(FAN_CAN_ID, 0, 1, data);

    if (result != CAN_OK)
    {
        Serial.println("Error sending CAN message.");
    }
}
