/***************************************************************************
 *                                                                         *
 * Copyright (c) 2008 Nuvoton Technolog. All rights reserved.              *
 *                                                                         *
 ***************************************************************************/
 
/****************************************************************************
* FILENAME
*   wb_timer.c
*
* VERSION
*   1.0
*
* DESCRIPTION
*   The timer related function of Nuvoton ARM9 MCU
*
* HISTORY
*   2008-06-25  Ver 1.0 draft by Min-Nan Cheng
*
* REMARK
*   None
 **************************************************************************/
#include <stdio.h>
#include "wblib.h"

/* Global variables */
PVOID  _sys_pvOldTimer0Vect, _sys_pvOldTimer1Vect;
UINT32 _sys_uTimer0ClockRate = EXTERNAL_CRYSTAL_CLOCK;
UINT32 _sys_uTimer1ClockRate = EXTERNAL_CRYSTAL_CLOCK;
UINT32 volatile _sys_uTimer0Count = 0, _sys_uTimer1Count = 0;
UINT32 volatile _sys_uTime0EventCount = 0, _sys_uTime1EventCount = 0;
UINT32 volatile _sys_uTimer0TickPerSecond;
BOOL   _sys_bIsSetTime0Event = FALSE, _sys_bIsSetTime1Event = FALSE;
BOOL volatile _sys_bIsTimer0Initial = FALSE;

#define SECONDS_PER_HOUR	3600
#define SECONDS_PER_DAY		86400
#define SECONDS_365_DAY		31536000
#define SECONDS_366_DAY		31622400

#define TimerEventCount		10

typedef void (*sys_pvTimeFunPtr)();   /* function pointer */
typedef struct timeEvent_t
{
	UINT32				active;
	UINT32				initTick;
	UINT32				curTick;
	sys_pvTimeFunPtr	funPtr;
} TimeEvent_T;
TimeEvent_T tTime0Event[TimerEventCount], tTime1Event[TimerEventCount];


UINT32	volatile _sys_ReferenceTime_Clock = 0;
UINT32	volatile _sys_ReferenceTime_UTC;

static UINT32 month_seconds[] = 
{
	31 * SECONDS_PER_DAY, 
	28 * SECONDS_PER_DAY, 
	31 * SECONDS_PER_DAY, 
	30 * SECONDS_PER_DAY, 
	31 * SECONDS_PER_DAY, 
	30 * SECONDS_PER_DAY, 
	31 * SECONDS_PER_DAY, 
	31 * SECONDS_PER_DAY, 
	30 * SECONDS_PER_DAY, 
	31 * SECONDS_PER_DAY, 
	30 * SECONDS_PER_DAY, 
	31 * SECONDS_PER_DAY
};


/* Interrupt service routine */
void sysTimer0ISR()
{
   int volatile i;

   /*----- check channel 0 -----*/
   if (inpw(REG_TMR_TISR) & 0x00000001)
   {
       _sys_uTimer0Count++;
       outpw(REG_TMR_TISR, 0x01); /* clear TIF0 */
       if (_sys_uTimer0Count >= 0xfffffff)
          _sys_uTimer0Count = 0;

	   if (_sys_bIsSetTime0Event)
	   {
	      for (i=0; i<TimerEventCount; i++)
	      {
	         if (tTime0Event[i].active)
	         {
	            tTime0Event[i].curTick--;
	            if (tTime0Event[i].curTick == 0)
	            {
				   (*tTime0Event[i].funPtr)();
				   tTime0Event[i].curTick = tTime0Event[i].initTick;
				}
	         }
	      }
	   }
   }
}

void sysTimer1ISR()
{
   int volatile i;

   /*----- check channel 1 -----*/
   if (inpw(REG_TMR_TISR) & 0x00000002)
   {
       _sys_uTimer1Count++;
       outpw(REG_TMR_TISR, 0x02); /* clear TIF1 */
       if (_sys_uTimer1Count >= 0xfffffff)
          _sys_uTimer1Count = 0;

	   if (_sys_bIsSetTime1Event)
	   {
	      for (i=0; i<TimerEventCount; i++)
	      {
	         if (tTime1Event[i].active)
	         {
	            tTime1Event[i].curTick--;
	            if (tTime1Event[i].curTick == 0)
	            {
				   (*tTime1Event[i].funPtr)();
				   tTime1Event[i].curTick = tTime1Event[i].initTick;
				}
	         }
	      }
	   }
   }
}


