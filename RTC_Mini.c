//RTC_MINI.C

#include <lpc21xx.h>
#include "LCD.h"
#include "LCD_DEFINES.h"
#include "delay.h"
#include "kpm.h"
#include "types.h"
#include "kpm_defines.h"
#include "rtc_mini.h"
#include "rtc_defines_mini.h"

// System clock and peripheral clock Macros
#define FOSC 12000000
#define CCLK (5*FOSC)
#define PCLK (CCLK/4)
#define _lpc2148

extern s32 mh[3];
extern  s32 mm[3];
 extern s32 ms[3];
extern u8 mc;
#define NO_INPUT 0xffffffff
 // Array to hold names of days of the week
char week[][4] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};


/*s32 med_hour[3];
s32 med_min[3];
 s32 med_sec[3];*/
// RTC Prescaler Calculation Macros
// RTC requires 32.768 kHz clock for 1-second increment.
// PREINT and PREFRAC registers divide PCLK to generate 32.768 kHz.

// PREINT = int (PCLK / 32768) - 1;
// PREFRAC = PCLK -((PREINT + 1) * 32768);
// Note: This information collected from LPC2129 Manual

#define PREINT_VAL (int) ((PCLK / 32768) - 1)
#define PREFRAC_VAL (PCLK -((PREINT_VAL + 1) * 32768))

//RTC Control Register (CCR) Bit Definitions
// Bit 0 – Clock Enable --> 1 = Enable RTC counters  0 = Disable RTC counters
#define RTC_ENABLE (1<<0)

// Bit 1 – Clock Reset --> 1 = Reset RTC counters    0 = Normal operation
#define RTC_RESET (1<<1)


//only for LPC2148
// Bit 4 – Clock Source Select
// 1 = Use external 32.768 kHz oscillator
// 0 = Use internal PCLK as RTC clock source
#define RTC_CLKSRC (1<<4)

extern s32 hour,minute,second,date,month,year,day;

/*
Initialize the Real-Time Clock (RTC)
This function disables the RTC, sets the prescaler values,
and then enables the RTC.
*/
void InitRTC(void)
{
  // Disable and reset the RTC
        CCR = RTC_RESET;

        #ifndef _LPC2148

  // Set prescaler integer and fractional parts
        PREINT = PREINT_VAL;
        PREFRAC = PREFRAC_VAL;

  // Enable the RTC
        CCR = RTC_ENABLE;

        #else
  // Enable the RTC with external clock source
        CCR = RTC_ENABLE | RTC_CLKSRC;
        #endif

}
u32 GetValidValue(char *msg, u32 max)
{
    u32 val;

    while(1)
    {
        CmdLCD(0x01);
        CmdLCD(0x80);
        StrLCD(msg);

        if(max == 23)          StrLCD("(00-23)");
        else if(max == 59)     StrLCD("(00-59)");
        else if(max == 31)     StrLCD("(01-31)");
        else if(max == 12)     StrLCD("(01-12)");
        else if(max == 6)      StrLCD("(0-6)");
   			CmdLCD(0xc0);
        if(max > 100)          val = ReadNDigitInput(4);   // year
        else                   val = ReadNDigitInput(2);   // others
  
        if(val <= max)         return val;
        CmdLCD(0x01);
        StrLCD("INVALID");
        delay_ms(1000);
    }
}

/*
get the RTC menu
hours and seconds ,and minutes through keypad
*/
void RTC_Menu()
{
  u32 key,H,M,S,D,Mo,Y,W;
  // Read current RTC values first
    H = HOUR;
    M = MIN;
    S = SEC;

    D = DOM;
    Mo = MONTH;
    Y = YEAR;

    W = DOW;
    while(1)
    {
        CmdLCD(0X01);
        CmdLCD(0x80);
        StrLCD("1.H 2.M 3.S 4.D");
        CmdLCD(0xc0);
        StrLCD("5.Mo 6.Y 7.W 8.EX");
        key=KeyScan();
        switch(key)
        {
           case '1': H = GetValidValue("HH:",23);     break;
           case '2': M = GetValidValue("MM:",59);     break;
           case '3': S = GetValidValue("SS:",59);     break;
           case '4': D = GetValidValue("Date:",31);   break;
           case '5': Mo = GetValidValue("Month:",12); break;
           case '6': Y= GetValidValue("year:",4096);  break;
           case '7': W = GetValidValue("Week:",6);    break;
           case '8': CmdLCD(0x01);
                     SetRTCTimeInfo(H,M,S);
                     SetRTCDateInfo(D,Mo,Y);
                     SetRTCDay(W);
                     return ;
        }
   }
}

/*
get the RTC menu
hours and seconds ,and minutes through keypad
*/
void Medicine_Menu()
{
        u32 key;
        while(1)
        {
            CmdLCD(0X01);
            CmdLCD(0x80);
            StrLCD("1.M1 2.M2 3.M3");
            CmdLCD(0xc0);
            StrLCD("4.exit");
            key=KeyScan();
                switch(key)
                {
                        case '1': if(SetMediTimeInfo(0))
                                  ShowMedicineTimes(0);
                                  break;
                        case '2': if(SetMediTimeInfo(1))
                                  ShowMedicineTimes(1);
                                  break;
                        case '3': if(SetMediTimeInfo(2))
                                  ShowMedicineTimes(2);
                                  break;
                        case '4': CmdLCD(0x01);
                                  return ;
                }
        }
}


