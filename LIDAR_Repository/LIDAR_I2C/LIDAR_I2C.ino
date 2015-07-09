/*
http://pulsedlight3d.com
This sketch demonstrates getting distance with the LIDAR-Lite Sensor
It utilizes the 'Arduino I2C Master Library' from DSS Circuits.
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
float velocity = 0; // The velocity value.


void setup() {
  Serial.begin(9600); //Opens serial connection at 9600bps.
  I2c.begin(); // Opens & joins the irc bus as master
  delay(100); // Waits to make sure everything is powered up before sending or receiving data
  I2c.timeOut(50); // Sets a timeout to ensure no locking up of sketch if I2C communication fails
}

void loop() {
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

  // Print distances and times:
  Serial.print("distPrev: "); Serial.print(distPrev);  Serial.print("\t");
  Serial.print("Distance value: ");  Serial.print(distNow);  Serial.print("\t");
  Serial.print("now: "); Serial.print(now);  Serial.print("\t");
  Serial.print("before: "); Serial.print(before);  Serial.print("\t");

  // Calulate velocity:
  elapsed = now - before; // Time elapsed between previous read (distPrev) and this read (distNow) -- for velocity calculation
  velocity = ((float)(distPrev - distNow)) / ((float)elapsed); // If the velocity is POSITIVE, then something is coming closer from behind (ignore negatives)

  // Print elapsed time and velocity:
  Serial.print("elapsed: "); Serial.print(elapsed);  Serial.print("\t");
  Serial.print("velocity: "); Serial.print(velocity);  Serial.print("\n");


  // Update values for next loop:
  before = now;
  distPrev = distNow;
  delay(1000);
}

