#include <msp430.h>
#include <stdlib.h>
#include <string.h>

#include "include/gpio.h"
#include "include/timer.h"
#include "include/uart.h"
#include "include/uart_cli.h"

static GpioPin led;
static volatile unsigned int blinkSteps = 0, blinkTick = 0;

static UartA0 uart;
static UartCli cli;
static TimerA0 timer;

static void cliHandler(const char* line)
{
	UartA0_write(&uart, "\r\nReceived: ");
	UartA0_write(&uart, line);
	UartA0_write(&uart, "\r\n");

	if (strcmp(line, "hello") == 0) UartA0_write(&uart, "YAYYYYY!!!\r\n");
	else if (strcmp(line, "exit") == 0)
	{
		UartA0_write(&uart, "byebye :(\r\n");
		blinkSteps = 6;
		blinkTick = 0;
	}
	else if (strcmp(line, "status") == 0)
	{
		UartA0_write(&uart, "LED is ");
		UartA0_write(&uart, GpioPin_read(&led) ? "ON\r\n" : "OFF\r\n");
	}
	else if (strcmp(line, "led on") == 0)
	{
		GpioPin_write(&led, 1);
		UartA0_write(&uart, "LED ON\r\n");
	}
	else if (strcmp(line, "led off") == 0)
	{
		GpioPin_write(&led, 0);
		UartA0_write(&uart, "LED OFF\r\n");
	}
	else if (strcmp(line, "blink") == 0)
	{
		blinkSteps = 6;
		blinkTick = 0;
		UartA0_write(&uart, "Blinking 3 times...\r\n");
	}
	else if (strncmp(line, "blink ", 6) == 0)
	{
		const char* arg = line + 6;
		const int n = atoi(arg);

		if (n <= 0) UartA0_write(&uart, "blink: argument must be > 0\r\n");
		else
		{
			blinkSteps = (unsigned int)(2 * n);
			blinkTick = 0;

			UartA0_write(&uart, "Blinking ");
			UartA0_write(&uart, arg);
			UartA0_write(&uart, " times...\r\n");
		}
	}
	else UartA0_write(&uart, "unknown cmd\r\n");
}

static void timerTick()
{
	if (blinkSteps > 0 && ++blinkTick >= 10)
	{
		blinkTick = 0;
		GpioPin_toggle(&led);

		blinkSteps--;
		if (blinkSteps == 0) GpioPin_write(&led, 0);
	}
}

int main()
{
	WDTCTL = WDTPW + WDTHOLD;

	GpioPin_init(&led, 1, BIT0);
	GpioPin_setDir(&led, GpioDir_Output);
	GpioPin_write(&led, 0);

	UartA0_init(&uart);

	UartCli_init(&cli, &uart);
	UartCli_setLineHandler(&cli, &cliHandler);
	UartA0_write(&uart, "MSP430 CLI ready\r\n");

	TimerA0_init(&timer, 10000, ID_0, &timerTick);

	__bis_SR_register(LPM0_bits | GIE);
	while (1);
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR()
{
	TA0CCTL0 &= ~CCIFG;
	TimerA0_handleInterrupt(&timer);
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR() { if (IFG2 & UCA0RXIFG) UartA0_handleInterrupt(&uart, (char)UCA0RXBUF); }
