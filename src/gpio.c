#include <msp430.h>
#include "include/gpio.h"

static int mapPort(const unsigned char port, volatile unsigned char** dir, volatile unsigned char** out,
                   volatile const unsigned char** in, volatile unsigned char** ren
#if defined(P1SEL) && defined(P1SEL2)
                   , volatile unsigned char **sel, volatile unsigned char** sel2
#endif
)
{
	switch (port)
	{
		case 1:
			*dir = &P1DIR;
			*out = &P1OUT;
			*in = &P1IN;
			*ren = &P1REN;
#if defined(P1SEL) && defined(P1SEL2)
			*sel = &P1SEL;
			*sel2 = &P1SEL2;
#endif
			return 0;
		case 2:
			*dir = &P2DIR;
			*out = &P2OUT;
			*in = &P2IN;
			*ren = &P2REN;
#if defined(P2SEL) && defined(P2SEL2)
			*sel = &P2SEL;
			*sel2 = &P2SEL2;
#endif
			return 0;
		default:
			return -1;
	}
}

int GpioPin_init(GpioPin* pin, unsigned char port, const unsigned char bit)
{
	if (!pin) return -1;

	volatile unsigned char *dir, *out, *ren;
	volatile const unsigned char* in;

#if defined(P1SEL) && defined(P1SEL2)
	volatile unsigned char *sel, *sel2;
	if (mapPort(port, &dir, &out, &in, &ren, &sel, &sel2) != 0) return -1;
	pin->GpioSel = sel;
	pin->GpioSel2 = sel2;
#else
	if (mapPort(port, &dir, &out, &in, &ren) != 0) return -1;
#endif

	pin->GpioDir = dir;
	pin->GpioOut = out;
	pin->GpioIn = in;
	pin->GpioRen = ren;
	pin->bit = bit;

	return 0;
}

void GpioPin_useGPIO(GpioPin* pin)
{
#if defined(P1SEL) && defined(P1SEL2)
	*(pin->GpioSel) &= ~(pin->bit);
	*(pin->GpioSel2) &= ~(pin->bit);
#endif
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
