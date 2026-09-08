#include <prg32.h>

// SCALE sets how many pixels to use for each element of the body of the snake
#define SCALE 4

#define WALL_COLOR PRG32_COLOR_GREEN
#define BODY_COLOR PRG32_COLOR_RED
#define FOOD_COLOR PRG32_COLOR_CYAN

#define ARENA_WIDTH  ((PRG32_GAME_W - 4 * SCALE) / SCALE)
#define ARENA_HEIGHT ((PRG32_GAME_H - 4 * SCALE) / SCALE)

#define MAXIMUM_CAPITONE_LENGTH ARENA_WIDTH * ARENA_HEIGHT

#define UP    'U'
#define DOWN  'D'
#define LEFT  'L'
#define RIGHT 'R'

uint8_t body_xs[MAXIMUM_CAPITONE_LENGTH];
uint8_t body_ys[MAXIMUM_CAPITONE_LENGTH];
uint16_t body_length;

#define head_x() body_xs[body_length-1]
#define head_y() body_ys[body_length-1]

uint8_t direction;
uint8_t direction_change;
uint32_t last_move;
uint8_t food_x;
uint8_t food_y;

static void line(int tl_x, int tl_y, int br_x, int br_y, int color);
static void draw_arena(void);
static uint8_t move_capitone(uint8_t direction);
static const char *uitoa(uint32_t x);
static void add_food(void);
static uint8_t is_body();

void capitone_init(void) {
    prg32_console_write("Starting Capitone!\n");
    uint8_t head_x = ARENA_WIDTH / 2;
    uint8_t head_y = ARENA_HEIGHT / 2;
    body_length = 3;
    body_xs[2] = head_x;
    body_ys[2] = head_y;
    body_xs[1] = head_x - 1;
    body_ys[1] = head_y;
    body_xs[0] = head_x - 2;
    body_ys[0] = 0;
    direction = RIGHT;
    food_x = ARENA_WIDTH;
    food_y = ARENA_HEIGHT;
    direction_change = 0;
    last_move = prg32_ticks_ms();
}

void capitone_update(void) {
    uint32_t now = prg32_ticks_ms();

    uint32_t current_input = prg32_input_read();

    if (current_input & PRG32_BTN_UP) {
        direction_change = UP;
    } else if (current_input & PRG32_BTN_DOWN) {
        direction_change = DOWN;
    } else if (current_input & PRG32_BTN_LEFT) {
        direction_change = LEFT;
    } else if (current_input & PRG32_BTN_RIGHT) {
        direction_change = RIGHT;
    }

    if (food_x >= ARENA_WIDTH || food_y >= ARENA_HEIGHT) {
        add_food();
    }

    if (now - last_move < 200) {
        prg32_console_write("<<< UPDATE\n");
        return;
    }

    // up and down are valid change of direction only when moving to left or to right
    // same idea for left and right        
    if (direction == LEFT || direction == RIGHT) {
        if (direction_change == UP || direction_change == DOWN) {
            direction = direction_change;
        }
    } else {
        if (direction_change == LEFT || direction_change == RIGHT) {
            direction = direction_change;
        }
    }
    direction_change = 0;

    move_capitone(direction);
    last_move = now;
}

void capitone_draw(void) {
    prg32_gfx_clear(PRG32_COLOR_BLACK);
    
    int wall_tl_x = SCALE;
    int wall_tl_y = SCALE;
    int wall_br_x = PRG32_GAME_W - SCALE * 2;
    int wall_br_y = PRG32_GAME_H - SCALE * 2;

    // draw the wall
    line(wall_tl_x, wall_tl_y, wall_br_x, wall_tl_y, WALL_COLOR);
    line(wall_br_x, wall_tl_y, wall_br_x, wall_br_y, WALL_COLOR+1);
    line(wall_br_x, wall_br_y, wall_tl_x, wall_br_y, WALL_COLOR+2);
    line(wall_tl_x, wall_br_y, wall_tl_x, wall_tl_y, WALL_COLOR+3);

    draw_arena();

    if (food_x < ARENA_WIDTH && food_y < ARENA_HEIGHT) {
        prg32_gfx_rect(food_x * SCALE, food_y * SCALE, SCALE, SCALE, FOOD_COLOR);
    }
}

static void line(int start_x, int start_y, int end_x, int end_y, int color) {
    if (start_y == end_y) {
        // horizontal line
        int x = start_x;
        int width = end_x - start_x;
        if (x > end_x) {
            x = end_x;
            width = start_x - end_x;
        }
        if (width < 0) {
            width = 1;
        }
        prg32_gfx_rect(x, start_y, width, SCALE, color);
    } else if (start_x == end_x) {
        // vertical line
        int y = start_y;
        int height = end_y - start_y;
        if (y > end_y) {
            y = end_y;
            height = start_y - end_y;
        }
        if (height < 0) {
            height = 1;
        }
        prg32_gfx_rect(start_x, y, SCALE, height, color);
    } else {
    }
}

static uint8_t move_capitone(uint8_t direction) {
    int previous_x = head_x();
    int previous_y = head_y();

    // first we move the head to the next location, we return 1 on collision
    switch (direction)
    {
    case UP:
        if (head_y() == 0) {
            return 1;
        }
        head_y() -= 1;
        break;
    
    case DOWN:
        if (head_y() + 1 == ARENA_HEIGHT) {
            return 1;
        } 
        head_y() += 1;
        break;

    case LEFT:
        if (head_x() == 0) {
            return 1;
        }
        head_x() -= 1;
        break;
    
    case RIGHT:
        if (head_x() + 1 == ARENA_WIDTH) {
            return 1;
        } 
        head_x() += 1;
        break;
    }

    int i;
    uint8_t current_x;
    uint8_t current_y;
    for (i = body_length - 2; i >= 0; i--) {
        current_x = body_xs[i];
        current_y = body_ys[i];
        body_xs[i] = previous_x;
        body_ys[i] = previous_y;
        previous_x = current_x;
        previous_y = current_y;
    }
}

static const char *uitoa(uint32_t x) {
    static char s[12];

    if (x == 0) {
        s[0] = '0';
        s[1] = 0;
        return s;
    }

    int i = 0;
    while (x != 0) {
        int rem = x % 10;
        s[i++] = rem + '0';
        x = x / 10;
    }
    
    s[i] = 0;
    
    /* Reverse the string */
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = s[start];
        s[start] = s[end];
        s[end] = temp;
        start++;
        end--;
    }

    return s;
}

static void add_food(void) {
    do {
        food_x = prg32_random_number(0, ARENA_WIDTH-1);
        food_y = prg32_random_number(0, ARENA_HEIGHT-1);
        prg32_console_write("added food ");
        prg32_console_write(uitoa(food_x));
        prg32_console_putc(' ');
        prg32_console_write(uitoa(food_y));
        prg32_console_putc('\n');
    } while (is_body(food_x, food_y));   
}

static uint8_t is_body(uint8_t x, uint8_t y) {
    int i;
    for (i = 0 < body_length; i++) {
        if (body_xs[i] == x && body_ys[i] == y) {
            return 1;
        }
    }
    return 0;
}