/*
Get the current RTC time
hour Pointer to store the current hour
minute Pointer to store the current minute
second Pointer to store the current second
*/
void GetRTCTimeInfo(s32 *hour, s32 *minute, s32 *second)
{
        *hour = HOUR;
        *minute = MIN;
        *second = SEC;
}

/*
Display the RTC time on LCD
hour value (0-23)
minute value (0-59)
second value (0-59) seperated by ':'
*/
void DisplayRTCTime(u32 hour, u32 minute, u32 second)
{
    CmdLCD(0x80);
        CharLCD(hour/10+'0');
        CharLCD(hour%10+'0');
        CharLCD(':');
        CharLCD(minute/10+'0');
        CharLCD(minute%10+'0');
        CharLCD(':');
        CharLCD(second/10+'0');
        CharLCD(second%10+'0');
}

void ShowMedicineTimes(u32 i)
{
    //for(i=0;i<3;i++)
    //{
        CmdLCD(0x01);

        CmdLCD(0x80);
        CharLCD('M');
        CharLCD('1'+i);
        StrLCD("SET");
        CmdLCD(0xc0);
        DisplayMedTime(mh[i],mm[i],ms[i]);
        delay_ms(1000);
CmdLCD(0x01);
        //}
}
/*
Display the Medicine Time on LCD
hour value (0-23)
minute value (0-59)
second value (0-59) seperated by ':'
*/
void DisplayMedTime(u32 hour, u32 minute, u32 second)
{
        CharLCD(hour/10+'0');
        CharLCD(hour%10+'0');
        CharLCD(':');
        CharLCD(minute/10+'0');
        CharLCD(minute%10+'0');
        CharLCD(':');
        CharLCD(second/10+'0');
        CharLCD(second%10+'0');
}

/*
Set the RTC time
Hour to set (0-23)
Minute to set (0-59)
Second to set (0-59)
*/
void SetRTCTimeInfo(u32 hour, u32 minute, u32 second)
{
        HOUR = hour;
        MIN = minute;
        SEC = second;
}

/*
Set the medicine time
Hour to set (0-23)
Minute to set (0-59)
Second to set (0-59)
*/
u8 SetMediTimeInfo(u32 index)
{
        /*
        med_hour[index]=GetValidValue("HH:",23);
        if(med_hour[index]==NO_INPUT)
                                return ;///exit menu completely
        med_min[index]=GetValidValue("MM:",59);
        if(med_min[index]==NO_INPUT)
                                return ;///exit menu completely
        med_sec[index]=GetValidValue("SS:",59);
        if(med_sec[index]==NO_INPUT)
                                return ;///exit menu completely
        */
         u32 H, M, S;

    // ---- HOUR ----
    H = GetValidValue("HH:",23);
    //if(H == NO_INPUT) return 0;

    // ---- MIN ----
    M = GetValidValue("MM:",59);
    //if(M == NO_INPUT) return 0;

    // ---- SEC ----
    S = GetValidValue("SS:",59);
    //if(S == NO_INPUT) return 0;

    // ---- STORE ----
    mh[index] = H;
    mm[index]  = M;
    ms[index]  = S;
        mc=1;
        return 1;
}

/*
Get the current RTC date
day Pointer to store the current date (1-31)
month Pointer to store the current month (1-12)
year Pointer to store the current year (four digits)
*/
void GetRTCDateInfo(s32 *date, s32 *month, s32 *year)
{
        *date = DOM;
        *month = MONTH;
        *year = YEAR;
}

/*
Display the RTC date on LCD
Day of month (1-31)
Month (1-12)
Year (four digits) and seperated by '/'
*/
void DisplayRTCDate(u32 date, u32 month, u32 year)
{
        CmdLCD(GOTO_LINE2_POS0);
        CharLCD(date/10+48);
        CharLCD(date%10+48);
        CharLCD('/');
        CharLCD(month/10+48);
        CharLCD(month%10+48);
        CharLCD('/');
        U32LCD(year);
}

/*
Set the RTC date
day of month to set (1-31)
month to set (1-12)
year to set (four digits)
*/
void SetRTCDateInfo(u32 date, u32 month, u32 year)
{
        DOM = date;
        MONTH = month;
        YEAR = year;
}
/*
Get the current day of the week
dow Pointer to store Day of Week (0=Sunday, ..., 6=Saturday)
*/
void GetRTCDay(s32 *dow)
{
        *dow = DOW;
}

/*
Display the current day of the week on LCD
dow (Day of Week) (0=Sunday, ..., 6=Saturday)
*/
void DisplayRTCDay(u32 dow)
{
        CmdLCD(GOTO_LINE1_POS0+10);
        StrLCD(week[dow]);

}

/*
Set the day of the week in RTC
Day of Week to set (0=Sunday, ..., 6=Saturday)
*/
void SetRTCDay(u32 dow)
{
        DOW = dow;
}
