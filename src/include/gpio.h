#pragma once

#include <msp430.h>
#include <stdint.h>

#include "base.h"

namespace MSP
{
    enum GpioDir
    {
        GpioDir_Input  = 0,
        GpioDir_Output = 1
    };

    enum GpioPull
    {
        GpioPull_None = 0,
        GpioPull_Up   = 1,
        GpioPull_Down = 2
    };

    class GpioPin : public NonCopyable
    {
    public:
        GpioPin(uint8_t port, uint8_t bit) : port_(port), bit_(bit)
        {
        }

        void setDir(GpioDir dir)
        {
            volatile uint8_t* dirReg = (port_ == 1) ? &P1DIR : &P2DIR;

            if (dir == GpioDir_Output) *dirReg |= bit_;
            else *dirReg &= ~bit_;
        }

        void write(bool high)
        {
            volatile uint8_t* outReg = (port_ == 1) ? &P1OUT : &P2OUT;

            if (high) *outReg |= bit_;
            else *outReg &= ~bit_;
        }

        void toggle()
        {
            volatile uint8_t* outReg = (port_ == 1) ? &P1OUT : &P2OUT;
            *outReg ^= bit_;
        }

        bool read()
        {
            volatile uint8_t* inReg = (port_ == 1) ? &P1IN : &P2IN;
            return (*inReg & bit_) != 0;
        }

        void setPull(GpioPull pull)
        {
            volatile uint8_t* renReg = (port_ == 1) ? &P1REN : &P2REN;
            volatile uint8_t* outReg = (port_ == 1) ? &P1OUT : &P2OUT;

            if (pull == GpioPull_None) *renReg &= ~bit_;
            else
            {
                *renReg |= bit_;

                if (pull == GpioPull_Up) *outReg |= bit_;
                else *outReg &= ~bit_;
            }
        }

    private:
        uint8_t port_, bit_;
    };
}
