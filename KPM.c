// kpm.c

#include <lpc21xx.h>
#include "type.h"
#include "defines.h"
#include "kpm_defines.h"
#include "kpm.h"
#include "lcd.h"
#include "lcd_defines.h"

// Lookup table for the 4x4 keypad layout.
u32 kpmLUT[4][4] =
{
    {'1','2','3','/'},
    {'4','5','6','*'},
    {'7','8','9','-'},
    {'c','0','=','+'}
};

u8 keyv;

#define IN_PUT 0xFFFFFFFF

//***************************************************************
// Function : InitKPM()
// Purpose  : Configures the keypad row pins as output pins.
//***************************************************************
void InitKPM(void)
{
    // Set all row lines as outputs.
    IODIR1 |= 15 << row1;
}

//***************************************************************
// Function : ColScan()
// Purpose  : Checks whether any key is pressed.
// Returns  : 0 -> Key detected
//            1 -> No key pressed
//***************************************************************
u32 ColScan(void)
{
    return ((((IOPIN1 >> cols1) & 15) < 15) ? 0 : 1);
}

//***************************************************************
// Function : KeyScan()
// Purpose  : Detects the pressed key and returns its value.
//***************************************************************
u32 KeyScan(void)
{
    u32 rno, cno;

    // Wait until a key press occurs.
    while(ColScan());

    // Determine the active row.
    rno = RowCheck();

    // Determine the active column.
    cno = ColCheck();

    // Read the corresponding key from the lookup table.
    keyv = kpmLUT[rno][cno];

    // Wait until the key is released.
    while(!ColScan());

    return keyv;
}

//***************************************************************
// Function : RowCheck()
// Purpose  : Identifies the row containing the pressed key.
//***************************************************************
u32 RowCheck(void)
{
    u32 rno;

    // Activate one row at a time.
    for(rno = 0; rno < 4; rno++)
    {
        IOPIN1 = (IOPIN1 & ~(15 << row1)) | ((~(1 << rno)) << row1);

        if(ColScan() == 0)
        {
            break;
        }
    }

    // Restore all rows to the idle state.
    IOPIN1 = (IOPIN1 & ~(15 << row1)) | (0x0 << row1);

    return rno;
}

//***************************************************************
// Function : ColCheck()
// Purpose  : Determines the column of the pressed key.
//***************************************************************
u32 ColCheck(void)
{
    u32 cno;

    // Scan each column until an active one is found.
    for(cno = 0; cno < 4; cno++)
    {
        if(((IOPIN1 >> (cols1 + cno)) & 1) == 0)
        {
            break;
        }
    }

    return cno;
}

//***************************************************************
// Function : ReadNum()
// Purpose  : Reads a numeric value from the keypad.
//            '=' is used to finish the input.
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
            // Display the entered digit.
            CharLCD(key);

            // Build the numeric value.
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
// Purpose  : Reads a user-defined number of digits.
//            'c' performs backspace.
//            '=' confirms the input.
//***************************************************************
u32 ReadNDigitInput(u8 max_digits)
{
    u8 key;
    u8 digits[5];
    u8 count = 0;

    while(1)
    {
        key = KeyScan();

        if(key >= '0' && key <= '9')
        {
            if(count < max_digits)
            {
                digits[count++] = key;
                CharLCD(key);
            }
        }

        else if(key == 'c')
        {
            if(count > 0)
            {
                count--;

                // Remove the previous digit from the display.
                CmdLCD(0x10);
                CharLCD(' ');
                CmdLCD(0x10);
            }
        }

        else if(key == '=')
        {
            if(count > 0)
            {
                u32 val = 0;
                u8 i;

                // Convert the digit array into an integer.
                for(i = 0; i < count; i++)
                {
                    val = (val * 10) + (digits[i] - '0');
                }

                return val;
            }
        }

        /*
        else if(key == '+')
        {
            return IN_PUT;
        }
        */
    }
}

//***************************************************************
// Function : Read2Digit()
// Purpose  : Reads exactly two digits from the keypad.
//***************************************************************
u32 Read2Digit(void)
{
    u32 d1, d2;

    // Read and display the first digit.
    d1 = KeyScan();
    CharLCD(d1);

    // Read and display the second digit.
    d2 = KeyScan();
    CharLCD(d2);

    // Return the combined two-digit number.
    return (d1 - '0') * 10 + (d2 - '0');
}
