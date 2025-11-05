#include <msp430.h>

#include "include/gpio.h"
#include "include/timer.h"
#include "include/uart.h"

#define CLI_MAX 32

static GpioPin led1, led2;

static char cliLine[CLI_MAX];
static volatile unsigned char cliReady = 0;
static volatile unsigned int cliLen = 0;

static void uartRx(const char c)
{
	static volatile unsigned char crlf = 0;

	if (cliReady) return;
	if (c == '\r' || c == '\n')
	{
		if (crlf)
		{
			crlf = 0;
			return;
		}

		crlf = 1;
		cliLine[cliLen] = '\0';
		cliReady = 1;

		return;
	}
	crlf = 0;

	if (c == 0x08 || c == 0x7F)
	{
		if (cliLen) cliLen--;
		return;
	}
	if (c >= 32 && c < 127 && cliLen + 1 < CLI_MAX) cliLine[cliLen++] = c;
}

static void timerTick()
{
	static const unsigned int onTime = 500, offTime = 500, numTimes = 2;
	static unsigned char state = 0;
	static unsigned int rem = 0, count = 0;

	if (state == 255) return;
	if (rem)
	{
		rem--;
		return;
	}

	switch (state)
	{
		case 0:
			if (count >= numTimes)
			{
				GpioPin_write(&led1, 0);
				state = 255;

				return;
			}

			GpioPin_write(&led1, 1);
			rem = onTime;
			state = 1;

			break;
		case 1:
			GpioPin_write(&led1, 0);
			rem = offTime;
			state = 0;
			count++;

			break;
		default:
			state = 255;
			break;
	}
}

int main()
{
	WDTCTL = WDTPW + WDTHOLD;
	if (CALBC1_1MHZ != 0xFF)
	{
		BCSCTL1 = CALBC1_1MHZ;
		DCOCTL = CALDCO_1MHZ;
	}

	UartA0_init();
	UartA0_setCallback(uartRx);

	GpioPin_init(&led1, 1, BIT0);
	GpioPin_useGPIO(&led1);
	GpioPin_setDir(&led1, GpioDir_Output);
	GpioPin_write(&led1, 0);

	GpioPin_init(&led2, 1, BIT6);
	GpioPin_useTimerA0(&led2);
	GpioPin_setDir(&led2, GpioDir_Output);
	GpioPin_write(&led2, 0);

	TimerA0_init(125, ID_3, &timerTick);
	TimerA0_start();

	static volatile unsigned int cliPrinted = 0;
	__enable_interrupt();
	UartA0_write("MSP430 Ready!\r\n");

	while (1)
	{
		__bis_SR_register(LPM0_bits | GIE);
		while (cliPrinted > cliLen)
		{
			UartA0_write("\b \b");
			cliPrinted--;
		}

		if (cliPrinted < cliLen)
		{
			unsigned int i = cliLen;
			while (i > cliPrinted)
			{
				i--;
				UartA0_writeChar(cliLine[i]);
			}

			cliPrinted = cliLen;
		}

		if (cliReady)
		{
			UartA0_write("\r\n");

			cliReady = 0;
			cliLen = 0;
			cliPrinted = 0;
		}
	}
}
