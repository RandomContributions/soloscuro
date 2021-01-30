#include "new-character.h"
#include "../main.h"
#include "../../src/gff.h"
#include "../../src/gfftypes.h"
#include "narrate.h"
#include "popup.h"
#include "../font.h"
#include "../sprite.h"
#include "../../src/spells.h"
#include <time.h>

static uint16_t background;
static uint16_t parchment[5];
static uint16_t done;
static uint16_t classes[8];
static uint16_t class_sel[8];
static uint16_t psionics[3];
static uint16_t spheres[4];
static uint16_t ps_sel[4]; // seletion for psionics/spheres.
static uint16_t die[11];
static uint16_t races[14];
static uint16_t spr;// sprite of the character on screen
static uint16_t psionic_label; // 2047
static uint16_t sphere_label; // 2046

static uint8_t show_psionic_label = 1;
static int8_t sphere_selection = -1;

enum {
    A_END = 0x0, // End of Allowed Classes
    A_C = 0x1, // Allowed Cleric
    A_D = 0x2, // Allowed Druid
    A_F = 0x4, // Allowed Fighter
    A_G = 0x8, // Allowed Gladiator
    A_P = 0x10, // Allowed Preserver
    A_S = 0x20, // Allowed pSionicist
    A_R = 0x40, // Allowed Ranger
    A_T = 0x80, // Allowed Thief
};

const uint16_t human_classes[] = {
    A_C, A_D, A_F, A_G, A_P, A_S, A_R, A_T, A_END
};
const uint16_t dwarf_classes[] = {
    A_C, A_F, A_G, A_S, A_T, A_C | A_F, A_C | A_S, A_F | A_S, A_F | A_T, A_S | A_T, 0x0
};
const uint16_t elf_classes[] = {
    A_C, A_F, A_G, A_P, A_S, A_R, A_T, 
    A_C | A_F, A_C | A_P, A_C | A_S, A_C | A_R, A_S | A_T, A_C | A_T,
    A_C | A_F | A_P, A_C | A_F | A_S, A_C | A_F | A_T,
    A_C | A_P | A_S, A_C | A_P | A_R, A_C | A_P | A_T,
    A_C | A_S | A_R, A_C | A_S | A_T,
    A_C | A_R | A_T,
    A_F | A_P, A_F | A_S, A_F | A_T,
    A_F | A_P | A_S, A_F | A_P | A_T,
    A_F | A_S | A_T,
    A_P | A_S, A_P | A_R, A_P | A_T,
    A_P | A_S | A_R, A_P | A_S | A_T,
    A_S | A_R, A_S | A_T,
    A_S | A_R | A_T,
    A_R | A_T,
    A_END
};
const uint16_t half_elf_classes[] = {
    A_C, A_D, A_F, A_G, A_P, A_S, A_R, A_T, 
    A_C | A_F, A_C | A_P, A_C | A_S, A_C | A_R, A_S | A_T, A_C | A_T,
    A_C | A_F | A_P, A_C | A_F | A_S, A_C | A_F | A_T,
    A_C | A_P | A_S, A_C | A_P | A_R, A_C | A_P | A_T,
    A_C | A_S | A_R, A_C | A_S | A_T,
    A_C | A_R | A_T,
    A_D | A_F, A_D | A_P, A_D | A_S, A_D | A_T,
    A_D | A_F | A_P, A_D | A_F | A_S, A_D | A_F | A_T,
    A_D | A_P | A_S, A_D | A_P | A_T,
    A_D | A_S | A_T,
    A_F | A_P, A_F | A_S, A_F | A_T,
    A_F | A_P | A_S, A_F | A_P | A_T,
    A_F | A_S | A_T,
    A_P | A_S, A_P | A_R, A_P | A_T, A_P | A_R,
    A_P | A_S | A_R, A_P | A_S | A_T, A_P | A_S | A_R, A_P | A_R | A_T,
    A_S | A_R, A_S | A_T, A_S | A_R,
    A_S | A_R | A_T,
    A_R | A_T,
    A_END
};
const uint16_t half_giant_classes[] = {
    A_C, A_F, A_G, A_S, A_R,
    A_C | A_F, A_C | A_S, A_C | A_R,
    A_F | A_S,
    A_S | A_R,
    A_END
};

