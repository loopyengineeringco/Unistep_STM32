# Unistep_STM32
STM32F030F4 based Unipolar stepper driver (step/dir/enable > coil control)

This an STM32F030F4 based Unipolar stepper driver - taking step/dir/enable and controlling 4 coils via mosfets/darlington array at 1/2 steps.
Hardware is minimal: bare STM32F030F4, (no external osc needed) and a ULN2003, or something more powerful if needed.

Arduino IDE, official STM32 core https://github.com/stm32duino.

Code adapted to STM32 from the brilliant ATTiny13A based project https://github.com/ChronicMechatronic/UniStep-V1.0 
by Chronic Mechatronic https://www.youtube.com/@ChronicMechatronic.

Differences to the UniStep project:
- STM32F030F4 has more IO and processing headroom, potential for extra functionality in the future.
- Higher maximum stepping rate (theoretically, not tested back to back)
- STM32F030F4 is a 3v3 chip, so step/dir/enable need to be at 3v3 or voltage-divided down to 3v3.
- dir/enable are broken out to individual IOs instead of being combined & needing ADC evaluation.

Steps for preparing a bare STM32F030F4 chip:
- BOOT0 (Pin1) put high, SWDIO and SWDCLK (pins 19 & 20 respectivelly) connected to STLINK V2, as well as ground to ground (Pin 15) and 3v3 to pins 4, 5, 16 (NRST, VDDA and VDD).
- Using STM32CubeProgrammer, connect to the MCU and disable the read-protection, then full erase.
- No need for bootloader, Arduino will now be able to upload using SWD without any need to set serial ports etc.
- BOOT0 needs to be high for programming and low to run the code.

Upload settings:
- Board: Generic STM32F0 series
- Debug symbols and core logs: "None"
- Optimize: "Smallest (-Os default)"
- Board part number: "STM32F030F4 Demo board (HSI internal RC oscillator)"
- C Runtime Library: "Newlib Nano (default)"
- Upload method: "STM32CubeProgrammer (SWD)"
- USB support (if available): "None"
- U(S)ART support: "Disabled (No Serial support)"

