int pwPin = 5; // PW input on pin 5
float distCM;

void setup() {
  Serial.begin(9600);
  pinMode(pwPin, INPUT);
}

void loop() {
  // ------- READ ULTRASONIC ------- //
  distCM = (pulseIn(pwPin, HIGH))/147*2.54; // pulse/147*2.54 to cm
  Serial.print("PW dist (cm): ");
  Serial.println(distCM);
  delay(100);
}
