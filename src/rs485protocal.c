/*------------------------------------------------------------------------------------------------
** Program 	:	rs485protocal.c
** Project	:	zOS
** CPU		:	STM32F100
** Date		:	19/3/2011
** Modify	:	Jetsada Machom <Zinuzoid Corporation Co.,Ltd.>
** Copyright:	
** Note 	:	
------------------------------------------------------------------------------------------------*/

#include "header.h"

#include "rs485protocal.h"
#include "roundrobintask.h"
#include "usart.h"
#include "util.h"
#include "zmath.h"

#include <stdio.h>

#define PACKET_MAX  32
#define DATA_MAX  8

#define PACKET_STX  0x03
#define PACKET_ETX  0x02

#define PACKET_CMD_SIN  1
#define PACKET_CMD_COS  2
#define PACKET_CMD_TAN  3
#define PACKET_CMD_LOG  4
#define PACKET_CMD_ANSSIN  5
#define PACKET_CMD_ANSCOS  6
#define PACKET_CMD_ANSTAN  7
#define PACKET_CMD_ANSLOG  8

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

typedef struct _TSTATERECV TSTATERECV;

struct _TSTATERECV
{
  TPACKET packet;
  void (*fnState)(TSTATERECV*,uint8);
  uint8 (*fnRecvChar)(uint8*);
};

TTASK Task485Recv;

TSTATERECV StateRecv;

TPACKET PHYPacketSend;

uint8 rand;

static void RS485RecvTask_10ms();
static void RS485PHYInit();
static void PacketInit(TPACKET *packet);
static uint8 PacketInsert(TPACKET *packet,uint8 data);
static void DataInit(TDATA *data);
static uint8 DataInsert(TDATA *tdata,uint8 data);
static uint8 CRCCalc(TPACKET packet);
static uint8 EncapData(TDATA *data,uint8 d1,uint8 d2);
static uint8 EncapPacket(TPACKET *packet,uint8 dst,uint8 cmd,TDATA data);
static uint8 PacketSend(TPACKET packet);

static void RS485StateInit(TSTATERECV *state,uint8 (*fnRecvChar)(uint8*));
static void RS485State(TSTATERECV *state,uint8 data);

static void RS485State_GetETX(TSTATERECV *state,uint8 data);
static void RS485State_GetSTX(TSTATERECV *state,uint8 data);
static void RS485State_GetPacket(TSTATERECV *state,uint8 data);

static void PacketProcess(TPACKET packet);

static void RS485SendTask_5ms();
void RS485PHYSendInit();

//------------------------------------------------------------------------------------------------
//PHY Sender Task
TTASK Task485Send;

void RS485PHYSendInit()
{
  TaskAdd(&Task485Send,"RS485Send",5,RS485SendTask_5ms,0);
}

static void RS485SendTask_5ms()
{
}
//End PHY Sender Task
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
//Application Layer
void RS485ProtocalInit()
{
  RS485PHYInit();
  RS485PHYSendInit();
  RS485StateInit(&StateRecv,USART2_RecvChar);
  TaskAdd(&Task485Recv,"RS485Recv",10,RS485RecvTask_10ms,&StateRecv);
}

uint8 RS485AskSin(int8 d1,int8 d2)
{
  TPACKET Packet;
  TDATA Data;

  EncapData(&Data,d1,d2);
  EncapPacket(&Packet,PACKET_SRCDST_1,PACKET_CMD_SIN,Data);

  PacketSend(Packet);
  return 1;
}

uint8 RS485AskCos(int8 d1,int8 d2)
{
  TPACKET Packet;
  TDATA Data;

  EncapData(&Data,d1,d2);
  EncapPacket(&Packet,PACKET_SRCDST_2,PACKET_CMD_COS,Data);

  PacketSend(Packet);
  return 1;
}

uint8 RS485AskTan(int8 d1,int8 d2)
{
  TPACKET Packet;
  TDATA Data;

  EncapData(&Data,d1,d2);
  EncapPacket(&Packet,PACKET_SRCDST_3,PACKET_CMD_TAN,Data);

  PacketSend(Packet);
  return 1;
}

uint8 RS485AskLog(int8 d1,int8 d2)
{
  TPACKET Packet;
  TDATA Data;

  EncapData(&Data,d1,d2);
  EncapPacket(&Packet,PACKET_SRCDST_4,PACKET_CMD_LOG,Data);

  PacketSend(Packet);
  return 1;
}