/* Timer library functions */
UINT32 sysGetTicks(INT32 nTimeNo)
{
   switch (nTimeNo)
   {
      case TIMER0:
           return _sys_uTimer0Count;

      case TIMER1:
           return _sys_uTimer1Count;

      default:
           ;
   }
   return Successful;
}

INT32 sysResetTicks(INT32 nTimeNo)
{
   switch (nTimeNo)
   {
      case TIMER0:
           _sys_uTimer0Count = 0;
           break;

      case TIMER1:
           _sys_uTimer1Count = 0;
           break;

      default:
           ;
   }
   return Successful;
}

INT32 sysUpdateTickCount(INT32 nTimeNo, UINT32 uCount)
{
   switch (nTimeNo)
   {
      case TIMER0:
           _sys_uTimer0Count = uCount;
           break;

      case TIMER1:
           _sys_uTimer1Count = uCount;
           break;

      default:
           ;
   }
   return Successful;
}

INT32 sysSetTimerReferenceClock(INT32 nTimeNo, UINT32 uClockRate)
{
   switch (nTimeNo)
   {
      case TIMER0:
           _sys_uTimer0ClockRate = uClockRate;
           break;

      case TIMER1:
           _sys_uTimer1ClockRate = uClockRate;
           break;

      default:
           ;
   }
   return Successful;
}

INT32 sysStartTimer(INT32 nTimeNo, UINT32 uTicksPerSecond, INT32 nOpMode)
{
   int volatile i;
   UINT32 _mTicr, _mTcr;
   
   _mTcr = 0x60000000 | (nOpMode << 27);

   switch (nTimeNo)
   {
      case TIMER0:
      	   rPCLKEN0 |= 0x100;
           _sys_bIsTimer0Initial = TRUE;
           _sys_uTimer0TickPerSecond = uTicksPerSecond;
      	   
           outpw(REG_TMR_TCSR0, 0);           /* disable timer */
           outpw(REG_TMR_TISR, 1);           /* clear for safty */

           for (i=0; i<TimerEventCount; i++)
              tTime0Event[i].active = FALSE;

           _sys_pvOldTimer0Vect = sysInstallISR(HIGH_LEVEL_SENSITIVE | IRQ_LEVEL_1, IRQ_TIMER0, (PVOID)sysTimer0ISR);
           sysEnableInterrupt(IRQ_TIMER0);

           _sys_uTimer0Count = 0;
           _mTicr = _sys_uTimer0ClockRate / uTicksPerSecond;
           outpw(REG_TMR_TICR0, _mTicr);
           outpw(REG_TMR_TCSR0, _mTcr);
           break;

      case TIMER1:
      	   rPCLKEN0 |= 0x200;
           outpw(REG_TMR_TCSR1, 0);           /* disable timer */
           outpw(REG_TMR_TISR, 2);           /* clear for safty */

           for (i=0; i<TimerEventCount; i++)
              tTime1Event[i].active = FALSE;

           _sys_pvOldTimer1Vect = sysInstallISR(HIGH_LEVEL_SENSITIVE | IRQ_LEVEL_1, IRQ_TIMER1, (PVOID)sysTimer1ISR);
           sysEnableInterrupt(IRQ_TIMER1);

           _sys_uTimer1Count = 0;
           _mTicr = _sys_uTimer1ClockRate / uTicksPerSecond;
           outpw(REG_TMR_TICR1, _mTicr);
           outpw(REG_TMR_TCSR1, _mTcr);
           break;

      default:
           ;
   }
   sysSetLocalInterrupt(ENABLE_IRQ);
   return Successful;
}

INT32 sysStopTimer(INT32 nTimeNo)
{
   switch (nTimeNo)
   {
      case TIMER0:
           _sys_bIsTimer0Initial = FALSE;
           sysDisableInterrupt(IRQ_TIMER0);		   
           sysInstallISR(HIGH_LEVEL_SENSITIVE | IRQ_LEVEL_1, IRQ_TIMER0, _sys_pvOldTimer0Vect);


           outpw(REG_TMR_TCSR0, 0);           /* disable timer */
           outpw(REG_TMR_TISR, 1);           /* clear for safty */

           _sys_uTime0EventCount = 0;
           _sys_bIsSetTime0Event = FALSE;
           break;

      case TIMER1:
           sysDisableInterrupt(IRQ_TIMER1);	  
           sysInstallISR(HIGH_LEVEL_SENSITIVE| IRQ_LEVEL_1, IRQ_TIMER1, _sys_pvOldTimer1Vect);


           outpw(REG_TMR_TCSR1, 0);           /* disable timer */
           outpw(REG_TMR_TISR, 2);           /* clear for safty */

           _sys_uTime1EventCount = 0;
           _sys_bIsSetTime1Event = FALSE;
           break;

      default:
           ;
   }
//   WB_SetLocalInterrupt(DISABLE_IRQ);
   return Successful;
}

