# STM32-DYNAMIXEL
STM32 DYNAMIXEL AX12A LIB implements dynamixel protocol 1.0

The example was written for the stm32g030 microcontroller and tested on it, theoretically it should work on all microcontrollers of the STM32 series only with a change in the version of the HAL library.

I advise you to use a voltage-level translator to communicate with the motor, I used TXS0102DCTR. Optionally, you can connect a 10Kohm pull-up resistor between the STM32 pin and 3.3V. A pull-up resistor is connected inside the STM32, but this is more reliable.

<img width="569" alt="scheme_screen" src="https://github.com/mmserty200000/STM32-DYNAMIXEL/assets/82755461/de2edeb5-8cdd-4593-86dc-fc7294ec40aa">


Usart must be configured as shown below:

<img width="304" alt="usart_screen" src="https://github.com/mmserty200000/STM32-DYNAMIXEL/assets/82755461/da7e0607-ac30-41f7-acd3-e0574af85d2f">

