/*
 * This sketch recieves distance values from a LIDAR-Lite Sensor, converts these values
 * to velocities, and outputs these values to the serial monitor.
 *
 * It utilizes the 'Arduino I2C Master Library' from DSS Circuits.
 * Also see: https://github.com/PulsedLight3D/LIDARLite_Basics
 */

#include <I2C.h>                             // For I2C communication with the LIDAR
#include "StructDefs.h"                      // For definition of struct type
#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.

// ----------fuzzy-------- //
#define LO 0
#define MED 1
#define HI 2
#define NUMVALS 101

fuzzy_values_t VEL = {0, 3, 6, 9, 12};
fuzzy_values_t DIST = {0, 10, 20, 30, 40};
fuzzy_values_t _PWM = {0, 63.75, 127.5, 191.25, 255};

const int RULES[3][3] = {{MED, LO, LO}, {HI, MED, LO}, {HI, HI, MED}};
// NOTE: [nRows][mColumns] or [vel][dist]
//---------end fuzzy---------//

unsigned long now_avg = 0; // For testing how long it takes to calculate one avg
unsigned long before_avg = 0; // For testing how long it takes to calculate one avg

int distNow = 0; // The current distance value.
unsigned long now = 0; // The current time -> when a distNow is measured.
int distPrev = 9999999; // The previous distance value measured.
unsigned long before = 0; // The previous time a distance value was measured.
unsigned long elapsed = 0; // The time elapsed between measuring distPrev and distNow.
float totalVel = 0; // The last couple velocity values combined (used to take an average)
float totalDist = 0; // The last couple distance values combined (used to take an average)
int counter = 0; // Counter for taking averages
const short AVG_COUNT = 5; // Number of data points that are averaged each loop

const int motorPinLeft = 5; // PWM pin for the left motor
const int motorPinRight = 6; // PWM pin for the right motor
// Map the motorPin from PWM_MIN (when the motor just starts to spin) to PWM_MAX (255)
const int PWM_MIN = 108; // The lowest possible number sent to the motorPins (motor doesn't spin here)
const int PWM_MAX = 255; // The highest possible number sent to the motorPins
const float VEL_MIN = 0.3; // The lowest possible velocity required to send voltage to the motorPin
const float VEL_MAX = 10; // The velocity that maxes out the voltage sent to the motorPin

const int ultraPinRight = 3; // interrupt input for the right ultrasonic -> int.0
const int LEDPinRight = 7; // PW output for the right LED
const int ultraPinLeft = 2; // interrupt input for the left ultrasonic -> int.1
const int LEDPinLeft = 4; // PW output for the left LED
#define MIN_ULTRA_DIST 12.7 // The minimum distance in cm to light the LED to its max
#define MAX_ULTRA_DIST 100.0 // The minimum distance in cm to just light the LED

ultra_state_t rightUltra; // The state of the right ultrasonic sensor
ultra_state_t leftUltra; // The state of the left ultrasonic sensor

void setup() {
  Serial.begin(9600); // Open serial connection at 9600bps
  pinMode(LEDPinRight, OUTPUT);
  pinMode(LEDPinLeft, OUTPUT);
  I2c.begin(); // Open & join the irc bus as master
  delay(100); // Wait to make sure everything is powered up before sending or receiving data
  I2c.timeOut(50); // Set a timeout to ensure no locking up of sketch if I2C communication fails
  analogWrite(motorPinLeft, 0); // Set the motors to not spin
  analogWrite(motorPinRight, 0); // Set the motors to not spin
  attachInterrupt(ultraPinRight - 2, rightInterrupt, CHANGE); // For reading the ultrasonics w/o pausing the code
  attachInterrupt(ultraPinLeft - 2, leftInterrupt, CHANGE); // An interrupt is called every time the left/right
  // ultrasonic has a change in state (rising/falling edge -> 1/0)
}

/* ----------------------------------------------------
 * ----------------------- MAIN -----------------------
 * ---------------------------------------------------- */
