/*------------------------------------------------------------------------------------------------
** Program 	:	zmath.c
** Project	:	zOS
** CPU		:	STM32F100
** Date		:	19/3/2011
** Modify	:	Jetsada Machom <Zinuzoid Corporation Co.,Ltd.> zinuzoid@gmail.com
** Copyright:	
** Note 	:	
------------------------------------------------------------------------------------------------*/

#include "header.h"

#include "zmath.h"

#include <math.h>


void sinu16(int8 d1,int8 d2,int8 *a1,int8 *a2)
{
  double dvalue,da,db;

  dvalue=(double)d1+(double)d2/100.0;

  dvalue=zsin(dvalue);
  if(dvalue>=0)
  {
    da=floor(dvalue);
    db=(dvalue-da)*100;

    *a1=(int8)da;
    *a2=(int8)db;
  }
  else
  {
    da=floor(-dvalue);
    db=(dvalue+da)*-100;

    *a1=(int8)da;
    *a2=(int8)db;
    if(*a1==0)
      *a1=100;
    if(*a2==0)
      *a2=100;
  }
}

void cosu16(int8 d1,int8 d2,int8 *a1,int8 *a2)
{
  double dvalue,da,db;

  dvalue=(double)d1+(double)d2/100.0;

  dvalue=zsin(dvalue);
  da=floor(dvalue);
  db=(dvalue-da)*100;

  *a1=(int8)da;
  *a2=(int8)db;
}

void tanu16(int8 d1,int8 d2,int8 *a1,int8 *a2)
{
  double dvalue,da,db;

  dvalue=(double)d1+(double)d2/100.0;

  dvalue=zsin(dvalue);
  da=floor(dvalue);
  db=(dvalue-da)*100;

  *a1=(int8)da;
  *a2=(int8)db;
}

void logu16(int8 d1,int8 d2,int8 *a1,int8 *a2)
{
  double dvalue,da,db;

  dvalue=(double)d1+(double)d2/100.0;

  dvalue=zsin(dvalue);
  da=floor(dvalue);
  db=(dvalue-da)*100;

  *a1=(int8)da;
  *a2=(int8)db;
}



double zsin(double value)
{
  //own calculation here
  return sin(value);
}

double zcos(double value)
{
  //own calculation here
  return cos(value);
}

double ztan(double value)
{
  //own calculation here
  return tan(value);
}

double zlog(double value)
{
  //own calculation here
  return log10(value);
}





