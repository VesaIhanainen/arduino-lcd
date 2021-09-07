#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

#define MS_DELAY 300

int main(void){
    struct Pins pins;
    pins.reset = 12;
    pins.enable = 11;
    pins.data_pins[0] = 5;
    pins.data_pins[1] = 4;
    pins.data_pins[2] = 3;
    pins.data_pins[3] = 2;
    struct Lcd lcd;
    LCDInit(&pins,&lcd);
    const char Buffer[5] = "Hello";
    while(1) {
        for (int i = 0; i < 5; i++)
        {
            write(&pins, Buffer[i]);
        }
        _delay_ms(500);
    }    
}