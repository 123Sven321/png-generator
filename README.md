# What is png-generator?
png-generator is a minimalistic C header for creating PNG images from raw RGB data with 8-bit color depth.

# How it's used.
png-generator provides a single function:
```
generate_png(uint8_t *color_data, size_t color_data_size, int width, int height, char *name)
```
+ **color_data:** &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;*8-bit array of pixel color data in format {R-pixel1, G-pixel1, B-pixel1, R-pixel2, G-pixel2, B-pixel2, ...}*
+ **color_data_size:** &nbsp;&nbsp;*size of the color data array in byte*
+ **width:** &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;*width of the image in pixel*
+ **height:** &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;*height of the image in pixel*
+ **name:** &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;*name of the file including the suffix .png*
<br>
This will create a PNG image file in the same directory the png-generator.c file is located.

### return
generate_png() returns an int as error code.
The codes are defined as macros in the header file and can help to find the faulty code.
```
#define SIGNATURE_ERROR 1
#define IHDR_ERROR      2
#define IDAT_ERROR      3
#define IEND_ERROR      4
#define SUCCESS         0
```
An error value only indicates an issue in the writing process of the corresponding png fragment, not errors in the data itself.


# Dependencies
+ **zlib:** *png-generator relies on the zlib library to compress the color data, so zlib has to be installed.*


# Example
An example would be visualizing the Mandelbrot set, where the iteration count is converted to color data and passed to the generate_png() function.
This example can be found in the example directory.
