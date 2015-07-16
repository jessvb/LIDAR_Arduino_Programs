int pwPin = 5; // PW input on pin 5
float distPW;

int analogPin = A0; // Analog input pin on A0
float distAN;

void setup() {
  Serial.begin(9600);
  pinMode(pwPin, INPUT);
}

void loop() {
  // ------- READ PW ------- //
  // Multiply PW input by 2.54/147 to get cm
  distPW = (pulseIn(pwPin, HIGH))/147*2.54;
  Serial.print("PW dist (cm): ");
  Serial.print(distPW);
  
  // ------- READ ANALOG ------- //
  // Multiply analog input by 2.54/2 to get cm:
  distAN = (analogRead(analogPin))*2.54/2;
  Serial.print("    Analog dist (cm): ");
  Serial.println(distAN);
  
  delay(1000);
}