const uint16_t halfling_classes[] = {
    A_C, A_D, A_F, A_G, A_S, A_R, A_T, 
    A_C | A_F, A_C | A_S, A_C | A_R, A_C | A_T,
    A_D | A_F, A_D | A_S, A_D | A_R, A_D | A_T,
    A_F | A_S, A_F | A_T,
    A_F | A_S | A_T,
    A_S | A_R, A_S | A_T,
    A_S | A_R | A_T,
    A_R | A_T,
    A_END
};
const uint16_t mul_classes[] = {
    A_C, A_D, A_F, A_G, A_S, A_R, A_T, 
    A_C | A_F, A_C | A_S, A_C | A_T,
    A_C | A_F | A_T,
    A_D | A_F, A_D | A_S, A_D | A_T,
    A_D | A_F | A_T,
    A_F | A_S, A_F | A_T,
    A_F | A_S | A_T,
    A_S | A_T,
    A_END
};
const uint16_t trikeen_classes[] = {
    A_C, A_D, A_F, A_G, A_S, A_R,
    A_C | A_F, A_C | A_S, A_C | A_R,
    A_C | A_F | A_S,
    A_C | A_S | A_R,
    A_D | A_F, A_D | A_S,
    A_D | A_F | A_S,
    A_F | A_S,
    A_S | A_R,
    A_END
};

static int die_pos = 0;
static int die_countdown = 0;

static int offsetx, offsety;
static float zoom;
static SDL_Renderer *renderer;
static ds_character_t pc; // the character we are creating.
static psin_t psi; // psi group
char sphere_text[32];

static void update_ui();

static int convert_to_actual_class(const uint8_t class) {
    switch (class) {
        case 0: return REAL_CLASS_AIR_CLERIC;
        case 1: return REAL_CLASS_AIR_DRUID;
        case 2: return REAL_CLASS_FIGHTER;
        case 3: return REAL_CLASS_GLADIATOR;
        case 4: return REAL_CLASS_PRESERVER;
        case 5: return REAL_CLASS_PSIONICIST;
        case 6: return REAL_CLASS_AIR_RANGER;
        case 7: return REAL_CLASS_THIEF;
    }

    return 0; // UNKNOWN CLASS
}

static int convert_to_class_sel(const uint8_t class) {
    switch (class) {
        case REAL_CLASS_AIR_CLERIC:
        case REAL_CLASS_EARTH_CLERIC:
        case REAL_CLASS_FIRE_CLERIC:
        case REAL_CLASS_WATER_CLERIC:
            return 0;
        case REAL_CLASS_AIR_DRUID:
        case REAL_CLASS_EARTH_DRUID:
        case REAL_CLASS_FIRE_DRUID:
        case REAL_CLASS_WATER_DRUID:
            return 1;
        case REAL_CLASS_FIGHTER: return 2;
        case REAL_CLASS_GLADIATOR: return 3;
        case REAL_CLASS_PRESERVER: return 4;
        case REAL_CLASS_PSIONICIST: return 5;
        case REAL_CLASS_AIR_RANGER: return 6;
        case REAL_CLASS_THIEF: return 7;
    }

    return 0; // UNKNOWN CLASS
}

static int is_class_allowed(const uint8_t race, const int8_t classes[3]) {
    uint16_t class = 1 << convert_to_class_sel(classes[0]);
    const uint16_t *allowed = NULL;
    if (classes[0] == -1) { return 0; }

    //printf("race = %d (%d, %d, %d)\n", race, classes[0], classes[1], classes[2]);
    if (classes[1] != -1) {
        // NO DUPS.
        if (classes[0] == classes[1] || classes[1] == classes[2]
            || classes[0] == classes[2]) { return 0; }
        class |= 1 << convert_to_class_sel(classes[1]);
        if (classes[2] != -1) {
            class |= 1 << convert_to_class_sel(classes[2]);
        }
    }

    switch(race) {
        case RACE_HUMAN: allowed = human_classes; break;
        case RACE_DWARF: allowed = dwarf_classes; break;
        case RACE_ELF: allowed = elf_classes; break;
        case RACE_HALFELF: allowed = half_elf_classes; break;
        case RACE_HALFGIANT: allowed = half_giant_classes; break;
        case RACE_HALFLING: allowed = halfling_classes; break;
        case RACE_MUL: allowed = mul_classes; break;
        case RACE_TRIKEEN: allowed = trikeen_classes; break;
    }

    while (allowed && *allowed) {
        if (*allowed == class) { return 1; }
        allowed++;
    }
    return 0;
}

