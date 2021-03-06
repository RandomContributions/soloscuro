/*
 * These are the functions needed to be implmeneted for porting to different systems.
 * This include both OS specific call and callbacks.
 */
#ifndef PORT_H
#define PORT_H

#include "ds-object.h"
#include "ds-region.h"

#ifdef _WIN32
#  ifdef _WIN64
#    define PRI_SIZET "%I64u"
#  else
#    define PRI_SIZET "%I32u"
#  endif
#else
#  define PRI_SIZET "%zu"
#endif

#ifdef _WIN32
#  ifdef _WIN64
#    define PRI_LI "%I64d"
#  else
#    define PRI_LI "%I32d"
#  endif
#else
#  define PRI_LI "%lld"
#endif

// Narrate functions
extern int8_t port_narrate_open(int16_t action, const char *text, int16_t index);
extern void port_narrate_clear();
extern void port_narrate_close();

// Object manipulation
extern void port_swap_objs(int obj_id, region_object_t *obj);
void port_add_obj(region_object_t *obj, gff_palette_t *pal);
extern void port_animate_obj(region_object_t *obj);
extern void port_update_obj(region_object_t *obj, const uint16_t xdiff, const uint16_t ydiff);
extern int port_ask_yes_no();

extern void port_change_region(dsl_region_t *reg);

#endif
