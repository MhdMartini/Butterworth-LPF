//Peter Mack
#ifndef FOUR2_PRIVATE
#define FOUR2_PRIVATE

#include "cplx.h"

cplx* fft_X(int);
cplx* fft_Y(int);

void fft_shift();
void fft_Four1(cplx* (*DATA)(int),int nn,int isign);

#endif
