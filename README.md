## Windows Driver for micro-wire ##

u-wire [by cpldcpu](https://github.com/cpldcpu/u-wire) is a minimal 
(but spec-compliant!) USB firmware for the AVR atTiny10 microcontroller.
Using incredibly clever assembly code, it provides support for the mandatory transactions required 
for usb using only 1kB of program memory and 32 bytes of RAM.
It can be used to build simple USB devices such a controllable RGB LED.

The standard windows driver provided here can be used to recognize 
and communicate with a device running cpldcpu's firmware.

### Requirements ###

- Tested with WDK for Windows 10, version 1903 (will likely work with other versions)
- Visual Studio with the Windows SDK and C++ tools
- Spectre-mitigated libraries for your target environment (selected in Visual Studio installation)

### Hardware Schematic ###

TBD

### Implementation ###

A prototype circuit built with a stripped-down usb cable:

![Circuit Prototype](assets/usb_hw_impl.jpg?raw=true "")

### Proof ###

TBD