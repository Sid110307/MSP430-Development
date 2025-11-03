#include "include/uart.h"

extern "C"
{
#pragma vector = USCIAB0RX_VECTOR
	__interrupt void USCIAB0RX_ISR()
{
	if (IFG2 & UCA0RXIFG)
	{
		char c = UCA0RXBUF;
		MSP::UartA0::instance().handleInterrupt(c);
	}
}
}
