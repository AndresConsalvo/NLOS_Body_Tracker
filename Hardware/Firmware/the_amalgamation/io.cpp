#include <Arduino.h>
#include "io.h"

#define TIMER_INTERVAL_MS       1000


int STATUS_LED_PIN = 5;
//#include "ESP8266TimerInterrupt.h"
//ESP8266Timer ITimer;
//volatile byte state = LOW;
//
//void IRAM_ATTR TimerHandler(){
//  state = !state;
//  Serial.print("Interrupt called - ");
//  Serial.println(state);  
//    digitalWrite(STATUS_LED_PIN, state);
//}

void io_init(){
    pinMode(STATUS_LED_PIN, OUTPUT);
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