static int has_class(const uint16_t class) {
    for (int i = 0; i < 3; i++) {
        if (pc.real_class[i] == class) { return 1; }
    }
    return 0;
}

static void set_class_frames() {
    int next_class =
        (pc.real_class[0] == -1) ? 0 :
        (pc.real_class[1] == -1) ? 1 :
        (pc.real_class[2] == -1) ? 2 :
        3;

    for (int i = 0; i < 8; i++) {
        pc.real_class[next_class] = convert_to_actual_class(i);
        if (sprite_get_frame(class_sel[i]) != 1) {
            sprite_set_frame(classes[i], 
                (next_class < 3 && is_class_allowed(pc.race, pc.real_class))
                ? 0 : 2);
        }
    }
    pc.real_class[next_class] = -1;
    update_ui();
}

static uint16_t new_sprite_create(SDL_Renderer *renderer, gff_palette_t *pal,
        const int offsetx, const int offsety, const float zoom,
        const int gff_idx, const int type_id, const int res_id) {
    SDL_Rect tmp = {offsetx, offsety, 0, 0};
    return sprite_create(renderer, &tmp, pal, 0, 0, zoom, gff_idx, type_id, res_id);
}

static void load_character_sprite() {
    gff_palette_t *pal = open_files[RESOURCE_GFF_INDEX].pals->palettes + 0;

    if (spr != SPRITE_ERROR) {
        sprite_free(spr);
        spr = SPRITE_ERROR;
    }
    switch(pc.race) {
        case RACE_HALFELF:
        case RACE_HUMAN:
            spr = new_sprite_create(renderer, pal, 150 + offsetx, 28 + offsetx,
                zoom, OBJEX_GFF_INDEX, GFF_BMP, (pc.gender == GENDER_MALE) ? 2095 : 2099);
            break;
        case RACE_DWARF:
            spr = new_sprite_create(renderer, pal, 150 + offsetx, 28 + offsetx,
                zoom, OBJEX_GFF_INDEX, GFF_BMP, (pc.gender == GENDER_MALE) ? 2055 : 2053);
            break;
        case RACE_ELF:
            spr = new_sprite_create(renderer, pal, 150 + offsetx, 28 + offsetx,
                zoom, OBJEX_GFF_INDEX, GFF_BMP, (pc.gender == GENDER_MALE) ? 2061 : 2059);
            break;
        case RACE_HALFGIANT:
            spr = new_sprite_create(renderer, pal, 150 + offsetx, 28 + offsetx,
                zoom, OBJEX_GFF_INDEX, GFF_BMP, (pc.gender == GENDER_MALE) ? 2072 : 2074);
            break;
        case RACE_HALFLING:
            spr = new_sprite_create(renderer, pal, 150 + offsetx, 28 + offsetx,
                zoom, OBJEX_GFF_INDEX, GFF_BMP, (pc.gender == GENDER_MALE) ? 2068 : 2070);
            break;
        case RACE_MUL:
            spr = new_sprite_create(renderer, pal, 150 + offsetx, 28 + offsetx,
                zoom, OBJEX_GFF_INDEX, GFF_BMP, 2093);
            break;
        case RACE_TRIKEEN:
            spr = new_sprite_create(renderer, pal, 150 + offsetx, 28 + offsetx,
                zoom, OBJEX_GFF_INDEX, GFF_BMP, 2097);
            break;
    }
}

static void init_pc() {
    memset(&pc, 0x0, sizeof(ds_character_t));
    pc.race = RACE_HUMAN;
    pc.gender = GENDER_MALE;
    pc.real_class[0] = pc.real_class[1] = pc.real_class[2] = -1;
    memset(&psi, 0x0, sizeof(psi));
}

