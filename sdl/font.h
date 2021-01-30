#ifndef FONT_H
#define FONT_H

#include <SDL2/SDL.h>
#include "screen-manager.h"

typedef uint16_t font_t;

enum {
    FONT_YELLOW,
    FONT_GREY,
    FONT_BLACK,
    FONT_RED,
    NUM_FONTS
};

void font_init(SDL_Renderer *renderer);
void print_line_len(SDL_Renderer *renderer, font_t font, const char *text, size_t x, size_t y, const uint32_t len);

#endif