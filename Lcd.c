//LCD.c

#include <lpc21xx.h>     
#include "LCD_defines.h"
#include "types.h"
#include "defines.h"
#include "delay.h"

//***************************************************************
// Function : WriteLCD()
// Purpose  : Transfers a single byte to the LCD.
//            The byte may represent either a command or data,
//            depending on the RS signal.
//***************************************************************
void WriteLCD(u8 data)
{
    // Select write mode.
    SCLRBIT(IOCLR0, RW);

    // Place the byte on the LCD data bus.
    WBYTE(IOPIN0, LCD_DATA, data);

    // Generate an enable pulse.
    SSETBIT(IOSET0, EN);
    delay_us(1);

    // Finish the enable pulse to latch the data.
    SCLRBIT(IOCLR0, EN);

    // Allow time for LCD execution.
    delay_ms(2);
}

//***************************************************************
// Function : CmdLCD()
// Purpose  : Sends an instruction to the LCD controller.
//***************************************************************
void CmdLCD(u8 cmd)
{
    // RS LOW selects the instruction register.
    SCLRBIT(IOCLR0, RS);

    // Send the command.
    WriteLCD(cmd);
}

//***************************************************************
// Function : CharLCD()
// Purpose  : Writes one displayable character to the LCD.
//***************************************************************
void CharLCD(u8 ascii)
{
    // RS HIGH selects the data register.
    SSETBIT(IOSET0, RS);

    // Transfer the character.
    WriteLCD(ascii);
}

//***************************************************************
// Function : InitLCD()
// Purpose  : Performs LCD initialization in 8-bit mode.
//***************************************************************
void InitLCD(void)
{
    // Configure LCD data pins as outputs.
    WBYTE(IODIR0, LCD_DATA, 255);

    // Configure LCD control pins as outputs.
    SETBIT(IODIR0, RS);
    SETBIT(IODIR0, RW);
    SETBIT(IODIR0, EN);

    // Wait for the LCD to stabilize after power-up.
    delay_ms(15);

    // Standard initialization sequence.
    CmdLCD(MODE_8BIT_1LINE);
    delay_ms(5);

    CmdLCD(MODE_8BIT_1LINE);
    delay_us(100);

    CmdLCD(MODE_8BIT_1LINE);

    // Configure display parameters.
    CmdLCD(MODE_8BIT_2LINE);
    CmdLCD(DISP_ON_CUR_ON);
    CmdLCD(CLEAR_LCD);
    CmdLCD(SHIFT_CUR_RIGHT);
}

//***************************************************************
// Function : StrLCD()
// Purpose  : Displays a complete null-terminated string.
//***************************************************************
void StrLCD(s8 *p)
{
    // Print characters until the string terminator is reached.
    while(*p)
        CharLCD(*p++);
}

//***************************************************************
// Function : U32LCD()
// Purpose  : Displays an unsigned 32-bit integer on the LCD.
//***************************************************************
void U32LCD(u32 n)
{
    u8 a[10], i = 0;

    // Handle zero separately.
    if(n == 0)
    {
        CharLCD('0');
        return;
    }

    // Convert the number into ASCII digits.
    while(n)
    {
        a[i++] = (n % 10) + '0';
        n /= 10;
    }

    // Display digits in the correct order.
    while(i)
        CharLCD(a[--i]);
}

/*
//***************************************************************
// Function : S32LCD()
// Purpose  : Displays a signed 32-bit integer.
//***************************************************************
void S32LCD(s32 n)
{
    // Print minus sign for negative values.
    if(n < 0)
    {
        CharLCD('-');
        n = -n;
    }

    // Display the magnitude.
    U32LCD(n);
}
*/
