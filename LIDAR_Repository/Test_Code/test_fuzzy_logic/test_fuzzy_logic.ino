#include "Structs.h"

#define LO 0
#define MED 1
#define HI 2
#define NUMVALS 101

fuzzy_values_t VEL = {0, 3, 6, 9, 12};
fuzzy_values_t DIST = {0, 10, 20, 30, 40};
fuzzy_values_t _PWM = {0, 63.75, 127.5, 191.25, 255};

const int RULES[3][3] = {{MED, LO, LO}, {HI, MED, MED}, {HI, HI, MED}}; // AFTER DONE TESTING CHANGE (HI MED MED) TO (HI MED LO) TO MAKE IT SYMMETRICAL
// NOTE: [nRows][mColumns] or [vel][dist]


void setup() {
  Serial.begin(9600); //Opens serial connection at 9600bps.

  /*float areaCentroid[2] = {0};
  float heights[2] = {0.3, 0.1};
  getAreaAndCentroid(areaCentroid, 0, 1.2, heights);
  Serial.println(areaCentroid[0]);
  Serial.println(areaCentroid[1]);*/

  float currVel = 7; // in meters/sec
  float currDist = 3; // in meters

  float velValues[3]; // [SLOW, MED, FAST]
  getLoMedHi(currVel, velValues, &VEL);

  float distValues[3]; // [SLOW, MED, FAST]
  getLoMedHi(currDist, distValues, &DIST);

  float pwmMembership[3]; // [LO, MED, HI]
  infer(velValues, distValues, pwmMembership);

  float finalPWM = defuzzification(pwmMembership, &_PWM);
  Serial.println(finalPWM);
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
 * arrays. The final array is used to find the membership of the PWM function (for
 * the low, med, high functions of */
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
  //  Serial.print("LO: "); Serial.println(pwmArr[LO]);
  //  Serial.print("MED: "); Serial.println(pwmArr[MED]);
  //  Serial.print("HI: "); Serial.println(pwmArr[HI]);
}

/* Defuzzification function. Given a membership array (maxVals) and the output fuzzy
 * value type (ex: PWM output struct), this function will return the final output
 * value (ex: 177 PWM). */
float defuzzification(float* maxVals, fuzzy_values_t* fuzzyVals) {

  // TO MAKE MORE EFFICIENT, COMBINE THE GETFXNPTS & AREAANDCENTROID FXNS SO YOU DON'T HAVE TO CALCULATE X OVER AND OVER AGAIN!!!!

  float pwmFxn[NUMVALS] = {0};
  getFxnPts(pwmFxn, maxVals, fuzzyVals);

  // Get the area & "weight" of each section of the graph:
  float totalArea = 0;
  float totalWeight = 0;
  const float dx = fuzzyVals->maxVal / (NUMVALS - 1); // base of the trapezoid
  int i = 0;
  for (i = 0; i < NUMVALS; i++) {
    // Get the area and centroid [area, centroid] of the current section of graph:
    float areaAndCentroid[2];
    float x = i * (fuzzyVals->maxVal / (NUMVALS - 1)); // x position on graph
    getAreaAndCentroid(areaAndCentroid, i, x, dx, pwmFxn);
    // Update the total area & total weight:
    totalArea += areaAndCentroid[0]; // array: [area, centroid]
    totalWeight += areaAndCentroid[0] * areaAndCentroid[1]; // + (area*centroid)
  }

  // Return the final, defuzzified output value:
  return totalWeight / totalArea;
}

/* This method modifies an array of NUMVALS values to the y-values of the fuzzyVals graph.
 * These values are needed to determine the final output of the defuzzification. */
void getFxnPts(float* fxn, float* maxVals, fuzzy_values_t* fuzzyVals) {
  int i = 0;
  for (i = 0; i < NUMVALS; i++) {
    float loMedHi[3] = {0};
    float x = i * (fuzzyVals->maxVal / (NUMVALS - 1)); // x position on graph
    getLoMedHi(x, loMedHi, fuzzyVals);

    // Take the minimum between the function caps and the function values, and then take the
    // maximum between each function. This gives the final output function values.
    fxn[i] = max(min(maxVals[LO], loMedHi[LO]), min(maxVals[MED], loMedHi[MED]));
    fxn[i] = max(fxn[i], min(maxVals[HI], loMedHi[HI]));
  }
}

/* This method takes an array of function values, an integer index for that array, the x
 * position at that index, and the distance between two points on the output graph (dx).
 * It then modifies a given array to contain [area, centroid] of the graph to the right
 * of the given index. */
void getAreaAndCentroid(float* areaAndCentroid, int i, float x, float dx, float* fxnPts) {
  //       /|
  //      / |
  // b-> |  | <-a  (a > b)
  //     |__|
  //      dx
  // If right value is greater than left value, a = right value & b = left value:
  float a;
  float b;
  if (fxnPts[i + 1] - fxnPts[i] > 0) {
    a = fxnPts[i + 1];
    b = fxnPts[i];
  } else {
    a = fxnPts[i];
    b = fxnPts[i + 1];
  }
  // Find the area based on the area of a trapezoid:
  areaAndCentroid[0] = dx * b + dx * (a - b) / 2;
  // Find the centroid based on the centroid of a trapezoid + left x position:
  areaAndCentroid[1] = ((1.0 / 3.0) * dx * (b + 2.0 * a) / (a + b)) + (x);


  //Serial.print("Area: "); Serial.println(areaAndCentroid[0]);
  //Serial.print("                 Centroid: "); Serial.println(areaAndCentroid[1]);
}
