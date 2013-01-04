/* First Night Signs */
#include <avr/wdt.h>

// How many channels
#define LETTERS 6

// milliseconds between updates
#define SPEED 50

// Minimum brightness
#define DIM 20

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

uint16_t levels[6];
uint8_t directions[6];

uint8_t offset = 50;

uint8_t step_size = 12;

void setup()
{
  wdt_reset(); // Reset and hope for the best
  wdt_disable(); // Disable the WDT

  for(i = 0; i < LETTERS; i++)
  {
     levels[i] = 255 - offset*i; 
     levels[i] = constrain(levels[i],DIM,BRIGHT);
  }
  


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
    //cur = step_ * x; // calculate our current step position.  Could also just do cur += step_ but I like this form better:
                     // this form lets me ignore rollover, although the sin() function should deal with the rollover just fine
                     // we may see a decrease in precision if we're running for a long time.
    for(i = 0; i < LETTERS; i++) // Cycle through our 6 or 4 letters
    {
      if(directions[i] == 0) // Go down
      {
         levels[i] -= step_size;
         if(levels[i] <= DIM)
         {
            directions[i] = 1; // Go up! 
         }
  
      }
      else if(directions[i] == 1)
      {  
          if(levels[i] + step_size > levels[i])
          {
          levels[i] += step_size;
          }
          else
          {
             directions[i] = 0; // Going to overflow, go down.
          }
          
          if(levels[i] >= BRIGHT)
          {
             directions[i] = 0; // Go down! 
          }
  
      }      
      analogWrite(patch[i],constrain(levels[i],0,255)); 

    }
  }
  wdt_reset();  
}


