//rtc_mini.h

#include"types.h"
void InitRTC(void);
void RTC_Menu(void);
void Medicine_Menu(void);
void ShowMedicineTimes(u32 index);
u32 GetValidValue(char *msg, u32 max);


void GetRTCTimeInfo(s32 *hour, s32 *minute, s32 *second);
void DisplayRTCTime(u32 hour, u32 minute, u32 second);
void SetRTCTimeInfo(u32 hour, u32 minute, u32 second);

void DisplayMedTime(u32 hour, u32 minute, u32 second);
u8 SetMediTimeInfo(u32 index);

void GetRTCDateInfo(s32 *,s32 *,s32 *);
void DisplayRTCDate(u32,u32,u32);
void SetRTCDateInfo(u32,u32,u32);

void GetRTCDay(s32 *);
void DisplayRTCDay(u32);
void SetRTCDay(u32);
