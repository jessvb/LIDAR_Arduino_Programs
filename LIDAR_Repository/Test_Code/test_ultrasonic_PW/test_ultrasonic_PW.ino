int pwPin = 5; // PW input on pin 5
long distMM;

void setup() {
  Serial.begin(9600);
  pinMode(pwPin, INPUT);
}

void loop() {
  // ------- READ ULTRASONIC ------- //
  distMM = pulseIn(pwPin, HIGH); // Distance in mm
  Serial.print("Distance in mm: ");
  Serial.println(distMM);
  delay(10);
}
