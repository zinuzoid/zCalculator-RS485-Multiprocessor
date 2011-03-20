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
  calcu16(d1,d2,a1,a2,zsin);
}

void cosu16(int8 d1,int8 d2,uint8 *a1,uint8 *a2)
{
  calcu16(d1,d2,a1,a2,zcos);
}

void tanu16(int8 d1,int8 d2,uint8 *a1,uint8 *a2)
{
  calcu16(d1,d2,a1,a2,ztan);
}

void logu16(int8 d1,int8 d2,uint8 *a1,uint8 *a2)
{
  calcu16(d1,d2,a1,a2,zlog);
}

//my calc
#define PI			3.1415926535897932384626433832795
#define PI2			6.283185307179586476925286766559
#define PIDIV2		1.5707963267948966192313216916398

#define SIN_ITERATION 9

const double _invfac_table[19]=
	{
		1.00000000000000000000000000000000,	//1/ 0!
		1.00000000000000000000000000000000,	//1/ 1!
		0.50000000000000000000000000000000,	//1/ 2!
		0.16666666666666666000000000000000,	//1/ 3!
		0.04166666666666666400000000000000,	//1/ 4!
		0.00833333333333333320000000000000,	//1/ 5!
		0.00138888888888888890000000000000,	//1/ 6!
		0.00019841269841269841000000000000,	//1/ 7!
		0.00002480158730158730200000000000,	//1/ 8!
		0.00000275573192239858930000000000,	//1/ 9!
		0.00000027557319223985888000000000,	//1/10!
		0.00000002505210838544172000000000,	//1/11!
		0.00000000208767569878681000000000,	//1/12!
		0.00000000016059043836821613000000,	//1/13!
		0.00000000001147074559772972500000,	//1/14!
		0.00000000000076471637318198164000,	//1/15!
		0.00000000000004779477332387385300,	//1/16!
		0.00000000000000281145725434552060,	//1/17!
		0.00000000000000015619206968586225,	//1/18!
  };

double zmodf(double val,double div)
{
	double fraction;
	fraction=val/div;
	fraction=(double)(s32)fraction;
	fraction=fraction*div;
	fraction=val-fraction;
	return fraction;
}

double neg_pow(u8 val)
{
	if(val%2==0)
		return 1.0;
	return -1.0;
}

double approx_sin(double rad)
{
	double ans=0,x2;
	u8 i;
	
	//move to [-2pi , 2pi]
	rad=zmodf(rad,PI2);
	
	//move to [-pi , pi]
	if(rad>=PI)
		rad=rad-PI2;
	else if(rad<=-PI)
		rad=rad+PI2;
		
	//move to [pi/2 , pi/2]
	if(rad>=PIDIV2)
		rad=PI-rad;
	else if(rad<=-PIDIV2)
		rad=-PI-rad;
	
	x2=rad*rad;
	for(i=1;i<=SIN_ITERATION;i++)
	{
		ans+=neg_pow(i+1)*rad*_invfac_table[i*2-1];
		rad=rad*x2;
	}
	return ans;
}

double degtorad(double deg)
{
	return deg*0.01745329252;//(PI/180)
}

double zsin(double value)
{
  return approx_sin(degtorad(value));
}
//End my calc

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





