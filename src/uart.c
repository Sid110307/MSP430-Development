#include <msp430.h>
#include "include/uart.h"

static volatile UartRxCallback rxCallback = 0;

void UartA0_init()
{
	P1SEL |= BIT1 | BIT2;
	P1SEL2 |= BIT1 | BIT2;

	UCA0CTL1 |= UCSWRST;
	UCA0CTL1 |= UCSSEL_2;

	UCA0BR0 = 104;
	UCA0BR1 = 0;
	UCA0MCTL = UCBRS_1;

	IFG2 &= ~(UCA0TXIFG | UCA0RXIFG);
	UCA0CTL1 &= ~UCSWRST;
	UC0IE |= UCA0RXIE;
}

void UartA0_setCallback(const UartRxCallback cb) { rxCallback = cb; }
unsigned char UartA0_busy() { return UCA0STAT & UCBUSY; }

void UartA0_writeChar(char c)
{
	while (!(IFG2 & UCA0TXIFG));
	UCA0TXBUF = c;
}

void UartA0_write(const char* str)
{
	if (!str) return;
	while (*str) UartA0_writeChar(*str++);
}

void UartA0_writeSync(const char* str)
{
	if (!str) return;

	UartA0_write(str);
	while (UartA0_busy());
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR()
{
	if (IFG2 & UCA0RXIFG)
	{
		const char c = UCA0RXBUF;
		if (rxCallback) rxCallback(c);

		__bic_SR_register_on_exit(LPM0_bits);
	}
}
