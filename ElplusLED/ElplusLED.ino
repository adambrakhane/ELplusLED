#include <Queue.h>

// LED pins
#define pinRED 9
#define pinGRN 10
#define pinBLU 11

// Time constants
#define GLOBAL_WAIT 5

// 0=normal operation; 1=action+prints; 2=prints only
#define DEBUG 1

// Define various ADC prescaler options (128 is default)
const unsigned char PS_16 = (1 << ADPS2); // Fastest
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Slowest

int knob;
boolean buttons[5];

byte led_out[3]; // Current led output
byte led_dest[3]; // What the leds are working towards
byte led_bgn[3];
float led_incr[3];
unsigned long fade_end=0; // When the fade should stop
long fade_remaining=0; // time remaining in the fade (updated at the begining of the fade function)
signed long fade_time=0;

enum ELstates { off, red, green, blue, redgreen, redblue, greenblue, redgreenblue };

unsigned long last_state_change = 0;

ELstates ELstatus=off;


void setup() {
  // Set pin data direction registers
  DDRD = DDRD | B11111100; // Pins 2-7 are OUTPUTS. 0,1 unchanged
  DDRB = B111111; // Pins 8-13 are OUTPUTS.
  DDRC = B00000; // Pins A0-A5 are inputs
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  digitalWrite(A0, HIGH);
  digitalWrite(A1, HIGH);
  digitalWrite(A2, HIGH);
  digitalWrite(A3, HIGH);

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
  sched.scheduleFunction(checkButtons, "CheckB", 300, 100);
  sched.scheduleFunction(doLEDfade, "LEDfade", 1000, 20);
  //sched.scheduleFunction(doELout, "ELout", 110, 3000);
  
    //sched.scheduleFunction(testFade, "tfade", 1000, 5123);
    //sched.scheduleFunction(testFade2, "tfade2", 3251, 3634);    
  
  // scheduleFunction(function, "name", initRun, Recur)
  for(int i=0;i<3;i++)
  {
    led_out[i]=0;
    led_dest[i]=0;
    led_bgn[i]=0;
  }


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

int doELout(unsigned long now)
{
  ELstates next_state;
  if(!buttons[4])
  {
    if(now<(8*knob+last_state_change))
      return 0;
  }
  switch(ELstatus)
  {
    case off:
      PORTD = B00000000;
      next_state = red;
      break;
    case red: 
      PORTD = B00100100;
      next_state = green;
      break;
    case green: 
      PORTD = B01001000;
      next_state = blue;
      break;
    case blue: 
      PORTD = B10010000;
      next_state = redgreen;
      break;
    case redgreen: 
      PORTD = B01101100;
      next_state = greenblue;
      break;
    case greenblue: 
      PORTD = B11011000;
      next_state = redgreenblue;
      break;
    case redgreenblue: 
      PORTD = B11111100;
      next_state = red;
      break;
  }
  last_state_change=now;
  Serial.println(next_state);
  ELstatus = next_state;
}

int doLEDfade(unsigned long now)
{  
  if((led_out[0]==led_dest[0] && led_out[1]==led_dest[1] && led_out[2]==led_dest[2]) || fade_remaining<=0)
  {
    led_out[0]=led_dest[0];
    led_out[1]=led_dest[1];
    led_out[2]=led_dest[2];
    fade_remaining=0;
    fade_end=0;
    fade_time=0;
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
      for(int i=0;i<3;i++) //DO THE FADE
      {
        led_out[i]=led_out[i]+((led_dest[i]-led_out[i])/((fade_time/110)));
        Serial.print(((led_dest[i]-led_out[i])/(fade_time/110)));Serial.print("^");
        //led_out[i]=led_out[i]+led_incr[i];
      }
//      Serial.print(led_out[0]); Serial.print("-"); Serial.print(led_out[1]); Serial.print("-"); Serial.print(led_out[2]);
//      Serial.println(fade_time);
    }
    #if (DEBUG > 0)
        Serial.print(led_out[0]/fade_time); Serial.print("  ");Serial.print(led_out[1]/fade_time); Serial.print("  ");Serial.print(led_out[2]/fade_time); Serial.print("  ");
        Serial.print(led_out[0]); Serial.print("("); Serial.print(led_dest[0]); Serial.print(") - ");
        Serial.print(led_out[1]); Serial.print("("); Serial.print(led_dest[1]); Serial.print(") - ");
        Serial.print(led_out[2]); Serial.print("("); Serial.print(led_dest[2]); Serial.print(") - ");
        Serial.print(fade_remaining);
        
        Serial.println();
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
  byte color[3];
  // Set global variables to reflect input data.
  // Add debounce code
  buttons[0]=!digitalRead(A0);
  buttons[1]=!digitalRead(A1);
  buttons[2]=!digitalRead(A2);
  buttons[3]=!digitalRead(A3);
  buttons[4]=digitalRead(A4);
  knob = analogRead(5);
 
  if(buttons[0]==true)
  {
    color[0]=255; color[1]=255; color[2]=255;
    PORTD = B11111100;
    ELstatus = redgreenblue;
    setLEDfade(color, now+803, now);
  }
  else if(buttons[1]==true)
  {
    color[0]=0; color[1]=150; color[2]=200;
    PORTD = B01001000;
    ELstatus = green;
    setLEDfade(color, now+803, now);
  }
  else if(buttons[2]==true)
  {
    color[0]=0; color[1]=255; color[2]=0;
    PORTD = B00100100;
    ELstatus = red;
    setLEDfade(color, now+803, now);
  }
  else if(buttons[3]==true)
  {
    color[0]=255; color[1]=0; color[2]=0;
    PORTD = B10010000;
    ELstatus = blue;
    setLEDfade(color, now+803, now);
  }
  //setLEDfade(color, now+knob*3, now);
}



int testFade(unsigned long now)
{
  byte color[3];
  color[0]=5; color[1]=100; color[2]=50;
  Serial.println("Fade start");
  setLEDfade(color, now+803, now);
  return 0;
}
int testFade2(unsigned long now)
{
  byte color[3];
  color[0]=24; color[1]=255; color[2]=10;
  Serial.println("Fade start");
  setLEDfade(color, now+500, now);
  return 0;
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
  fade_end=stop_time;
  fade_remaining=fade_end-now;
  fade_time=fade_remaining;
  for(int i=0;i<3;i++)
  {
    led_dest[i]=color[i];
    led_bgn[i]=led_out[i];
    led_incr[i]=(float)(led_dest[i]-led_bgn[i])/fade_time;
  }
  return;
}



void loop() {
}


