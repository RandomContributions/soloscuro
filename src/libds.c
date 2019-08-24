#include <lua5.1/lua.h>
#include <lua5.1/lualib.h>
#include <lua5.1/lauxlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "gff.h"
#include "gff-image.h"
#include "gff-xmi.h"
#include "gff-map.h"

static int lua_gff_init(lua_State *L) {
    gff_init();
    
    return 0;
}

int lua_gff_load_directory(lua_State *L) {
    size_t len;
    const char *path = luaL_checklstring(L, 1, &len);

    gff_load_directory(path);

    return 0;
}

#define BUF_SIZE (1024)
int lua_gff_find_index(lua_State *L) {
    size_t len;
    const char *name = luaL_checklstring(L, 1, &len);
    char buf[1024];

    if (len >= (BUF_SIZE-1)) { return -1; }

    for (int i = 0; i < len; i++) {
        buf[i] = tolower(name[i]);
    }
    buf[len] = '\0';

    lua_pushinteger(L, gff_find_index(buf));

    return 1;
}

int lua_gff_open(lua_State *L) {
    size_t len;
    const char *path = luaL_checklstring(L, 1, &len);

    int id = gff_open(path);
    lua_pushinteger(L, id);

    return 1;
}

int lua_gff_list(lua_State *L) {
    size_t len;
    const char** files = gff_list(&len);

    lua_createtable(L, len, 0);
    for (size_t i = 0; i < len; i++) {
        lua_pushstring(L, files[i]);
        lua_rawseti(L, -2, i + 1);
    }

    free(files);
    printf("returning...\n");

    return 1;
}

int lua_gff_get_number_of_types(lua_State *L) {
    int idx = luaL_checkinteger (L, 1);

    lua_pushinteger(L, gff_get_number_of_types(idx));

    return 1;
}

int lua_gff_get_number_of_palettes(lua_State *L) {
    int idx = luaL_checkinteger(L, 1);

    lua_pushinteger(L, gff_get_number_of_palettes(idx));

    return 1;
}

int lua_gff_get_type_id(lua_State *L) {
    int idx = luaL_checkinteger(L, 1);
    int type_index = luaL_checkinteger(L, 2);

    lua_pushinteger(L, gff_get_type_id(idx, type_index));

    return 1;
}

int lua_get_gff_type_name(lua_State *L) {
    char buf[1024];
    int gff_type = luaL_checkinteger(L, 1);

    get_gff_type_name(gff_type, buf);

    lua_pushstring(L, buf);

    return 1;
}

int lua_gff_get_id_list(lua_State *L) {
    int idx = luaL_checkinteger(L, 1);
    int type_id = luaL_checkinteger(L, 2) & GFFMAXCHUNKMASK;

    lua_createtable(L, 0, 0);

    unsigned int num_ids = gff_get_gff_type_length(idx, type_id);
    unsigned int *ids = gff_get_id_list(idx, type_id);

    for (unsigned int i = 0; i < num_ids; i++) {
        lua_pushinteger(L, i + 1);
        lua_pushinteger(L, ids[i]);
        lua_settable(L, -3);
    }

    free(ids);

    return 1;
}

int lua_gff_get_data_as_text(lua_State *L) {
    int idx = luaL_checkinteger(L, 1);
    int type_id = luaL_checkinteger(L, 2) & GFFMAXCHUNKMASK;
    int res_id = luaL_checkinteger(L, 3);
    unsigned long len;

    char* msg = gff_get_raw_bytes(idx, type_id, res_id, &len);
    msg[len-1] = '\0';
    lua_pushstring(L, msg);

    return 1;
}

int lua_gff_write_raw_bytes(lua_State *L) {
    int idx = luaL_checkinteger(L, 1);
    int type_id = luaL_checkinteger(L, 2) & GFFMAXCHUNKMASK;
    int res_id = luaL_checkinteger(L, 3);
    const char *path = luaL_checkstring(L, 4);

    lua_pushinteger(L, gff_write_raw_bytes(idx, type_id, res_id, path));

    return 1;
}

char* lua_gff_get_raw_bytes(int idx, int type_id, int res_id, unsigned long *len) {
    return NULL;
}

int lua_gff_close (lua_State *L) {
    lua_Integer id = luaL_checkinteger (L, 1);

    gff_close(id);

    return 0;
}

int lua_gff_cleanup (lua_State *L) {
    gff_cleanup();

    return 0;
}

/* Image functions */

int lua_get_frame_count(lua_State *L) {
    lua_Integer gff_index = luaL_checkinteger (L, 1);
    lua_Integer type_id = luaL_checkinteger (L, 2);
    lua_Integer res_id = luaL_checkinteger (L, 3);

    lua_pushinteger(L, get_frame_count(gff_index, type_id, res_id));

    return 1;
}

