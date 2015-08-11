#include "Structs.h"

#define LO 0
#define MED 1
#define HI 2

fuzzy_values_t VEL = {0, 3, 6, 9, 12};
fuzzy_values_t DIST = {0, 10, 20, 30, 40};
fuzzy_values_t _PWM = {0, 63.75, 127.5, 191.25, 255};

const int RULES[3][3] = {{MED, LO, LO}, {HI, MED, MED}, {HI, HI, MED}};
// NOTE: [nRows][mColumns] & [vel][dist]


void setup() {
  Serial.begin(9600); //Opens serial connection at 9600bps.

  float currVel = 10; // in meters/sec
  float velValues[3]; // [SLOW, MED, FAST]
  getLoMedHi(currVel, velValues, &VEL);

  float currDist = 2.7; // in meters
  float distValues[3]; // [SLOW, MED, FAST]
  getLoMedHi(currDist, distValues, &DIST);

  float pwmMembership[3]; // [LO, MED, HI]
  infer(velValues, distValues, pwmMembership);
}

void loop() {
}

/* Fuzzy function. Given a specific velocity/distance, fuzzy value type,
 * and an array, it will modify the array to the correct [SLOW, MED,
 * FAST] values */
void getLoMedHi(float currValue, float* loMedHi, fuzzy_values_t* fuzzyVals) {
  if (currValue >= fuzzyVals->minVal && currValue < fuzzyVals->medVal) {
    // low function -> lo value
    loMedHi[LO] = -1 / (fuzzyVals->medVal) * currValue + 1; // y = -1/run*x + 1
    // 1 - lo value -> med value
    // loMedHi[1] = 1 / (fuzzyVals->medVal) * currValue; // y = 1/run*x
    loMedHi[MED] = 1 - loMedHi[0];
    // zero -> hi value
    loMedHi[HI] = 0;
  } else if (currValue >= fuzzyVals->medVal && currValue <= fuzzyVals->maxVal) {
    // zero -> lo value
    loMedHi[LO] = 0;
    // right medium function -> med value
    loMedHi[MED] = -1 / (fuzzyVals->medVal) * currValue + 2; // y = -1/run*x + 2
    // 1 - med value -> hi value
    // loMedHi[2] = 1 / (fuzzyVals->medVal) * currValue - 1; // y = 1/run*x - 1
    loMedHi[HI] = 1 - loMedHi[1];
  } else {
    Serial.print("ERROR: "); Serial.print(currValue); Serial.println(" not in range.");
  }
}

/* Inference function. Given two arrays of loMedHi values for velocity and distance,
 * this function will output ('modify') an array that combines the two initial
 * arrays. The final array is used to find the membership of the PWM function */
void infer(float* velArr, float* distArr, float* pwmArr) {
  // Initialize the pwm array to 0.
  pwmArr[LO] = 0; pwmArr[MED] = 0; pwmArr[HI] = 0;

  // Cycle through the cases (vel=lo,dist=lo; vel=lo,dist=med; etc), use the RULES
  // to determine if the vel/dist pair applies to a lo/med/hi pwm, take the minimum
  // value between vel/dist, and give that value to pwm, if it's larger than the
  // current value for pwm (take the maximum to accummulate).
  int i = LO;
  int j = LO;
  for (i = LO; i <= HI; i++) {
    for (j = LO; j <= HI; j++) {
      float smaller = min(velArr[i], distArr[j]);
      if (smaller > pwmArr[RULES[i][j]])
        pwmArr[RULES[i][j]] = smaller;
    }
  }
  Serial.print("LO: "); Serial.println(pwmArr[LO]);
  Serial.print("MED: "); Serial.println(pwmArr[MED]);
  Serial.print("HI: "); Serial.println(pwmArr[HI]);
}
