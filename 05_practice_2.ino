#define PIN_LED 7
unsigned int count, toggle;
void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);

  Serial.println("on");

  
  count = 0;
  toggle = 0;
}

void loop() {

  digitalWrite(PIN_LED, toggle);
  delay(1000);

  toggle = 1;
  while (count <=10) {

    digitalWrite(PIN_LED, toggle);
    delay(100);
    toggle = !toggle;
    count++;
  }
  while (1) {}

}
