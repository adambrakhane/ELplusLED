#include <Queue.h>
#define EL1R 2
#define EL1G 3
#define EL1B 4
#define EL2R 5
#define EL2G 6
#define EL2B 7

// Time constants
#define GLOBAL_WAIT 10


// Define various ADC prescaler options (128 is default)
const unsigned char PS_16 = (1 << ADPS2); // Fastest
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1); // Slowest
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

boolean btn_vals[5];
int ana_vals[5];



void setup() {
  // Set pin data direction registers
  DDRD = DDRD | B11111100; // Pins 2-7 are OUTPUTS. 0,1 unchanged
  DDRB = B111111; // Pins 8-13 are OUTPUTS.
  DDRC = B00000; // Pins A0-A5 are inputs

  // Change ADC prescaler value to make analog reads faster
  ADCSRA &= ~PS_128;  // remove bits set by Arduino library
  ADCSRA |= PS_32; // Set new value
  // See project readme for info

  // Initialize the Queue
  Queue sched;
  sched.scheduleFunction(checkButtons, "CheckB", 1000, 200);
  // scheduleFunction(function, "name", initRun, Recur)
  

  while(1) {
      sched.Run(millis());  
      delay(GLOBAL_WAIT);
  }
}

  

// check button statuses 
int checkButtons(unsigned long now)
{
  unsigned int inC = PORTC;

}

void loop() {
}
