int ultraPinRight = 2; // PW input for the right ultrasonic
int LEDPinRight = 4; // PW output for the right LED
int ultraPinLeft = 3; // PW input for the left ultrasonic
int LEDPinLeft = 7; // PW output for the left LED
#define MAX_DIST 100.0
#define MIN_DIST 12.7

void setup() {
  Serial.begin(9600);
  pinMode(ultraPinRight, INPUT);
  pinMode(ultraPinLeft, INPUT);
  pinMode(LEDPinRight, OUTPUT);
  pinMode(LEDPinLeft, OUTPUT);
}

void loop() {
  readDistWriteLED (ultraPinRight, LEDPinRight);
  readDistWriteLED (ultraPinLeft, LEDPinLeft);
  delay(10);
}

void readDistWriteLED (int distPin, int ledPin) {
  // ------- READ ULTRASONIC ------- //
  // Convert to cm: pulse/147*2.54 = cm
  float distCM = (pulseIn(distPin, HIGH)) / 147 * 2.54;

  if (distPin == ultraPinLeft) {
    Serial.print("LEFT distance (cm): ");
    Serial.println(distCM);
  } else {
    Serial.print("                             RIGHT distance (cm): ");
    Serial.println(distCM);
  }

  // ------- WRITE LED ------- //
  if (distCM <= MIN_DIST) {
    // If very close, light the LED to its maximum
    analogWrite(ledPin, 255);
  } else if (distCM >= MAX_DIST) {
    // If far away, don't light the LED
    analogWrite(ledPin, 0);
  } else {
    // If in between max & min distance, map the light intensity
    // between 0 and 150 (difficult to see difference between
    // 150<->255, thus the gradient is better between 0<->150)
    analogWrite(ledPin, map(distCM, MIN_DIST, MAX_DIST, 150, 0));
  }
}
