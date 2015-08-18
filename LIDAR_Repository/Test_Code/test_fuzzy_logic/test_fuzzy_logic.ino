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



  float currVel = 10; // in meters/sec
  float currDist = 15; // in meters

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
  } else if (currValue >= fuzzyVals->medVal && currValue <= (fuzzyVals->maxVal + 0.001)) {
    // zero -> lo value
    loMedHi[LO] = 0;
    // right medium function -> med value
    loMedHi[MED] = -1 / (fuzzyVals->medVal) * currValue + 2; // y = -1/run*x + 2
    // 1 - med value -> hi value
    // loMedHi[2] = 1 / (fuzzyVals->medVal) * currValue - 1; // y = 1/run*x - 1
    loMedHi[HI] = 1 - loMedHi[1];
  } else {
    Serial.print("ERROR: "); Serial.print(currValue); Serial.println(" not in range.");
    Serial.print("Range: "); Serial.print (fuzzyVals->minVal);
    Serial.print("-"); Serial.println (fuzzyVals->maxVal);
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

/* Defuzzification function. Given a membership array of the output function(maxVals)
 * and the ouput fuzzy value type (ex: PWM output struct), this function will return
 * the final output value (ex: 177 PWM). */
float defuzzification(float* maxVals, fuzzy_values_t* fuzzyVals) {
  float totalArea = 0;
  float totalMass = 0;

  float y_prev = -1;
  float y_curr = -1;
  float slope_curr = -999;
  float slope_prev = -999;
  float x_0 = 0;
  float y_0 = -1;

  int i = 0;
  float x_curr = 0;
  float dx = (fuzzyVals->maxVal) / (NUMVALS - 1);
  for (i = 0; i < NUMVALS; i++) {
    float loMedHi[3] = {0};
    getLoMedHi(x_curr, loMedHi, fuzzyVals); // Get possible function values at x_curr
    y_curr = getY(maxVals, loMedHi); // Get the function value at x_curr

    // Make sure we have two y-values & two slopes, then check if the current slope
    // matches the last slope. If it doesn't match, calculate the centroid/area and
    // add them to the totals.
    if (y_prev == -1) {
      // For first time through loop, update y_0:
      y_0 = y_curr;
      Serial.print("y_0: "); Serial.println(y_0);
    } else {
      slope_curr = (y_curr - y_prev) / (dx);
      if (slope_prev != -999 && slope_curr != slope_prev) {
        // Area complete! Add centroid/area to totals :)
        float base = (x_curr - dx) - x_0; // Base of trapezoid
        float a; // Larger side of trapezoid
        float b; // Smaller side of trapezoid
        //       /|
        //      / |
        // b-> |  | <-a  (a > b)
        //     |__|
        //      dx
        // If right value is greater than left value, a = right value & b = left value:
        if ( (y_prev - y_0) > 0) {
          a = y_prev;
          b = y_0;
        } else {
          a = y_0;
          b = y_prev;
        }
        // Find the area based on the area of a trapezoid:
        float area = base * b + base * (a - b) / 2;
        totalArea += area;
        // Find the centroid based on the centroid of a trapezoid + left x position:
        totalMass += area * ((1.0 / 3.0) * base * (b + 2.0 * a) / (a + b) + x_0);

        // Update x_0 & y_0:
        x_0 = x_curr - dx; Serial.print("x_0: "); Serial.println(x_0);
        y_0 = y_prev; Serial.print("y_0: "); Serial.println(y_0);
      }
    }

    // Increment x:
    x_curr += dx;
    // Update y_prev & slope_prev:
    y_prev = y_curr;
    slope_prev = slope_curr;
  }

  // Add last area to totals:
  float base = x_curr - x_0; // Base of trapezoid
  float a; // Larger side of trapezoid
  float b; // Smaller side of trapezoid
  if (y_curr - y_0 > 0) {
    a = y_curr;
    b = y_0;
  } else {
    a = y_0;
    b = y_curr;
  }
  // Find the area based on the area of a trapezoid:
  float area = base * b + base * (a - b) / 2;
  totalArea += area;
  // Find the centroid based on the centroid of a trapezoid + left x position:
  totalMass += area * ((1.0 / 3.0) * base * (b + 2.0 * a) / (a + b) + x_0);
  return totalMass / totalArea;
}


/* Take the minimum between the function caps and the function values, and then take the
 * maximum between each function. This gives the final output function values. */
float getY(float* maxVals, float* loMedHi) {
  float y = max(min(maxVals[LO], loMedHi[LO]), min(maxVals[MED], loMedHi[MED]));
  y = max(y, min(maxVals[HI], loMedHi[HI]));
  return y;
}

