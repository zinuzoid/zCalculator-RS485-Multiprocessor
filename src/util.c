/*------------------------------------------------------------------------------------------------
** Program 	:	util.c
** Project	:	zOS
** CPU		:	STM32F100
** Date		:	16/3/2011
** Modify	:	Jetsada Machom <Zinuzoid Corporation Co.,Ltd.> zinuzoid@gmail.com
** Copyright:	
** Note 	:	
------------------------------------------------------------------------------------------------*/

#include "header.h"

uint8 zstrcmp(const uint8 *s1, const uint8 *s2,uint8 len)
{
  while(
        (len)&&
        (*s1!='\0')&&
        (*s2!='\0')
       )
  {
    if(*s1!=*s2)
    {
      return 0;
    }
    s1++;
    s2++;
    len--;
  }
  if((*s1=='\0')&&(*s2=='\0')||(len==0))
    return 1;
  return 0;
}

uint8 zstrcmpnc(const uint8 *s1, const uint8 *s2,uint8 len)//non case sensitive
{
  while(
        (len)&&
        (*s1!='\0')&&
        (*s2!='\0')
       )
  {
    if(!(
       (*s1==*s2)||
       (*s1==(*s2+('a'-'A')))||//non case sensitive
       (*s1==(*s2-('a'-'A')))
      ))
    {
      return 0;
    }
    s1++;
    s2++;
    len--;
  }
  if((*s1=='\0')&&(*s2=='\0')||(len==0))
    return 1;
  return 0;
}

void zstrcpy(char *dst,char *src)
{
    while(*src)
    {
      *dst=*src;
      src++;
    }
}

//CRC
uint8 CRC8(uint8 data,uint8 prevcrc)
{
  uint8 chk,crc;
  chk=data^prevcrc;
  crc=0;
  if(chk&0x01)  crc^=0x5e;
  if(chk&0x02)  crc^=0xbc;
  if(chk&0x04)  crc^=0x61;
  if(chk&0x08)  crc^=0xc2;
  if(chk&0x10)  crc^=0x9d;
  if(chk&0x20)  crc^=0x23;
  if(chk&0x40)  crc^=0x46;
  if(chk&0x80)  crc^=0x8c;
  return crc;
}
//End CRC




