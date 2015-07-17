/*
 * This sketch recieves distance values from a LIDAR-Lite Sensor, converts these values
 * to velocities, and outputs these values to the serial monitor.
 *
 * It utilizes the 'Arduino I2C Master Library' from DSS Circuits.
 * Also see: https://github.com/PulsedLight3D/LIDARLite_Basics
 */

#include <I2C.h>
#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.

int distNow = 0; // The current distance value.
unsigned long now = 0; // The current time -> when a distNow is measured.
int distPrev = 9999999; // The previous distance value measured.
unsigned long before = 0; // The previous time a distance value was measured.
unsigned long elapsed = 0; // The time elapsed between measuring distPrev and distNow.
float avgVel = 0; // The average velocity for last couple of distances
float avgDist = 0; // The average distance for last couple data points
int counter = 0; // Counter for taking averages
const short AVG_COUNT = 5; // Number of data points that are averaged each loop

int fanPin = 3; // PWM pin for the fan is pin #3
// Map the fanPin from PWM_MIN (when the fan just starts to spin) to PWM_MAX (255)
const int PWM_MIN = 108; // The lowest possible number sent to the fanPin (fan doesn't spin here)
const int PWM_MAX = 255; // The highest possible number sent to the fanPin
const float VEL_MIN = 0.3; // The lowest possible velocity required to send voltage to the fanPin
const float VEL_MAX = 3; // The velocity that maxes out the voltage sent to the fanPin

int ultraPinRight = 5; // PW input for the right ultrasonic
int LEDPinRight = 10; // PW output for the right LED
int ultraPinLeft = 6; // PW input for the left ultrasonic
int LEDPinLeft = 9; // PW output for the left LED
#define MIN_ULTRA_DIST 12.7
#define MAX_ULTRA_DIST 100.0

void setup() {
  Serial.begin(9600); // Open serial connection at 9600bps
  pinMode(ultraPinRight, INPUT);
  pinMode(ultraPinLeft, INPUT);
  pinMode(LEDPinRight, OUTPUT);
  pinMode(LEDPinLeft, OUTPUT);
  I2c.begin(); // Open & join the irc bus as master
  delay(100); // Wait to make sure everything is powered up before sending or receiving data
  I2c.timeOut(50); // Set a timeout to ensure no locking up of sketch if I2C communication fails
  analogWrite(fanPin, 0); // Set the fan to not spin
}

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

  // Get ultrasonic distances:
  float distLeft = (float)(pulseIn(ultraPinLeft, HIGH)) / 147 * 2.54; // Convert to cm: pulse/147*2.54 = cm
  float distRight = (float)(pulseIn(ultraPinRight, HIGH)) / 147 * 2.54;

  //---------- CALCULATE AVERAGE VELOCITY & DISTANCE ----------//
  elapsed = now - before; // Time elapsed between previous read (distPrev) and this read (distNow) -- for velocity calculation
  // Calculate velocity and add it to avgVel:
  avgVel += (((float)(distPrev - distNow)) / ((float)elapsed)) * 10; // Multiply by 10 b/c 1 cm/ms = 10 m/s
  // Note: If the velocity is POSITIVE, then something is coming closer from behind (if negative, then something's moving away)
  avgDist += distNow;

  counter++; // Another velocity/dist has been added to avgVel/avgDist
  // if AVG_COUNT data points have been collected, take the average:
  if (counter >= AVG_COUNT) {
    avgVel = avgVel / ((float)counter); // Calculate average velocity
    avgDist = avgDist / ((float)counter); // Calculate average distance
    // Print velocities with a V and newline to communicate with Processing
    Serial.print(avgVel); Serial.print('V');
    // Print distances with a D and newline to communicate with Processing
    Serial.print(avgDist); Serial.println('D');
    counter = 0; // Reset the counter

    //---------- ADJUST FAN SPEED & LED FADE ----------//
    // Fan speed:
    if (avgVel < VEL_MIN) { // Velocity less than the minimum
      analogWrite(fanPin, 0); // Bottomed out
    } else if (avgVel > VEL_MAX) { // Velocity more than the maximum
      analogWrite(fanPin, 255); // Maxed out
    } else {
      analogWrite(fanPin, map(avgVel, VEL_MIN, VEL_MAX, PWM_MIN, PWM_MAX)); // Within range
    }
  }
  // LED fade:
  writeLED(LEDPinRight, distRight);
  writeLED(LEDPinLeft, distLeft);


  //---------- UPDATE VALUES FOR NEXT LOOP ----------//
  before = now;
  distPrev = distNow;
  delay(10);
}

/* Changes the intensity of the LED based on the distance provided */
void writeLED (int ledPin, float dist) {
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
}
