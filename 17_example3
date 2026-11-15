#include <Servo.h>

// Arduino pin assignment

#define PIN_IR    0         // IR sensor at Pin A0
#define PIN_LED   9
#define PIN_SERVO 10

#define _DUTY_MIN 540   
#define _DUTY_NEU 1470  
#define _DUTY_MAX 2400 

#define _DIST_MIN  100.0   // minimum  100
#define _DIST_MAX  250.0   // maximum 250

#define EMA_ALPHA  20      // ema용

#define LOOP_INTERVAL 20  // Loop Interval (unit: msec)

Servo myservo;
unsigned long last_loop_time;   // unit: msec

float dist_prev = _DIST_MIN;
float dist_ema = _DIST_MIN;

void setup()
{
  pinMode(PIN_LED, OUTPUT);
  
  myservo.attach(PIN_SERVO); 
  myservo.writeMicroseconds(_DUTY_NEU);
  
  Serial.begin(1000000);    // 1,000,000 bps
}

void loop()
{
  unsigned long time_curr = millis();
  int duty;
  float a_value, dist_raw;

  // wait until next event time
  if (time_curr < (last_loop_time + LOOP_INTERVAL))
    return;
  last_loop_time += LOOP_INTERVAL;

  a_value = analogRead(PIN_IR);
  dist_raw = ((6762.0 / (a_value - 9.0)) - 4.0) * 10.0 - 60.0;

  // 1. Range Filter & LED 제어
  if ((dist_raw >= _DIST_MIN) && (dist_raw <= _DIST_MAX)) {
    digitalWrite(PIN_LED, HIGH);  // 범위 내일 때 LED 켜기
    dist_prev = dist_raw;         // 정상 값이면 이전 값 to로
  } else {
    digitalWrite(PIN_LED, LOW);   // 범위를 벗어나면 LED 끄기
    dist_raw = dist_prev;         // 범위
  }

  // 2. EMA Filter
  dist_ema = ((float)EMA_ALPHA * dist_raw + (100.0 - (float)EMA_ALPHA) * dist_ema) / 100.0;

      //map말고
  duty = (int)(_DUTY_MIN + (dist_ema - _DIST_MIN) * (_DUTY_MAX - _DUTY_MIN) / (_DIST_MAX - _DIST_MIN));
  
  myservo.writeMicroseconds(duty);

  Serial.print("_DUTY_MIN:");  Serial.print(_DUTY_MIN);
  Serial.print("_DIST_MIN:");  Serial.print(_DIST_MIN);
  Serial.print(",IR:");        Serial.print(a_value);
  Serial.print(",dist_raw:");  Serial.print(dist_raw);
  Serial.print(",ema:");       Serial.print(dist_ema);
  Serial.print(",servo:");     Serial.print(duty);
  Serial.print(",_DIST_MAX:"); Serial.print(_DIST_MAX);
  Serial.print(",_DUTY_MAX:"); Serial.print(_DUTY_MAX);
  Serial.println("");
}
