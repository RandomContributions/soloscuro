#include <stdlib.h>
#include <string.h>
#include "gff-image.h"
#include "gff.h"
#include "gfftypes.h"

#define NUM_PALETTES (256)

static unsigned char* create_initialized_image_rgb(const unsigned int w, const unsigned h);
static int plnr_get_mask(const int bits_to_read);
static int plnr_get_bits(const unsigned char *data, const int bits_read, const int bits_to_read);
static int plnr_get_next(unsigned char* chunk, int bits_per_symbol);
static int intensity_multiplier = 4;
static size_t num_palettes;
static gff_palette_t palettes[NUM_PALETTES];
static gff_palette_t *master_palette;

void gff_image_init() {
    num_palettes = 0;
    master_palette = NULL;
    memset(palettes, 0, sizeof(gff_palette_t) * NUM_PALETTES);
}

gff_palette_t* create_palettes(int gff_index, unsigned int *len) {
    gff_chunk_list_t* pal_chunk = search_for_chunk_by_name(open_files+gff_index, GT_PAL);
    if (pal_chunk == NULL) { goto cp_search_error; }

    unsigned int* ids = gff_get_id_list(gff_index, GT_PAL);
    if (pal_chunk->chunkCount & GFFSEGFLAGMASK) {
        gff_seg_header_t  *seg_header = (gff_seg_header_t*)&pal_chunk->chunks[0];
        int num_entries = 0;
        for (int j = 0; j < seg_header->segRef.numEntries; j++) {
            num_entries += seg_header->segRef.entries[j].consecChunks;
        }
        *len = num_entries;
        if (num_palettes + *len >= NUM_PALETTES) { // We are out of space!
            fprintf(stderr, "Ran out of palettes!\n");
            *len = 0;
            return NULL;
        }

        gff_palette_t* cpal = palettes + num_palettes;
        if (gff_index == gff_get_master()) { 
            master_palette = cpal; 
        }

        num_palettes += *len;
        printf("Number of palettes: %d\n", *len);
        for (int i = 0; i < *len; i++) {
            unsigned long tlen;
            unsigned char* raw_pal = (unsigned char*)gff_get_raw_bytes(gff_index, GT_PAL, ids[i], &tlen);
            //unsigned char* raw_pal = (unsigned char*)gff_get_raw_bytes(gff_index, GT_PAL, ids[i], &tlen);
            //printf("pal[%d] id = %u, tlen = %lu\n", i, ids[i], tlen);
            for (int j = 0; j < PALETTE_SIZE; j++) {
                cpal[i].color[j].r = intensity_multiplier * ((unsigned char)(raw_pal[j * 3 + 0]));
                cpal[i].color[j].g = intensity_multiplier * ((unsigned char)(raw_pal[j * 3 + 1]));
                cpal[i].color[j].b = intensity_multiplier * ((unsigned char)(raw_pal[j * 3 + 2]));
            }
        }
        free(ids);
        return cpal;
    } 

    if (ids == NULL) { goto cp_search_error; }

    *len = pal_chunk->chunkCount;

    if (num_palettes + *len >= NUM_PALETTES) { // We are out of space!
        fprintf(stderr, "Ran out of palettes!\n");
        *len = 0;
        return NULL;
    }

    gff_palette_t* cpal = palettes + num_palettes;
    if (gff_index == gff_get_master()) { 
        master_palette = cpal; 
    }

    num_palettes += *len;
    printf("Number of palettes: %d\n", *len);
    for (int i = 0; i < *len; i++) {
        unsigned long tlen;
        unsigned char* raw_pal = (unsigned char*)gff_get_raw_bytes(gff_index, GT_PAL, ids[i], &tlen);
        //printf("pal[%d] id = %u, tlen = %lu\n", i, ids[i], tlen);
        for (int j = 0; j < PALETTE_SIZE; j++) {
            cpal[i].color[j].r = intensity_multiplier * ((unsigned char)(raw_pal[j * 3 + 0]));
            cpal[i].color[j].g = intensity_multiplier * ((unsigned char)(raw_pal[j * 3 + 1]));
            cpal[i].color[j].b = intensity_multiplier * ((unsigned char)(raw_pal[j * 3 + 2]));
        }
    }

    free(ids);
    return cpal;

cp_search_error:
    return NULL;
}

int get_frame_count(int gff_index, int type_id, int res_id) {
    unsigned long len = 0;
    unsigned char* chunk = (unsigned char*)gff_get_raw_bytes(gff_index, type_id, res_id, &len);
    short num_frames = *((unsigned short*)(chunk + 4));
    return num_frames;
}

#define get_distance_to_chunk(a, b) ((unsigned long)a - (unsigned long)b)