void loop() {
  //---------- GET DISTANCE ---------//
  // Write 0x04 to register 0x00
  uint8_t nackack = 100; // Setup variable to hold ACK/NACK resopnses
  while (nackack != 0) { // While NACK keep going (i.e. continue polling until sucess message (ACK) is received )
    nackack = I2c.write(LIDARLite_ADDRESS, RegisterMeasure, MeasureValue); // Write 0x04 to 0x00
    // (I2c.write() returns 0 when successful write)
    delay(1); // Wait 1 ms to prevent overpolling
  }

  byte distanceArray[2]; // array to store distance bytes from read function

  // Read 2byte distance from register 0x8f
  nackack = 100; // Setup variable to hold ACK/NACK resopnses
  while (nackack != 0) { // While NACK keep going (i.e. continue polling until sucess message (ACK) is received )
    nackack = I2c.read(LIDARLite_ADDRESS, RegisterHighLowB, 2, distanceArray); // Read 2 Bytes from LIDAR-Lite Address and store in array
    delay(1); // Wait 1 ms to prevent overpolling
  }
  // Convert bytes to distance values:
  distNow = (distanceArray[0] << 8) + distanceArray[1];  // Shift high byte [0] 8 to the left and add low byte [1] to create 16-bit int
  now = millis(); // The time that distNow was measured.


  //---------- CALCULATE AVERAGE VELOCITY & DISTANCE ----------//
  elapsed = now - before; // Time elapsed between previous read (distPrev) and this read (distNow) -- for velocity calculation
  // Calculate velocity and add it to totalVel:
  totalVel += (((float)(distPrev - distNow)) / ((float)elapsed)) * 10; // Multiply by 10 b/c 1 cm/ms = 10 m/s
  // Note: If the velocity is POSITIVE, then something is coming closer from behind (if negative, then something's moving away)
  totalDist += distNow;
  counter++; // One more data point collected for each distance/velocity
  // if AVG_COUNT data points have been collected, take the average:
  if (counter >= AVG_COUNT) {
    float avgVel = takeAverage(totalVel, counter);
    totalVel = 0; // Reset totalVel for next average
    float avgDist = takeAverage(totalDist, counter);
    totalDist = 0; // Reset totalDist for next average
    // Print to serial to communicate with Processing:
    Serial.print(avgVel); Serial.print('V');
    Serial.print(avgDist); Serial.println('D');


    //---------- ADJUST MOTOR SPEED ----------//
    writeMotorSpeed (motorPinLeft, avgVel, avgDist);
    writeMotorSpeed (motorPinRight, avgVel, avgDist);

    counter = 0; // Reset the counter for taking averages
    before_avg = now_avg; // Update the previous time for the next loop
    now_avg = millis(); // Update the current time
  }

  //---------- CALCULATE LIDAR DISTANCES ----------//
  if (rightUltra.newDt) {
    pulseCalc(&rightUltra, LEDPinRight);
  }
  if (leftUltra.newDt) {
    pulseCalc(&leftUltra, LEDPinLeft);
  }

  //---------- UPDATE VALUES FOR NEXT LOOP ----------//
  before = now;
  distPrev = distNow;

  delay(10);
}

/* ----------------------------------------------------
 * ------------------ HELPER METHODS ------------------
 * ---------------------------------------------------- */

/* Takes the average given the total and a counter */
float takeAverage (float total, int counter) {
  return total / ((float)counter);
}

/* Changes the motor speed based on velocity and distance according to a fuzzy logic
 * approximation function. */
void writeMotorSpeed (int motorPin, float velocity, float distance) {
    float velValues[3]; // [SLOW, MED, FAST]
    getLoMedHi(velocity, velValues, &VEL);

    float distValues[3]; // [SLOW, MED, FAST]
    getLoMedHi(distance, distValues, &DIST);

    float pwmMembership[3]; // [LO, MED, HI]
    infer(velValues, distValues, pwmMembership);

    float finalPWM = defuzzification(pwmMembership, &_PWM);
    analogWrite(motorPin, finalPWM);
}

/* Calculates distance for the ultrasonics based on pulse length */
void pulseCalc (ultra_state_t *ultra, int ledPin) {
  // Calculate the distance in centimeters:
  ultra->distance = ultra->dt / 147.0 * 2.54; // Convert to cm: pulse/147*2.54 = cm

  //Add the distance to the total distance and then average if needed
  ultra->totalDist += ultra->distance;
  ultra->counter++; // Another distance was calculated

  if (ultra->counter >= AVG_COUNT) {
    // Change the LED intensity:
    writeLED(ledPin, takeAverage(ultra->totalDist, ultra->counter));
    // Reset the counter and total distance for the next average:
    ultra->counter = 0;
    ultra->totalDist = 0;
  }

  // Update pulse flag (no new pulse time value):
  ultra->newDt = 0;
}

