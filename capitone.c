#include <prg32.h>

// SCALE sets how many pixels to use for each element of the body of the snake
#define SCALE 4

#define WALL_COLOR PRG32_COLOR_GREEN
#define BODY_COLOR PRG32_COLOR_RED

#define ARENA_WIDTH  ((PRG32_GAME_W - 4 * SCALE) / SCALE)
#define ARENA_HEIGHT ((PRG32_GAME_H - 4 * SCALE) / SCALE)

#define MAXIMUM_CAPITONE_LENGTH ARENA_WIDTH * ARENA_HEIGHT

#define UP    1
#define DOWN  2
#define LEFT  3
#define RIGHT 4

uint16_t arena[ARENA_WIDTH * ARENA_HEIGHT];
uint8_t head_x;
uint8_t head_y;
uint16_t body_length;
uint8_t direction;
uint32_t last_move;

#define arena_set(x, y, v) arena[(y) * ARENA_WIDTH + (x)] = (v)
#define arena_get(x, y) arena[(y) * ARENA_WIDTH + (x)]
#define arean_is_body(x, y) (arena_get(x, y) > 0 && arena_get(x, y) <= MAXIMUM_CAPITONE_LENGTH)

static void line(int tl_x, int tl_y, int br_x, int br_y, int color);
static void draw_arena(void);
static uint8_t move_capitone(uint8_t direction);
static const char *uitoa(uint32_t x);

void capitone_init(void) {
    prg32_console_write("Starting Capitone!\n");
    head_x = ARENA_WIDTH / 2;
    head_y = ARENA_HEIGHT / 2;
    int x, y;
    for (y = 0; y < ARENA_HEIGHT; y++) {
        for (x = 0; x < ARENA_WIDTH; x++) {
            arena[y * ARENA_WIDTH + x] = 0;
        }
    }
    body_length = 3;
    arena_set(head_x, head_y, body_length);
    arena_set(head_x - 1, head_y, body_length - 1);
    arena_set(head_x - 2, head_y, body_length - 2);
    direction = RIGHT;
    last_move = prg32_ticks_ms();
}

void capitone_update(void) {
    uint32_t now = prg32_ticks_ms();

    uint32_t current_input = prg32_input_read();

    // up and down are valid change of direction only when moving to left or to right
    // same idea for left and right
    if (direction == LEFT || direction == RIGHT) {
        if (current_input & PRG32_BTN_UP) {
            direction = UP;
        } else if (current_input & PRG32_BTN_DOWN) {
            direction = DOWN;
        }
    } else {
        if (current_input & PRG32_BTN_LEFT) {
            direction = LEFT;
        } else if (current_input & PRG32_BTN_RIGHT) {
            direction = RIGHT;
        }
    }

    if (now - last_move < 200) {
        return;
    }
    prg32_console_write("Moving\n");
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

static void draw_arena(void) {
    // any number greater than 0 and smaller than MAXIMUM_CAPITONE_LENGTH is the body
    int x, y;
    for (y = 0; y < ARENA_HEIGHT; y++) {
        for (x = 0; x < ARENA_WIDTH; x++) {
            if (arean_is_body(x, y)) {
                prg32_gfx_rect(SCALE*2+x*SCALE, SCALE*2+y*SCALE, SCALE, SCALE, BODY_COLOR);
            }
        }
    }
}

static uint8_t move_capitone(uint8_t direction) {
    int x = head_x;
    int y = head_y;

    // first we move the head to the next location, we return 1 on collision
    switch (direction)
    {
    case UP:
        if (head_y == 0) {
            return 1;
        }
        head_y -= 1;
        break;
    
    case DOWN:
        if (head_y + 1 == ARENA_HEIGHT) {
            return 1;
        } 
        head_y += 1;
        break;

    case LEFT:
        if (head_x == 0) {
            return 1;
        }
        head_x -= 1;
        break;
    
    case RIGHT:
        if (head_x + 1 == ARENA_WIDTH) {
            return 1;
        } 
        head_x += 1;
        break;
    }

    arena_set(head_x, head_y, body_length);

    uint16_t next_body_piece = body_length - 1;

    prg32_console_write("head is at ");
    prg32_console_write(uitoa(x));
    prg32_console_putc(' ');
    prg32_console_write(uitoa(y));
    prg32_console_write("\n");

    for (;next_body_piece != 0;) {
        prg32_console_write("looking for body piece ");
        prg32_console_write(uitoa(next_body_piece));
        prg32_console_write(" around ");
        prg32_console_write(uitoa(x));
        prg32_console_putc(' ');
        prg32_console_write(uitoa(y));
        prg32_console_write("\n");
        // let's find the position of the next piece, if not found it means we reached the head
        int next_x = -1;
        int next_y = -1;

        // check top
        if (y - 1 >= 0 && arena_get(x, y - 1) == next_body_piece) {
            next_x = x;
            next_y = y - 1;
        } else {
            prg32_console_write("value on top ");
            prg32_console_write(uitoa(arena_get(x, y - 1)));
            prg32_console_write("\n");
        }

        // check bottom
        if (y + 1 < ARENA_HEIGHT && arena_get(x, y + 1) == next_body_piece) {
            next_x = x;
            next_y = y + 1;
        }

        // check right
        if (x + 1 < ARENA_WIDTH && arena_get(x + 1, y) == next_body_piece) {
            prg32_console_write("is on the right\n");
            next_x = x + 1;
            next_y = y;
        }

        // check left
        if (x - 1 >= 0 && arena_get(x - 1, y) == next_body_piece) {
            prg32_console_write("is on the left\n");
            next_x = x - 1;
            next_y = y;
        }

        // we found the next piece
        if (next_x >= 0) {
            arena_set(x, y, next_body_piece);
            x = next_x;
            y = next_y;
            next_body_piece = next_body_piece - 1;
            if (next_body_piece == 0) {
                arena_set(x, y, 0);
            }
        } else {
            prg32_console_write("body piece not found ");
            prg32_console_hex32((uint32_t) next_body_piece);
            prg32_console_putc('\n');
        }
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