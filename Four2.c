/**************************
 * Peter Mack
 * Code originally written as a student in C++ in 2016
 * Modified and converted back to C in 2019
 * Contains 2D FFT functionality
 */

#include "cplx.h"
#include "Four2_Private.h"
#include "Four2.h"
#include "math.h"

//Pointer to working Image
struct cplx *fft_image;
//working image size
unsigned fft_sizeX;
unsigned fft_sizeY;
//working counter
unsigned fft_rr;

//index working image elments by x axis
cplx *fft_X(int i){
  return &(fft_image[i + fft_rr*fft_sizeX]);
}

//index working image elments by x axis
cplx *fft_Y(int i){
  return &(fft_image[fft_rr + i*fft_sizeX]);
}

//center spectrum in frequency domain by inverting some pixels in
//  the spacial domain
void fft_shift(){
  for(unsigned j = 0; j < fft_sizeY; j++){
    for(unsigned i = 0; i < fft_sizeX; i++){
      fft_image[i+j*fft_sizeX].real *= ((i+j) % 2) ? -1.0 : 1.0;
      fft_image[i+j*fft_sizeX].imag *= ((i+j) % 2) ? -1.0 : 1.0;

    }
  }
}

//compute 2d FFT of image:  WARNING: Not thread safe
void fft_Four2(float *im, //image
               unsigned sX, //width
               unsigned sY, //height
               bool inv){ //inverse fft when true

  //transfer image and size to static variables
  fft_image = (cplx*)im; //note: pointers are compatible by design of cplx
  fft_sizeX=sX;
  fft_sizeY=sY;

  int isign = inv ? 1 : -1;  // 1 if inv else -1
  //invert every other pixel in spacial domain for centering in frequency domain
  //only for forward fft
  if(!inv){
    fft_shift();
  }

  //run 1D fft for each row
  for(fft_rr = 0; fft_rr < fft_sizeY; fft_rr++){
    fft_Four1(fft_X,fft_sizeX,isign);
  }

  //run 2D fft: run 1D fft for each column
  for(fft_rr = 0; fft_rr < fft_sizeX; fft_rr++){
    fft_Four1(fft_Y, fft_sizeY, isign);
  }

  //correct magnitude and uncenter result for inverse FFT
  if(inv){
    for(unsigned i = 0; i < fft_sizeX*fft_sizeY; i++){
      fft_image[i].real /= fft_sizeX*fft_sizeY;
      fft_image[i].imag /= fft_sizeX*fft_sizeY;
    }
    fft_shift();
  }

}

//macro for fft
#define SWAP(a,b) temp=(a);(a)=(b);(b)=temp

//1D Fast Four Transform Provided by Professor Megherbi
//  (modified for C++ and application by Peter Mack as a student)
//  (modified further and converted back to C by Peter Mack as a TA)
void fft_Four1(cplx* (*DATA)(int),
               int nn,
               int isign)
{
  int mmax,m,i,jstep,j;
  double wtemp,theta;
  cplx w, wp;
  cplx tempc;
  float temp;
  i=0;
  for (j=0;j<nn;j+=1) {
    if (i > j) {
            SWAP(DATA(i)->real,DATA(j)->real);
            SWAP(DATA(i)->imag,DATA(j)->imag);

    }
    m=nn/2;
    while (m >= 1 && i >= m) {
            i -= m;
            m >>= 1;
    }
    i += m;
  }
  mmax=1;
  while (nn > mmax) {
    jstep=mmax*2;
    theta=2*asin(1)/(isign*(mmax));
    wtemp=sin(0.5*theta);
    wp.real = -2.0*wtemp*wtemp;
    wp.imag = sin(theta);
    w.real = 1.0;
    w.imag = 0.0;
    for (m=0;m<mmax;m+=1) {
      for (j=m;j<nn;j+=jstep) {
        i= j + mmax;
        tempc.real=w.real * DATA(i)->real - w.imag * DATA(i)->imag;
        tempc.imag=w.real * DATA(i)->imag + w.imag * DATA(i)->real;
        DATA(i)->real=DATA(j)->real-tempc.real;
        DATA(i)->imag=DATA(j)->imag-tempc.imag;
        DATA(j)->real+=tempc.real;
        DATA(j)->imag+=tempc.imag;
      }

      w.real=(wtemp=w.real)*wp.real-w.imag*wp.imag+w.real;
      w.imag=w.imag*wp.real+wtemp*wp.imag+w.imag;

    }
    mmax=jstep;
  }
}
