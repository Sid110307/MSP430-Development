#pragma once

#include <msp430.h>

namespace MSP
{
  struct Clock
  {
    static const unsigned long Freq_SMCLK = 1000000UL;
  };

  struct NonCopyable
  {
  protected:
    NonCopyable()
    {
    }

    ~NonCopyable()
    {
    }

  private:
    NonCopyable(const NonCopyable&);
    NonCopyable& operator=(const NonCopyable&);
  };
}
