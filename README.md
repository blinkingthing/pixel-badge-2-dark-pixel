# Pixel Badge 2 - Dark Pixel
by @blinkingthing

## What is it?
Pixel Badge 2 - Dark Pixel is an ESP32 based conference badge that looks like a giant WS2812S.

### Features
* Blinking LEDs
* Touch Controls
* User Defined Colors
* Memory!
* SAO control
* On-board USB -> UART
* Broken out ESP32 pins for devboard fun!
* Daisy-chainable with compatible WS2812 LEDs
* *Cool* art!
* Lanyard holes (attach it to something!)

### What does it do
Pixel Badge 2 has three LEDs with pre-programmed animations that can be altered to use custom colors. The brass 'bond-wires' are touch sensitive inputs that can change the animations, colors, and save and reload default color settings from memory. All pins on the ESP32 are broken out so this can be used as a dev board. There are WS2812 compatible data inputs and outputs so this board can either be chained into an existing chain of LEDs, or output data to a chain of compatible LEDs. The board has on-board USB -> Serial and can be programmed with Arduino directly through it's micro USB port. It also has a li-po charging circuit that can charge a 3.7v li-po battery via the same USB cable. 

### Touch Controls

![Image of touch controls](images/touch-controls.png)

1. Change Animation Left
2. Change Animation Right
3. Press and hold - Shift Pixel 1 Hue
4. Press and hold - Shift Pixel 2 Hue
5. Press and hold - Shift Pixel 3 Hue
6. Reset colors to default / Decrease Brightness
7. Store current custom colors to memory / Increase Brightness

### Built-in Animations

1. Pulse (Set User Defined Colors)
2. Chase (User Defined Colors)
3. Bounce (User Defined Colors)
4. Steady (User Defined Colors)
5. Synced Fade (User Defined Colors)
6. SAO Control Mode
7. Solid Green
8. Solid Blue
9. Magenta
10. Pink
11. White
12. White Chase
13. Red Chase
14. Blue Chase
15. Rainbow
16. Rainbow Strobe
17. Rainbow Fade

### User Defined Colors

When the badge is set to animations 1-5, you can hold down touch controls 3,4, or 5 to adjust the color of the three different LEDs. Custom colors can *only* be saved in mode 1. To save custome colors, make sure you're in mode 1, and touch control #7. You will get a green flash confirming that you've saved custom colors. You can also return to the default colors by touching control #6 in mode 1. In all other User Defined Color modes, can increase or decrease the brightness of the LEDs with touch controls #6 and #7. 

### SAO Control

All thes commands were built around LonghornEngineer's Doom Guy SAO. (https://github.com/LonghornEngineer/DOOM_SAO)




### Serial Control

screen /dev/tty.SLAB_USBtoUART 115200

### DC27-29 Notes

If you received a Pixel Badge 2 between DC27-29 (2019-21) there's a good chance you have an early firmware version that allowed for sending I2C commands to capable SAOs (Mainly the Doom guy SAO). This mode can be recognized by a LED 1 blinking green. 
