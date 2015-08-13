/**
 * \file DadsMP3Player.ino
 *
 * \based on work by Michael P. Flaga
 */

// libraries
#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <SFEMP3Shield.h>

int trigger0 = A0;
int trigger1 = A1;
int trigger2 = A2;
int trigger3 = A3;
int trigger4 = A4;
int trigger5 = A5;

int previousTrigger = 0;

long lastCheck; //This stores the last millisecond since we had a trigger

#define BUTTON_DEBOUNCE_PERIOD 100 //ms
SdFat sd;
SFEMP3Shield MP3player;

void setup() {
  Serial.begin(115200);

  pinMode(trigger0, INPUT_PULLUP);
  pinMode(trigger1, INPUT_PULLUP);
  pinMode(trigger2, INPUT_PULLUP);
  pinMode(trigger3, INPUT_PULLUP);
  pinMode(trigger4, INPUT_PULLUP);
  pinMode(trigger5, INPUT_PULLUP);

  if(!sd.begin(9, SPI_HALF_SPEED)) sd.initErrorHalt();
  if (!sd.chdir("/")) sd.errorHalt("sd.chdir");

  MP3player.begin();
  MP3player.setVolume(1, 1);
  
  previousTrigger = 255; //Setup the new vs old trigger
  
  Serial.println("Looking for Buttons to be depressed...");
}


//checkTriggers checks the level of the various inputs
//It returns 255 if no trigger is activated
//Otherwise, it returns the number of the first trigger it sees
int checkTriggers(void) {

  int foundTrigger = 255;

  //Once a trigger is activated, we don't want to trigger on it perpetually
  //But after 3 seconds, reset the previous trigger number
  if( (previousTrigger != 255) && (millis() - lastCheck) > 3000) {
    lastCheck = millis();
    previousTrigger = 255;
    Serial.println("Previous trigger reset");
  }

  if (getInput() > 0) {
    delay(BUTTON_DEBOUNCE_PERIOD);
    int value = getInput();
    if (value > 0) {
      foundTrigger = value;
    }
  }

  if(foundTrigger != previousTrigger && foundTrigger != 255){ //We've got a new trigger!
    previousTrigger = foundTrigger;
    return(foundTrigger);
  }
  else
    return(255); //No triggers pulled low (activated)
}

int getInput(void) {
  byte pin0 = digitalRead(trigger0);
  byte pin1 = digitalRead(trigger1);
  byte pin2 = digitalRead(trigger2);
  byte pin3 = digitalRead(trigger3);
  byte pin4 = digitalRead(trigger4);
  byte pin5 = digitalRead(trigger5);

  return 63 - (pin0 + (pin1 * 2) + (pin2 * 4) + (pin3 * 8) + (pin4 * 16) + (pin5 * 32));
}


void loop() {
  int triggerNumber = 255;

  while(triggerNumber == 255) triggerNumber = checkTriggers(); //Wait for a trigger to be activated

  Serial.print("Playing track #");
  Serial.println(triggerNumber);
  MP3player.playTrack(triggerNumber);
}

