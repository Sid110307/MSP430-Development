#include "include/timer.h"

extern "C"
{
#pragma vector = TIMER0_A0_VECTOR
	__interrupt void TIMER0_A0_ISR()
{
	TA0CCTL0 &= ~CCIFG;
	MSP::TimerA0::instance().handleInterrupt();
}
}
