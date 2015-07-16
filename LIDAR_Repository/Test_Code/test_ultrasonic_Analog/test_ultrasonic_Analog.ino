int analogPin = A0; // Analog input pin on A0
float distCM;

void setup() {
  Serial.begin(9600);
}

void loop() {
  // ------- READ ULTRASONIC ------- //
  // Multiply analog input by 2.54/2 to get cm:
  distCM = (analogRead(analogPin))*2.54/2;
  Serial.print("Analog dist (cm): ");
  Serial.println(distCM);
  delay(100);
}
