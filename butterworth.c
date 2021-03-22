#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imgIo.h"
#include "Four2.h"
#include <math.h>

static unsigned ORDER = 4;  // order of the filter
static float CUTOFF = 200;  // cutoff frequency


void print_help(){
    // print help menu
    printf("Please run the butterworth LPF as follows:\n\t./butterworth <image_path> <power>\n");
    printf("\n\t<image_path>\tFull path to an image");
    printf("\n\t<power>\t\t1 for only calculating the power of the filtered input image.\n\t\t\t0 for outputting images for the spectrum and filtered image\n");
}

float mag(float r, float i)
{
    // take real and imaginary values and return the magnitude
    return sqrt(pow(r, 2) + pow(i, 2));
}

void save_spectrum(char imgName[], float *fft_1D_array, unsigned sizeX, unsigned sizeY)
{
    // take in name of output image, a 1D float array, and original image dimensions
    // loop through every other value of the 1D float array
    // copy the magnitude of each two consecutive values to the index in a new unsigned char array
    // while looping, keep track of min and max magnitudes
    // finally normalize and save the image
    float min = 100000;
    float max = -100000;
    float temp_f;

    unsigned char *spectrum1D_u;
    float *spectrum1D_f;
    spectrum1D_f = (float*) malloc(sizeof(float) * sizeX * sizeY);
    spectrum1D_u = (unsigned char*) malloc(sizeof(unsigned char) * sizeX * sizeY);

    for (unsigned i = 0; i < 2 * sizeX * sizeY; i+=2){
        temp_f = mag(fft_1D_array[i], fft_1D_array[i + 1]);
        spectrum1D_f[i/2] = temp_f;

        if (temp_f > max){
            max = temp_f;
        }
        if (temp_f < min){
            min = temp_f;
        }
    }

    // normalize
    for (unsigned i = 0; i < sizeX * sizeY; i++){
        spectrum1D_u[i] = 255 * (spectrum1D_f[i] - min)/(max - min);
    }

    // save image
    writeImg(imgName, spectrum1D_u, sizeX, sizeY);
}


int main(int argc, char *argv[])
{
    // handle input arguments
    bool power;
    if (argc != 3){
        print_help();
        return 0;
    }
    else if ((argc == 2) && !(strcmp(argv[1], "--help"))){
        print_help();
        return 0;
    }

    power = strcmp(argv[2], "0");
    char spectrumName[] = "spectrum.pgm";
    char outName[] = "filtered.pgm";

    // get image size
    unsigned sizeX;  // width
    unsigned sizeY;  // height
    unsigned *size;
    size=(unsigned *) malloc(2);
    size = get_size(argv[1]);
    sizeX = size[0];
    sizeY = size[1];

    // read the image as 1D array, and copy its content to 1D array of twice the size - every other value
    // [real1, real2,,, realn] -> [real1, 0, real2, 0,,, realn, 0]
    unsigned char *image1D = readImg(argv[1]);

    float *image1DExp;  // expanded array
    image1DExp = (float*) calloc(sizeof(float), 2 * sizeX * sizeY);
    for (unsigned i = 0; i < sizeX * sizeY; i++){
        image1DExp[2 * i] = image1D[i];
    }

    // create array to hold the forward fourier result
    float *imgFFT;
    imgFFT = (float*) calloc(sizeof(float), 2 * sizeX * sizeY);
    for (unsigned i = 0; i < 2 * sizeX * sizeY; i++){
        imgFFT[i] = image1DExp[i];
    }

    // apply forward ft on the resulting array
    fft_Four2(imgFFT, sizeX, sizeY, false);

    // save the spectrum as a pgm image

    if (!power){
        // save the spectrum as a pgm image. Skip if power argument is 1
        save_spectrum(spectrumName, imgFFT, sizeX, sizeY);
    }

    // apply the centered Butterworth filter
    float u, v, d, filter_val;
    for (unsigned i = 0; i < sizeY; i++){
        for (unsigned j = 0; j < sizeX; j++){
            u = (float)j - (float)(sizeX/2);
            v = (float)i - (float)(sizeY/2);
            d = sqrt(u*u + v*v);
            filter_val = 1/(1 + pow(d / CUTOFF, 2));
            imgFFT[2*(sizeX * i + j)] *= filter_val;
            imgFFT[2*(sizeX * i + j) + 1] *= filter_val;
        }
    }

    // create imgFFTInv, copy imgFFT into it and apply the inverse fourier to it
    float *imgFFTInv;
    imgFFTInv = (float*) calloc(sizeof(float), 2 * sizeX * sizeY);
    for (unsigned i = 0; i < 2 * sizeX * sizeY; i++){
        imgFFTInv[i] = imgFFT[i];
    }

    // if power is true, calculate the power and leave
    if (power){
        int temp;
        long total_power = 0;
        for (unsigned i = 0; i < 2 * sizeX * sizeY; i += 2){
            temp = pow(imgFFTInv[i], 2) + pow(imgFFTInv[i + 1], 2);
            total_power += temp;
        }
        printf("Total power: %ld\n", total_power);
        return 0;
    }

    fft_Four2(imgFFTInv, sizeX, sizeY, true);

    unsigned char *outImg;
    outImg = (unsigned char *) calloc(sizeof(unsigned char), sizeX * sizeY);
    for (unsigned i = 0; i < sizeX * sizeY; i++){
        // take absolute to account for filter shifting
        outImg[i] = abs(imgFFTInv[2 * i]);
    }
    writeImg(outName, outImg, sizeX, sizeY);

    return 0;

}
