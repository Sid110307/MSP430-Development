#include <msp430.h>
#include "include/uart.h"

static volatile unsigned int txHead = 0, txTail = 0;
static volatile char txBuffer[TX_BUF_SIZE];
static volatile UartRxCallback rxCallback = 0;

static unsigned char txIsEmpty() { return txHead == txTail; }
static unsigned char txIsFull() { return (txHead + 1 & TX_BUF_SIZE - 1) == txTail; }

static unsigned char txPutChar(const char c)
{
	const unsigned int next = txHead + 1 & TX_BUF_SIZE - 1;
	if (next == txTail) return 0;

	txBuffer[txHead] = c;
	txHead = next;

	return 1;
}

static unsigned char txGetChar(char* out)
{
	if (txIsEmpty()) return 0;

	*out = txBuffer[txTail];
	txTail = txTail + 1 & TX_BUF_SIZE - 1;

	return 1;
}

static void txKick() { IE2 |= UCA0TXIE; }

void UartA0_init()
{
	P1SEL |= BIT1 | BIT2;
	P1SEL2 |= BIT1 | BIT2;

	UCA0CTL1 |= UCSWRST;
	UCA0CTL1 = UCA0CTL1 & ~UCSSEL_3 | UCSSEL_2;

	UCA0BR0 = 104;
	UCA0BR1 = 0;
	UCA0MCTL = UCBRS_1;

	IFG2 &= ~(UCA0TXIFG | UCA0RXIFG);
	IE2 |= UCA0RXIE;
	UCA0CTL1 &= ~UCSWRST;

	txHead = 0;
	txTail = 0;
}

void UartA0_setCallback(const UartRxCallback cb) { rxCallback = cb; }
unsigned char UartA0_busy() { return !txIsEmpty() || UCA0STAT & UCBUSY || IE2 & UCA0TXIE; }

void UartA0_write(const char* str)
{
	if (!str) return;

	while (*str)
	{
		if (!txPutChar(*str)) break;
		++str;
	}
	if (!txIsEmpty()) txKick();
}

void UartA0_writeSync(const char* str)
{
	if (!str) return;
	while (*str)
	{
		while (txIsFull()) txKick();
		txPutChar(*str++);
	}

	txKick();
	while (UartA0_busy()) __bis_SR_register(LPM0_bits | GIE);
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

#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR()
{
	if (IFG2 & UCA0TXIFG)
	{
		char out;

		if (txGetChar(&out)) UCA0TXBUF = out;
		else IE2 &= ~UCA0TXIE;
	}
}
