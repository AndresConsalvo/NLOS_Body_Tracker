#include <Arduino.h>
#include "io.h"
#include "wifi_connect.h"
#define TIMER_INTERVAL_MS       250


int STATUS_LED_PIN = 2;
//#include "ESP8266TimerInterrupt.h"
//ESP8266Timer ITimer;

byte led_cycle[21] = {HIGH, LOW, HIGH, LOW, HIGH, LOW, HIGH, HIGH, LOW, HIGH, HIGH, LOW, HIGH, HIGH, LOW, HIGH, LOW, HIGH, LOW, HIGH, LOW};
byte state = LOW;
int count = 0;
void IRAM_ATTR TimerHandler(){ 
  /* Timer SOS Cycle
   * 0  Led on
   * 1  Led off
   * 2  Led on
   * 3  Led off
   * 4  Led on
   * 5  Led off
   * 6  Led on
   * 7  Led on
   * 8  Led off
   * 9  Led on
   * 10 Led on
   * 11 Led off
   * 12 Led on
   * 13 Led on
   * 14 Led off
   * 15 Led on
   * 16 Led off
   * 17 Led on
   * 18 Led off
   * 19 Led on
   * 20 Led off
   * */   
  if(getWifiStatus()){
    count += 1;
    if(count >= 4){
      state = !state;
      count = 0;
    }
    digitalWrite(STATUS_LED_PIN, state);
//    Serial.print("Connected: set Led to ");
//    Serial.println(state);
  }
  else{
    digitalWrite(STATUS_LED_PIN, led_cycle[count]);
    count = (count >= 20) ? 0 : count + 1;
//    Serial.print("Not connected: set Led to ");
//    Serial.println(led_cycle[count]);
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
//    ITimer.attachInterruptInterval(TIMER_INTERVAL_MS * 1000, TimerHandler);
}

void setLedHigh(){
    digitalWrite(STATUS_LED_PIN, HIGH);
}

void setLedLo(){
    digitalWrite(STATUS_LED_PIN, LOW);
}

void signalSOS(){
    int delay_ms = 500;
    delay(delay_ms);
    for(int x = 0; x < 3; x ++){
        //S
        setLedHigh();
        delay(delay_ms);
        setLedLo();
        delay(delay_ms);
    }
    delay_ms = 250;
    for(int x = 0; x < 3; x ++){
        //O
        setLedHigh();
        delay(delay_ms);
        setLedLo();
        delay(delay_ms);
    }
    delay_ms = 500;
    for(int x = 0; x < 3; x ++){
        //S
        setLedHigh();
        delay(delay_ms);
        setLedLo();
        delay(delay_ms);
    }
}

void signalConnected(){
    int delay_ms = 500;
    delay(delay_ms);
    for(int x = 0; x < 10; x ++){
        //S
        setLedHigh();
        delay(delay_ms);
        setLedLo();
        delay(delay_ms);
    }
}
