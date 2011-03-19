/*------------------------------------------------------------------------------------------------
** Program 	:	rs485protocal.c
** Project	:	zOS
** CPU		:	STM32F100
** Date		:	17/3/2011
** Modify	:	Jetsada Machom <Zinuzoid Corporation Co.,Ltd.>
** Copyright:	
** Note 	:	
------------------------------------------------------------------------------------------------*/

#include "header.h"

#include "rs485protocal.h"
#include "roundrobintask.h"
#include "usart.h"
#include "util.h"

#define PACKET_MAX  32
#define DATA_MAX  8

#define PACKET_STX  0x03
#define PACKET_ETX  0x02

#define PACKET_SRCDST_1  1
#define PACKET_SRCDST_2  2
#define PACKET_SRCDST_3  3
#define PACKET_SRCDST_4  4

#define PACKET_CMD_SIN  1
#define PACKET_CMD_COS  1
#define PACKET_CMD_TAN  1
#define PACKET_CMD_LOG  1

#define PACKET_MYID PACKET_SRCDST_1

typedef struct _TPACKET
{
  uint8 value[PACKET_MAX];
  uint16 idx;
}TPACKET;

typedef struct _TDATA
{
  uint8 value[DATA_MAX];
  uint16 idx;
}TDATA;

TTASK Task485;

TPACKET PacketRecv;

static void RS485ReadTask_10ms();
static void RS485PHYInit();
static void PacketInit(TPACKET *packet);
static uint8 PacketInsert(TPACKET *packet,uint8 data);
static void DataInit(TDATA *data);
static uint8 DataInsert(TDATA *tdata,uint8 data);
static uint8 CRCCalc(TPACKET *packet);
static uint8 EncapData(TDATA *data,uint8 d1,uint8 d2);
static uint8 EncapPacket(TPACKET *packet,uint8 dst,uint8 cmd,TDATA data);
static uint8 PacketSend(TPACKET packet);

//------------------------------------------------------------------------------------------------
//Application Layer
void RS485ProtocalInit()
{
  RS485PHYInit();
  TaskAdd(&Task485,"RS485",10,RS485ReadTask_10ms,(void*)USART2_RecvChar);

  //initial call temporaly
  ASKSin(0);
}

uint8 ASKSin(float value)
{
  TPACKET Packet;
  TDATA Data;

  EncapData(&Data,'A','B');
  EncapPacket(&Packet,PACKET_SRCDST_2,PACKET_CMD_SIN,Data);

  PacketSend(Packet);

  return 1;
}
//End Application Layer
//------------------------------------------------------------------------------------------------

static void RS485ReadTask_10ms(uint8 (*RecvChar)(uint8*))
{
  if(!RecvChar)
    return;

  uint8 data;
  if(RecvChar(&data))
  {
    USART1_SendStr("\r\n2:");
    USART1_SendChar(data);
  }
}

//------------------------------------------------------------------------------------------------
//Packet Encapsulate
static void PacketInit(TPACKET *packet)
{
  if(!packet)
    return;

  packet->idx=0;
}

static uint8 PacketInsert(TPACKET *packet,uint8 data)
{
  if(!packet)
    return 0;

  packet->value[packet->idx]=data;
  packet->idx++;

  return 1;
}

static void DataInit(TDATA *data)
{
  if(!data)
    return;

  data->idx=0;
}

static uint8 DataInsert(TDATA *tdata,uint8 data)
{
  if(!tdata)
    return 0;

  tdata->value[tdata->idx]=data;
  tdata->idx++;

  return 1;
}

static uint8 EncapData(TDATA *data,uint8 d1,uint8 d2)
{
  if(!data)
    return 0;

  DataInit(data);
  DataInsert(data,d1);
  DataInsert(data,d2);

  return 1;
}

static uint8 EncapPacket(TPACKET *packet,uint8 dst,uint8 cmd,TDATA data)
{
  uint8 dstsrc,len,i;
  if(!packet)
    return 0;

  dstsrc=(dst<<4)|(PACKET_MYID&0x0f);
  len=1+data.idx;

  PacketInit(packet);

  PacketInsert(packet,PACKET_STX);
  PacketInsert(packet,dstsrc);
  PacketInsert(packet,len);
  PacketInsert(packet,cmd);
  for(i=0;i<data.idx;i++)
  {
    PacketInsert(packet,data.value[i]);
  }
  PacketInsert(packet,CRCCalc(packet));
  PacketInsert(packet,PACKET_ETX);

  return 1;
}

static uint8 CRCCalc(TPACKET *packet)
{
  uint8 i,crc;

  if(!packet)
    return 0;

  crc=0;

  for(i=0;i<packet->idx;i++)
  {
    crc=CRC8(packet->value[i],crc);
  }
  return crc;
}

static uint8 PacketSend(TPACKET packet)
{
  uint8 i;

  for(i=0;i<packet.idx;i++)
  {
    USART2_SendChar(packet.value[i]);
  }

  return 1;
}
//EndPacket Encapsulate
//------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------
//Physical Driver
static void RS485PHYInit()
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
  GPIO_InitTypeDef GPIOInit;
  //RS485-DE
	GPIOInit.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIOInit.GPIO_Pin=GPIO_Pin_1;
	GPIOInit.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_Init(GPIOA,&GPIOInit);
  GPIO_WriteBit(GPIOA,GPIO_Pin_1,Bit_RESET);
}
//End Physical Driver
//------------------------------------------------------------------------------------------------










