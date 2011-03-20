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

static void calcu16(int8 d1,int8 d2,uint8 *a1,uint8 *a2,double (*fnMath)(double));


static void calcu16(int8 d1,int8 d2,uint8 *a1,uint8 *a2,double (*fnMath)(double))
{
  double dvalue;
  int16 ans;
  
  if(d1>0)
    dvalue=(double)d1+(double)d2/100.0;
  else
    dvalue=(double)d1-(double)d2/100.0;
  dvalue=fnMath(dvalue);
  ans=(int16)(dvalue*100.0);

  *a1=ans&0x00ff;
  *a2=ans>>8;  
}

void sinu16(int8 d1,int8 d2,uint8 *a1,uint8 *a2)
{
  calcu16(d1,d2,a1,a2,sin);
}

void cosu16(int8 d1,int8 d2,uint8 *a1,uint8 *a2)
{
  calcu16(d1,d2,a1,a2,cos);
}

void tanu16(int8 d1,int8 d2,uint8 *a1,uint8 *a2)
{
  calcu16(d1,d2,a1,a2,tan);
}

void logu16(int8 d1,int8 d2,uint8 *a1,uint8 *a2)
{
  calcu16(d1,d2,a1,a2,log10);
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





