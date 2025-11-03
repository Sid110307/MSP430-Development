#include <msp430.h>
#include <stdlib.h>
#include <string.h>

#include "include/base.h"
#include "include/gpio.h"
#include "include/timer.h"
#include "include/uart.h"
#include "include/uart_cli.h"

static MSP::GpioPin led(1, BIT0);
static volatile unsigned int blinkSteps = 0, blinkTick = 0;

static void cliHandler(const char* line)
{
  MSP::UartA0& uart = MSP::UartA0::instance();
  uart.write("\r\nReceived: ");
  uart.write(line);
  uart.write("\r\n");

  if (strcmp(line, "hello") == 0) uart.write("YAYYYYY!!!\r\n");
  else if (strcmp(line, "exit") == 0)
  {
    uart.write("byebye :(\r\n");
    blinkSteps = 6;
    blinkTick = 0;
  }
  else if (strcmp(line, "status") == 0)
  {
    uart.write("LED is ");;
    uart.write(led.read() ? "ON\r\n" : "OFF\r\n");
  }
  else if (strcmp(line, "led on") == 0)
  {
    led.write(true);
    uart.write("LED ON\r\n");
  }
  else if (strcmp(line, "led off") == 0)
  {
    led.write(false);
    uart.write("LED OFF\r\n");
  }
  else if (strcmp(line, "blink") == 0)
  {
    blinkSteps = 6;
    blinkTick = 0;
    uart.write("Blinking 3 times...\r\n");
  }
  else if (strncmp(line, "blink ", 6) == 0)
  {
    const char* arg = line + 6;
    int n = atoi(arg);

    if (n <= 0) uart.write("blink: argument must be > 0\r\n");
    else
    {
      blinkSteps = (unsigned int)(2 * n);
      blinkTick = 0;

      uart.write("Blinking ");
      uart.write(arg);
      uart.write(" times...\r\n");
    }
  }
  else uart.write("unknown cmd\r\n");
}

static void timerTick()
{
  if (blinkSteps > 0 && ++blinkTick >= 10)
  {
    blinkTick = 0;
    led.toggle();

    blinkSteps--;
    if (blinkSteps == 0) led.write(false);
  }
}

int main()
{
  WDTCTL = WDTPW + WDTHOLD;

  led.setDir(MSP::GpioDir_Output);
  led.write(false);

  MSP::UartA0& uart = MSP::UartA0::instance();
  uart.init();

  MSP::UartCli& cli = MSP::UartCli::instance();
  cli.init();
  cli.setLineHandler(&cliHandler);
  uart.write("MSP430 CLI ready\r\n");

  MSP::TimerA0& timer = MSP::TimerA0::instance();
  timer.init(10000, &timerTick);

  __bis_SR_register(LPM0_bits | GIE);

  while (true);
}
