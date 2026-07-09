//lcd.h
#include "types.h"
void WriteLCD(u8 data);
void CmdLCD(u8 cmd);
void CharLCD(u8 ascii);
void InitLCD(void);
void StrLCD(s8* p);
void U32LCD(u32 n);
void S32LCD(s32 n);
void F32LCD(f32 fnum, u8 nDP);
void HEXLCD(u32 n);
void BinLCD(u32 n, u8 nbd);
void BuildCGRAM(u8* p, u8 nb);
