# MSP430 development environment

This repository provides a small firmware environment for the Texas Instruments MSP430G2553. It includes GPIO, ADC, timer, UART, PWM, and BASIC-related modules together with build and flashing targets.

## Toolchain

The current Makefile is configured for Windows and expects:

- TI Code Composer Studio compiler tools
- TI UniFlash
- An MSP430G2553 target
- GNU Make or a compatible `make` command

The tool locations at the top of `Makefile` are machine-specific. Update `CCS_ROOT` and related paths before building.

## Build and flash

```sh
make
make flash
```

Build output is written to `bin/`, including the linked firmware and TI-TXT image. To remove generated output:

```sh
make clean
```

## Source layout

- `src/main.c` — firmware entry point
- `src/gpio.c`, `adc.c`, `timer.c`, `uart.c` — peripheral support
- `src/lib/pwm/` — PWM helpers
- `src/lib/basic/` — BASIC tokenizer and runtime support
- `lib/MSP430G2553.ccxml` — UniFlash target configuration
