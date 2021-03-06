## Windows Driver for micro-wire ##

u-wire [by cpldcpu](https://github.com/cpldcpu/u-wire) is a minimal
(but spec-compliant!) USB firmware for the AVR atTiny10 microcontroller.
Using very clever assembly code, it provides support for the mandatory transactions required
for usb implementations using only 1kB of program memory and 32 bytes of RAM.
It can be used to build simple USB devices.

The standard kernel-mode windows driver provided here can be used to recognize
and communicate with devices running cpldcpu's firmware.

### Demo ###

![device recognition](assets/device_recognition.gif?raw=true "")

![basic functionality](assets/fade_and_color.gif?raw=true "")

### Requirements ###

- Tested with WDK for Windows 10, version 2004 (will likely work with other versions)
- Visual Studio with the Windows SDK and C++ tools
- Spectre-mitigated libraries for your target environment (selected in Visual Studio installation)

### Hardware Schematic ###

![Hardware Diagram](hw/hw_diagram.png?raw=true "")

### GUI ###

A bare-bones pyQt 5 GUI is included in /app/. It acts as a wrapper around C code
that uses IOCTL requests to communicate with the driver interface. The driver must of course
already be installed.

<!--
### Implementation ###

A prototype circuit built with a stripped-down usb cable:

![Circuit Prototype](assets/usb_hw_impl.jpg?raw=true "")
-->
### WinDbg Verification ###

Some debugging output showing the device driver is working correctly:

![Windbg](assets/windbg_verif.PNG?raw=true "")
