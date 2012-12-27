/* First Night Signs */
#include <avr/wdt.h>

// How many channels
#define LETTERS 6

// milliseconds between updates
#define SPEED 50 

// Minimum brightness
#define DIM 127

// Maximum brightness
#define BRIGHT 255

// How many steps?  Cycle time is steps * speed
#define STEPS 255

// This is a "fixture patch" -- it maps output pins to a logical sequence from 0 to 5.
uint8_t patch[6] = {
  3,5,6,9,10,11};

uint8_t x;
uint8_t i;

// Timer holds the last cycle run time in millis()
// Adding timer to SPEED gets us the millis() value that should trigger the next cycle
// This lets us run accurate cycles and deal with rollover.  
uint32_t timer;

// These keep track of various portions of the cycle

// offset is the distance between each letters in radians
float offset = 2*PI/LETTERS;

// cur is the current position.  Only needed because we want to minimize floating point ops
float cur = 0;

// Step is the step size between each cycle, in radians
float step_ = 2*PI/STEPS;

void setup()
{
  wdt_reset(); // Reset and hope for the best
  wdt_disable(); // Disable the WDT

  wdt_reset(); // Reset the WDT
  wdt_enable(WDTO_120MS); // Enable WDT at 120ms -- arduino will reset within 120ms of a lockup


}


void loop()
{
  if(timer + SPEED < millis()) // This operation deals correctly with clock rollover
  {
    timer = millis(); // Reset the timer
    x++; // increment x
    if(x > STEPS) // Roll x over if we're using less than 255 steps
    {
      x = 0;
    }
    cur = step_ * x; // calculate our current step position.  Could also just do cur += step_ but I like this form better:
                     // this form lets me ignore rollover, although the sin() function should deal with the rollover just fine
                     // we may see a decrease in precision if we're running for a long time.
    for(i = 0; i < LETTERS; i++) // Cycle through our 6 or 4 letters
    {
      analogWrite(patch[i],map(sin(cur+offset*i),-1,1,DIM,BRIGHT)); 
/*
    This requires a little more explanation.
    
    patch[0] returns 3, patch[1] returns 5.  This lets us get all 6 PWM pins in a sequence without having to do any real tricks.
    offset*i returns the offset for the current letter.  That is, offset*0 is no offset, offset*1 is the first offset, etc.
    
    cur+offset*i therefore returns the current angular position of our chase for this letter
    sin() returns -1 to 1
    We use the map function to map from -1 to 1 to DIM to BRIGHT.   This lets us scale the brightness of the chase. 
    In this case, I want to go from 127 to 255.
    
    The end result, is basically:
    analogWrite(3,127--255); with the range 127 to 255 being determined by a sin wave function.


*/
    }
  }
  wdt_reset();  
}

