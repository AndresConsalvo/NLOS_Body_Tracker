#include <Arduino.h>
#include "io.h"


int STATUS_LED_PIN = 14;



void io_init(){
    pinMode(14, OUTPUT);
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
