 unsigned long time1, time2;
 
void setup() {
  Serial.begin(9600); //Opens serial connection at 9600bps.
}

void loop() {
  // put your main code here, to run repeatedly:
  
    //get times
  time1 = millis();
  delay(20);
  time2 = millis();

  Serial.print("Time1: ");
  Serial.println(time1);
  Serial.print("Time2: ");
  Serial.println(time2);
  Serial.print("Elapsed time: ");
  Serial.println(time2 - time1);
  
  delay(50);

}
