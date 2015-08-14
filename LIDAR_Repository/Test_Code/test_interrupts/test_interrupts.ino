/* This code tests using interrupts with the ultrasonics */


const int ultraPinRight = 3; // interrupt input for the right ultrasonic 2 -> int.0
const int LEDPinRight = 7; // PW output for the right LED
const int ultraPinLeft = 2; // interrupt input for the left ultrasonic 3 -> int.1
const int LEDPinLeft = 4; // PW output for the left LED

typedef struct _ultra_state {
  volatile uint32_t startTime; // Start time of pulse
  volatile uint32_t dt; // Pulse duration in microseconds
  volatile boolean pulseStarted; // Flag to ensure correct initial conditions
  volatile boolean lastState; // Keep track of the last logical state of the pin
  volatile float distance; // Distance reading based on the pulse read
} ultra_state_t;

ultra_state_t rightUltra;
ultra_state_t leftUltra;

void setup() {
  Serial.begin(9600); // Open serial connection at 9600bps
  pinMode(LEDPinRight, OUTPUT);
  pinMode(LEDPinLeft, OUTPUT);
  attachInterrupt(ultraPinRight - 2, rightInterrupt, CHANGE); // pin - 2 b/c pin2=int0 & pin3=int1
  attachInterrupt(ultraPinLeft - 2, leftInterrupt, CHANGE);
}

void loop() {
  Serial.println("o hey");
  delay(1000);
}

/* When the right ultrasonic starts/finishes a pulse, this method will be called */
void rightInterrupt() {
  // If the pulse is high, then save the start time
  boolean hi_lo = digitalRead(ultraPinRight);
  if (hi_lo > 0 && rightUltra.pulseStarted == 0) {
    rightUltra.startTime = micros();
    rightUltra.pulseStarted = 1;
  } else if (rightUltra.pulseStarted == 1) {
    rightUltra.dt = micros() - rightUltra.startTime;

    // Calculate the distance in centimeters:
    rightUltra.distance = rightUltra.dt / 147.0 * 2.54;
    Serial.print("                  RIGHT: ");
    Serial.println(rightUltra.distance);

    if (rightUltra.distance < 100)
      digitalWrite(LEDPinRight, HIGH);
    else
      digitalWrite(LEDPinRight, LOW);

    // Clear the flag:
    rightUltra.pulseStarted = 0;
  }
}

/* When the left ultrasonic starts/finishes a pulse, this method will be called */
void leftInterrupt() {
  // If the pulse is high, then save the start time
  boolean hi_lo = digitalRead(ultraPinRight);
  if (hi_lo > 0 && leftUltra.pulseStarted == 0) {
    leftUltra.startTime = micros();
    leftUltra.pulseStarted = 1;
  } else if (leftUltra.pulseStarted == 1) {
    leftUltra.dt = micros() - leftUltra.startTime;

    // Calculate the distance in centimeters:
    leftUltra.distance = leftUltra.dt / 147.0 * 2.54;
    Serial.print("LEFT: "); Serial.println(leftUltra.distance);

    if (leftUltra.distance < 100)
      digitalWrite(LEDPinLeft, HIGH);
    else
      digitalWrite(LEDPinLeft, LOW);

    // Clear the flag:
    leftUltra.pulseStarted = 0;
  }
}