static void new_character_init(SDL_Renderer *_renderer, const uint32_t x, const uint32_t y, const float _zoom) {
    gff_palette_t *pal = open_files[RESOURCE_GFF_INDEX].pals->palettes + 0;
    offsetx = x; offsety = y;
    zoom = _zoom;
    renderer = _renderer;

    init_pc();
    spr = SPRITE_ERROR;
    background = new_sprite_create(renderer, pal, 0 + x, 0 + y, zoom, RESOURCE_GFF_INDEX, GFF_BMP, 13001);
    parchment[0] = new_sprite_create(renderer, pal, 0 + x, 0 + y, zoom, RESOURCE_GFF_INDEX, GFF_BMP, 20084);
    parchment[1] = new_sprite_create(renderer, pal, 135 + x, 20 + y, zoom, RESOURCE_GFF_INDEX, GFF_BMP, 20083);
    parchment[2] = new_sprite_create(renderer, pal, 130 + x, 70 + y, zoom, RESOURCE_GFF_INDEX, GFF_BMP, 20085);
    parchment[3] = new_sprite_create(renderer, pal, 210 + x, 0 + y, zoom, RESOURCE_GFF_INDEX, GFF_BMP, 20086);
    parchment[4] = new_sprite_create(renderer, pal, 210 + x, 90 + y, zoom, RESOURCE_GFF_INDEX, GFF_BMP, 20087);
    done = new_sprite_create(renderer, pal, 250 + x, 160 + y, zoom, RESOURCE_GFF_INDEX, GFF_ICON, 2000);
    sphere_label = new_sprite_create(renderer, pal, 217 + x, 140 + y, zoom, RESOURCE_GFF_INDEX, GFF_ICON, 2046);
    psionic_label = new_sprite_create(renderer, pal, 217 + x, 140 + y, zoom, RESOURCE_GFF_INDEX, GFF_ICON, 2047);

    for (int i = 0; i < 8; i++) {
        classes[i] = new_sprite_create(renderer, pal, 220 + x, 10 + y + (i*8),
                zoom, RESOURCE_GFF_INDEX, GFF_ICON, 2002 + i);
        class_sel[i] = new_sprite_create(renderer, pal, 220 + x, 10 + y + (i*8),
                zoom, RESOURCE_GFF_INDEX, GFF_BMP, 20047);
        sprite_set_frame(classes[i], 2);
    }
    for (int i = 0; i < 3; i++) {
        psionics[i] = new_sprite_create(renderer, pal, 220 + x, 105 + y + (i*8),
                zoom, RESOURCE_GFF_INDEX, GFF_ICON, 2038 + i);
        sprite_set_frame(psionics[i], 0);
    }
    for (int i = 0; i < 4; i++) {
        spheres[i] = new_sprite_create(renderer, pal, 220 + x, 105 + y + (i*8),
                zoom, RESOURCE_GFF_INDEX, GFF_ICON, 2042 + i);
        ps_sel[i] = new_sprite_create(renderer, pal, 220 + x, 105 + y + (i*8),
                zoom, RESOURCE_GFF_INDEX, GFF_BMP, 20047);
        sprite_set_frame(spheres[i], 0);
    }
    for (int i = 0; i < 11; i++) {
        die[i] = new_sprite_create(renderer, pal, 142, 65,
                zoom, RESOURCE_GFF_INDEX, GFF_BMP, 20048 + i);
    }
    for (int i = 0; i < 14; i++) {
        races[i] = new_sprite_create(renderer, pal, 25, 12,
                zoom, RESOURCE_GFF_INDEX, GFF_BMP, 20000 + i);
    }

    strcpy(sphere_text, "PSI DISCIPLINES");

    srand(time(NULL));
    load_character_sprite(renderer);
    set_class_frames(); // go ahead and setup the new class frames
}

static void update_die_countdown() {
    if (die_countdown) {
        die_countdown--;
        if (die_countdown > 30) {
            die_pos = 1;
        } else if (die_countdown > 20) {
            die_pos = 2;
        } else if (die_countdown > 10) {
            die_pos = 3;
        } else if (die_countdown > 0) {
            die_pos = 4;
        } else {
            die_pos = 5 + (rand() % 6);
        }
    }
}

static int get_race_id() { // for the large portrait
    if (pc.race < RACE_MUL) { return 2 * (pc.race - 1) + (pc.gender - 1); }
    return 12 + (pc.race - RACE_MUL);
}

void new_character_render(void *data, SDL_Renderer *renderer) {
    sprite_render(renderer, background);
    for (int i = 0; i < 5; i++) {
        sprite_render(renderer, parchment[i]);
    }
    for (int i = 0; i < 8; i++) {
        sprite_render(renderer, classes[i]);
        sprite_render(renderer, class_sel[i]);
    }
    sprite_render(renderer, spr);
    update_die_countdown();
    sprite_render(renderer, die[die_pos]);
    sprite_render(renderer, races[get_race_id()]);
    print_line_len(renderer, FONT_BLACK, sphere_text, 450, 193, 1<<12);

    for (int i = 0; i < 4; i++) {
        if (i < 3 && show_psionic_label) {
            sprite_render(renderer, psionics[i]);
        }
        if (!show_psionic_label) {
            sprite_render(renderer, spheres[i]);
        }
        sprite_render(renderer, ps_sel[i]);
    }

    sprite_render(renderer, show_psionic_label ? sphere_label : psionic_label);
}

