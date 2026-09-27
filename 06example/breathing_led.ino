#define PIN_LED 9 //사용할 아두이노 핀 번호를 9번으로 정의한다.
int freqStep = 0; //duty의 1퍼센트 당 차지하는 시간을 저장할 변수
int freqdelayOn = 0; // led가 켜져있을 시간
int freqdelayOff = 0; // led가 꺼져있을 시간
int nowperiod = 0; //현재 period

const unsigned long BREATH_PERIOD_US = 1000000UL; // 1초  1000000UL / 2초  2000000UL
unsigned long halfPeriod = BREATH_PERIOD_US / 2; //미리 저장해놓기 - 반주기마다 밝기가 증/감으로 바뀐다
//어차피 양수인데 숫자가 너무 커서 unsigned long으로 선언 
unsigned long elapsedUs = 0; // 누적시간을 저장할 곳

void set_period(int period){//period를 정하는 함수
  nowperiod = period; //매개변수로 받은 period를 현재 period로 결정한다
  freqdelayOff = period; //첫번째 꺼져있을 시간의 길이를 period의 길이(전체 freq)으로 지정한다
  freqStep = period / 100; //0%~100%, 1%=freqstep 1개인 것
}

void set_duty(int duty){ 
  // duty를 받아(몇퍼센트인지) 켜져있을시간freqdelayOn/꺼져있을 시간freqdelayOff를 계산한다
  // duty는 목표 밝기라고 할 때
  // digitalWrite() & delayMicroseconds()를 활용해 구현한다(아래처럼)
  // ------ o----- oo---- ooo--- oooo-- ooooo- oooooo ooooo- oooo-- ooo-- oo---- o----- ------ <this 
  // period 기준으로 duty를 계산하기 때문에 
  // period 초기화를 이 함수 실행하기 전에 먼저 지정해주어야 함
  freqdelayOn = duty * freqStep; //켜져있을 시간은 밝기퍼센트 * 단계(의 크기?step의 크기)
  freqdelayOff = nowperiod - freqdelayOn;//꺼져있을 시간은 전체-켜져있을 시간
}


void setup() { 
  // put your setup code here, to run once:
  pinMode(PIN_LED, OUTPUT); //핀 지정
  Serial.begin(115200);
  set_period(1000); //period 설정하는 곳 , 1000 = 1ms / 10000 = 10ms / 100=0.1ms
} 

void loop() {
  //이번 루프 한 번(대략 PWM 한 주기, nowperiod us)만큼시간지났다고 보기
  //        1초(BREATH_PERIOD_US)를 넘기면 다시 0부터 출발한다. 
  elapsedUs += nowperiod; //1루프 지날때마다 누적 시간을 nowperiod만큼 증가 
  if (elapsedUs >= BREATH_PERIOD_US) { //누적시간이 전체 breathing 주기 도달이라면
    elapsedUs -= BREATH_PERIOD_US; //다시 0초~근처 부터 누적한다
  }

  int duty;//현재 시점에 적용할 밝기비(0~100)저장할 변수
  if (elapsedUs < halfPeriod) {
    duty = (int)((elapsedUs * 100UL) / halfPeriod);// elapsedUs가 0일 때 0 halfPeriod일 때 100 
  } else {
    duty = (int)(((BREATH_PERIOD_US - elapsedUs) * 100UL) / halfPeriod); // 뒤 0.5초: 100% -> 0%
  }
  set_duty(duty);


  digitalWrite(PIN_LED, HIGH);
  delayMicroseconds(freqdelayOn);
  digitalWrite(PIN_LED, LOW);
  delayMicroseconds(freqdelayOff); 
}
