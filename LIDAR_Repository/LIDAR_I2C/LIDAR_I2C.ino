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
int counter = 0; // Counter for taking average velocity values
const short AVG_COUNT = 5; // Number of data points that are averaged each loop

int fanPin = 3; // PWM pin for the fan is pin #3
// Map the fanPin from PWM_MIN (when the fan just starts to spin) to PWM_MAX (255)
const int PWM_MIN = 108; // The lowest possible number sent to the fanPin (fan doesn't spin here)
const int PWM_MAX = 255; // The highest possible number sent to the fanPin
const float VEL_MIN = 0.3; // The lowest possible velocity required to send voltage to the fanPin
const float VEL_MAX = 3; // The velocity that maxes out the voltage sent to the fanPin

void setup() {
  Serial.begin(9600); //Opens serial connection at 9600bps.
  I2c.begin(); // Opens & joins the irc bus as master
  delay(100); // Waits to make sure everything is powered up before sending or receiving data
  I2c.timeOut(50); // Sets a timeout to ensure no locking up of sketch if I2C communication fails
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

  //---------- CALCULATE AVERAGE VELOCITY ----------//
  elapsed = now - before; // Time elapsed between previous read (distPrev) and this read (distNow) -- for velocity calculation
  // Calculate velocity and add it to avgVel:
  avgVel += (((float)(distPrev - distNow)) / ((float)elapsed)) * 10; // Multiply by 10 b/c 1 cm/ms = 10 m/s
  // Note: If the velocity is POSITIVE, then something is coming closer from behind (if negative, then something's moving away)

  counter++; // Another velocity has been added to avgVel
  // if AVG_COUNT data points have been collected, take the average:
  if (counter >= AVG_COUNT) {
    avgVel = avgVel / ((float)counter); // Calculate average
    Serial.println(avgVel); // Print velocities with a newline to communicate with Processing
    counter = 0; // Reset the counter

    //---------- ADJUST FAN SPEED ----------//
    if (avgVel < VEL_MIN) { // Velocity less than the minimum
      analogWrite(fanPin, 0); // Bottomed out
    } else if (avgVel > VEL_MAX) { // Velocity more than the maximum
      analogWrite(fanPin, 255); // Maxed out
    } else {
      analogWrite(fanPin, map(avgVel, VEL_MIN, VEL_MAX, PWM_MIN, PWM_MAX)); // Within range
    }
  }

  // Update values for next loop:
  before = now;
  distPrev = distNow;
  delay(10);
}

