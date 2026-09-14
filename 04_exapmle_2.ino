void setup() {
  // Initialize serial and wait for port to open
  Serial.begin(115200);
  while (!Serial){
    ; //wait for serial port to connect
    //not needed in Arduino Uno but im just doing this 
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Hello World!");
  delay(1000);

}
