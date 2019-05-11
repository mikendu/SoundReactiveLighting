#include "AnalogLED.h"

AnalogLED::AnalogLED( int redPin, int greenPin, int bluePin ) : pin_r(redPin),
																pin_g(greenPin),
																pin_b(bluePin)
{
}

void AnalogLED::setup()
{	
	pinMode(pin_r, OUTPUT);
	pinMode(pin_g, OUTPUT);
	pinMode(pin_b, OUTPUT);
}

void AnalogLED::show( CRGB& color )
{
	CRGB adjustment = CLEDController::computeAdjustment(255, TypicalSMD5050, Halogen);
	PixelController<RGB> pixelArray = PixelController<RGB>(color, 1, adjustment, BINARY_DITHER);



	while(pixelArray.has(1))
	{
		uint8_t adjustedR = pixelArray.loadAndScale0();
		uint8_t adjustedG = pixelArray.loadAndScale1();
		uint8_t adjustedB = pixelArray.loadAndScale2();

		pixelArray.advanceData();
		pixelArray.stepDithering();


		analogWrite(pin_r, adjustedR);
		analogWrite(pin_g, adjustedG);
		analogWrite(pin_b, adjustedB);
	}
}