/* When the right ultrasonic starts/finishes a pulse, this method will be called */
void rightInterrupt() {
  // If the pulse is high, then save the start time:
  if (digitalRead(ultraPinRight) > 0 && rightUltra.pulseStarted == 0) {
    rightUltra.startTime = micros();
    rightUltra.pulseStarted = 1;
  }
  // If the pulse finishes:
  else if (rightUltra.pulseStarted) {
    // Get the total pulse time:
    rightUltra.dt = micros() - rightUltra.startTime;

    // Clear the flags (pulse hasn't started any more & new pulse time ready):
    rightUltra.pulseStarted = 0;
    rightUltra.newDt = 1;
  }
}

/* When the left ultrasonic starts/finishes a pulse, this method will be called */
void leftInterrupt() {
  // If the pulse is high, then save the start time:
  if (digitalRead(ultraPinLeft) > 0 && leftUltra.pulseStarted == 0) {
    leftUltra.startTime = micros();
    leftUltra.pulseStarted = 1;
  }
  // If the pulse finishes:
  else if (leftUltra.pulseStarted) {
    // Get the total pulse time:
    leftUltra.dt = micros() - leftUltra.startTime;

    // Clear the flags (pulse hasn't started any more & new pulse time ready):
    leftUltra.pulseStarted = 0;
    leftUltra.newDt = 1;
  }
}

/* Changes the intensity of the LED based on the distance provided */
void writeLED (int ledPin, float dist) {
  if (dist < (MIN_ULTRA_DIST + MAX_ULTRA_DIST) / 2)
    digitalWrite(ledPin, HIGH);
  else
    digitalWrite(ledPin, LOW);

  /*
  if (dist <= MIN_ULTRA_DIST) {
    // If very close, light the LED to its maximum
    analogWrite(ledPin, 255);
  } else if (dist >= MAX_ULTRA_DIST) {
    // If far away, don't light the LED
    analogWrite(ledPin, 0);
  } else {
    // If in between max & min distance, map the light intensity
    // between 0 and 150 (difficult to see difference between
    // 150<->255, thus the gradient is better between 0<->150)
    analogWrite(ledPin, map(dist, MIN_ULTRA_DIST, MAX_ULTRA_DIST, 150, 0));
  }
  */
}



/* ------------ FUZZY FUNCTIONS!! ----------- */;

/* Fuzzy function. Given a specific velocity/distance, fuzzy value type,
 * and an array, it will modify the array to the correct [SLOW, MED,
 * FAST] values */
void getLoMedHi(float currValue, float* loMedHi, fuzzy_values_t* fuzzyVals) {
  // Check which area the current value sits in:
  // (Note: float comparison -> subtract/add 0.001 just in case!)
  if (currValue >= (fuzzyVals->minVal - 0.001) && currValue < fuzzyVals->medVal) {
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
  // value between vel/dist, and give that value to pwm if it's larger than the
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
      //      Serial.print("y_0: "); Serial.println(y_0);
    } else {
      slope_curr = (y_curr - y_prev) / (dx);
      // Make sure we have 2 slopes & they aren't equal (floating pt comparison)
      if (slope_prev != -999 && abs(slope_curr - slope_prev) > 0.001 ) {
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
        if (area > 0.001) {
          totalArea += area;
          // Find the centroid based on the centroid of a trapezoid + left x position:
          totalMass += area * ((1.0 / 3.0) * base * (b + 2.0 * a) / (a + b) + x_0);
        }

        // Update x_0 & y_0:
        x_0 = x_curr - dx; //Serial.print("x_0: "); Serial.println(x_0);
        y_0 = y_prev; //Serial.print("y_0: "); Serial.println(y_0);
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
  if (area > 0.001) {
    totalArea += area;
    // Find the centroid based on the centroid of a trapezoid + left x position:
    totalMass += area * ((1.0 / 3.0) * base * (b + 2.0 * a) / (a + b) + x_0);
  }
  return totalMass / totalArea;
}


/* Take the minimum between the function caps and the function values, and then take the
 * maximum between each function. This gives the final output function values. */
float getY(float* maxVals, float* loMedHi) {
  float y = max(min(maxVals[LO], loMedHi[LO]), min(maxVals[MED], loMedHi[MED]));
  y = max(y, min(maxVals[HI], loMedHi[HI]));
  return y;
}


