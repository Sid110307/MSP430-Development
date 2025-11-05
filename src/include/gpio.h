#pragma once

typedef enum
{
	GpioDir_Input,
	GpioDir_Output
} GpioDir;

typedef enum
{
	GpioPull_None,
	GpioPull_Up,
	GpioPull_Down
} GpioPull;

typedef struct
{
	volatile unsigned char* GpioDir;
	volatile unsigned char* GpioOut;
	volatile const unsigned char* GpioIn;
	volatile unsigned char* GpioRen;
	volatile unsigned char* GpioSel;
	volatile unsigned char* GpioSel2;
	unsigned char bit;
} GpioPin;

int GpioPin_init(GpioPin* pin, unsigned char port, unsigned char bit);
void GpioPin_useGPIO(const GpioPin* pin);
void GpioPin_useTimerA0(const GpioPin* pin);
void GpioPin_setDir(const GpioPin* pin, GpioDir dir);
void GpioPin_write(const GpioPin* pin, unsigned char high);
void GpioPin_toggle(const GpioPin* pin);
unsigned char GpioPin_read(const GpioPin* pin);
void GpioPin_setPull(const GpioPin* pin, GpioPull pull);
