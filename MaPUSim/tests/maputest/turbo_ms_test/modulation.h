#ifndef MODULATION_H
#define MODULATION_H

#include "stdio.h"
#include "math.h"
#include "stdlib.h"

#include "define.h"

void modulation(Complex *output, int *input, int sym_len, int modu_order);
void demodulation(float *LLR, Complex *received_sym, float SNR, int sym_len, int modu_order);
void modulation_s(Complex * symbols, int* input_bits, int b_len,int qm);
//void demodulation_serial(float *output_soft_bit,Complex * input_symbol,int len, int qm);
void ULRecvDemodulaton(float *output_soft_bit,Complex * input_symbol,float noise_var,int len, int qm);

#endif
