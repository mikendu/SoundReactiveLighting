
/* Custom library with a wide range of utilities
 *  and helper functions specific to this purpose.
 */
#include <LightBox.h>

CRGB ledColor = CRGB();
AnalogLED ledStrip = AnalogLED(PIN_R, PIN_G, PIN_B);
bool colorMode = false;

int incomingAudio;
float alpha = 0.001f;
float average = 0.0f;

int clipPin = 7;
int clipIndicatorTime = 50;
int clippingCounter = clipIndicatorTime;
bool clipping = false;

int currMin = 255;
int currMax = 0;
int prevMax = 0;
int prevMin = 255;
int prev = 0;
int prevSlope = 0;
int amplitude = 0;
float prevAmp = 0.0f;

void setup() 
{

    cli();//disable interrupts
    
    //set up continuous sampling of analog pin 0
    
    //clear ADCSRA and ADCSRB registers
    ADCSRA = 0;
    ADCSRB = 0;
    
    ADMUX |= (1 << REFS0); //set reference voltage
    ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only
    
    ADCSRA |= (1 << ADPS2) | (1 << ADPS0); //set ADC clock with 32 prescaler- 16mHz/32=500kHz
    ADCSRA |= (1 << ADATE); //enabble auto trigger
    ADCSRA |= (1 << ADIE); //enable interrupts when measurement complete
    ADCSRA |= (1 << ADEN); //enable ADC
    ADCSRA |= (1 << ADSC); //start ADC measurements
    
    sei();//enable interrupts
      
    
    
    ledStrip.setup();
    Serial.begin(115200);
    pinMode(clipPin, OUTPUT);
}

ISR(ADC_vect) 
{
    //when new ADC value ready
    incomingAudio = ADCH;//update the variable incomingAudio with new value from A0 (between 0 and 255)
    
    if(incomingAudio == 0 || incomingAudio == 255)
    {
        clipping = true;
        //clippingCounter = clipIndicatorTime;
        digitalWrite(clipPin, HIGH);
        //ledColor = CHSV(0, 0, 255);
    }

    int slope = 0;
    
    // Increasing
    if(incomingAudio > prev)
        slope = 1;
    
    if(incomingAudio == prev)
        slope = 0;
    
    if(incomingAudio < prev)
        slope = -1;


    if(slope > 0)
    {
        if(incomingAudio > currMax)
            currMax = incomingAudio;
    }

    if(slope < 0)
    {
        if(incomingAudio < currMin)
            currMin = incomingAudio;
    }

    // Downward turn
    if(prevSlope >= 0 && slope < 0 && incomingAudio > 127)
    {
        prevMax = currMax;
        currMax = 127;
    }

    if(slope > 0 && prevSlope <= 0 && incomingAudio < 127)
    {
        prevMin = currMin;
        currMin = 127;
    }

    float alpha = 0.1f;
    amplitude = prevMax - prevMin;
    prevAmp = (alpha * (float)amplitude) + ((1.0f - alpha) * prevAmp);
    int rounded = (int)prevAmp;
    //Serial.println(rounded);
    prevSlope = slope;
    prev = incomingAudio;

    //ledColor = CHSV(0, 0, rounded);
    //ledStrip.show(ledColor);
}


void loop() 
{     
    //incomingAudio = ADCH;
    //ledColor = CHSV(0, 0, audio);
    
    //incomingAudio = analogRead(A0);
    //float newAudio = (int)(((float)incomingAudio / (float)1024) * 255);
    //average = (alpha * audio) + ((1.0f - alpha) * average);
    //int avg = (int) average;
    //Serial.println(avg);


    if(clipping)
    {
        /*
        if(clippingCounter > 0)
        {
            clippingCounter--;
        }
        else
        {   
            clipping = false;
            digitalWrite(clipPin, LOW);
            //ledColor = CHSV(0, 0, 0);
        }*/

        clipping = false;
        digitalWrite(clipPin, LOW);
    }

    delay(clipIndicatorTime);

    
    //ledStrip.show(ledColor);
    //ledColor = CHSV(0, 0, newAudio);
    /*
    int delayBtwn = 6;
    for(int i = 0; i < 510; i++)
    {
        delay(delayBtwn);
        int value = (i < 255) ? i : (510 - i);

        
        ledColor = (colorMode) ? CHSV(value, 255, 255) : CHSV(0, 0, value);
        //ledColor = CRGB(101, 17, 219);
        //ledColor = CHSV(188, 235, 219);
        //ledColor = CHSV(0, 0, 255);
        //ledColor = CRGB::White;
        ledStrip.show(ledColor);
    }

    delay(delayBtwn);
    colorMode = !colorMode;*/
}


