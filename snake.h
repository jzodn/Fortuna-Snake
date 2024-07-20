#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "lcd.h"

#define UP 10
#define DOWN 11
#define LEFT 12
#define RIGHT 13

#define CENTRE_BUTTON PE7
#define UP_BUTTON PC2
#define DOWN_BUTTON PC4
#define LEFT_BUTTON PC5
#define RIGHT_BUTTON PC3

uint8_t height;
uint8_t width;
uint16_t score;
uint8_t fruitx, fruity;
uint8_t length;

struct coord {
    uint8_t x;
    uint8_t y;
    uint8_t direction;
};

typedef struct coord coords;

coords head, body[25];

void init();
void setup();
void demo();
void gen_fruit();
void move();
void move_up();
void move_down();
void move_left();
void move_right();
void check_fruit();
uint8_t check_if_lost();
void move_body();