//delay.c

//***************************************************************
// Function : delay_us()
// Purpose  : Produces a software delay in microseconds.
//            The loop count is adjusted for approximate timing.
//***************************************************************
void delay_us(unsigned int dlyus)
{
    // Convert the requested delay into loop iterations.
    for(dlyus *= 12; dlyus > 0; dlyus--);
}

//***************************************************************
// Function : delay_ms()
// Purpose  : Produces a software delay in milliseconds.
//            Uses a calibrated loop for approximate timing.
//***************************************************************
void delay_ms(unsigned int dlyms)
{
    // Scale milliseconds into the required number of iterations.
    for(dlyms *= 12000; dlyms > 0; dlyms--);
}

//***************************************************************
// Function : delay_s()
// Purpose  : Produces a software delay in seconds.
//            This is implemented using a large execution loop.
//***************************************************************
void delay_s(unsigned int dlys)
{
    // Convert seconds into equivalent loop count.
    for(dlys *= 12000000; dlys > 0; dlys--);
}
