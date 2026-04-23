#include <SPI.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>

// demo: CAN-BUS Shield, receive data with check mode
// send data coming to fast, such as less than 10ms, you can use this way
// 80, 100, 200, 300 are CAN IDs
unsigned char Flag_Recv = 0;
unsigned char len = 0;
unsigned char buf[8];
char str[20];
// int fan_pin= ;
// int currtain_pin= ;
// int ems_pin= ;
// int light_pin= ;

MCP_CAN CAN(9); // Set CS to pin 9

void setup() 
{
  Serial.begin(115200);

  START_INIT:

  if(CAN_OK == CAN.begin(CAN_125KBPS)) // init can bus : baudrate = 500k
  {
    Serial.println("CAN BUS Shield init ok!");
    CAN.init_Mask(0,0,0x000);
    CAN.init_Filt(0,0,0x7FC);
    pinMode(fan_pin, OUTPUT);
    pinMode(currtain_pin, OUTPUT);
    pinMode(ems_pin, OUTPUT);
    pinMode(light_pin, OUTPUT);
  }

  else
  {
    Serial.println("CAN BUS Shield init fail");
    Serial.println("Init CAN BUS Shield again");
    delay(100);
    goto START_INIT;
  }

  CAN.init_Mask(0,0,0x7FC);
  CAN.init_Filt(1,0,0x000)
}

void loop(){ 
  if(CAN_MSGAVAIL == CAN.checkReceive()) // check if data coming
  {
    CAN.readMsgBuf(&len, buf); // read data, len: data length, buf: data buf
    Serial.println(buf[0]);
    if (buf[0] == 0){
      // ems
    }
    if else (buf[0] == 1){
      //curtains
    }
    if else (buf[0] == 2){
      // fan
    }
    else {
      // light
    }
  }
}
/*********************************************************************************************************
END FILE
*********************************************************************************************************/