void sysClearWatchDogTimerCount()
{
   UINT32 volatile _mWtcr;

   _mWtcr = inpw(REG_WDT_CR);
   _mWtcr |= 0x01;             /* write WTR */
   outpw(REG_WDT_CR, _mWtcr);
}

void sysClearWatchDogTimerInterruptStatus()
{
   UINT32 volatile _mWtcr;

   _mWtcr = inpw(REG_WDT_CR);
   _mWtcr |= 0x08;       /* clear WTIF */
   outpw(REG_WDT_CR, _mWtcr);
}

void sysDisableWatchDogTimer()
{

   UINT32 volatile _mWtcr;

   _mWtcr = inpw(REG_WDT_CR);
   _mWtcr &= 0xFFFFFF7F;
   outpw(REG_WDT_CR, _mWtcr);
  
}

void sysDisableWatchDogTimerReset()
{

   UINT32 volatile _mWtcr;

   _mWtcr = inpw(REG_WDT_CR);
   _mWtcr &= 0xFFFFFFFD;
   outpw(REG_WDT_CR, _mWtcr);
 
}

void sysEnableWatchDogTimer()
{

   UINT32 volatile _mWtcr;

   _mWtcr = inpw(REG_WDT_CR);
   _mWtcr |= 0x80;
   outpw(REG_WDT_CR, _mWtcr);
   
}

void sysEnableWatchDogTimerReset()
{

   UINT32 volatile _mWtcr;

   _mWtcr = inpw(REG_WDT_CR);
   _mWtcr |= 0x02;
   outpw(REG_WDT_CR, _mWtcr);
  
}

PVOID sysInstallWatchDogTimerISR(INT32 nIntTypeLevel, PVOID pvNewISR)
{
   PVOID _mOldVect = NULL;
   UINT32 volatile _mWtcr;

   _mWtcr = inpw(REG_WDT_CR);
   _mWtcr |= 0x40;
   outpw(REG_WDT_CR, _mWtcr);
   _mOldVect = sysInstallISR(nIntTypeLevel, IRQ_WDT, pvNewISR);
   sysEnableInterrupt(IRQ_WDT);
   sysSetLocalInterrupt(ENABLE_IRQ);

   return _mOldVect;
}

INT32 sysSetWatchDogTimerInterval(INT32 nWdtInterval)
{

   UINT32 volatile _mWtcr;

   _mWtcr = inpw(REG_WDT_CR) & ~0x700;
   _mWtcr = _mWtcr | (nWdtInterval << 8);
   outpw(REG_WDT_CR, _mWtcr);
 
   return Successful;
}


INT32 sysSetTimerEvent(INT32 nTimeNo, UINT32 uTimeTick, PVOID pvFun)
{
	int volatile i;
	int val=0;

	switch (nTimeNo)
	{
		case TIMER0:
			_sys_bIsSetTime0Event = TRUE;
			_sys_uTime0EventCount++;
			for (i=0; i<TimerEventCount; i++)
			{
				if (tTime0Event[i].active == FALSE)
				{
					tTime0Event[i].active = TRUE;
					tTime0Event[i].initTick = uTimeTick;
					tTime0Event[i].curTick = uTimeTick;
					tTime0Event[i].funPtr = (sys_pvTimeFunPtr)pvFun;
					val = i+1;
					break;
				}
			}
			break;

		case TIMER1:
			_sys_bIsSetTime1Event = TRUE;
			_sys_uTime1EventCount++;
			for (i=0; i<TimerEventCount; i++)
			{
				if (tTime1Event[i].active == FALSE)
				{
					tTime1Event[i].active = TRUE;
					tTime1Event[i].initTick = uTimeTick;
					tTime1Event[i].curTick = uTimeTick;
					tTime1Event[i].funPtr = (sys_pvTimeFunPtr)pvFun;
					val = i+1;
					break;
				}
			}
			break;

		default:
			;
	}

	return val;
}

