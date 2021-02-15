#include <SDL2/SDL.h>
#include <stdio.h>
#include "map.h"
#include "player.h"
#include "lua.h"
#include "mouse.h"
#include "screen-manager.h"
#include "gameloop.h"
#include "screens/narrate.h"
#include "screens/screen-main.h"
#include "screens/inventory.h"
#include "../src/dsl.h"
#include "../src/replay.h"
#include "../src/ds-load-save.h"

void browse_loop(SDL_Surface*, SDL_Renderer *rend);
void screen_debug_init(SDL_Surface *sur, SDL_Renderer *rend, const char *arg);
void export_all_images(const char *filename);
void export_all_items(const char *base_path);

static uint32_t last_tick = 0;
static const uint32_t TICK_AMT = 1000 / TICKS_PER_SEC;// Not fully correct...
static SDL_Window *win = NULL;
static SDL_Surface *screen = NULL;
static SDL_Renderer *renderer = NULL;
static float zoom = 2.0;

static uint32_t xmappos, ymappos;
static int32_t xmapdiff, ymapdiff;

const uint32_t getCameraX() { return xmappos; }
const uint32_t getCameraY() { return ymappos; }

void handle_mouse_motion() {
    int x, y;

    SDL_GetMouseState(&x, &y);

    screen_handle_mouse(x, y);
}

void handle_mouse_down(uint32_t button) {
    int x, y;

    SDL_GetMouseState(&x, &y);

    screen_handle_mouse_down(button, x, y);
}

void handle_mouse_up(uint32_t button) {
    int x, y;

    SDL_GetMouseState(&x, &y);

    screen_handle_mouse_up(button, x, y);
}

void main_exit_game() {
    game_loop_signal(WAIT_FINAL, 0);
}

void handle_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        switch(event.type) {
            case SDL_QUIT:
                main_exit_game();
                break;
            case SDL_KEYUP:
                if (ui_lua_keyup(event.key.keysym.sym)) { break; }
                if (event.key.keysym.sym == SDLK_s) { player_unmove(PLAYER_LEFT); }
                if (event.key.keysym.sym == SDLK_e) { player_unmove(PLAYER_UP); }
                if (event.key.keysym.sym == SDLK_f) { player_unmove(PLAYER_RIGHT); }
                if (event.key.keysym.sym == SDLK_d) { player_unmove(PLAYER_DOWN); }
                if (event.key.keysym.sym == SDLK_UP) { ymapdiff = 0;}
                if (event.key.keysym.sym == SDLK_DOWN) { ymapdiff = 0;}
                if (event.key.keysym.sym == SDLK_LEFT) { xmapdiff = 0;}
                if (event.key.keysym.sym == SDLK_RIGHT) { xmapdiff = 0;}
                break;
                break;
            case SDL_KEYDOWN:
                if (ui_lua_keydown(event.key.keysym.sym)) { break; }
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    game_loop_signal(WAIT_FINAL, 0);
                }
                if (event.key.keysym.sym == SDLK_i) {
                    screen_toggle_screen(renderer, &inventory_screen, 0, 0);
                }
                if (event.key.keysym.sym == SDLK_s) { player_move(PLAYER_LEFT); }
                if (event.key.keysym.sym == SDLK_e) { player_move(PLAYER_UP); }
                if (event.key.keysym.sym == SDLK_f) { player_move(PLAYER_RIGHT); }
                if (event.key.keysym.sym == SDLK_d) { player_move(PLAYER_DOWN); }
                if (event.key.keysym.sym == SDLK_UP) { ymapdiff = -2;}
                if (event.key.keysym.sym == SDLK_DOWN) { ymapdiff = 2;}
                if (event.key.keysym.sym == SDLK_LEFT) { xmapdiff = -2;}
                if (event.key.keysym.sym == SDLK_RIGHT) { xmapdiff = 2;}
                break;
            case SDL_MOUSEMOTION:
                handle_mouse_motion();
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (game_loop_is_waiting_for(WAIT_NARRATE_CONTINUE)) {
                    game_loop_signal(WAIT_NARRATE_CONTINUE, 0);
                }
                handle_mouse_down(event.button.button);
                break;
            case SDL_MOUSEBUTTONUP:
                //if (game_loop_is_waiting_for(WAIT_NARRATE_CONTINUE)) {
                    //game_loop_signal(WAIT_NARRATE_CONTINUE, 0);
                //}
                handle_mouse_up(event.button.button);
                break;
        }
    }

    xmappos += xmapdiff;
    ymappos += ymapdiff;
    handle_mouse_motion();
    player_update();
}

void main_set_xscroll(int amt) { xmapdiff = amt; }
void main_set_yscroll(int amt) { ymapdiff = amt; }

void render() {
    screen_render(renderer, xmappos, ymappos);
}

// Simple timing for now...
void tick() {
    uint32_t cticks = SDL_GetTicks();
    uint32_t amt_to_wait = 0;
    last_tick += TICK_AMT;
    //debug("last_time = %u, cticks = %u\n", last_tick, cticks);
    if (last_tick < cticks) {
        debug("gameloop took too long, not sleeping...\n");
    } else {
        amt_to_wait = last_tick - cticks;
        //debug("going to wait %ums\n", amt_to_wait);
        SDL_Delay(amt_to_wait);
    }
}

