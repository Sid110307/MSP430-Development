#include <msp430.h>
#include <string.h>

#include "include/gpio.h"
#include "include/timer.h"
#include "include/uart.h"
#include "lib/basic/basic.h"

static GpioPin led;

static volatile unsigned char cliReady = 0, crlf = 0;
static volatile unsigned int cliLen = 0;

static char cliLine[CLI_MAX];
static char basicBuffer[BASIC_BUF_SIZE];

static void cliHandler()
{
	static unsigned int basicLen = 0;

	if (strcmp(cliLine, "run") == 0)
	{
		if (basicLen == 0)
		{
			UartA0_writeSync("No program loaded.\r\n");
			return;
		}

		basicBuffer[basicLen] = '\0';
		basicInit(basicBuffer);
		do basicRun(); while (!basicFinished());

		UartA0_writeSync("\r\nProgram finished.\r\n");
	}
	else if (strcmp(cliLine, "clear") == 0)
	{
		basicLen = 0;
		basicBuffer[0] = '\0';

		UartA0_writeSync("Program cleared.\r\n");
	}
	else if (strcmp(cliLine, "list") == 0)
	{
		if (basicLen == 0) UartA0_writeSync("No program loaded.\r\n");
		else
		{
			UartA0_writeSync(basicBuffer);
			UartA0_writeSync("\r\n");
		}
	}
	else if (cliLine[0] == '\0') UartA0_writeSync("\r\n");
	else
	{
		const unsigned int len = strlen(cliLine);
		if (basicLen + len + 2 >= BASIC_BUF_SIZE)
		{
			UartA0_writeSync("Program too long.\r\n");
			return;
		}

		strcpy(&basicBuffer[basicLen], cliLine);
		basicLen += len;
		basicBuffer[basicLen++] = '\n';
		basicBuffer[basicLen] = '\0';

		UartA0_writeSync("\r\n");
	}
}

static void uartRx(const char c)
{
	if (cliReady) return;
	if (c == '\r' || c == '\n')
	{
		if (crlf)
		{
			crlf = 0;
			return;
		}

		crlf = 1;
		UartA0_write("\r\n");
		cliLine[cliLen] = '\0';
		cliReady = 1;

		return;
	}
	crlf = 0;

	if (c == 0x08 || c == 0x7F)
	{
		if (cliLen)
		{
			cliLen--;
			UartA0_write("\b \b");
		}
		return;
	}
	if (c >= 32 && c < 127)
	{
		if (cliLen + 1 < CLI_MAX)
		{
			cliLine[cliLen++] = c;
			UartA0_write((char[]){c, '\0'});
		}
		else UartA0_write("\a");
	}
}

static void timerTick()
{
	static unsigned char st = 0;
	static unsigned int rem = 0;

	if (st == 255) return;
	if (rem)
	{
		rem--;
		return;
	}

	switch (st)
	{
		case 0: GpioPin_write(&led, 1);
			rem = 500;
			st = 1;

			break;
		case 1: GpioPin_write(&led, 0);
			rem = 500;
			st = 2;

			break;
		case 2: GpioPin_write(&led, 1);
			rem = 500;
			st = 3;

			break;
		case 3:
		default:
			GpioPin_write(&led, 0);
			st = 255;

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

	GpioPin_init(&led, 1, BIT0);
	GpioPin_useGPIO(&led);
	GpioPin_setDir(&led, GpioDir_Output);
	GpioPin_write(&led, 0);

	UartA0_init();
	UartA0_setCallback(uartRx);
	UartA0_write("MSP430 BASIC ready\r\n> ");

	TimerA0_init(125, ID_3, &timerTick);
	TimerA0_start();

	__enable_interrupt();
	while (1)
	{
		__bis_SR_register(LPM0_bits | GIE);
		while (IFG2 & UCA0RXIFG) uartRx((char)UCA0RXBUF);

		if (cliReady)
		{
			cliReady = 0;
			cliLen = 0;
			cliHandler();

			UartA0_write("> ");
		}
	}
}
