#include <Arduino.h>
#include <AccelStepper.h>
#include <MultiStepper.h>

#include "pins.h"
#include "Plotter.h"
#include "HMI.h"
#include "cloud.h"

// ===== Stepper Motors =====
AccelStepper X(AccelStepper::DRIVER, X_STEP,   X_DIR);
AccelStepper Y(AccelStepper::DRIVER, Y_STEP,   Y_DIR);
AccelStepper Z(AccelStepper::DRIVER, Z_STEP,   Z_DIR);
AccelStepper VAL(AccelStepper::DRIVER, VAL_STEP, VAL_DIR);
MultiStepper steppers;

// ===== Plotter Object =====
Plotter plotter(X, Y, Z, VAL, steppers);

bool shapeMade = false;

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);

  // Init subsystems
  HMI_init(); // HMI Initialized

  Cloud_init(); // Cloud Initialized

  plotter.begin(); // Plotter Initialized
  plotter.enableDrivers(true);
  plotter.goHome();
}

void loop()
{


  HMI_poll(); // Poll the HMI
  checkClient();

  // ===== Handle start requests =====

  if (HMI_batchRequested())
  {
    HMI_clearDesignRequests();

    uint8_t n = HMI_getBatchNumber();
    if (n == 0) n = 1; // at least 1 pancake

    Serial.println("Dispensing batch...");
    Point panCenter = { PAN_CENTER_X, PAN_CENTER_Y };

    plotter.dispenseBatch(
      n,                   // number of hex points
      panCenter,           // pan center
      PAN_RADIUS_STEPS,    // radius
      SITE_SPACING_STEPS,  // spacing between points
      PAN_EDGE_MARGIN_STEPS,
      1000,                // valve open time per site (ms)
      HEX_START_ANGLE_RAD  // starting angle
    );
    plotter.closeValve();
    Cloud_sendData();
  }

  if (HMI_squareRequested())
  {
    HMI_clearDesignRequests();

    Serial.println("Drawing square...");
    Point square[4];
    plotter.makeSquareShape(SQUARE_SIZE_STEPS, square);
    plotter.tracePath(square, 4);
    plotter.closeValve();
    Cloud_sendData();
    delay(1000);
  }

  if (HMI_triangleRequested())
  {
    HMI_clearDesignRequests();

    Serial.println("Drawing triangle...");
    Point triangle[3];
    plotter.makeTriangleShape(TRI_SIDE_STEPS, triangle);
    plotter.tracePath(triangle, 3);
    plotter.closeValve();
    Cloud_sendData();
    delay(1000);
  }

   // ===== Handle STOP from any page =====

  if (HMI_stopRequested())
  {
    Serial.println("STOP requested – homing and stopping motors.");
    plotter.goHome();

    HMI_clearStop();
    HMI_clearDesignRequests();
  }

 

  
;}