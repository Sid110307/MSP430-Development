#pragma once

#include <msp430.h>

typedef struct
{
	void (*rxCallback)(char);
} UartA0;

static void UartA0_init(UartA0* self)
{
	P1SEL |= BIT1 | BIT2;
	P1SEL2 |= BIT1 | BIT2;

	UCA0CTL1 |= UCSWRST;
	UCA0CTL1 |= UCSSEL_2;

	UCA0BR0 = 104;
	UCA0BR1 = 0;
	UCA0MCTL = UCBRS_1;

	UCA0CTL1 &= ~UCSWRST;
	IE2 |= UCA0RXIE;

	self->rxCallback = 0;
}

static void UartA0_writeChar(const UartA0* self, const char c)
{
	(void)self;

	while (!(IFG2 & UCA0TXIFG))
	{
	}

	UCA0TXBUF = c;
}

static void UartA0_write(const UartA0* self, const char* str) { while (*str) UartA0_writeChar(self, *str++); }
static void UartA0_setCallback(UartA0* self, void (*callback)(char)) { self->rxCallback = callback; }
static void UartA0_handleInterrupt(const UartA0* self, const char c) { if (self->rxCallback) self->rxCallback(c); }
