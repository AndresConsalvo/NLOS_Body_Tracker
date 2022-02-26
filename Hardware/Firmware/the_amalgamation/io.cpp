#include <Arduino.h>
#include "io.h"
#include "wifi_connect.h"
#define TIMER_INTERVAL_MS       250

int STATUS_LED_PIN = 2;

byte led_cycle[27] = {HIGH, LOW, HIGH, LOW, HIGH, LOW, LOW, LOW, HIGH, HIGH, LOW, HIGH, HIGH, LOW, HIGH, HIGH, LOW, LOW, LOW, HIGH, LOW, HIGH, LOW, HIGH, LOW, LOW, LOW};
byte state = LOW;
int count = 0;
void TimerHandler(){ 
  if(getWifiStatus()){
    count += 1;
    if(count >= 3){
      state = !state;
      count = 0;
    }
    digitalWrite(STATUS_LED_PIN, state);
  }
  else{
    digitalWrite(STATUS_LED_PIN, led_cycle[count]);
    count = (count >= 26) ? 0 : count + 1;
  }
}

hw_timer_t * timer = NULL;
void timer_init(){
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &TimerHandler, true);
  timerAlarmWrite(timer, 1000 * TIMER_INTERVAL_MS, true);
  timerAlarmEnable(timer);
}

void io_init(){
    pinMode(STATUS_LED_PIN, OUTPUT);
    timer_init();
}
