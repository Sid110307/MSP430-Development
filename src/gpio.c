#include <msp430.h>
#include "include/gpio.h"

static int mapPort(const unsigned char port, volatile unsigned char** dir, volatile unsigned char** out,
                   volatile const unsigned char** in, volatile unsigned char** ren, volatile unsigned char** sel,
                   volatile unsigned char** sel2)
{
	*sel = 0;
	*sel2 = 0;

	switch (port)
	{
		case 1:
			*dir = &P1DIR;
			*out = &P1OUT;
			*in = &P1IN;
			*ren = &P1REN;
			*sel = &P1SEL;
			*sel2 = &P1SEL2;

			return 0;
		case 2:
			*dir = &P2DIR;
			*out = &P2OUT;
			*in = &P2IN;
			*ren = &P2REN;
			*sel = &P2SEL;
			*sel2 = &P2SEL2;

			return 0;
		default:
			return -1;
	}
}

int GpioPin_init(GpioPin* pin, unsigned char port, const unsigned char bit)
{
	if (!pin) return -1;

	volatile unsigned char *dir, *out, *ren, *sel, *sel2;
	volatile const unsigned char* in;

	if (mapPort(port, &dir, &out, &in, &ren, &sel, &sel2) != 0) return -1;
	pin->GpioDir = dir;
	pin->GpioOut = out;
	pin->GpioIn = in;
	pin->GpioRen = ren;
	pin->GpioSel = sel;
	pin->GpioSel2 = sel2;
	pin->bit = bit;

	return 0;
}

void GpioPin_useGPIO(const GpioPin* pin)
{
	if (pin->GpioSel) *(pin->GpioSel) &= ~(pin->bit);
	if (pin->GpioSel2) *(pin->GpioSel2) &= ~(pin->bit);
}

void GpioPin_useTimerA0(const GpioPin* pin)
{
	if (pin->GpioSel) *(pin->GpioSel) |= pin->bit;
	if (pin->GpioSel2) *(pin->GpioSel2) &= ~pin->bit;
}

void GpioPin_setDir(const GpioPin* pin, const GpioDir dir)
{
	if (dir == GpioDir_Output) *pin->GpioDir |= pin->bit;
	else *pin->GpioDir &= ~pin->bit;
}

void GpioPin_write(const GpioPin* pin, const unsigned char high)
{
	if (high) *pin->GpioOut |= pin->bit;
	else *pin->GpioOut &= ~pin->bit;
}

void GpioPin_toggle(const GpioPin* pin) { *pin->GpioOut ^= pin->bit; }
unsigned char GpioPin_read(const GpioPin* pin) { return (*pin->GpioIn & pin->bit) != 0; }

void GpioPin_setPull(const GpioPin* pin, const GpioPull pull)
{
	if (pull == GpioPull_None)
	{
		*pin->GpioRen &= ~pin->bit;
		return;
	}
	*pin->GpioRen |= pin->bit;

	if (pull == GpioPull_Up) *pin->GpioOut |= pin->bit;
	else *pin->GpioOut &= ~pin->bit;
}
