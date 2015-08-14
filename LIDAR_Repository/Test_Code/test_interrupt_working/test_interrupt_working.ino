/* This code tests using interrupts with the ultrasonics */

const int ultraPinRight = 2; // interrupt input for the right ultrasonic -> int.0
const int LEDPinRight = 7; // PW output for the right LED
const int ultraPinLeft = 3; // interrupt input for the left ultrasonic -> int.1
const int LEDPinLeft = 4; // PW output for the left LED

unsigned long starttimeright = 0;
unsigned long starttimeleft = 0;
boolean startedright = 0;
boolean startedleft = 0;

void setup() {
  Serial.begin(9600); // Open serial connection at 9600bps
  pinMode(LEDPinRight, OUTPUT);
  pinMode(LEDPinLeft, OUTPUT);
  attachInterrupt(0, rightInterrupt, CHANGE);
  attachInterrupt(1, leftInterrupt, CHANGE);
}

void loop() {
  Serial.println("o hey");
  delay(1000);
}

/* When the right ultrasonic starts/finishes a pulse, this method will be called */
void rightInterrupt() {
  if (startedright == 0) {
    starttimeright = micros();
    startedright = 1;
  } else {
    Serial.print("                      Right dist: ");
    Serial.println(float(micros() - starttimeright) / 147.0 * 2.54);
    startedright = 0;
  }
}

/* When the left ultrasonic starts/finishes a pulse, this method will be called */
void leftInterrupt() {
  if (startedleft == 0) {
    starttimeleft = micros();
    startedleft = 1;
  } else {
    Serial.print("Left dist: ");
    Serial.println(float(micros() - starttimeleft) / 147.0 * 2.54);
    startedleft = 0;
  }
}
