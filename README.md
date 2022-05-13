basic project created around an STM32F103C6T8 "study" board sourced from ebay.

STM32-base (https://stm32-base.org/boards/) lists similar looking boards with other CPUs and other boards with the STM32F103C8T6, but not this combination.
The closest match I've found are these:
  http://www.chinalctech.com/cpzx/STM32_Study_Board/360.html
  http://www.lctech-inc.com/cpzx/3/STM32kfb/2019/0402/434.html
The "MINI STM32 Schematic Diagram" from Open Impulse appears to match:
  https://www.openimpulse.com/blog/products-page/product-category/stm32f103c8t6-arm-development-board-cortex-m3/

Project
The base project was created using ST's CubeMX with LL divers for everything except USB.
Compiler/IDE is IAR EWARM 7.8

This is a playground project and will change as things get added and removed.

In time it may include:
- RTC for time keeping, but the board has no battery back up ;-(
- USB
- UART
- ??? who knows.
- I2C using DMA. presently bit-banged.

TM1637 support
I2C-like support for a TM1637 is implemented using bit-banged I/O.
The original source is from https://github.com/rogerdahl/stm32-tm1637
The code has been modified in several ways:
- I/O is mapped to I2C1 pins on STM32F103C8T6.
- use uSecDelay() for uSec delays. This function used the SysTick counter for accurate timing.
- IC2 CLK is push-pull, I2C data is push-pull for all cycles except during ACK from TM1637.
- ACK is checked as part of byte write function.
- return ACK status (presently ignored.)
- made brightness a variable, fixed an issue with the initial setting.

commit notes
04/22/2022 MAR
- systick is running at 1mS. Main loop sleeps until SysTick changes.
- UART with basic echo: implemented with IRQs.
- TM1637 is working, toggling colon every 500mS and updating the displayed count every second.
