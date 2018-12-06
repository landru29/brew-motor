  #include <inttypes.h>
  #include <avr/interrupt.h>
  #include "Arduino.h"


#define ENA 0
#define DIR 1
#define PLS 2
#define LEDPIN 13
#define PITCH 700

#define FREQ 16000 //kHz

unsigned int microseconds = 600;
unsigned int timer1_counter;
unsigned int frequencyHz;
unsigned int targetHz;
int stable;

unsigned int getPreload() {
  double f = (double)frequencyHz / 1000;
  unsigned int sub = (int) ((double)FREQ / f);
  // preload: 65535 - 16000000 / (prescale * freq)
  return 65535 - sub;
}

ISR(TIMER1_OVF_vect)        // interrupt service routine 
{
  stable--;
  if (stable<=0) {
    stable = frequencyHz / 50;
    stepFrequency();
  }
  TCNT1 = getPreload();   // preload timer
  digitalWrite(PLS, digitalRead(PLS) ^ 1);
}

void setFrequency (unsigned int hz) {
  frequencyHz = hz;
  targetHz = hz;
  stable = hz / 50;
  TCNT1 = getPreload();
}

void stepFrequency() {
  unsigned int pitch = abs(frequencyHz-targetHz) < PITCH ? abs(frequencyHz-targetHz) : PITCH;
  if (frequencyHz < targetHz) {
    frequencyHz += pitch;
  }
  if (frequencyHz > targetHz) {
    frequencyHz -= pitch;
  }
}

void timerSetup () {
   noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;

  // Set timer1_counter to the correct value for our interrupt interval
  //timer1_counter = 64911;   // preload timer 65536-16MHz/256/100Hz
  //timer1_counter = 64286;   // preload timer 65536-16MHz/256/50Hz
  
  setFrequency(5000);
  TCCR1B |= (1 << CS10);    // no prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();         
}


void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(PLS, OUTPUT);
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);
  digitalWrite(ENA, HIGH);
  digitalWrite(DIR, HIGH);
  digitalWrite(PLS, HIGH);
  delay(1000);
  digitalWrite(LEDPIN, HIGH);
  timerSetup ();
}

void gotoFreq(unsigned int f) {
  targetHz = f;
}

int maxFreq = 65000;

void loop() {
  gotoFreq(maxFreq);
  delay(5000);
}
