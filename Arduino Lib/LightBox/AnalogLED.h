#ifndef ANALOG_LED_H
#define ANALOG_LED_H


#include "CommonHeaders.h"


#define PIN_R 9
#define PIN_G 10
#define PIN_B 11

class AnalogLED
{

private: 

	const int pin_r;
	const int pin_g;
	const int pin_b;

public:

	AnalogLED( int redPin, int greenPin, int bluePin  );

	void setup();
	void show( CRGB& color );

};



#endif