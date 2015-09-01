int pwm = 6; // PWM pin is pin #3
// Increment value by 10% each time:
#define INCR 5

void setup() {
  Serial.begin(9600);
  analogWrite(pwm, 0);
  delay(500);
}

void loop() {
  for(int fanspd = 20; fanspd <= 100; fanspd += INCR){
    analogWrite(pwm, map(fanspd, 0, 100, 0, 255));
    Serial.print(fanspd); Serial.print("%   @");
    Serial.println(map(fanspd, 0, 100, 0, 255));
    delay(1000); // Delay ten seconds (10s = 10000ms)
  }
}
