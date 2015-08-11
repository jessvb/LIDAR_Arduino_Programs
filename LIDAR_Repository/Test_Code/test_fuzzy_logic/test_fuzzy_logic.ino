#include "Structs.h"

#define LO 0
#define MED 1
#define HI 2

fuzzy_values_t VEL = {0, 3, 6, 9, 12};
fuzzy_values_t DIST = {0, 10, 20, 30, 40};
fuzzy_values_t _PWM = {0, 63.75, 127.5, 191.25, 255};

void setup() {
  Serial.begin(9600); //Opens serial connection at 9600bps.
}

void loop() {
  float velValues[3]; // [SLOW, MED, FAST]
  float currVel = 10; // in meters/sec

  getLoMedHi(currVel, velValues, &VEL);
  
  Serial.println("Velocity:");
  Serial.print("Lo: "); Serial.println(velValues[0]);
  Serial.print("Med: "); Serial.println(velValues[1]);
  Serial.print("Hi: "); Serial.println(velValues[2]);
  
  float distValues[3]; // [SLOW, MED, FAST]
  float currDist = 2.7; // in meters

  getLoMedHi(currDist, distValues, &DIST);

  Serial.println("Distance:");
  Serial.print("Lo: "); Serial.println(distValues[0]);
  Serial.print("Med: "); Serial.println(distValues[1]);
  Serial.print("Hi: "); Serial.println(distValues[2]);
  
  float pwmValues[3]; // [SLOW, MED, FAST]
  float currPWM = 200; // from 0 to 255

  getLoMedHi(currPWM, pwmValues, &_PWM);

  Serial.println("PWM:");
  Serial.print("Lo: "); Serial.println(pwmValues[0]);
  Serial.print("Med: "); Serial.println(pwmValues[1]);
  Serial.print("Hi: "); Serial.println(pwmValues[2]);
  
  delay(100000);
}

/* Fuzzy function. Given a specific velocity/distance, fuzzy value type,
 * and an array, it will modify the array to the correct [SLOW, MED,
 * FAST] values */
void getLoMedHi(float currValue, float* loMedHi, fuzzy_values_t* fuzzyVals) {
  if (currValue >= fuzzyVals->minVal && currValue < fuzzyVals->medVal) {
    // low function -> lo value
    loMedHi[0] = -1 / (fuzzyVals->medVal) * currValue + 1; // y = -1/run*x + 1
    // 1 - lo value -> med value
    // loMedHi[1] = 1 / (fuzzyVals->medVal) * currValue; // y = 1/run*x
    loMedHi[1] = 1 - loMedHi[0];
    // zero -> hi value
    loMedHi[2] = 0;
  } else if (currValue >= fuzzyVals->medVal && currValue <= fuzzyVals->maxVal) {
    // zero -> lo value
    loMedHi[0] = 0;
    // right medium function -> med value
    loMedHi[1] = -1 / (fuzzyVals->medVal) * currValue + 2; // y = -1/run*x + 2
    // 1 - med value -> hi value
    // loMedHi[2] = 1 / (fuzzyVals->medVal) * currValue - 1; // y = 1/run*x - 1
    loMedHi[2] = 1 - loMedHi[1];
  } else {
    Serial.print("ERROR: "); Serial.print(currValue); Serial.println(" not in range.");
  }
}
