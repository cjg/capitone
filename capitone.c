#include <prg32.h>

// SCALE sets how many pixels to use for each element of the body of the snake
#define SCALE 4

#define WALL_COLOR PRG32_COLOR_GREEN
#define BODY_COLOR PRG32_COLOR_RED

#define ARENA_WIDTH  ((PRG32_GAME_W - 4 * SCALE) / SCALE)
#define ARENA_HEIGHT ((PRG32_GAME_H - 4 * SCALE) / SCALE)

#define MAXIMUM_CAPITONE_LENGTH ARENA_WIDTH * ARENA_HEIGHT

uint16_t arena[ARENA_WIDTH * ARENA_HEIGHT];
uint8_t head_x;
uint8_t head_y;
uint8_t tail_x;
uint8_t tail_y;

#define arena_set(x, y, v) arena[(y) * ARENA_WIDTH + (x)] = (v)
#define arena_get(x, y) arena[(y) * ARENA_WIDTH + (x)]
#define arean_is_body(x, y) (arena_get(x, y) > 0 && arena_get(x, y) <= MAXIMUM_CAPITONE_LENGTH)

static void line(int tl_x, int tl_y, int br_x, int br_y, int color);
static void draw_arena(void);

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
    arena_set(head_x, head_y, 3);
    arena_set(head_x - 1, head_y, 2);
    arena_set(head_x - 2, head_y, 1);
    tail_x = head_x - 2;
    tail_y = head_y;
}

void capitone_update(void) {
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