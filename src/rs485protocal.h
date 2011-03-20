/*------------------------------------------------------------------------------------------------
** Program 	:	rs485protocal.h
** Project	:	zOS
** CPU		:	STM32F100
** Date		:	19/3/2011
** Modify	:	Jetsada Machom <Zinuzoid Corporation Co.,Ltd.>
** Copyright:	
** Note 	:	
------------------------------------------------------------------------------------------------*/

#ifndef _RS485PROTOCAL_H
#define _RS485PROTOCAL_H

#define PACKET_CMD_SIN  1
#define PACKET_CMD_COS  2
#define PACKET_CMD_TAN  3
#define PACKET_CMD_LOG  4
#define PACKET_CMD_ANSSIN  5
#define PACKET_CMD_ANSCOS  6
#define PACKET_CMD_ANSTAN  7
#define PACKET_CMD_ANSLOG  8


#define PACKET_SRCDST_1  1
#define PACKET_SRCDST_2  2
#define PACKET_SRCDST_3  3
#define PACKET_SRCDST_4  4

#define PACKET_MYID PACKET_SRCDST_1

uint8 GetRand();

void RS485ProtocalInit();
uint8 RS485AskSin(int8 d1,int8 d2);
uint8 RS485AskCos(int8 d1,int8 d2);
uint8 RS485AskTan(int8 d1,int8 d2);
uint8 RS485AskLog(int8 d1,int8 d2);
uint8 RS485Ans(uint8 cmd,uint8 dst,int8 d1,int8 d2);

#endif




