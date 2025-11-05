#pragma once

typedef enum
{
	AdcRef_Vcc,
	AdcRef_Internal1V5,
	AdcRef_Internal2V5
} AdcRef;

typedef enum
{
	AdcClock_Internal = ADC10SSEL_0,
	AdcClock_ACLK     = ADC10SSEL_1,
	AdcClock_MCLK     = ADC10SSEL_2,
	AdcClock_SMCLK    = ADC10SSEL_3
} AdcClock;

typedef struct
{
	unsigned char channel, bitMask;
} AdcChannel;

void Adc_init(AdcRef ref, AdcClock clock, unsigned char divider, unsigned char sampleHoldTime);
void Adc_shutdown();
void Adc_start(AdcChannel channel);

unsigned int Adc_readChannel(AdcChannel channel);
unsigned int Adc_toMillivolts(unsigned int adcValue);
