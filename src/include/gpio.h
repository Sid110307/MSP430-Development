#pragma once

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
	volatile unsigned char* GpioDir;
	volatile unsigned char* GpioOut;
	volatile const unsigned char* GpioIn;
	volatile unsigned char* GpioRen;
#if defined(P1SEL) && defined(P1SEL2)
	volatile unsigned char* GpioSel;
	volatile unsigned char* GpioSel2;
#endif
	unsigned char bit;
} GpioPin;

int GpioPin_init(GpioPin* pin, unsigned char port, unsigned char bit);
void GpioPin_useGPIO(GpioPin* pin);
void GpioPin_setDir(const GpioPin* pin, GpioDir dir);
void GpioPin_write(const GpioPin* pin, unsigned char high);
void GpioPin_toggle(const GpioPin* pin);
unsigned char GpioPin_read(const GpioPin* pin);
void GpioPin_setPull(const GpioPin* pin, GpioPull pull);