int get_frame_width(int gff_index, int type_id, int res_id, int frame_id) {
    unsigned long len = 0;
    unsigned char* chunk = (unsigned char*)gff_get_raw_bytes(gff_index, type_id, res_id, &len);
    unsigned int frame_offset = *((unsigned int*)(chunk + 6 + frame_id*4));
    // bounds check
    if (get_distance_to_chunk(chunk, open_files[gff_index].data) + frame_offset > open_files[gff_index].len) {
        return -1;
    }
    unsigned short width = *(unsigned short*)(chunk + frame_offset);
    return width;
}

int get_frame_height(int gff_index, int type_id, int res_id, int frame_id) {
    unsigned long len = 0;
    unsigned char* chunk = (unsigned char*)gff_get_raw_bytes(gff_index, type_id, res_id, &len);
    unsigned int frame_offset = *((unsigned int*)(chunk + 6 + frame_id*4));
    // bounds check
    if (get_distance_to_chunk(chunk, open_files[gff_index].data) + frame_offset > open_files[gff_index].len) {
        return -1;
    }
    unsigned short hieght = *(unsigned short*)(chunk + frame_offset + 2);
    return hieght;
}

static unsigned char* create_initialized_image_rgb(const unsigned int w, const unsigned h) {
    unsigned char *img = malloc(sizeof(unsigned char) * 4 * w * h);
    if (img == NULL) { return NULL; }
    memset(img, 0, sizeof(unsigned char) * 4 * w * h);
    return img;
}

unsigned char* create_ds1_rgba(unsigned char *chunk, int cpos, const int width, const int height, gff_palette_t *cpal) {
    int num_rows = 0;
    unsigned char* img = create_initialized_image_rgb(width, height);
    while (num_rows < height) {
        int row_num = *(chunk + cpos++);

        if (row_num == 0xFF) { break; }
        if (row_num >= height) { goto ds1_img_error; }

        unsigned char *img_row = img + (4*row_num*width);// The current row to edit.
        num_rows++;
        while (1) {
            int startx = *(chunk + cpos++);
            int flags = *(chunk + cpos++);
            cpos++;
            int compressed_length = *(chunk + cpos++);

            if (flags & 0x01) { // We are column 256...
                startx += 256;
            }
            unsigned char *row = chunk + cpos;

            // Work on pixels.
            for (int i = 0; i < compressed_length; ) {
                int code = *(row + i++);
                int run_len = code / 2 + 1;
                if ((code %2) == 0) { // Even = row of regular palette entries.
                    for (int j = 0; j < run_len; j++) {
                        int pal_index = row[i++];
                        *(img_row + 4*(startx) + 0) = cpal->color[pal_index].r;
                        *(img_row + 4*(startx) + 1) = cpal->color[pal_index].g;
                        *(img_row + 4*(startx) + 2) = cpal->color[pal_index].b;
                        *(img_row + 4*(startx) + 3) = 0xFF;
                        startx++;
                    }
                } else { // Odd = row of duplicate pallete entries.
                    int repeated = *(row + i++);
                    for (int j = 0; j < run_len; j++) {
                        *(img_row + 4*(startx) + 0) = cpal->color[repeated].r;
                        *(img_row + 4*(startx) + 1) = cpal->color[repeated].g;
                        *(img_row + 4*(startx) + 2) = cpal->color[repeated].b;
                        *(img_row + 4*(startx) + 3) = 0xFF;
                        startx++;
                    }
                }
            }

            // Now calculate the next bit.
            cpos += compressed_length;

            if (flags & 0x80) { // Last run flag
                break;
            }
        }
    }
    return img;
ds1_img_error:
    if (img) { free(img); }
    return NULL;
}

/* PLNR CODE */
static int plnr_get_mask(const int bits_to_read) {
    switch (bits_to_read) {
        case 1: return 0x01;
        case 2: return 0x03;
        case 3: return 0x07;
        case 4: return 0x0f;
        case 5: return 0x1f;
        case 6: return 0x3f;
        case 7: return 0x7f;
        case 8: return 0xff;
    }

    fprintf(stderr, "bit_mask > 8 bits is not implemented!\n");
    return 0xff;
}

static int plnr_get_bits(const unsigned char *data, const int bits_read, const int bits_to_read) {
    int byte_offset = bits_read / 8;
    int bit_offset = 4 - (bits_read % 8);
    int bit_mask = plnr_get_mask(bits_to_read);
    const unsigned char *byte = data + byte_offset;
    //printf("bit_offset = %d, bits_to_read = %d\n", bit_offset, bits_to_read);
    if (bit_offset + bits_to_read <= 8) {
        int first_byte = ((*byte) >> bit_offset) & bit_mask;
        int nbyte = ((first_byte & 0x08) >> 3);
        nbyte |= ((first_byte & 0x04) >> 1);
        nbyte |= ((first_byte & 0x02) << 1);
        nbyte |= ((first_byte & 0x01) << 3);
        return first_byte;
    } else {
        printf("split bits!\n");
    }

    return 0;
}

