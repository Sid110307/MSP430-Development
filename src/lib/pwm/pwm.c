#include <msp430.h>
#include <string.h>

#include "../../include/uart.h"
#include "pwm.h"

void pwmHandleCli(const char* line)
{
	if (strncmp(line, "pwm", 3) == 0)
	{
		const char* p = line + 3;
		while (*p == ' ' || *p == '\t') p++;

		unsigned int v = 0;
		if (*p < '0' || *p > '9')
		{
			UartA0_write("ERR\r\n");
			return;
		}

		while (*p >= '0' && *p <= '9')
		{
			v = v * 10 + (unsigned)(*p - '0');
			p++;
		}
		if (v > 100) v = 100;

		TA0CCR1 = (unsigned long)(TA0CCR0 + 1) * v / 100;
		UartA0_write("OK\r\n");
	}
	else UartA0_write("ERR\r\n");
}
