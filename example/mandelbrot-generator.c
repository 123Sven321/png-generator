#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "../png-generator.h"


static const int iterations = 1000;
static const int width = 1000;                                       //image width
static const int height = 1000;                                      //image height

struct Complex{
    double real;
    double imag;
};

void *iteration_steps(double start, double end, int size, double *output){

    double steps = (start * (-1) + end) / size;

    for(int i = 0; i < size; i++){
        output[i] = start + i*steps;
    }
}


int main(){

    struct Complex z;
    z.real = 0;
    z.imag = 0;

    struct Complex c;

    double *steps_real = (double *)malloc(width * sizeof(double));
    double *steps_imag = (double *)malloc(height * sizeof(double));
    iteration_steps(-2, 1, width, steps_real);                          //customize range of real part
    iteration_steps(-1.5, 1.5, height, steps_imag);                     //customize range of imaginary part

    int *mandelbrot_array = (int *)malloc(sizeof(int) * width *height);

    //generating the mandelbrotset
    unsigned int index = 0;
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){

            c.real = steps_real[x];
            c.imag = steps_imag[y];

            for(int j = 0; j < iterations; j++){
                
                double new_real = ((z.real*z.real)-(z.imag*z.imag)) + c.real;
                double new_imag = (2*z.real*z.imag) + c.imag;
                z.real = new_real;
                z.imag = new_imag;

                double betrag = z.real * z.real + z.imag * z.imag;

                if(betrag > 4){
                    mandelbrot_array[index] = j + 1;
                    break;
                }
                
            }

            if(z.real * z.real + z.imag * z.imag <= 4){
                mandelbrot_array[index] = iterations + 1;
            }

            z.real = 0;
            z.imag = 0;
            index = index + 1;

            double progress = (((double)index + 1) / ((double)width * (double)height)) * 100;
            printf("\rGenerate data: %.2f%%", progress);
            fflush(stdout);
        }
    }
    printf("\n");
    free(steps_real);
    free(steps_imag);


    int heatmap[14] = {0xAC0000, 0xffff00, 0x00ffe4, 0x00ffe4, 0x00ffe4, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000};

    //transform data to color data
    uint8_t *color_array = generate_color_array(mandelbrot_array, width*height, 1, 1001, heatmap, 14);

    printf("\nColor array written.\n\n");

    //generate png using the png-generator.h header file
    int result = generate_png(color_array, width*height*3, width, height, "mandelbrot-set.png");

    if(result == SUCCESS){
        printf("File written successfully\n");
    }else if(result == SIGNATURE_ERROR){
        printf("Error in Signature\n");
    }else if(result == IHDR_ERROR){
        printf("Error in IHDR\n");
    }else if(result == IDAT_ERROR){
        printf("Error in IDAT\n");
    }else if(result == IEND_ERROR){
        printf("Error in IEND\n");
    }

    free(mandelbrot_array);
    free(color_array);
    
    return 0;
}
