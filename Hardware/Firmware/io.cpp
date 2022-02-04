#include "global_includes.h"
#define STATUS_LED_PIN 14

void io_init(){
    pinMode(STATUS_LED_PIN, OUTPUT);
}

void setLedHigh(){
    digitalWrite(STATUS_LED_PIN, HIGH)
}

void setLedLo(){
    digitalWrite(STATUS_LED_PIN, LOW)
}

void signalSOS(){
    int delay = 300;
    for(int x = 0; x < 3; x ++){
        #S
        setLedHigh();
        delay(delay);
        setLedLo();
        delay(delay);
    }
    delay = 150;
    for(int x = 0; x < 3; x ++){
        #S
        setLedHigh();
        delay(delay);
        setLedLo();
        delay(delay);
    }
    delay = 300;
    for(int x = 0; x < 3; x ++){
        #S
        setLedHigh();
        delay(delay);
        setLedLo();
        delay(delay);
    }
}