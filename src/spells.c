#include <string.h>
#include "spells.h"
#include "gff.h"
#include "gfftypes.h"

static void load_name_from_gff(const uint8_t id, const uint16_t offset, const uint8_t max, char name[32]) {
    char buf[1<<10];
    int pos = 0;

    if (id >= max) {
        name[0] = '\0';
        return;
    }

    gff_chunk_header_t chunk = gff_find_chunk_header(RESOURCE_GFF_INDEX, GFF_SPIN, offset + id);
    gff_read_chunk(RESOURCE_GFF_INDEX, &chunk, buf, sizeof(buf));

    while (pos < 31 && buf[pos] != ':') {
        name[pos] = buf[pos];
        pos++;
    }

    name[pos] = '\0';
}

void spell_get_psionic_name(uint8_t psi, char name[32]) {
    load_name_from_gff(psi, 139, PSIONIC_MAX, name);
}

void spell_get_wizard_name(uint8_t spell, char name[32]) {
    load_name_from_gff(spell, 0, WIZ_MAX, name);
}

void spell_get_cleric_name(uint8_t spell, char name[32]) {
    load_name_from_gff(spell, 68, CLERIC_MAX, name);
}

void spell_get_psin_name(uint8_t psin, char name[32]) {
    switch(psin) {
        case PSIONIC_PSYCHOKINETIC:
            strcpy(name, "Psyhchokinetic");
            break;
        case PSIONIC_PSYCHOMETABOLISM:
            strcpy(name, "Psychometabolism");
            break;
        case PSIONIC_TELEPATH:
            strcpy(name, "Telepathy");
            break;
        case SPHERE_AIR:
            strcpy(name, "Air");
            break;
        case SPHERE_EARTH:
            strcpy(name, "Earth");
            break;
        case SPHERE_FIRE:
            strcpy(name, "Fire");
            break;
        case SPHERE_WATER:
            strcpy(name, "Water");
            break;
        default:
            strcpy(name, "UNKNOWN");
    }
}

int spell_has_psin(psin_t *psin, const uint8_t psi) {
    switch (psi) {
        case PSIONIC_PSYCHOKINETIC:
            return psin->types[0];
            break;
        case PSIONIC_PSYCHOMETABOLISM:
            return psin->types[2];
            break;
        case PSIONIC_TELEPATH:
            return psin->types[4];
            break;
    }

    return 0;
}

void spell_set_psin(psin_t *psin, const uint8_t psi, const int on) {
    switch (psi) {
        case PSIONIC_PSYCHOKINETIC:
            psin->types[0] = (on) ? 1 : 0;
            break;
        case PSIONIC_PSYCHOMETABOLISM:
            psin->types[2] = (on) ? 1 : 0;
            break;
        case PSIONIC_TELEPATH:
            psin->types[4] = (on) ? 1 : 0;
            break;
    }
}