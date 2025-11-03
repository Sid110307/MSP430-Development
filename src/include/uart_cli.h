#pragma once

#include <stdint.h>
#include "uart.h"

typedef struct
{
	void (*lineHandler)(const char*);
	char buffer[256];
	uint16_t bufferIndex;
	UartA0* uart;
} UartCli;

static void UartCli_rxCallbackStatic(char c);

static void UartCli_init(UartCli* self, UartA0* uart)
{
	self->lineHandler = 0;
	self->bufferIndex = 0;
	self->buffer[0] = '\0';
	self->uart = uart;

	UartA0_setCallback(uart, UartCli_rxCallbackStatic);
}

static void UartCli_setLineHandler(UartCli* self, void (*handler)(const char*)) { self->lineHandler = handler; }
static void UartCli_write(const UartCli* self, const char* str) { UartA0_write(self->uart, str); }

static void UartCli_rxCallback(UartCli* self, char c)
{
	if (c == '\r' || c == '\n')
	{
		if (self->bufferIndex > 0)
		{
			self->buffer[self->bufferIndex] = '\0';
			if (self->lineHandler) self->lineHandler(self->buffer);
			self->bufferIndex = 0;
		}
		UartA0_write(self->uart, "\r\n");
	}
	else if (self->bufferIndex < sizeof(self->buffer) - 1)
	{
		self->buffer[self->bufferIndex++] = c;
		UartA0_writeChar(self->uart, c);
	}
}

static void UartCli_rxCallbackStatic(const char c)
{
	extern UartCli cli;
	UartCli_rxCallback(&cli, c);
}
