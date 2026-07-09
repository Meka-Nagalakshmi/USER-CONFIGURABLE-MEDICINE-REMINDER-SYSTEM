//EINT0_INTERRUPT_MINI.C

#include <lpc21xx.h>
#include "delay.h"
#include "types.h"
#include "LCD_DEFINES.h"
#include "LCD.h"
#include "KPM.h"
#include "kpm_defines.h"
#include "eint0_defines_mini.h"
#include "eint0_mini.h"
#include "rtc_mini.h"

// Variables declared in another source file.
extern u8 alert_done;
extern u8 buzzer;

//***************************************************************
// Function : InitEINT0()
// Purpose  : Configures External Interrupts (EINT0 & EINT1)
//            and initializes the buzzer output pin.
//***************************************************************
void InitEINT0()
{
    // Reset the pin function selection bits.
    PINSEL0 &= ((u32)~3 << 2) | ((u32)~3 << 6);

    // Assign interrupt functionality to the required pins.
    PINSEL0 |= EINT0_INPUT | EINT1_INPUT;

    // Enable both interrupt channels in the VIC.
    VICIntEnable = (1 << EINT0_VIC_CHNO) | (1 << EINT1_VIC_CHNO);

    // Configure vector slot 0 for EINT0.
    VICVectCntl0 = (1 << 5) | EINT0_VIC_CHNO;

    // Configure vector slot 1 for EINT1.
    VICVectCntl1 = (1 << 5) | EINT1_VIC_CHNO;

    // Register the interrupt service routines.
    VICVectAddr0 = (u32)EINT0_ISR;
    VICVectAddr1 = (u32)EINT1_ISR;

    // Select edge-triggered mode for both interrupts.
    EXTMODE = (1 << 1) | (1 << 0);

    // Configure the buzzer GPIO pin as an output.
    IODIR0 |= 1 << EINT1_BUZZER;
}

//***************************************************************
// Function : menu()
// Purpose  : Displays the administrator menu and processes
//            the selected option.
//***************************************************************
void menu()
{
    u32 keyv;

    // Continue until the Exit option is selected.
    while(1)
    {
        // Display available menu options.
        CmdLCD(0x01);
        CmdLCD(0x80);
        StrLCD("1.EDITRTC 3.EXIT");

        CmdLCD(0xC0);
        StrLCD("2.EDIT MEDICINE");

        // Read the user's selection.
        keyv = KeyScan();

        switch(keyv)
        {
            case '1':
                RTC_Menu();          // Open RTC settings.
                break;

            case '2':
                Medicine_Menu();     // Open medicine schedule settings.
                break;

            case '3':
                CmdLCD(0x01);        // Clear the LCD before leaving.
                return;
        }
    }
}

//***************************************************************
// Function : EINT0_ISR()
// Purpose  : Interrupt routine executed when EINT0 occurs.
//            Opens the settings menu.
//***************************************************************
void EINT0_ISR(void) __irq
{
    // Launch the menu interface.
    menu();

    // Clear the interrupt status.
    EXTINT = 1 << 0;

    // Notify the VIC that interrupt servicing is complete.
    VICVectAddr = 0;
}

//***************************************************************
// Function : EINT1_ISR()
// Purpose  : Interrupt routine executed when EINT1 occurs.
//            Stops the buzzer after medicine confirmation.
//***************************************************************
void EINT1_ISR(void) __irq
{
    // Execute only if the alarm is active.
    if(buzzer)
    {
        // Disable the buzzer output.
        IOCLR0 = 1 << EINT1_BUZZER;

        // Display acknowledgement message.
        CmdLCD(0x01);
        CmdLCD(0x80);
        StrLCD("MEDICINE");

        CmdLCD(0xC0);
        StrLCD("TAKEN");

        // Keep the message visible briefly.
        delay_ms(2000);

        // Clear the alarm status.
        buzzer = 0;
    }

    // Reset the EINT1 interrupt flag.
    EXTINT = 1 << 1;

    // End interrupt processing.
    VICVectAddr = 0;
}
