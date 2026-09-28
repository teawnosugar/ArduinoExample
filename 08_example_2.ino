// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12   // sonar sensor TRIGGER
#define PIN_ECHO 13   // sonar sensor ECHO

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25      // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100.0   // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300.0   // maximum distance to be measured (unit: mm)

float _dist_mid = (_DIST_MIN+_DIST_MAX)/2; ///추가함1

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL) // coefficent to convert duration to distance

unsigned long last_sampling_time;   // unit: msec

void setup() {
  // initialize GPIO pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);  // sonar TRIGGER
  pinMode(PIN_ECHO, INPUT);   // sonar ECHO
  digitalWrite(PIN_TRIG, LOW);  // turn-off Sonar 
  
  // initialize serial portmn
  Serial.begin(57600);
}

int convert(float d) {
  if (d <= _dist_mid)
    //return (d - _DIST_MIN) / (_dist_mid - _DIST_MIN) * 255;   // 100~200: 0 → 255
    return (_DIST_MAX - d) / (_DIST_MAX - _dist_mid) * 255;
  else
    //return (_DIST_MAX - d) / (_DIST_MAX - _dist_mid) * 255;   // 200~300: 255 → 0
    return (d - _DIST_MIN) / (_dist_mid - _DIST_MIN) * 255;
    //현재 5v->9번핀으로 흐르는 중이라 뒤집어줌
}



void loop() { 
  float distance;

  // wait until next sampling time. // polling
  // millis() returns the number of milliseconds since the program started.
  //    will overflow after 50 days.
  if (millis() < (last_sampling_time + INTERVAL))
    return; //여기서 시간이 남으면 loop에서 아무것도 안 하고 돌도록
    //거리를 잴 때 걸리는 시간이 상대적 짧거나 길 수 있음 

  distance = USS_measure(PIN_TRIG, PIN_ECHO); // read distance

  if ((distance == 0.0) || (distance > _DIST_MAX)) {
      distance = _DIST_MAX + 10.0;    // Set Higher Value
      digitalWrite(PIN_LED, 1);       // LED OFF
  } else if (distance < _DIST_MIN) {
      distance = _DIST_MIN - 10.0;    // Set Lower Value
      digitalWrite(PIN_LED, 1);       // LED OFF
  } else {    // In desired Range
  //    digitalWrite(PIN_LED, 0);       // LED ON    
    analogWrite(PIN_LED, convert(distance));

  }

  // output the distance to the serial port
  Serial.print("Min:");        Serial.print(_DIST_MIN);
  Serial.print(",distance:");  Serial.print(distance);
  Serial.print(",Max:");       Serial.print(_DIST_MAX);
  Serial.println("");
  
  // do something here
  //delay(50); // Assume that it takes 50ms to do something.
  
  // update last sampling time
  last_sampling_time += INTERVAL;
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm

  // Pulse duration to distance conversion example (target distance = 17.3m)
  // - pulseIn(ECHO, HIGH, timeout) returns microseconds (음파의 왕복 시간)
  // - 편도 거리 = (pulseIn() / 1,000,000) * SND_VEL / 2 (미터 단위)
  //   mm 단위로 하려면 * 1,000이 필요 ==>  SCALE = 0.001 * 0.5 * SND_VEL
  //
  // - 예, pusseIn()이 100,000 이면 (= 0.1초, 왕복 거리 34.6m)
  //        = 100,000 micro*sec * 0.001 milli/micro * 0.5 * 346 meter/sec
  //        = 100,000 * 0.001 * 0.5 * 346
  //        = 17,300 mm  ==> 17.3m
}
