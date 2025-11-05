#include <msp430.h>
#include "include/adc.h"

static AdcRef adcRef = AdcRef_Vcc;

void Adc_init(const AdcRef ref, const AdcClock clock, const unsigned char divider, const unsigned char sampleHoldTime)
{
	adcRef = ref;

	ADC10CTL0 = 0;
	ADC10CTL1 = 0;

	unsigned int ctl0 = ADC10ON | sampleHoldTime;
	switch (ref)
	{
		case AdcRef_Vcc:
			ctl0 |= SREF_0;
			break;
		case AdcRef_Internal1V5:
			ctl0 |= SREF_1 | REFON;
			break;
		case AdcRef_Internal2V5:
			ctl0 |= SREF_1 | REFON | REF2_5V;
			break;
		default:
			break;
	}

	ADC10CTL0 = ctl0;
	ADC10CTL1 = clock | divider | INCH_0;

	if (ref != AdcRef_Vcc) __delay_cycles(1000);
}

void Adc_shutdown()
{
	ADC10CTL0 &= ~ENC;
	ADC10CTL0 &= ~(REFON | ADC10ON);
}

void Adc_start(const AdcChannel channel)
{
	ADC10CTL0 &= ~ENC;
	ADC10CTL1 = ADC10CTL1 & ~INCH_7 | (unsigned int)channel.channel << 12;
	ADC10AE0 |= channel.bitMask;
	ADC10CTL0 |= ENC | ADC10SC;
}

unsigned int Adc_readChannel(const AdcChannel channel)
{
	Adc_start(channel);
	while (ADC10CTL1 & ADC10BUSY);

	const unsigned int result = ADC10MEM;
	ADC10CTL0 &= ~ENC;

	return result;
}

unsigned int Adc_toMillivolts(const unsigned int adcValue)
{
	unsigned int refMillivolts = 3300;

	switch (adcRef)
	{
		case AdcRef_Internal1V5:
			refMillivolts = 1500;
			break;
		case AdcRef_Internal2V5:
			refMillivolts = 2500;
			break;
		case AdcRef_Vcc:
		default:
			break;
	}

	return (unsigned int)(((unsigned long)adcValue * refMillivolts + 511) / 1023);
}
