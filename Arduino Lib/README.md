# Custom Arduino Libraries
This directory contains the custom Arduino libraries and utilites written for this project. 


## Dependencies
*Some of the utilities in these libraries depend on code from existing libraries*


### Fast LED Library

* Info: http://fastled.io

Robust LED lighting library with many useful features, including gamma correction & dithering, HSV color space, power management and more. This is the underlying functionality for the **AnalogLED** custom library, which is essentially a wrapper around FastLED that adapts it for use with analog (non-addressable, PWM controlled) leds.