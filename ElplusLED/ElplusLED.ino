#include <Queue.h>
#define EL1R 2
#define EL1G 3
#define EL1B 4
#define EL2R 5
#define EL2G 6
#define EL2B 7

// LED pins
#define pinRED 9
#define pinGRN 10
#define pinBLU 11

// Time constants
#define GLOBAL_WAIT 5

// 0=normal operation; 1=action+prints; 2=prints only
#define DEBUG 2

// Define various ADC prescaler options (128 is default)
const unsigned char PS_16 = (1 << ADPS2); // Fastest
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Slowest

int input_vals[6];

byte led_out[3]; // Current led output
byte led_dest[3]; // What the leds are working towards
unsigned long fade_end=0; // When the fade should stop
unsigned long fade_remaining=0; // time remaining in the fade (updated at the begining of the fade function)


void setup() {
  // Set pin data direction registers
  DDRD = DDRD | B11111100; // Pins 2-7 are OUTPUTS. 0,1 unchanged
  DDRB = B111111; // Pins 8-13 are OUTPUTS.
  DDRC = B00000; // Pins A0-A5 are inputs

  // Change ADC prescaler value to make analog reads faster
  ADCSRA &= ~PS_128;  // remove bits set by Arduino library
  ADCSRA |= PS_32; // Set new value
  // See project readme for info

#if (DEBUG > 0)
  Serial.begin(9600);  
  Serial.println("---- Serial Monitor ----");
#endif

  // Initialize the Queue
  Queue sched;
  sched.scheduleFunction(checkButtons, "CheckB", 1000, 300);
  sched.scheduleFunction(doLEDfade, "LEDfade", 1200, 20);
  // scheduleFunction(function, "name", initRun, Recur)
  for(int i=0;i<3;i++)
  {
    led_out[i]=0;
    led_dest[i]=0;
  }
  for(int i=0;i<6;i++)
    input_vals[i]=0;

  while(1) {
    sched.Run(millis());  
    delay(GLOBAL_WAIT);
  }
}

/* ===================
 Begin scheduled functions
 
 Prototype MUST be:
 int function_name(unsigned long now)
 ==================*/

int doLEDfade(unsigned long now)
{  
  if(led_out==led_dest || fade_remaining<=0)
  {
    led_out[0]=led_dest[0];
    led_out[1]=led_dest[1];
    led_out[2]=led_dest[2];
    fade_remaining=0;
    fade_end=0;
    return 0;
  }
  else // Fade must occur
  {
    fade_remaining = fade_end-now;
    if(fade_remaining<=0)
    {
      led_out[0]=led_dest[0];
      led_out[1]=led_dest[1];
      led_out[2]=led_dest[2];
    }
    else
    {
      for(int i=0;i<3;i++)
      {
        led_out[i]=led_out[i]+((led_dest[i]-led_out[i])/fade_remaining);
      }
    }
#if (DEBUG > 0)
    Serial.println("do LED fade");
#endif
#if (DEBUG < 2)
    analogWrite(pinRED,led_out[0]);
    analogWrite(pinGRN,led_out[1]);
    analogWrite(pinBLU,led_out[2]);
#endif
  }
  return 0;
}

// check button statuses 
int checkButtons(unsigned long now)
{
  unsigned int inC = PORTC;
  // Set global variables to reflect input data.
  // Add debounce code
}

/* ===================
 End scheduled functions
 
 Begin helper functions
 ==================*/

/* setLEDfade()
 Inputs: -3dim array containing new color (byte[3])
 -time to finish at
 -current time(pass through now)
 Returns: nothing
 Does: set all global vars appropriately(led_dest,fade_remaining,fade_end)
 */
void setLEDfade(byte color[3], unsigned long stop_time, unsigned long now)
{
  if(stop_time<now) // Time machine not yet implemented
    return;
  for(int i=0;i<3;i++)
  {
    led_dest[i]=color[i];
  }
  fade_end=stop_time;
  fade_remaining=fade_end-now;
  return;
}
void loop() {
}


