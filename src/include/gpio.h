#pragma once

#include <msp430.h>
#include <stdint.h>

typedef enum
{
	GpioDir_Input  = 0,
	GpioDir_Output = 1
} GpioDir;

typedef enum
{
	GpioPull_None = 0,
	GpioPull_Up   = 1,
	GpioPull_Down = 2
} GpioPull;

typedef struct
{
	uint8_t port, bit;
} GpioPin;

static void GpioPin_init(GpioPin* self, const uint8_t port, const uint8_t bit)
{
	self->port = port;
	self->bit = bit;
}

static void GpioPin_setDir(const GpioPin* self, const GpioDir dir)
{
	volatile uint8_t* dirReg = self->port == 1 ? &P1DIR : &P2DIR;

	if (dir == GpioDir_Output) *dirReg |= self->bit;
	else *dirReg &= (uint8_t)~self->bit;
}

static void GpioPin_write(const GpioPin* self, const uint8_t high)
{
	volatile uint8_t* outReg = self->port == 1 ? &P1OUT : &P2OUT;

	if (high) *outReg |= self->bit;
	else *outReg &= (uint8_t)~self->bit;
}

static void GpioPin_toggle(const GpioPin* self)
{
	volatile uint8_t* outReg = self->port == 1 ? &P1OUT : &P2OUT;
	*outReg ^= self->bit;
}

static uint8_t GpioPin_read(const GpioPin* self)
{
	const volatile uint8_t* inReg = self->port == 1 ? &P1IN : &P2IN;
	return (*inReg & self->bit) != 0;
}

static void GpioPin_setPull(const GpioPin* self, const GpioPull pull)
{
	volatile uint8_t *renReg = self->port == 1 ? &P1REN : &P2REN, *outReg = self->port == 1 ? &P1OUT : &P2OUT;

	if (pull == GpioPull_None) *renReg &= (uint8_t)~self->bit;
	else
	{
		*renReg |= self->bit;

		if (pull == GpioPull_Up) *outReg |= self->bit;
		else *outReg &= (uint8_t)~self->bit;
	}
}
