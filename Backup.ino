
/* Custom library with a wide range of utilities
 *  and helper functions specific to this purpose.
 */
#include <LightBox.h>

CRGB ledColor = CRGB();
AnalogLED ledStrip = AnalogLED(PIN_R, PIN_G, PIN_B);
#define LED_PIN A16
float counter = 0.0f;

void setup() 
{
    //ledStrip.setup();
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
}

void loop() 
{     
    if(counter > 1000)
        counter = 0.0f;

    counter += 0.0001f;
    float value = (0.5f * sin(counter)) + 0.5f;
    short brightness = (short)round(255 * value);
    analogWrite(LED_PIN, brightness);
}