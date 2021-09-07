#include "lcd.h"
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


#define MS_DELAY 3000
// uint8_t DDRD = 0x0;
// uint8_t DDRB = 0x1;
// uint8_t TCCR2A = 0x2;
// uint8_t TCCR0A = 0x3;
// uint8_t TCCR1A = 0x4;
// uint8_t COM2B1 = 0x5;
// uint8_t COM0B1 = 0x6;
// uint8_t COM0A1 = 0x7;
// uint8_t COM0A2 = 0x8;
// uint8_t COM1B1 = 0x9;
// uint8_t COM2A1 = 0x10;
// uint8_t COM1A1 = 0x11;
// uint8_t SREG = 0x12;
// uint8_t PORTD = 0x69;
// uint8_t PORTB = 0x29;

// #define _delay_ms(n) printf("%u\n",n)
// #define _delay_us(n) printf("%u\n",n)
// #define _BV(n) printf("%u\n",n)
// #define cli() printf("cli()\n")




void pinMode(uint8_t port, uint8_t mode){
    if(2 <= port && port <= 7){
        if(mode == 1){
            DDRD |= _BV(port);
        }
        else{
            DDRD &= ~_BV(port);
        }
    }
    if (8 <= port && port <= 13){
        if(mode == 1){
            DDRB |= _BV(port-8);
        }
        else{
            DDRB &= ~_BV(port-8);
        }
    }
}

void digitalWrite(uint8_t port, uint8_t value){
    if( port == 3)
    {
        TCCR2A &= ~_BV(COM2B1);
    }
    if( port == 5){
        TCCR0A &= ~_BV(COM0B1);
    }
    if ( port == 6){
        TCCR0A &= ~_BV(COM0A1);
    }
    if ( port == 9){
        TCCR1A &= ~_BV(COM1A1);
    }
    if ( port == 10){
        TCCR1A &= ~_BV(COM1B1);
    }
    if ( port == 11){
        TCCR2A &= ~_BV(COM2A1);
    }
    
    uint8_t oldSREG = SREG;
    cli();
    if(2 <= port && port <= 7){
        if(value == 1){
            PORTD |= _BV(port);
        }
        else{
            PORTD &= ~_BV(port);
        }
    }
    if (8 <= port && port <= 13){
        if(value == 1){
            PORTB |= _BV(port-8);
        }
        else{
            PORTB &= ~_BV(port-8);
        }
    }
    SREG = oldSREG;
}

void pulseEnable(struct Pins *pins){
    digitalWrite(pins->enable, 0);
    _delay_ms(1);
    digitalWrite(pins->enable, 1);
    _delay_ms(1);
    digitalWrite(pins->enable, 0);
    _delay_ms(100);
}

void writeBits(struct Pins *pins, uint8_t value){
    for (int i = 0; i < 4; i++)
    {
        digitalWrite(pins->data_pins[i],(value >> i ) & 0x01);
        //digitalWrite(pins->data_pins[i],(value & 1 << i ));
    }
    pulseEnable(pins);
    
}

void send(struct Pins *pins, uint8_t value, uint8_t mode){
    digitalWrite(pins->reset,mode);
    // digitalWrite(pins->read_write,0);
  
    writeBits(pins,value >> 4);
    writeBits(pins,value);

    // writeBits(pins,value);
    // writeBits(pins,value >> 4);
}

void command(struct Pins *pins, uint8_t value){
    send(pins,value,0);
}

uint8_t write(struct Pins *pins, uint8_t value){
    send(pins,value,1);
    return 1;
}

void display(struct Pins *pins, struct Lcd *lcd){
    lcd->_displaycontrol |= LCD_DISPLAYON;
    command(pins,LCD_DISPLAYCONTROL | lcd->_displaycontrol);
}

void clear(struct Pins *pins){
    command(pins,LCD_CLEARDISPLAY);
    _delay_ms(2000);
}

void LCDInit(struct Pins *pins, struct Lcd *lcd){
    lcd->_displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
    lcd->_row_offsets[0] = 0x00;
    lcd->_row_offsets[1] = 0x40;
    lcd->_row_offsets[2] = 0x00+16;
    lcd->_row_offsets[3] = 0x40+16;
    pinMode(pins->reset, 1);
    // pinMode(pins->read_write, 0);
    pinMode(pins->enable, 1);
    for (int i = 0; i < 4; i++)
    {
        pinMode(pins->data_pins[i],1);
    }

    // Initialize
    _delay_us(50000);
    digitalWrite(pins->reset, 0);
    digitalWrite(pins->enable, 0);
    // digitalWrite(pins->read_write, 0);
    // Set to 4-bit interface 
    //writeBits(pins,0x02);

    // Send function set command sequence
    command(pins,LCD_FUNCTIONSET | lcd->_displayfunction);
    _delay_ms(4500);  // wait more than 4.1 ms

    // second try
    command(pins,LCD_FUNCTIONSET | lcd->_displayfunction);
    _delay_ms(150);

    // third go
    command(pins,LCD_FUNCTIONSET | lcd->_displayfunction);

    command(pins, LCD_FUNCTIONSET | lcd->_displayfunction);

    lcd->_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    display(pins,lcd);
    
    clear(pins);

    lcd->_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    command(pins, LCD_ENTRYMODESET | lcd->_displaymode); 
}

