boolean onoff = LOW;


#define LEDPIN1 4
#define LEDPIN2 7

void setup() {
  Serial.begin(9600); // Open serial connection at 9600bps
  pinMode(LEDPIN1, OUTPUT);
  pinMode(LEDPIN2, OUTPUT);

}

void loop() {
  if (onoff) {
    Serial.println("ON");
    digitalWrite(LEDPIN1, HIGH);
    digitalWrite(LEDPIN2, LOW);
    onoff = LOW;
  } else {
        Serial.println("OFF");

    digitalWrite(LEDPIN1, LOW);
    digitalWrite(LEDPIN2, HIGH);
    onoff = HIGH;
  }
  delay(1000);
}
