/** 
 * Author: Jez O'Donnell
 * Licence: This work is licensed under the Creative Commons Attribution License.
 *           View this license at http://creativecommons.org/about/licenses/
 */

#include "snake.h"

int main() {
    init();
    setup();
    move();

    return 0;
}

/**
 *  Initialises lcd, variables, ports and random function
 */
void init() {
    init_lcd();
    height = 35;
    width = 40;

    /* Configure I/O Ports */
    /* Enable encoder inputs */
    DDRC &= ~_BV(UP_BUTTON) & ~_BV(DOWN_BUTTON) & ~_BV(LEFT_BUTTON) & ~_BV(RIGHT_BUTTON);
    PORTC |= _BV(UP_BUTTON) | _BV(DOWN_BUTTON) | _BV(LEFT_BUTTON) | _BV(RIGHT_BUTTON);

    DDRE &= ~_BV(CENTRE_BUTTON);
    PORTE |= _BV(CENTRE_BUTTON);

    srand(time(NULL));
}

/**
 *  Sets up the display for the game
 */
void setup() {
    score = 0;

    display_color(WHITE, BLACK);

    // Creates border
    uint8_t i, j;
    for(i = 0; i < width; i++) {
        for(j = 0; j < height; j++) {
            if (i == 0 || i == width - 1 || j == 0 || j == height - 1) {
                display_string_xy("#", i*8, j*6);
            } else {
                display_string_xy(" ",i*8, j*6);
            }
        }
        display_char('\n');   
    }

    //Setup the snake
    length = 4;
    head.x = 13;
    head.y = 20;
    head.direction = RIGHT;

    body[0] = head;

    display_color(CYAN, BLACK);

    display_string_xy("<", body[0].x*6, body[0].y*8);

    for (i=1; i<length; i++) {
        coords section;
        section.x = body[0].x - i;
        section.y = body[0].y;
        section.direction = RIGHT;
        body[i] = section;
        display_string_xy("*", body[i].x*6, body[i].y*8);
    }


    // Waits for player to start
    display_string_xy("PRESS CENTRE BUTTON TO START", 70, 110);
    uint8_t flag = 1;
    while(flag) {
        if (!(PINE & _BV(CENTRE_BUTTON))) {
            flag = 0;
        }
    }
    display_string_xy("                            ", 70, 110);

    gen_fruit();
}

/**
 *  Randomly generates fruit within the border
 */
void gen_fruit() {
    fruitx = (rand() % (51)) + 1;
    fruity = (rand() % (24)) + 1;

    uint8_t i;
    uint8_t flag = 1;
    while (flag) {
        flag = 0;
        for (i=0; i<length; i++) {
            if (fruitx == body[i].x && fruity == body[i].y) {
                    flag = 1;
                    fruitx = (rand() % (51)) + 1;
                    fruity = (rand() % (24)) + 1;
            }
        }
    }


    display_color(CRIMSON, BLACK);
    display_string_xy("*", fruitx*6, fruity*8);
}

/**
 *  Checks users input to see what direction they want to go
 *   Checks if the player has gotten a fruit or lost the game
 */
void move() {
    uint8_t flag = 0;

    do {
        check_fruit();

        display_color(GOLD, BLACK);

        char buffer[12];
        sprintf(buffer, "SCORE: %03d", score);
        display_string_xy(buffer, 40, 222);

        if (!(PINC & _BV(UP_BUTTON))) {
            body[0].direction = UP;
        } else if (!(PINC & _BV(DOWN_BUTTON))) {
            body[0].direction = DOWN;
        } else if (!(PINC & _BV(LEFT_BUTTON))) {
            body[0].direction = LEFT;
        } else if (!(PINC & _BV(RIGHT_BUTTON))) {
            body[0].direction = RIGHT;
        }

        if (body[0].direction == UP) {
            move_up();
        } else if (body[0].direction == DOWN) {
            move_down();
        } else if (body[0].direction == LEFT) {
            move_left();
        } else if (body[0].direction == RIGHT) {
            move_right();
        }

        flag = check_if_lost();

        while(flag) {
            if (!(PINE & _BV(CENTRE_BUTTON))) {
                clear_screen();
                init();
                setup();
                flag = 0;
            }
        }

        _delay_ms(20);
    } while (1);
}

