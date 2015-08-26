const int motorPinLeft = 5; // PWM pin for the left motor
const int motorPinRight = 6; // PWM pin for the right motor
// Map the motorPin from PWM_MIN (when the motor just starts to spin) to PWM_MAX (255)
const int PWM_MIN = 108; // The lowest possible number sent to the motorPins (motor doesn't spin here)
const int PWM_MAX = 255; // The highest possible number sent to the motorPins


void setup() {
  Serial.begin(9600); // Open serial connection at 9600bps
  analogWrite(motorPinLeft, 0); // Set the motors to not spin
  analogWrite(motorPinRight, 0); // Set the motors to not spin
}

void loop() {
  int i = 0;
  for (i = 0; i <= 255; i += 127.5 / 2) {
    Serial.println(i);
    analogWrite(motorPinLeft, i);
    //analogWrite(motorPinRight, 0);

    //delay(10000);

    //analogWrite(motorPinLeft, 0);
    //analogWrite(motorPinRight, 255);

    delay(5000);

  }
}