uint8 RS485Ans(uint8 cmd,uint8 dst,int8 d1,int8 d2)
{
  TPACKET Packet;
  TDATA Data;

  EncapData(&Data,d1,d2);
  EncapPacket(&Packet,dst,cmd,Data);

  PacketSend(Packet);
  return 1;
}

uint8 GetRand()
{
  return ((rand++)%4)+1;
}
//End Application Layer
//------------------------------------------------------------------------------------------------

static void RS485RecvTask_10ms(TSTATERECV *state)
{
  uint8 data;

  if(!state)
    return;

  while(state->fnRecvChar(&data))
     RS485State(state,data);
}

//------------------------------------------------------------------------------------------------
//Receive State
static void RS485StateInit(TSTATERECV *state,uint8 (*fnRecvChar)(uint8*))
{
  if(!state)
    return;

  PacketInit(&state->packet);
  state->fnRecvChar=fnRecvChar;
  state->fnState=RS485State_GetETX;
}
static void RS485State(TSTATERECV *state,uint8 data)
{
  if(!state)
    return;

  //USART1_SendChar(data);//echo rs485 recv to usart1

  state->fnState(state,data);
}

static void RS485State_GetETX(TSTATERECV *state,uint8 data)
{
  if(data==PACKET_STX)
  {
    PacketInit(&state->packet);
    PacketInsert(&state->packet,data);
    state->fnState=RS485State_GetSTX;
  }
  else if(data==PACKET_ETX)
  {
    
  }
  else
  {
    
  }
}

static void RS485State_GetSTX(TSTATERECV *state,uint8 data)
{
  if(data==PACKET_STX)
  {

  }
  else if(data==PACKET_ETX)
  {
    PacketInit(&state->packet);
    state->fnState=RS485State_GetETX;
  }
  else
  {
    PacketInsert(&state->packet,data);
    state->fnState=RS485State_GetPacket;
  }
}

