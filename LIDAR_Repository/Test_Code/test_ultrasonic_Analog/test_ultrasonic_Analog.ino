int analogPin = A0; // Analog input pin on A0
long distMM;

void setup() {
  Serial.begin(9600);
}

void loop() {
  // ------- READ ULTRASONIC ------- //
  // Multiply analog input by 5 to get mm:
  distMM = (analogRead(analogPin))*5;
  Serial.print("Distance in mm: ");
  Serial.println(distMM);
  delay(10);
}