int new_character_handle_mouse_movement(const uint32_t x, const uint32_t y) {
    static uint16_t last_spr = SPRITE_ERROR;
    uint16_t cspr = SPRITE_ERROR;

    for (int i = 0; i < 8; i++) {
        if (sprite_in_rect(classes[i], x, y)) {
            cspr = classes[i];
        }
    }

    if (sprite_in_rect(sphere_label, x, y)) {
        cspr = show_psionic_label ? sphere_label : psionic_label;
    }

    if (show_psionic_label) {
        for (int i = 0; i < 3; i++) {
            if (sprite_in_rect(psionics[i], x, y)) {
                cspr = psionics[i];
            }
        }
    } else {
        for (int i = 0; i < 4; i++) {
            if (sprite_in_rect(spheres[i], x, y)) {
                cspr = spheres[i];
            }
        }
    }

    if (sprite_get_frame(cspr) < 2) {
        sprite_set_frame(cspr, 1);
    }

    if (last_spr != SPRITE_ERROR && last_spr != cspr) {
        if (sprite_get_frame(last_spr) < 2) {
            sprite_set_frame(last_spr, 0);
        }
    }

    last_spr = cspr;

    return 1;// handle
}

int new_character_handle_mouse_down(const uint32_t button, const uint32_t x, const uint32_t y) {
    return 1;// handle
}

static void fix_race_gender() { // move the race/gender to the appropiate spot
    int reset = 0;
    if (pc.gender > GENDER_FEMALE) {
        pc.gender = GENDER_MALE;
        pc.race++;
        reset = 1;
    } else if (pc.gender < GENDER_MALE) {
        pc.gender = GENDER_FEMALE;
        pc.race--;
        reset = 1;
    }
    if (pc.race < RACE_HUMAN) { pc.race = RACE_TRIKEEN; reset = 1;}
    if (pc.race > RACE_TRIKEEN) { pc.race = RACE_HUMAN; reset = 1;}

    if (pc.race == RACE_MUL && pc.gender == GENDER_FEMALE) {
        pc.race = RACE_TRIKEEN;
    }

    if (pc.race == RACE_TRIKEEN && pc.gender == GENDER_MALE) {
        pc.race = RACE_MUL;
    }

    for (int i = 0; reset && i < 8; i++) {
        pc.real_class[0] = pc.real_class[1] = pc.real_class[2] = -1;
        sprite_set_frame(class_sel[i], 0);
    }

    load_character_sprite(); // go ahead and get the new sprite
    set_class_frames(); // go ahead and setup the new class frames
}

static int find_class_selection(const uint8_t real_class) {
    for (int i = 0; i < 3; i++) {
        if (pc.real_class[i] == real_class) { return i; }
    }
    return -1;
}

static int find_class_selection_position(const uint8_t class_selection) {
    int sel = -1;
    switch(class_selection) {
        case 0:
            sel = find_class_selection(REAL_CLASS_AIR_CLERIC);
            if (sel == -1) { sel = find_class_selection(REAL_CLASS_EARTH_CLERIC);}
            if (sel == -1) { sel = find_class_selection(REAL_CLASS_FIRE_CLERIC);}
            if (sel == -1) { sel = find_class_selection(REAL_CLASS_WATER_CLERIC);}
            return sel;
        case 1:
            sel = find_class_selection(REAL_CLASS_AIR_DRUID);
            if (sel == -1) { sel = find_class_selection(REAL_CLASS_EARTH_DRUID);}
            if (sel == -1) { sel = find_class_selection(REAL_CLASS_FIRE_DRUID);}
            if (sel == -1) { sel = find_class_selection(REAL_CLASS_WATER_DRUID);}
            return sel;
        case 2: return find_class_selection(REAL_CLASS_FIGHTER);
        case 3: return find_class_selection(REAL_CLASS_GLADIATOR);
        case 4: return find_class_selection(REAL_CLASS_PRESERVER);
        case 5: return find_class_selection(REAL_CLASS_PSIONICIST);
        case 6:
            sel = find_class_selection(REAL_CLASS_AIR_RANGER);
            if (sel == -1) { sel = find_class_selection(REAL_CLASS_EARTH_RANGER);}
            if (sel == -1) { sel = find_class_selection(REAL_CLASS_FIRE_RANGER);}
            if (sel == -1) { sel = find_class_selection(REAL_CLASS_WATER_RANGER);}
            return sel;
        case 7: return find_class_selection(REAL_CLASS_THIEF);
    }
    return -1;
}

