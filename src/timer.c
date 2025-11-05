#include <msp430.h>
#include "include/timer.h"

static volatile TimerA0Callback timerCallback = 0;

void TimerA0_init(const unsigned int interval, const unsigned int divider, const TimerA0Callback cb)
{
	if (divider != ID_0 && divider != ID_1 && divider != ID_2 && divider != ID_3) return;
	timerCallback = cb;

	TA0CTL = 0;
	TA0CCTL0 = 0;

	TA0CCR0 = interval > 0 ? interval - 1 : 0;
	TA0CCTL1 = OUTMOD_7;
	TA0CCTL0 = CCIE;
	TA0CCTL0 &= ~CCIFG;

	TA0CTL = TASSEL_2 | divider | MC_1 | TACLR;
}

void TimerA0_start()
{
	TA0CCTL0 &= ~CCIFG;
	TA0CTL &= ~MC_3;
	TA0CTL |= MC_1;
}

void TimerA0_stop() { TA0CTL &= ~MC_3; }

void TimerA0_setInterval(const unsigned int interval)
{
	const unsigned int mode = TA0CTL & MC_3;

	TA0CTL &= ~MC_3;
	TA0CCR0 = interval > 0 ? interval - 1 : 0;
	TA0CCTL0 &= ~CCIFG;
	TA0CTL |= mode;
}

void TimerA0_setCallback(const TimerA0Callback cb) { timerCallback = cb; }

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR()
{
	TA0CCTL0 &= ~CCIFG;
	if (timerCallback) timerCallback();

	__bic_SR_register_on_exit(LPM0_bits);
}