int lua_get_frame_width(lua_State *L) {
    lua_Integer gff_index = luaL_checkinteger (L, 1);
    lua_Integer type_id = luaL_checkinteger (L, 2);
    lua_Integer res_id = luaL_checkinteger (L, 3);
    lua_Integer frame_id = luaL_checkinteger (L, 4);

    lua_pushinteger(L, get_frame_width(gff_index, type_id, res_id, frame_id));

    return 1;
}

int lua_get_frame_height(lua_State *L) {
    lua_Integer gff_index = luaL_checkinteger (L, 1);
    lua_Integer type_id = luaL_checkinteger (L, 2);
    lua_Integer res_id = luaL_checkinteger (L, 3);
    lua_Integer frame_id = luaL_checkinteger (L, 4);

    lua_pushinteger(L, get_frame_height(gff_index, type_id, res_id, frame_id));

    return 1;
}

int lua_get_frame_rgba(lua_State *L) {
    lua_Integer gff_index = luaL_checkinteger (L, 1);
    lua_Integer type_id = luaL_checkinteger (L, 2);
    lua_Integer res_id = luaL_checkinteger (L, 3);
    lua_Integer frame_id = luaL_checkinteger (L, 4);

    int w = get_frame_width(gff_index, type_id, res_id, frame_id);
    int h = get_frame_height(gff_index, type_id, res_id, frame_id);
    char* data = (char*)get_frame_rgba_with_palette(gff_index, type_id, res_id, frame_id, -1);

    if (data == NULL) {
        lua_pushinteger(L, 0);
    } else {
        lua_pushlstring(L, data, 4 * w * h);
        free(data);
    }

    return 1;
}

int lua_get_frame_rgba_with_palette(lua_State *L) {
    lua_Integer gff_index = luaL_checkinteger (L, 1);
    lua_Integer type_id = luaL_checkinteger (L, 2);
    lua_Integer res_id = luaL_checkinteger (L, 3);
    lua_Integer frame_id = luaL_checkinteger (L, 4);
    lua_Integer palette_id = luaL_checkinteger (L, 5);

    int w = get_frame_width(gff_index, type_id, res_id, frame_id);
    int h = get_frame_height(gff_index, type_id, res_id, frame_id);
    char* data = (char*)get_frame_rgba_with_palette(gff_index, type_id, res_id, frame_id, palette_id);

    if (data == NULL) {
        lua_pushinteger(L, 0);
    } else {
        lua_pushlstring(L, data, 4 * w * h);
        free(data);
    }

    return 1;
}
/* End Image Functions */

/* Sound Functions */
int lua_get_chunk_as_midi(lua_State *L) {
    lua_Integer gff_index = luaL_checkinteger (L, 1);
    lua_Integer type_id = luaL_checkinteger (L, 2);
    lua_Integer res_id = luaL_checkinteger (L, 3);
    unsigned long xmi_len = 0;
    unsigned int midi_len = 0;

    char* xmi_data = gff_get_raw_bytes(gff_index, type_id, res_id, &xmi_len);
    unsigned char* midi_data = xmi_to_midi((unsigned char*)xmi_data, xmi_len, &midi_len);
    FILE *tmp = fopen("t.mid", "wb+");
    fwrite(midi_data, 1, midi_len, tmp);
    fclose(tmp);

    if (midi_data == NULL) {
        lua_pushinteger(L, 0);
    } else {
        lua_pushlstring(L, (char*)midi_data, midi_len);
        free(midi_data);
    }

    return 1;
}
/* End Sound Functions */

/* Begin Map Functions */

int lua_load_tile_ids(lua_State *L) {
    lua_Integer gff_index = luaL_checkinteger (L, 1);
    lua_Integer res_id = luaL_checkinteger (L, 2);

    gff_load_map_tile_ids(gff_index, res_id);

    return 0;
}

int lua_load_map_flags(lua_State *L) {
    lua_Integer gff_index = luaL_checkinteger (L, 1);
    lua_Integer res_id = luaL_checkinteger (L, 2);

    gff_load_map_flags(gff_index, res_id);

    return 0;
}

int lua_load_map(lua_State *L) {
    lua_Integer gff_index = luaL_checkinteger (L, 1);

    gff_load_map(gff_index);

    return 0;
}

int lua_get_tile_id(lua_State *L) {
    lua_Integer gff_index = luaL_checkinteger (L, 1);
    lua_Integer row = luaL_checkinteger (L, 2);
    lua_Integer column = luaL_checkinteger (L, 3);

    lua_pushinteger(L, get_tile_id(gff_index, row, column));

    return 1;
}