static void set_ps_sel_frames() {
    int ps_selections = 0;

        // Setup selection correctionly
    if (show_psionic_label) {
        sprite_set_frame(ps_sel[0], spell_has_psin(&psi, PSIONIC_PSYCHOKINETIC));
        sprite_set_frame(ps_sel[1], spell_has_psin(&psi, PSIONIC_PSYCHOMETABOLISM));
        sprite_set_frame(ps_sel[2], spell_has_psin(&psi, PSIONIC_TELEPATH));
        sprite_set_frame(ps_sel[3], 0);

        ps_selections = spell_has_psin(&psi, PSIONIC_PSYCHOKINETIC)
               + spell_has_psin(&psi, PSIONIC_PSYCHOMETABOLISM)
               + spell_has_psin(&psi, PSIONIC_TELEPATH);

        if (ps_selections > 0) {
            sprite_set_frame(psionics[0], spell_has_psin(&psi, PSIONIC_PSYCHOKINETIC) ? 0 : 2);
            sprite_set_frame(psionics[1], spell_has_psin(&psi, PSIONIC_PSYCHOMETABOLISM) ? 0 : 2);
            sprite_set_frame(psionics[2], spell_has_psin(&psi, PSIONIC_TELEPATH) ? 0 : 2);
        } else {
            sprite_set_frame(psionics[0], 0);
            sprite_set_frame(psionics[1], 0);
            sprite_set_frame(psionics[2], 0);
        }
    } else {
        sprite_set_frame(ps_sel[0], (sphere_selection == 0) ? 1 : 0);
        sprite_set_frame(ps_sel[1], (sphere_selection == 1) ? 1 : 0);
        sprite_set_frame(ps_sel[2], (sphere_selection == 2) ? 1 : 0);
        sprite_set_frame(ps_sel[3], (sphere_selection == 3) ? 1 : 0);
        if (sphere_selection == -1) {
            sprite_set_frame(spheres[0], 0);
            sprite_set_frame(spheres[1], 0);
            sprite_set_frame(spheres[2], 0);
            sprite_set_frame(spheres[3], 0);
        } else {
            sprite_set_frame(spheres[0], (sphere_selection == 0) ? 0 : 2);
            sprite_set_frame(spheres[1], (sphere_selection == 1) ? 0 : 2);
            sprite_set_frame(spheres[2], (sphere_selection == 2) ? 0 : 2);
            sprite_set_frame(spheres[3], (sphere_selection == 3) ? 0 : 2);
        }
    }
}

static void toggle_psi(const uint16_t i) {
    int cframe = sprite_get_frame(ps_sel[i]);
    int ps_selections = spell_has_psin(&psi, PSIONIC_PSYCHOKINETIC)
           + spell_has_psin(&psi, PSIONIC_PSYCHOMETABOLISM)
           + spell_has_psin(&psi, PSIONIC_TELEPATH);

    if (ps_selections > 2) { return; }

    spell_set_psin(&psi, i, cframe == 1 ? 0 : 1);

    set_ps_sel_frames();
}

static void toggle_sphere(const uint16_t i) {
    int cframe = sprite_get_frame(ps_sel[i]);
    sprite_set_frame(ps_sel[i], cframe == 1 ? 0 : 1);

    sphere_selection = (cframe) ? -1 : i;

    set_ps_sel_frames();
}

static void deselect_class(uint8_t class_selection) {
    int class_pos = find_class_selection_position(class_selection);
    for (int i = class_pos; i < 2; i++) {
        pc.real_class[i] = pc.real_class[i + 1];
    }

    if (convert_to_actual_class(class_selection) == REAL_CLASS_PSIONICIST) {
        spell_set_psin(&psi, PSIONIC_PSYCHOKINETIC, 0);
        spell_set_psin(&psi, PSIONIC_PSYCHOMETABOLISM, 0);
        spell_set_psin(&psi, PSIONIC_TELEPATH, 0);
    }

    pc.real_class[2] = -1;
    sprite_set_frame(class_sel[class_selection], 0);
    show_psionic_label = 1;

    set_class_frames();
    set_ps_sel_frames();
}

