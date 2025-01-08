#ifndef PNG_GENERATOR
#define PNG_GENERATOR

#include <stdint.h>
#include <stdlib.h>

#define MAX_IDAT_SIZE 32000

#define SIGNATURE_ERROR 1
#define IHDR_ERROR      2
#define IDAT_ERROR      3
#define IEND_ERROR      4
#define SUCCESS         0

uint8_t *generate_color_array(int *data, size_t data_point_count, int data_min, int data_max, uint32_t *heatmap, size_t color_amount);
int generate_png(uint8_t *color_data, size_t data_size_byte, int width, int height, char *name);

#endif