static void RS485State_GetPacket(TSTATERECV *state,uint8 data)
{
  if(data==PACKET_STX)//FIXME if check double STX
  {
    uint8 nextdata;
    if(state->fnRecvChar(&nextdata))
    {
      if(nextdata==PACKET_STX)
      {
        PacketInsert(&state->packet,data);
      }
      else if(nextdata==PACKET_ETX)
      {
        //not possible protection
      }
      else
      {
        PacketInit(&state->packet);
        state->fnState=RS485State_GetETX;
      }
    }
    else
    {
      PacketInit(&state->packet);
      state->fnState=RS485State_GetETX;
    }
  }
  else if(data==PACKET_ETX)
  {
    uint8 nextdata;
    if(state->fnRecvChar(&nextdata))
    {
      if(nextdata==PACKET_ETX)
      {
        PacketInsert(&state->packet,data);
      }
      else if(nextdata==PACKET_STX)
      {
        //processing packet here
        PacketProcess(state->packet);
        state->fnState=RS485State_GetSTX;
        PacketInsert(&state->packet,nextdata);
      }
    }
    else
    {
      //processing packet here
      PacketInsert(&state->packet,data);
      PacketProcess(state->packet);
      PacketInit(&state->packet);
      state->fnState=RS485State_GetETX;
    }
  }
  else
  {
    PacketInsert(&state->packet,data);
  }
}
//End Receive State
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
//Package Process
static void PacketProcess(TPACKET packet)
{
  uint8 crc=0;

  packet.idx--;
  packet.idx--;

  crc=packet.value[packet.idx];
  if(crc==CRCCalc(packet))
  {
    uint8 dst,src,len,cmd;
    uint8 d1,d2;
    int16 ans;
    double dans;
    char tmp[10];

    USART1_SendStr("\r\nGot Packet. ");

    dst=packet.value[1]>>4;
    src=packet.value[1]&0x0f;
    len=packet.value[2];
    cmd=packet.value[3];
    d1=packet.value[4];
    d2=packet.value[5];//FIXME use len parameter

    USART1_SendChar(dst+'0');
    USART1_SendChar(' ');
    USART1_SendChar(src+'0');
    USART1_SendChar(' ');
    USART1_SendChar(len+'0');
    USART1_SendChar(' ');
    USART1_SendChar(cmd+'0');
    sprintf(tmp," %2d",d1);
    USART1_SendStr(tmp);
    sprintf(tmp," %2d",d2);
    USART1_SendStr(tmp);

    if(dst==PACKET_MYID)
    {
      switch(cmd)
      {
      case PACKET_CMD_SIN:
        sinu16(d1,d2,&d1,&d2);
        RS485Ans(PACKET_CMD_ANSSIN,src,d1,d2);
        break;
      case PACKET_CMD_COS:
        //cosu16(d1,d2,&d1,&d2);
        //RS485Ans(PACKET_CMD_ANSCOS,src,d1,d2);
        break;
      case PACKET_CMD_TAN:
        //tanu16(d1,d2,&d1,&d2);
        //RS485Ans(PACKET_CMD_ANSTAN,src,d1,d2);
        break;
      case PACKET_CMD_LOG:
        //logu16(d1,d2,&d1,&d2);
        //RS485Ans(PACKET_CMD_ANSLOG,src,d1,d2);
        break;
      case PACKET_CMD_ANSSIN://use int16 represent
      case PACKET_CMD_ANSCOS:
      case PACKET_CMD_ANSTAN:
      case PACKET_CMD_ANSLOG:
        USART1_SendStr("\r\n");
        ans=d1|(d2<<8);
        dans=(double)(ans/100.0);
        sprintf(tmp,"%.2f",dans);
        USART1_SendStr(tmp);
        USART1_SendStr("\r\n\r\nz> ");//FIXME use prompt from cmd
        break;
      default:
        //error here
        USART1_SendStr("CMDERROR.");
        break;
      }
      //do math here then send
    }
  }
  else
  {
    USART1_SendStr("\r\nCRC Error Drop Packet.");
  }

}
//End Package Process
//------------------------------------------------------------------------------------------------

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

  if(packet->idx==PACKET_MAX-1)
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

  if(tdata->idx==DATA_MAX-1)
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
  TPACKET crctestpacket;
  uint8 dstsrc,len,crc,i;
  if(!packet)
    return 0;

  dstsrc=(dst<<4)|(PACKET_MYID&0x0f);
  len=1+data.idx;

  PacketInit(packet);
  PacketInit(&crctestpacket);

  PacketInsert(packet,PACKET_STX);
  PacketInsert(&crctestpacket,PACKET_STX);
  
  PacketInsert(packet,dstsrc);
  PacketInsert(&crctestpacket,dstsrc);
  if(dstsrc==PACKET_STX)
    PacketInsert(packet,PACKET_STX);
  else if(dstsrc==PACKET_ETX)
    PacketInsert(packet,PACKET_ETX);
  
  PacketInsert(packet,len);
  PacketInsert(&crctestpacket,len);
  if(len==PACKET_STX)
    PacketInsert(packet,PACKET_STX);
  else if(len==PACKET_ETX)
    PacketInsert(packet,PACKET_ETX);
  
  PacketInsert(packet,cmd);
  PacketInsert(&crctestpacket,cmd);
  if(cmd==PACKET_STX)
    PacketInsert(packet,PACKET_STX);
  else if(cmd==PACKET_ETX)
    PacketInsert(packet,PACKET_ETX);
  
  for(i=0;i<data.idx;i++)
  {
    PacketInsert(packet,data.value[i]);    
    PacketInsert(&crctestpacket,data.value[i]);
    if(data.value[i]==PACKET_STX)
      PacketInsert(packet,data.value[i]);
    else if(data.value[i]==PACKET_ETX)
      PacketInsert(packet,data.value[i]);    
  }
  crc=CRCCalc(crctestpacket);
  PacketInsert(packet,crc);
  PacketInsert(&crctestpacket,crc);
  if(crc==PACKET_STX)
    PacketInsert(packet,PACKET_STX);
  else if(crc==PACKET_ETX)
    PacketInsert(packet,PACKET_ETX);
  
  PacketInsert(packet,PACKET_ETX);

  return 1;
}

static uint8 CRCCalc(TPACKET packet)
{
  uint8 i,crc;

  crc=0;

  for(i=0;i<packet.idx;i++)
  {
    crc=CRC8(packet.value[i],crc);
  }
  return crc;
}

static uint8 PacketSend(TPACKET packet)
{
  uint8 i;

  for(i=0;i<packet.idx;i++)
  {
    USART2_SendChar(packet.value[i]);//FIXME use coilision detect
  }

  return 1;
}
//End Packet Encapsulate
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










