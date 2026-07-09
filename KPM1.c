//kpm.c
#include "types.h"
#include "defines.h"
#include <lpc21xx.h>
#include "kpm_defines.h"
#include "lcd.h"
#include "delay.h"

// Keypad lookup table used to convert row/column numbers into key values.
u8 kpmLUT[4][4]={{'1','2','3','/'},
                 {'4','5','6','*'},
                 {'7','8','9','-'},
                 {'C','0','=','+'}};

//***************************************************************
// Function : InitKPM()
// Purpose  : Sets the keypad row lines as output pins.
//***************************************************************
void InitKPM(void)
{
    // Configure the four row pins for output operation.
    WNIBBLE(IODIR1,ROW0,15);
}

//***************************************************************
// Function : ColScan()
// Purpose  : Determines whether any key is currently pressed.
// Returns  : 0 -> Key detected
//            1 -> No key press
//***************************************************************
u32 ColScan(void)
{
    // Read all column inputs and check for an active LOW signal.
    return ((RNIBBLE(IOPIN1,COL0) < 15) ? 0 : 1);
}

//***************************************************************
// Function : RowCheck()
// Purpose  : Finds the row in which the pressed key exists.
//***************************************************************
u32 RowCheck(void)
{
    u32 rno;

    // Enable one row at a time and monitor the columns.
    for(rno=0; rno<4; rno++)
    {
        WNIBBLE(IOPIN1,ROW0,~(1<<rno));

        if(ColScan()==0)
        {
            break;
        }
    }

    // Restore the keypad rows to the idle condition.
    WNIBBLE(IOPIN1,ROW0,0x0);

    return rno;
}

//***************************************************************
// Function : ColCheck()
// Purpose  : Finds the active column of the pressed key.
//***************************************************************
u32 ColCheck(void)
{
    u32 cno;

    // Scan each column until the active one is found.
    for(cno=0; cno<4; cno++)
    {
        if(RBIT(IOPIN1,(COL0+cno))==0)
        {
            break;
        }
    }

    return cno;
}

//***************************************************************
// Function : KeyScan()
// Purpose  : Detects a key press and returns its character.
//***************************************************************
u32 KeyScan(void)
{
    u32 rno,cno,keyv;

    // Wait until a key is pressed.
    while(ColScan());

    // Identify the row and column.
    rno = RowCheck();
    cno = ColCheck();

    // Fetch the corresponding character.
    keyv = kpmLUT[rno][cno];

    // Wait until the key is released.
    while(!ColScan());

    return keyv;
}

//***************************************************************
// Function : ReadNum()
// Purpose  : Reads a complete numeric value from the keypad.
//            Input ends when '=' is pressed.
//***************************************************************
u32 ReadNum(void)
{
    u8 key;
    u32 sum = 0;

    while(1)
    {
        key = KeyScan();

        if(key >= '0' && key <= '9')
        {
            // Display the digit on the LCD.
            CharLCD(key);

            // Form the final number.
            sum = (sum * 10) + (key - '0');
        }
        else if(key == '=')
        {
            break;
        }
    }

    return sum;
}

//***************************************************************
// Function : ReadNDigitInput()
// Purpose  : Reads a user-specified number of digits.
//            'C' deletes the previous digit.
//            '=' confirms the entered value.
//***************************************************************
u32 ReadNDigitInput(u8 max_digits)
{
    u8 key;
    u8 digits[5];
    u8 count = 0;

    while(1)
    {
        key = KeyScan();

        // Accept only numeric keys.
        if(key >= '0' && key <= '9')
        {
            if(count < max_digits)
            {
                digits[count++] = key;
                CharLCD(key);
            }
        }

        // Remove the last entered digit.
        else if(key == 'C' || key == 'c')
        {
            if(count > 0)
            {
                count--;

                CmdLCD(0x10);
                CharLCD(' ');
                CmdLCD(0x10);
            }
        }

        // Finish input and convert it into an integer.
        else if(key == '=')
        {
            if(count > 0)
            {
                u32 val = 0;
                u8 i;

                for(i = 0; i < count; i++)
                {
                    val = (val * 10) + (digits[i] - '0');
                }

                return val;
            }
        }
    }
}