static void select_class(uint8_t class) {
    int pos = 0;
    if (pc.real_class[pos] != -1) { pos++; }
    if (pc.real_class[pos] != -1) { pos++; }
    sprite_set_frame(class_sel[class], 1);
    pc.real_class[pos] = convert_to_actual_class(class);
    set_class_frames();

    if (pc.real_class[pos] == REAL_CLASS_PSIONICIST) {
        spell_set_psin(&psi, PSIONIC_PSYCHOKINETIC, 1);
        spell_set_psin(&psi, PSIONIC_PSYCHOMETABOLISM, 1);
        spell_set_psin(&psi, PSIONIC_TELEPATH, 1);
    }

    set_ps_sel_frames();
}

int new_character_handle_mouse_up(const uint32_t button, const uint32_t x, const uint32_t y) {
    if (sprite_in_rect(die[die_pos], x, y)) {
        die_countdown = 40;
    }
    for (int i = 0; i < 8; i++) {
        if (sprite_in_rect(classes[i], x, y)) {
            if (sprite_get_frame(class_sel[i]) == 1) {
                deselect_class(i);
            } else if (sprite_get_frame(classes[i]) < 2) {
                select_class(i);
            } 
        }
    }
    if (sprite_in_rect(races[pc.race], x, y)) {
        if (button == SDL_BUTTON_LEFT) {
            pc.gender++;
        } else {
            pc.gender--;
        }
        fix_race_gender();
    }
    for (int i = 0; i < 4; i++) {
        if (i < 3 && show_psionic_label) {
            if (sprite_in_rect(psionics[i], x, y)) {
                if (sprite_get_frame(psionics[i]) < 2) {
                    toggle_psi(i);
                }
            }
        }
        if (!show_psionic_label) {
            if (sprite_in_rect(spheres[i], x, y)) {
                if (sprite_get_frame(spheres[i]) < 2) {
                    toggle_sphere(i);
                }
            }
        }
        sprite_render(renderer, ps_sel[i]);
    }
    if (show_psionic_label && sprite_get_frame(psionic_label) < 2 && sprite_in_rect(psionic_label, x, y)) {
        show_psionic_label = 0;
        set_ps_sel_frames();
    } else if (!show_psionic_label && sprite_get_frame(sphere_label) < 2 && sprite_in_rect(sphere_label, x, y)) {
        show_psionic_label = 1;
        set_ps_sel_frames();
    }
    return 1;// handle
}

static void update_ui() {
    int show_spheres =
        has_class(REAL_CLASS_AIR_CLERIC)
        || has_class(REAL_CLASS_EARTH_CLERIC)
        || has_class(REAL_CLASS_FIRE_CLERIC)
        || has_class(REAL_CLASS_WATER_CLERIC)
        || has_class(REAL_CLASS_AIR_DRUID)
        || has_class(REAL_CLASS_EARTH_DRUID)
        || has_class(REAL_CLASS_FIRE_DRUID)
        || has_class(REAL_CLASS_WATER_DRUID)
        || has_class(REAL_CLASS_AIR_RANGER)
        || has_class(REAL_CLASS_EARTH_RANGER)
        || has_class(REAL_CLASS_FIRE_RANGER)
        || has_class(REAL_CLASS_WATER_RANGER);
    sprite_set_frame(sphere_label, show_spheres ? 0 : 2);
    sprite_set_frame(psionic_label, show_spheres ? 0 : 2);
}

void new_character_free() {
    sprite_free(background);
    for (int i = 0; i < 5; i++) {
        sprite_free(parchment[i]);
    }
    for (int i = 0; i < 8; i++) {
        sprite_free(classes[i]);
        sprite_free(class_sel[i]);
    }
    for (int i = 0; i < 3; i++) {
        sprite_free(psionics[i]);
    }
    for (int i = 0; i < 4; i++) {
        sprite_free(spheres[i]);
    }
    for (int i = 0; i < 11; i++) {
        sprite_free(die[i]);
    }
}

sops_t new_character_screen = {
    .init = new_character_init,
    .cleanup = new_character_free,
    .render = new_character_render,
    .mouse_movement = new_character_handle_mouse_movement,
    .mouse_down = new_character_handle_mouse_down,
    .mouse_up = new_character_handle_mouse_up,
    .return_control = NULL,
    .data = NULL
};