void sysClearTimerEvent(INT32 nTimeNo, UINT32 uTimeEventNo)
{
	switch (nTimeNo)
	{
		case TIMER0:
			tTime0Event[uTimeEventNo-1].active = FALSE;
			_sys_uTime0EventCount--;
			if (_sys_uTime0EventCount == 0)
				_sys_bIsSetTime0Event = FALSE;
			break;

		case TIMER1:
			tTime1Event[uTimeEventNo-1].active = FALSE;
			_sys_uTime1EventCount--;
			if (_sys_uTime1EventCount == 0)
				_sys_bIsSetTime1Event = FALSE;
			break;

		default:
			;
	}
}

/* 
 *  The default time: 2005.05.01 Sun 00:00 00:00
 *
 *  one day = 3600 * 24 = 86400
 *  one year = 86400 * 365 = 31536000
 *  year 2005 = 31536000 * 35 + 8 * 86400 = 1104451200
 */

UINT32 sysDOS_Time_To_UTC(DateTime_T ltime)
{
	int		i, leap_year_cnt;
	UINT32	utc;
	
	if ((ltime.mon < 1) || (ltime.mon > 12) || (ltime.day < 1) || (ltime.day > 31) ||
		(ltime.hour > 23) || (ltime.min > 59) || (ltime.sec > 59))
	{
		//_debug_msg("DOS_Time_To_UTC - illegal time!! %d-%d-%d %d:%d:%d\n", year, month, day, hour, minute, second);
		return Fail;
	}

	ltime.year = ltime.year - 1970;		/* DOS is 1980 started, UTC is 1970 started */
	
	leap_year_cnt = (ltime.year + 1) / 4;
	
	utc = ltime.year * SECONDS_365_DAY + leap_year_cnt * SECONDS_PER_DAY;
	
	if ((ltime.year + 2) % 4 == 0)
		month_seconds[1] = 29 * SECONDS_PER_DAY;	/* leap year */
	else
		month_seconds[1] = 28 * SECONDS_PER_DAY;	/* non-leap year */
	
	for (i = 0; i < ltime.mon - 1; i++) 
		utc += month_seconds[i];
		
	utc += (ltime.day - 1) * SECONDS_PER_DAY;
	
	utc += ltime.hour * SECONDS_PER_HOUR + ltime.min * 60 + ltime.sec;
	
	return utc;
}


void  sysUTC_To_DOS_Time(UINT32 utc, DateTime_T *dos_tm)
{
	int		the_year = 1970;
	int		i, seconds;
		
	while (1)
	{
		if (the_year % 4 == 0)
			seconds = SECONDS_366_DAY;
		else
			seconds = SECONDS_365_DAY;
		if (utc >= seconds)
		{
			utc -= seconds;
			the_year++;
		}
		else
			break;
	}
	
	dos_tm->year = the_year;
	
	if (the_year % 4 == 0)
		month_seconds[1] = 29 * SECONDS_PER_DAY;
	else
		month_seconds[1] = 28 * SECONDS_PER_DAY;
	
	dos_tm->mon = 1;
	for (i = 0; i < 11; i++)
	{
		if (utc >= month_seconds[i])
		{
			utc -= month_seconds[i];
			dos_tm->mon++;
		}
		else
			break;
	}
	
	dos_tm->day = 1 + (utc / SECONDS_PER_DAY);
	utc %= SECONDS_PER_DAY;
	
	dos_tm->hour = utc / SECONDS_PER_HOUR;
	utc %= SECONDS_PER_HOUR;
	
	dos_tm->min = utc / 60;
	dos_tm->sec = utc % 60;
}


void sysSetLocalTime(DateTime_T ltime)
{
	_sys_ReferenceTime_Clock = _sys_uTimer0Count;
	_sys_ReferenceTime_UTC = sysDOS_Time_To_UTC(ltime);
}

void sysGetCurrentTime(DateTime_T *curTime)
{
	UINT32 clock, utc_time;

	clock = _sys_uTimer0Count;
	utc_time = _sys_ReferenceTime_UTC + (clock - _sys_ReferenceTime_Clock) / _sys_uTimer0TickPerSecond;

	sysUTC_To_DOS_Time(utc_time, curTime);
}


void sysDelay(UINT32 uTicks)
{
    UINT32 volatile btime;

    if(!_sys_bIsTimer0Initial)
    {
        sysStartTimer(TIMER0, 100, PERIODIC_MODE);
    }

    btime = sysGetTicks(TIMER0);
    while(1)
    {
        if((sysGetTicks(TIMER0) - btime) > uTicks)
        {
            break;
        }
    }
}
