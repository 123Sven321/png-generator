# png-generator
png-generator is a minimalistic C header for creating PNG images from raw RGB data with 8-bit color depth.

# How to Use
png-generator provides two functions:

## generate_png()
```
int generate_png(uint8_t *color_data, size_t data_point_count, int width, int height, char *name);
```
Argument             | Description
---------------------|-----------------
**color_data**       | *8-bit array of pixel color data in RGB format `{R-pixel1, G-pixel1, B-pixel1, R-pixel2, G-pixel2, B-pixel2, ...}`. Can be generated using the `generate_color_array()` function*
**data_point_count** | *Number of elements in the raw data array, not the color array*
**width**            | *Width of the image in pixels*
**height**           | *Height of the image in pixels*
**name**             | *Name of the output PNG file, including the `.png` suffix*

This function creates a PNG image file in the same directory where the `png-generator.c` file is located.

### return
`generate_png()` returns an integer error code.
The error codes are defined as macros in the header file and indicate issues during the writing process of specific PNG chunks.
```
#define SIGNATURE_ERROR 1
#define IHDR_ERROR      2
#define IDAT_ERROR      3
#define IEND_ERROR      4
#define SUCCESS         0
```
> [!WARNING]
> An error code only indicates an issue during the writing process of a PNG fragment, not errors in the PNG data.

## generate_color_array()
```
uint8_t *generate_color_array(int *data, size_t data_point_count, int data_min, int data_max, uint32_t *heatmap, size_t color_amount);
```
Argument             | Description
---------------------|-----------------
**data**             | *Raw integer data to be visualized*
**data_point_count** | *Number of elements in the raw data array*
**data_min**         | *Smallest value in the raw data array*
**data_max**         | *Largest value in the raw data array*
**heatmap**          | *Array of colors in hexadecimal format (e.g., `0xffffff`)*
**color_amount**     | *Number of colors in the the heatmap array*

This function generates a color data array based on the given raw data and the specified colors in the heatmap array. The resulting color array can be used as input for the `generate_png()` function.

### How it works
+ Colors are treated as points in a 3D space, with **R, G,** and **B** representing the **X, y,** and **z** coordinates.
+ Lines are drawn between adjacent colors in the heatmap, creating "sections".
+ For each value in the data, the function determines:
  + Which section the value belongs to.
  + The "progress" of the value wihtin that section.
+ Using this information, the function calculates the corresponding color (a point in 3D spcae) and writes its RGB values into the color array.

### return
`generate_color_array()` returns a pointer to the generated color array.

# Dependencies
+ **zlib:** *png-generator relies on the zlib library for compressing the color data.*

# Example
A practical example of using png-generator would be visualizing the Mandelbrot set. In this case, the iteration count is converted into color data using `generate_color_array()` and then passed to the `generate_png()` function to create an image.
<br>
This example is available in the example directory.
