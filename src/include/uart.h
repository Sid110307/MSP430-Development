#pragma once

#include <msp430.h>
#include "base.h"

namespace MSP
{
	class UartA0 : public NonCopyable
	{
	public:
		static UartA0& instance()
		{
			static UartA0 instance;
			return instance;
		}

		void init()
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
		}

		void writeChar(char c)
		{
			while (!(IFG2 & UCA0TXIFG));
			UCA0TXBUF = c;
		}

		void write(const char* str) { while (*str) writeChar(*str++); }

		void setCallback(void (*callback)(char)) { rxCallback_ = callback; }

		void handleInterrupt(char c) { if (rxCallback_) rxCallback_(c); }

	private:
		UartA0() : rxCallback_(0)
		{
		}

		void (*rxCallback_)(char);
	};
}
