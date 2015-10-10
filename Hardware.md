# Hardware description of the XARIAS project

**Other pages:** [AboutXARIAS](AboutXARIAS.md), [Functionality](Functionality.md), [Hardware](Hardware.md), [SourceCode](SourceCode.md), [InstallHOWTO](InstallHOWTO.md), [Author](Author.md)


# Hardware #

## XARIAS board b02 ##
XARIAS board b02 is the first board of XARIAS on PCB. The previous (b01) was very experimental and was developed on cardboard.

![xarias board](http://lh6.ggpht.com/xtensa/SGq03BBXCKI/AAAAAAAABiI/07IVXxMHRd0/p1150567.jpg?imgmax=720)

### MCU ###
The heart of the system is ATMega32 MCU.

### LCD ###
Currently Hitachi HD44780 and Samsung KS0108 compatible displays are supported.

### DS1307 RTC ###
DS1307 RTC from Dallas Semiconductor. DS1307 is the best choice here, because it has programmable squarewave output signal, 56 bytes of nonvolatile memory and automatic powerfail detect and switch circuity. Output signal is used to trigger measurement routines and nonvolatile memory is used to store preferences and statistics.

### CD74HC14E ###
High Speed CMOS Logic Hex Schmitt-Triggered Inverters are used to achieve correct square wave signal and get rid of most distortions.

### DS1804 ###
Double digital potentiometer (10k) is used to control brightnes and contrast of the LCD display. DS1804-10 is controlled through I2C bus as well as DS1307 clock.

## AC board ##
AC board is a separate board that is designed to be autonomous system that controlles air conditioner. This board controls two main devices - native AC board that originally was turned on or off manually and blower motor. Blower motor speed is controlled using PWM, so smooth regulation is available. AC board is equipped with 1-wire bus. Several DS18B20 temperature sensors are attached to this bus. AC board communicate with main XARIAS board through I2C bus.

![xarias AC board](http://lh5.ggpht.com/xtensa/SGq07beRxnI/AAAAAAAABiY/NuoTS1YCT3Y/p1150575.jpg?imgmax=720)

### MCU ###
The heart of the AC board is ATMega8 MCU. It is more than enough for this job.

### ICL7667 ###
ICL7667 mosfet driver is used to convert CMOS signal to 12V signal. You can also use MAX626 or TCS426 drivers as they are fully compatible.

### IRFZ48N ###
Good MOSFET is very important if you want it to drive 4 to 9 amper motor as blower motor is. IRFZ48N is very good choice as it has ultra low on-resitance (RDS=0.014 Ohm).

## Keyboard ##

XARIAS uses simple 4x3 serial keyboard with small improvement to make it possible to press several buttons at the time. Here is keyboard map:
| | **COL1** | **COL2** | **COL3** | **COL4** |
|:|:---------|:---------|:---------|:---------|
| **ROW1** | OK       | UP       | RIGHT    | LEFT     |
| **ROW2** | ESC      | DOWN     | AC       | FN       |
| **ROW3** | FL       | SPD      | TRP      | RPM      |
