#include <AccelStepper.h>
#include "Nextion.h"

#define BAUD_RATE = 9600;
int stopPressed = 1;
int valveOpenTime = 250;
// Pin Assignments for X, Y, Z and Valve Motors

//X Motor
  #define x_step
  #define x_dir
  #define x_en
  AccelStepper X_Motor(AccelStepper::DRIVER, x_tep, x_dir); // X Direction Motor

// Y Motor
  #define y_step
  #define y_dir
  #define y_en
  AccelStepper Y_Motor(AccelStepper::DRIVER, y_tep, y_dir); // Y Direction Motor

// Z Motor
  #define z_step
  #define z_dir
  #define z_en
  AccelStepper Z_Motor(AccelStepper::DRIVER, z_tep, z_dir); // Z Direction Motor

// Valve Motor
  #define vlv_step
  #define vlv_dir
  #define vlv_en
  AccelStepper Valve_Motor(AccelStepper::DRIVER, vlv_tep, vlv_dir); // Valve Motor

// HMI Button Assignments EDIT: BUTTON ASSIGNMENTS
  NexButton b0 = NexButton(2, 2, "b0"); 
  NexButton b4 = NexButton(2, 3, "b4"); 
  NexButton b1 = NexButton(2, 4, "b1"); 
  NexButton b2 = NexButton(2, 5, "b2"); 
  NexButton b3 = NexButton(2, 6, "b3"); 
  NexButton b6 = NexButton(3, 3, "b6"); 
  NexButton b7 = NexButton(3, 2, "b7"); 
  NexButton b8 = NexButton(3, 4, "b8"); 
  NexButton b9 = NexButton(3, 5, "b9"); 
  NexButton b10 = NexButton(3, 6, "b10"); 
  NexButton b11 = NexButton(4, 3, "b11"); 
  NexButton b12 = NexButton(4, 2, "b12"); 
  NexButton b13 = NexButton(4, 4, "b13"); 
  NexButton b14 = NexButton(4, 5, "b14"); 
  NexButton b15 = NexButton(4, 6, "b15"); 
  NexText t0Page2 = NexText(2, 8, "t0");
  NexText t0Page3 = NexText(3, 8, "t0");
  NexText t0Page4 = NexText(4, 8, "t0");

// Identify Touch Events for Every Button
  NexTouch *nex_listen_list[] = {
    &b0, &b4, &b1, &b2, &b3,
    &b4, &b6, &b7, &b8, &b9,
    &b10, &b11, &b12, &b13, &b14,
    &b15, NULL
  };

