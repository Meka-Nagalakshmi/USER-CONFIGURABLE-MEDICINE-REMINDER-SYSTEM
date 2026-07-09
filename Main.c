// mainmini1.C

#include "types.h"
#include <lpc21xx.h>
#include "delay.h"
#include "LCD_DEFINES.h"
#include "LCD.h"
#include "kpm.h"
#include "kpm_defines.h"
#include "eint0_mini.h"
#include "rtc_mini.h"
#include "eint0_defines_mini.h"

// Variables used to store the current RTC time and date values.
s32 hour, minute, second, date, month, year, day;

// Arrays for storing the scheduled timings of three medicine reminders.
s32 mh[3] = {0,0,0};
s32 mm[3] = {0,0,0};
s32 ms[3] = {0,0,0};

// Indicates whether medicine reminder timings are available.
u8 mc = 0;

// Indicates whether the buzzer is currently enabled.
u8 buzzer = 0;

// Stores the previous second value to detect one-second updates.
u32 prev_sec = 0;

// Counts how long the buzzer has remained active.
u32 scount = 0;

int main()
{
    u32 i;

    // Initialize all required peripherals.
    InitRTC();
    SetRTCTimeInfo(10,45,9);
    SetRTCDateInfo(20,03,2026);
    SetRTCDay(1);

    InitLCD();
    InitKPM();
    InitEINT0();

    // Clear the LCD before entering the main program.
    CmdLCD(0x01);

    // Program execution continues indefinitely.
    while(1)
    {
        // Read the latest RTC information.
        GetRTCTimeInfo(&hour, &minute, &second);
        GetRTCDateInfo(&date, &month, &year);
        GetRTCDay(&day);

        // Update the elapsed-second counter whenever time changes.
        if(second != prev_sec)
        {
            prev_sec = second;

            if(buzzer)
            {
                scount++;
            }
        }

        // Refresh the clock display while no alarm is active.
        if(buzzer == 0)
        {
            DisplayRTCTime(hour, minute, second);
            DisplayRTCDate(date, month, year);
            DisplayRTCDay(day);
        }

        // Automatically stop the buzzer after one minute.
        if(buzzer && scount >= 60)
        {
            IOCLR0 = 1 << EINT1_BUZZER;

            CmdLCD(0x01);
            CmdLCD(0x80);
            StrLCD(" MEDICINE ");

            CmdLCD(0xC0);
            StrLCD(" NOT TAKEN ");

            delay_ms(2000);

            buzzer = 0;
            scount = 0;
        }

        // Verify whether the current time matches any medicine schedule.
        if(mc == 1)
        {
            for(i = 0; i < 3; i++)
            {
                if(hour == mh[i] &&
                   minute == mm[i] &&
                   second == ms[i] &&
                   buzzer == 0)
                {
                    CmdLCD(0x01);
                    CmdLCD(0x80);
                    StrLCD("Take Medicine");

                    // Activate the buzzer to notify the user.
                    IOSET0 = 1 << EINT1_BUZZER;

                    buzzer = 1;
                    scount = 0;
                }
            }
        }
    }
}
