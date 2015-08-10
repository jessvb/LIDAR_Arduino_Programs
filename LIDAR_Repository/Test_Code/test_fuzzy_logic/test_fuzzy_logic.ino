#include "Structs.h"

fuzzy_values_t VEL = {0, 3, 6, 9, 12};
//fuzzy_values_t DIST = {0, 10, 20, 30, 40};
//fuzzy_values_t pwm_t _PWM = {0, 63.75, 127.5, 191.25, 255};

void setup() {
  Serial.begin(9600); //Opens serial connection at 9600bps.
}

void loop() {
  float velValues[3]; // [SLOW, MED, FAST]
  float currVel = 10; // Ten meters/sec

  getLoMedHi(currVel, velValues, &VEL);

  Serial.print("Lo: "); Serial.println(velValues[0]);
  Serial.print("Med: "); Serial.println(velValues[1]);
  Serial.print("Hi: "); Serial.println(velValues[2]);
  delay(100000);
}

/* Fuzzy function. Given a specific velocity/distance, fuzzy value type,
 * and an array, it will modify the array to the correct [SLOW, MED,
 * FAST] values */
void getLoMedHi(float currValue, float* loMedHi, fuzzy_values_t* fuzzyVals) {
  if (currValue >= fuzzyVals->minVal && currValue < fuzzyVals->medVal) {
    // low function -> lo value
    loMedHi[0] = -1 / (fuzzyVals->medVal) * currValue + 1; // y = -1/run*x + 1
    // left medium function -> med value
    loMedHi[1] = 1 / (fuzzyVals->medVal) * currValue; // y = 1/run*x
    // zero -> hi value
    loMedHi[2] = 0;
  } else if (currValue >= fuzzyVals->medVal && currValue <= fuzzyVals->maxVal) {
    // zero -> lo value
    loMedHi[0] = 0;
    // right medium function -> med value
    loMedHi[1] = -1 / (fuzzyVals->medVal) * currValue + 2; // y = -1/run*x + 2
    // high function -> hi value
    loMedHi[2] = 1 / (fuzzyVals->medVal) * currValue - 1; // y = 1/run*x - 1
  } else {
    Serial.print("ERROR: "); Serial.print(currValue); Serial.println(" not in range.");
  }
}