int plnr_last_value = 0;
int plnr_remaining = 0;
int plnr_num_bits_read = 0;

static int plnr_get_next(unsigned char* image_data, int bits_per_symbol) {
    if (plnr_remaining == 0) {
        int first_slice = plnr_get_bits(image_data, plnr_num_bits_read, bits_per_symbol);
        plnr_num_bits_read += bits_per_symbol;
        if (first_slice == 0) {
            int second_slice = plnr_get_bits(image_data, plnr_num_bits_read, bits_per_symbol);
            plnr_num_bits_read += bits_per_symbol;
            if (second_slice == 0) {
                plnr_last_value = 0;
                plnr_remaining = 1;
            } else {
                // forward the value
                plnr_remaining = second_slice + 2;
            }
        } else {
            plnr_last_value = first_slice;
            plnr_remaining = 1;
        }
    }

    plnr_remaining--;

    return plnr_last_value;
}
/* END PLNR CODE */

unsigned char* get_frame_rgba_with_palette(int gff_index, int type_id, int res_id, int frame_id, int palette_id) {
    gff_palette_t *cpal = NULL;
    printf("master_palette = %p\n", master_palette);
    if (palette_id < 0 || open_files[gff_index].num_palettes < 1) {
        cpal = master_palette;
    } else {
        cpal = open_files[gff_index].palettes + palette_id;
    }

    unsigned long len = 0;
    unsigned char* chunk = (unsigned char*)gff_get_raw_bytes(gff_index, type_id, res_id, &len);
    if (len < 11) {
        fprintf(stderr, "1ERROR: unable to load image (%d), it is too small (%lu.)\n", res_id, len);
    }
    unsigned int frame_offset = *((unsigned int*)(chunk + 6 + frame_id*4));
    unsigned short width = *(unsigned short*)(chunk + frame_offset);
    unsigned short height = *(unsigned short*)(chunk + frame_offset + 2);

    printf("offset = %d, width = %d, height = %d\n", frame_offset, width, height);
    char *frame_type = ((char*)(chunk + frame_offset + 5));
    if (cpal == NULL) {
        printf("palette is NULL! returning no image.\n");
        return NULL;
    }
    if (strncmp(frame_type, "PLNR", 4) == 0) {
        printf("PLNR: Not fully implemented!\n");
        int bits_per_symbol = *(chunk + frame_offset + 9);
        int dictionary_size = 1 << bits_per_symbol;
        if (len < frame_offset + 10 + dictionary_size) {
            fprintf(stderr, "2ERROR: unable to load image (%d), it is too small (%lu.)\n", res_id, len);
        }
        unsigned char *pixel_value_dictionary = malloc(dictionary_size);
        for (int i = 0; i < dictionary_size; i++) {
            pixel_value_dictionary[i] = *(unsigned char*)(chunk + frame_offset + 10 + i);
            //printf("pvd[%d] = %d\n", i, pixel_value_dictionary[i]);
        }
        int codestart = frame_offset + 10 + dictionary_size;
        unsigned char *data = chunk + codestart;
        plnr_last_value = 0;
        plnr_remaining = 0;
        plnr_num_bits_read = 0;
        unsigned char* img = create_initialized_image_rgb(width, height);
        for (int j = 0; j < height; j++) {
            //printf("[%d]: ", j);
            int row_offset = j * 4 * width;
            for (int k = 0; k < width; k++) {
                int palette_index = plnr_get_next(data, bits_per_symbol);
                gff_color_t color = cpal->color[pixel_value_dictionary[palette_index]];
                *(img + row_offset + 4*(k) + 0) = color.r;
                *(img + row_offset + 4*(k) + 1) = color.r;
                *(img + row_offset + 4*(k) + 2) = color.r;
                if (color.r == 0 && color.g == 0 && color.b == 0) {
                    *(img + row_offset + 4*(k) + 3) = 0x00;
                } else {
                    *(img + row_offset + 4*(k) + 3) = 0xFF;
                }
            }
        }
        return img;

    //int image_start = frame_offset + 10 + dictionary_size;
    } else if (strncmp(frame_type, "PLAN", 4) == 0) {
        printf("PLAN\n");
    } else {
        printf("DS1 frame detected.\n");
        unsigned char* ret_img = create_ds1_rgba(chunk, frame_offset + 4, width, height, cpal);
        if (ret_img == NULL) {
            fprintf(stderr, "Error creating image: %d\n", res_id);
        }
        return ret_img;
    }
    return NULL;
}