/**
 *  Checks if the player has gone over a fruit
 */
void check_fruit() {
    if (body[0].x == fruitx && body[0].y == fruity) {
        score++;

        coords section;

        if (body[length-1].direction == UP) {
        section.x = body[length-1].x;
        section.y = body[length-1].y + 1;
        section.direction = UP;
        } else if (body[length-1].direction == DOWN) {
        section.x = body[length-1].x;
        section.y = body[length-1].y - 1;
        section.direction = DOWN;
        } else if (body[length-1].direction == LEFT) {
        section.x = body[length-1].x + 1;
        section.y = body[length-1].y;
        section.direction = LEFT;
        } else if (body[length-1].direction == RIGHT) {
        section.x = body[length-1].x - 1;
        section.y = body[length-1].y;
        section.direction = RIGHT;
        }
        body[length] = section;

        length++;

        gen_fruit();
    }
}

/**
 *  Checks if the player has lost the game
 *  @returns 1 if lost otherwise returns 0
 */
uint8_t check_if_lost() {
     int i,check=0;
    for (i=1; i<length; i++) {
        if (body[0].x==body[i].x && body[0].y==body[i].y)
        {
            check++;
        }
    }
    if (body[0].x <= 0|| body[0].x >= 52 || body[0].y >= 25 || body[0].y <= 0 || check != 0 )
    {
        display_color(RED, BLACK);
        display_string_xy("GAME OVER", 15, 25);
        display_string_xy("PRESS CENTRE BUTTON TO RESTART", 15, 55);
        return 1;
    }

    return 0;
}

/**
 *  Moves the player up by 1
 */
void move_up() {
    display_color(CYAN, BLACK);
    display_string_xy("*", body[0].x*6, body[0].y*8);
    body[0].y -= 1;
    body[0].direction = UP;
    display_string_xy("v", body[0].x*6, body[0].y*8);
    move_body();
}

/**
 *  Moves the player down by 1
 */
void move_down() {
    display_color(CYAN, BLACK);
    display_string_xy("*", body[0].x*6, body[0].y*8);
    body[0].y += 1;
    body[0].direction = DOWN;
    display_string_xy("^", body[0].x*6, body[0].y*8);
    move_body();
}

/**
 *  Moves the player left by 1
 */
void move_left() {
    display_color(CYAN, BLACK);
    display_string_xy("*", body[0].x*6, body[0].y*8);
    body[0].x -= 1;
    body[0].direction = LEFT;
    display_string_xy(">", body[0].x*6, body[0].y*8);
    move_body();
}

/**
 *  Moves the player right by 1
 */
void move_right() {
    display_color(CYAN, BLACK);
    display_string_xy("*", body[0].x*6, body[0].y*8);
    body[0].x += 1;
    body[0].direction = RIGHT;
    display_string_xy("<", body[0].x*6, body[0].y*8);
    move_body();
}

/**
 *  Moves the players body
 */
void move_body() {
    display_string_xy(" ", body[length-1].x*6, body[length-1].y*8);

    uint8_t i;
    for (i=length-1; i>0; i--) {


        if (body[i].direction == UP) {
            body[i].y -= 1;
            body[i].direction = body[i-1].direction;
        } else if (body[i].direction == DOWN) {
            body[i].y += 1;
            body[i].direction = body[i-1].direction;
        } else if (body[i].direction == LEFT) {
            body[i].x -= 1;
            body[i].direction = body[i-1].direction;
        } else if (body[i].direction == RIGHT) {
            body[i].x += 1;
            body[i].direction = body[i-1].direction;
        }
    }
}