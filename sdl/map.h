#ifndef MAP_H
#define MAP_H

#include <SDL2/SDL.h>
#include "animate.h"
#include "screen-manager.h"

typedef struct map_s {
    SDL_Texture **tiles;
    uint32_t num_tiles;
    uint32_t gff_file;
    uint32_t map_id;
    SDL_Texture **objs;
    SDL_Rect *obj_locs;
    animate_t **anims;
    uint16_t *flags;
    uint32_t num_objs;
} map_t;

void map_init(map_t *map);
void map_free(map_t *map);
void map_load_region(map_t *map, SDL_Renderer *renderer, int id);
void map_render(void *data, SDL_Renderer *renderer);
int map_handle_mouse(const uint32_t x, const uint32_t y);
int map_handle_mouse_click(const uint32_t x, const uint32_t y);

int cmap_is_block(const int row, const int column);
int cmap_is_actor(const int row, const int column);
int cmap_is_danger(const int row, const int column);

// From main.c
uint32_t getCameraX();
uint32_t getCameraY();

extern sops_t map_screen;

#endif
