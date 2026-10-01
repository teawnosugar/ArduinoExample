// Arduino pin assignment
#define PIN_LED  9 //led pin 설정해줌
#define PIN_TRIG 12 //초음파 만드는 것을 trigger 하는 pin
#define PIN_ECHO 13 // 돌아온 감지 입력 pin

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25       // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100     // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300     // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL)     // coefficent to convert duration to distance

#define N 3 //sample N

//#define _EMA_ALPHA 0.5    // EMA weight of new sample (range: 0 to 1)
                          // Setting EMA to 1 effectively disables EMA filter.
// global variables
unsigned long last_sampling_time;   // unit: msec
//float dist_prev = _DIST_MAX;        // Distance last-measured
//float dist_ema=0;                     // EMA distance

float samples[N]; int q_head = 0;int q_count = 0;
//샘플 들어갈 실수배열, 다음샘플 넣을 위치(원형큐 head), 현재큐 샘플수 선언

void setup() {
  // initialize GPIO pins
  pinMode(PIN_LED,OUTPUT);
  pinMode(PIN_TRIG,OUTPUT);
  pinMode(PIN_ECHO,INPUT);
  digitalWrite(PIN_TRIG, LOW);
  // initialize serial port
  Serial.begin(57600);
}


void loop() {
  float dist_raw, dist_median; // raw와 meidan 담을 변수 선언.
   // wait until next sampling time. 
  // millis() returns the number of milliseconds since the program started. 
  // will overflow after 50 days.
  if (millis() < last_sampling_time + INTERVAL)
    return;
  // get a distance reading from the USS
  dist_raw = USS_measure(PIN_TRIG,PIN_ECHO);
  
  queue_push(dist_raw);            //최신 샘플 큐에 넣는 함수
  dist_median = queue_median();    //중위수 찾는 함수(큐가 전역이라 매개변수 X)


  Serial.print("Min:");   Serial.print(_DIST_MIN);
  Serial.print(",raw:"); Serial.print(min(dist_raw, _DIST_MAX + 100));
  Serial.print(",median:"); Serial.print(min(dist_median, _DIST_MAX + 100));
  Serial.print(",Max:");  Serial.print(_DIST_MAX);
  Serial.println("");


  // do something here
  if ((dist_raw < _DIST_MIN) || (dist_raw > _DIST_MAX))
    digitalWrite(PIN_LED, 1);       // LED OFF
  else
    digitalWrite(PIN_LED, 0);       // LED ON
  // update last  sampling time
  last_sampling_time += INTERVAL;
}



// 원형 큐에 새 샘플 추가 (머리가 다음으로 가도 빙글빙글 돌도록 원형 큐로 구현)
void queue_push(float value) //value에 raw 받기
{ samples[q_head] = value;
  q_head = (q_head + 1) % N;   // 인덱스가 N되면 0으로 돌아가도록
  if (q_count < N) //작으면 증가해서 다음 인덱스를 가리키도록
    q_count++;
}

// 큐에 저장된 샘플들의 중위수 반환
float queue_median()   //큐에서 중위수 찾기
{
  float sorted[N]; //복사본용 
  // 원본 큐는 보존하고 복사본을 정렬 (삽입 정렬)
  // 왜냐하면 계속 새 raw값을 받아 저장하는 큐를 유지해야 하기 때문임.
  for (int i = 0; i < q_count; i++) {
    float key = samples[i];
    int j = i - 1;
    while (j >= 0 && sorted[j] > key) {
      sorted[j + 1] = sorted[j];
      j--;
    }
    sorted[j + 1] = key;
  }

  if (q_count % 2 == 1) 
    return sorted[q_count / 2];  // 홀수: 가운데 값
  else
    return (sorted[q_count / 2 - 1] + sorted[q_count / 2]) / 2.0; // 짝수: 가운데 두 값의 평균
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
