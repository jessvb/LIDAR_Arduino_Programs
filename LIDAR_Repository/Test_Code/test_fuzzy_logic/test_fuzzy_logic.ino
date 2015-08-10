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

}