void port_init(int argc, char *argv[]) {
}

void port_cleanup() {
}

static int sdl_init(const int what) {
    return SDL_Init(what);
}

static void init(int args, char *argv[]) {
    xmappos = 560;
    ymappos = 50;
    xmapdiff = ymapdiff = 0;

    if (sdl_init( SDL_INIT_VIDEO) ) {
        error( "Unable to init video!\n");
        exit(1);
    }
    win = SDL_CreateWindow( "Dark Sun: Shattered Lands", SDL_WINDOWPOS_UNDEFINED,
        //SDL_WINDOWPOS_UNDEFINED, 2*320, 2*200, SDL_WINDOW_SHOWN );
        SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN );
    if( win == NULL ) {
        error( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        exit(1);
    }

    //Get window surface
    screen = SDL_GetWindowSurface( win );

    renderer = SDL_CreateRenderer(win, -1, 0);

    last_tick = SDL_GetTicks();

    gameloop_init();

    screen_init(renderer);

    player_init();
    mouse_init(renderer);

    for (int i = 0; i < args; i++) {
        if (!strcmp(argv[i], "--lua") && i < (args - 1)) {
            if (ui_lua_load("main.lua") ) {
                printf("Init being handled by lua.\n");
                return;
            }
        }
        if (!strcmp(argv[i], "--browse") && i < (args)) {
            printf("Entering browsing mode!\n");
            browse_loop(screen, renderer);
            exit(1);
        }
        if (!strcmp(argv[i], "--screen") && i < (args - 1)) {
            printf("Entering screen mode!\n");
            screen_debug_init(screen, renderer, argv[i + 1]);
            return;
        }
        if (!strcmp(argv[i], "--extract-images") && i < (args - 1)) {
            export_all_images(argv[i + 1]);
            exit(0);
        }
        if (!strcmp(argv[i], "--extract-items") && i < (args - 1)) {
            export_all_items(argv[i + 1]);
            exit(0);
        }
    }

    // Start the main game.
    screen_push_screen(renderer, &main_screen, 0, 0);
}

static void cleanup() {
    ui_lua_close();
    // Order matters.
    player_close();
    screen_free();

    dsl_cleanup();
    gff_cleanup();
    mouse_free();

    SDL_DestroyRenderer(renderer);
    //SDL_DestroySurface(screen);
    SDL_DestroyWindow( win );

    //Quit SDL subsystems
    SDL_Quit();
}

static char *ds1_gffs = NULL;
static char *replay = NULL;

void parse_args(int argc, char *argv[]) {
    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "--ds1") && i < (argc-1)) {
            ds1_gffs = argv[++i];
        }
        if (!strcmp(argv[i], "--replay") && i < (argc-1)) {
            replay = argv[++i];
        }
    }

    if (!ds1_gffs) {
        error("Unable to get the location of the DarkSun 1 GFFs, please pass with '--ds1 <location>'\n");
        exit(1);
    }
    replay_init("replay.lua");
}

int main(int argc, char *argv[]) {
    parse_args(argc, argv);

    // Order matters.
    gff_init();
    gff_load_directory(ds1_gffs);
    dsl_init();

    init(argc, argv);

    if (replay) {
        replay_game(replay);
    }

    game_loop();

    cleanup();
    replay_cleanup();

    return 0;
}

static int done = 0;
static int accum = 0;
static uint8_t wait_flags[WAIT_MAX_SIGNALS];

typedef struct animation_s {
    struct animation_s *next;
} animation_t;

void gameloop_init() {
    memset(wait_flags, 0x0, sizeof(uint8_t) * WAIT_MAX_SIGNALS);
    wait_flags[WAIT_FINAL] = 1;
}

//static animation_t *animations[TICKS_PER_SEC];
int game_loop_is_waiting_for(int signal) {
    if (signal < 0 || signal >= WAIT_MAX_SIGNALS) {
        error("Received signal %d!\n", signal);
        return 0;
    }

    return wait_flags[signal];
}

void game_loop_signal(int signal, int _accum) {
    if (signal < 0 || signal >= WAIT_MAX_SIGNALS) {
        error("Received signal %d!\n", signal);
        return;
    }
    replay_print("rep.signal(%d, %d)\n", signal, _accum);
    if (wait_flags[signal]) {
        wait_flags[signal]--;
        accum = _accum;
        done = 1;
    } else {
        warn("signal %d received, but not waiting on it...\n", signal);
    }
}

int game_loop_wait_for_signal(int signal) {
    if (signal < 0 || signal >= WAIT_MAX_SIGNALS) {
        error("Received signal %d!\n", signal);
        return 0 ;
    }
    wait_flags[signal]++;
    while (wait_flags[signal]) {
        handle_input();
        //Logic here...
        render();
        tick();
    }
    done = 0;
    return accum;
}

void game_loop() {
    int rep_times = 0;

    while (!done) {
        handle_input();
        //Logic here...
        render();
        tick();
        rep_times++;
        if (in_replay_mode() && rep_times > 10) {
            replay_next();
            rep_times = 0;
        }
    }
}

SDL_Renderer *main_get_rend() { return renderer; }
const float main_get_zoom() { return zoom; }

void main_exit_system() {
    /*
    */
    game_loop_signal(WAIT_FINAL, 0);
}
