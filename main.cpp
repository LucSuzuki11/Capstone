#include<AccelStepper.h>
#include "Nextion.h"
#include "config.h"

void clear_Serial_1(){ // Commands must be sent after each Nextion Call
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);
}

void clear_Serial_2(){ // Commands must be sent after each Nextion Call
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
}

// Motor Functionality

// HMI Functionality


// Design Implementation
// 1. Batching
/*
  LOGIC:
  Receive user input - # of Pancakes
  Find initial point - Corner
  Determine X + Y Offset
  e.g. Batch of 12

  [i] <-x offset -> []             []           []
  |
  y offset
  |
  []               []             []           []
  


  []               []             []           []

*/


void setup(){
  Serial.begin(BAUD_RATE);
  Serial2.begin(BAUD_RATE);
}

void loop{
  nexLoop(nex_listen_list);
}

void resetPosition(){
  stopPressed = 0;

  X_Motor.moveTo(0); // Reset X Position
  while(X_Motor.distanceToGo()!=0){X_Motor.run();}
  
  Y_Motor.moveTo(0); // Reset Y Position
  while(Y_Motor.distanceToGo()!=0){Y_Motor.run();}

  Z_Motor.moveTo(0); // Reset Z Position
  while(Z_Motor.distanceToGo()!=0){Z_Motor.run();}

  Valve_Motor.moveTo(0); // Reset Valve Position
  while(Valve_Motor.distanceToGo()!=0){Valve_Motor.run();}

  delay(5000);
}

void stopMotors(){ //Activated Upon stopPressed = 1 | Stops all Motors
  X_Motor.stop();
  Y_Motor.stop();
  Z_Motor.stop();
  Valve_Motor.stop();
}

void valveOpen(){
  Valve_Motor.moveTo(400); // Open Valve
  while(Valve_Motor.distanceToGo()!=0 && stopPressed == 0){
    nexLoop(nex_listen_list); // Continuously check for stop button
    Valve_Motor.run();
  }

  delay(250); // Valve Open Time **CHANGE**

  Valve_Motor.moveTo(0); // Close Valve
  while(Valve_Motor.distanceToGo()!=0 && stopPressed == 0){
    nexLoop(nex_listen_list); // Continuously check for stop button
    Valve_Motor.run();
  }
}