int lua_map_is_block(lua_State *L) {
    lua_Integer gff_index = luaL_checkinteger (L, 1);
    lua_Integer row = luaL_checkinteger (L, 2);
    lua_Integer column = luaL_checkinteger (L, 3);

    lua_pushboolean(L, gff_map_is_block(gff_index, row, column) > 0);

    return 1;
}

int lua_map_is_actor(lua_State *L) {
    lua_Integer gff_index = luaL_checkinteger (L, 1);
    lua_Integer row = luaL_checkinteger (L, 2);
    lua_Integer column = luaL_checkinteger (L, 3);

    lua_pushboolean(L, gff_map_is_actor(gff_index, row, column) > 0);

    return 1;
}

int lua_map_is_danger(lua_State *L) {
    lua_Integer gff_index = luaL_checkinteger (L, 1);
    lua_Integer row = luaL_checkinteger (L, 2);
    lua_Integer column = luaL_checkinteger (L, 3);

    lua_pushboolean(L, gff_map_is_danger(gff_index, row, column) > 0);

    return 1;
}

int lua_map_get_number_of_objects(lua_State *L) {
    lua_Integer gff_index = luaL_checkinteger (L, 1);
    lua_Integer res_id = luaL_checkinteger (L, 2);

    lua_pushinteger(L, gff_map_get_num_objects(gff_index, res_id));

    return 1;
}

int lua_map_get_object_bmp(lua_State *L) {
    lua_Integer gff_index = luaL_checkinteger (L, 1);
    lua_Integer res_id = luaL_checkinteger (L, 2);
    lua_Integer obj_id = luaL_checkinteger (L, 3);

    int w, h;

    char *data = (char*)gff_map_get_object_bmp(gff_index, res_id, obj_id, &w, &h);

    if (data == NULL) {
        lua_pushinteger(L, 0);
    } else {
        lua_pushlstring(L, data, 4 * w * h);
        free(data);
    }

    lua_pushinteger(L, w);
    lua_pushinteger(L, h);

    return 3;
}

int lua_map_get_object_location(lua_State *L) {
    lua_Integer gff_index = luaL_checkinteger (L, 1);
    lua_Integer res_id = luaL_checkinteger (L, 2);
    lua_Integer obj_id = luaL_checkinteger (L, 3);

    uint16_t x, y;
    uint8_t z;
    gff_map_get_object_location(gff_index, res_id, obj_id, &x, &y, &z);

    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    lua_pushinteger(L, z);

    return 3;
}
/* End Map Functions */

//library to be registered
static const struct luaL_Reg lslib [] = {
      {"gff_init", lua_gff_init},
      {"gff_load_directory", lua_gff_load_directory},
      {"gff_find_index", lua_gff_find_index},
      {"gff_open", lua_gff_open},
      {"gff_list", lua_gff_list},
      {"gff_close", lua_gff_close},
      {"gff_cleanup", lua_gff_cleanup},
      {"gff_get_number_of_types", lua_gff_get_number_of_types},
      {"gff_get_number_of_palettes", lua_gff_get_number_of_palettes},
      {"gff_get_type_id", lua_gff_get_type_id},
      {"get_gff_type_name", lua_get_gff_type_name},
      {"gff_get_id_list", lua_gff_get_id_list},
      {"gff_get_data_as_text", lua_gff_get_data_as_text},
      {"gff_write_raw_bytes", lua_gff_write_raw_bytes},

      // Image Functions
      {"get_frame_count", lua_get_frame_count},
      {"get_frame_width", lua_get_frame_width},
      {"get_frame_height", lua_get_frame_height},
      {"get_frame_rgba", lua_get_frame_rgba},
      {"get_frame_rgba_with_palette", lua_get_frame_rgba_with_palette},

      // Sound Functions
      {"get_chunk_as_midi", lua_get_chunk_as_midi},

      // Map Functions
      {"load_tile_ids", lua_load_tile_ids},
      {"load_map_flags", lua_load_map_flags},
      {"load_map", lua_load_map},
      {"get_tile_id", lua_get_tile_id},
      {"map_is_block", lua_map_is_block},
      {"map_is_actor", lua_map_is_actor},
      {"map_is_danger", lua_map_is_danger},
      {"map_get_number_of_objects", lua_map_get_number_of_objects},
      {"map_get_object_bmp", lua_map_get_object_bmp},
      {"map_get_object_location", lua_map_get_object_location},

      // The End
      {NULL, NULL}  /* sentinel */
    };

// How Lua registers the functions.
int luaopen_libds (lua_State *L){
    luaL_register(L, "libds", lslib);
    return 1;
}