/*============================================================================*/
//  Information
/*============================================================================*/
/* 
*  @file    modulation.c
*  @brief   
*  @author  wujunning
*  @date    2013/09/02
/*----------------------------------------------------------------------------*/

#ifndef MODULATION_C
#define MODULATION_C

/*============================================================================*/
//  Header files
/*============================================================================*/
#include "modulation.h"

/*============================================================================*/
//  Function definition
/*============================================================================*/
/*----------------------------------------------------------------------------*/
// void modulation (Complex* symbols,int s_len, int* input_bits, int b_len, int qm)
// @brief: Mapping the scrambled bits into Complex symbols(modulation mapper)
// @symbols: output: the output symbol stream after modulation mapping
// @input_bits: input: the input bit stream to mudulation mapper module
// @b_len: input: the length of the scrambled bits
// @qm: input: the modulation scheme,which is BPSK,QPSK,16QAM,64QAM
/*----------------------------------------------------------------------------*/

void modulation(Complex * symbols, int* input_bits, int b_len,int qm){

	//Variables
	int i;
	int temp;
	float  one_over_sqrt_2  = 1/sqrt(2.0);
	float  one_over_sqrt_10 = 1/sqrt(10.0);
	float  one_over_sqrt_42 = 1/sqrt(42.0);

	//memory allocate

	

	//Execute
	switch(qm)
	{
	case 0:
		// 3GPP TS 36.211 v10.7.0 section 7.1.1
        for(i=0; i<b_len; i++)
        {
            if(0 == input_bits[i])
            {
                (symbols+i)->real = one_over_sqrt_2;
                (symbols+i)->imag = one_over_sqrt_2;
            }else{
                (symbols+i)->real = -one_over_sqrt_2;
                (symbols+i)->imag = -one_over_sqrt_2;
            }
        }
		//*M_symb = b_len;
        break;
	case 1:
		// 3GPP TS 36.211 v10.7.0 section 7.1.2
        for(i=0; i<(b_len/2); i++)
        {
            switch((input_bits[i*2] << 1) |
                   input_bits[i*2+1])
            {
            case 0:
                (symbols+i)->real = +one_over_sqrt_2;
                (symbols+i)->imag = +one_over_sqrt_2;
                break;
            case 1:
                (symbols+i)->real = +one_over_sqrt_2;
                (symbols+i)->imag = -one_over_sqrt_2;
                break;
            case 2:
                (symbols+i)->real = -one_over_sqrt_2;
                (symbols+i)->imag = +one_over_sqrt_2;
                break;
            case 3:
                (symbols+i)->real = -one_over_sqrt_2;
                (symbols+i)->imag = -one_over_sqrt_2;
                break;
            }
        }
        //*M_symb = (b_len/2);
        if((b_len % 2) != 0)
        {
            //*M_symb = (b_len/2) + 1;
            // Add a trailing zero
            if(0 == input_bits[b_len-1])
            {
                (symbols+i)->real = +one_over_sqrt_2;
                (symbols+i)->imag = +one_over_sqrt_2;
            }else{
                (symbols+i)->real = -one_over_sqrt_2;
                (symbols+i)->imag = +one_over_sqrt_2;
            }
        }
        break;
	case 2:
		// 3GPP TS 36.211 v10.7.0 section 7.1.3
        for(i=0; i<(b_len/4); i++)
        {
            switch((input_bits[i*4+0] << 3) |
                   (input_bits[i*4+1] << 2) |
                   (input_bits[i*4+2] << 1) |
                   input_bits[i*4+3])
            {
            case 0:
                (symbols+i)->real = +1*one_over_sqrt_10;
                (symbols+i)->imag = +1*one_over_sqrt_10;
                break;
            case 1:
                (symbols+i)->real = +1*one_over_sqrt_10;
                (symbols+i)->imag = +3*one_over_sqrt_10;
                break;
            case 2:
                (symbols+i)->real = +3*one_over_sqrt_10;
                (symbols+i)->imag = +1*one_over_sqrt_10;
                break;
            case 3:
                (symbols+i)->real = +3*one_over_sqrt_10;
                (symbols+i)->imag = +3*one_over_sqrt_10;
                break;
            case 4:
                (symbols+i)->real = +1*one_over_sqrt_10;
                (symbols+i)->imag = -1*one_over_sqrt_10;
                break;
            case 5:
                (symbols+i)->real = +1*one_over_sqrt_10;
                (symbols+i)->imag = -3*one_over_sqrt_10;
                break;
            case 6:
                (symbols+i)->real = +3*one_over_sqrt_10;
                (symbols+i)->imag = -1*one_over_sqrt_10;
                break;
            case 7:
                (symbols+i)->real = +3*one_over_sqrt_10;
                (symbols+i)->imag = -3*one_over_sqrt_10;
                break;
            case 8:
                (symbols+i)->real = -1*one_over_sqrt_10;
                (symbols+i)->imag = +1*one_over_sqrt_10;
                break;
            case 9:
                (symbols+i)->real = -1*one_over_sqrt_10;
                (symbols+i)->imag = +3*one_over_sqrt_10;
                break;
            case 10:
                (symbols+i)->real = -3*one_over_sqrt_10;
                (symbols+i)->imag = +1*one_over_sqrt_10;
                break;
            case 11:
                (symbols+i)->real = -3*one_over_sqrt_10;
                (symbols+i)->imag = +3*one_over_sqrt_10;
                break;
            case 12:
                (symbols+i)->real = -1*one_over_sqrt_10;
                (symbols+i)->imag = -1*one_over_sqrt_10;
                break;
            case 13:
                (symbols+i)->real = -1*one_over_sqrt_10;
                (symbols+i)->imag = -3*one_over_sqrt_10;
                break;
            case 14:
                (symbols+i)->real = -3*one_over_sqrt_10;
                (symbols+i)->imag = -1*one_over_sqrt_10;
                break;
            case 15:
                (symbols+i)->real = -3*one_over_sqrt_10;
                (symbols+i)->imag = -3*one_over_sqrt_10;
                break;
            }
        }
        //*M_symb = (b_len/4);
        if((b_len % 4) != 0)
        {
            //*M_symb = (b_len/4) + 1;
            if((b_len % 4) == 1)
            {
                temp = input_bits[b_len-1] << 3;
            }else if((b_len % 4) == 2){
                temp = ((input_bits[b_len-2] << 3) |
                        (input_bits[b_len-1] << 2));
            }else if((b_len % 4) == 3){
                temp = ((input_bits[b_len-3] << 3) |
                         (input_bits[b_len-2] << 2) |
                         (input_bits[b_len-1] << 1));
            }
            switch(temp)
            {
            case 0:
                (symbols+b_len/4)->real = +1*one_over_sqrt_10;
                (symbols+b_len/4)->imag = +1*one_over_sqrt_10;
                break;
            case 1:
                (symbols+b_len/4)->real = +1*one_over_sqrt_10;
                (symbols+b_len/4)->imag = +3*one_over_sqrt_10;
                break;
            case 2:
                (symbols+b_len/4)->real = +3*one_over_sqrt_10;
                (symbols+b_len/4)->imag = +1*one_over_sqrt_10;
                break;
            case 3:
                (symbols+b_len/4)->real = +3*one_over_sqrt_10;
                (symbols+b_len/4)->imag = +3*one_over_sqrt_10;
                break;
            case 4:
                (symbols+b_len/4)->real = +1*one_over_sqrt_10;
                (symbols+b_len/4)->imag = -1*one_over_sqrt_10;
                break;
            case 5:
                (symbols+b_len/4)->real = +1*one_over_sqrt_10;
                (symbols+b_len/4)->imag = -3*one_over_sqrt_10;
                break;
            case 6:
                (symbols+b_len/4)->real = +3*one_over_sqrt_10;
                (symbols+b_len/4)->imag = -1*one_over_sqrt_10;
                break;
            case 7:
                (symbols+b_len/4)->real = +3*one_over_sqrt_10;
                (symbols+b_len/4)->imag = -3*one_over_sqrt_10;
                break;
            case 8:
                (symbols+b_len/4)->real = -1*one_over_sqrt_10;
                (symbols+b_len/4)->imag = +1*one_over_sqrt_10;
                break;
            case 9:
                (symbols+b_len/4)->real = -1*one_over_sqrt_10;
                (symbols+b_len/4)->imag = +3*one_over_sqrt_10;
                break;
            case 10:
                (symbols+b_len/4)->real = -3*one_over_sqrt_10;
                (symbols+b_len/4)->imag = +1*one_over_sqrt_10;
                break;
            case 11:
                (symbols+b_len/4)->real = -3*one_over_sqrt_10;
                (symbols+b_len/4)->imag = +3*one_over_sqrt_10;
                break;
            case 12:
                (symbols+b_len/4)->real = -1*one_over_sqrt_10;
                (symbols+b_len/4)->imag = -1*one_over_sqrt_10;
                break;
            case 13:
                (symbols+b_len/4)->real = -1*one_over_sqrt_10;
                (symbols+b_len/4)->imag = -3*one_over_sqrt_10;
                break;
            case 14:
                (symbols+b_len/4)->real = -3*one_over_sqrt_10;
                (symbols+b_len/4)->imag = -1*one_over_sqrt_10;
                break;
            case 15:
                (symbols+b_len/4)->real = -3*one_over_sqrt_10;
                (symbols+b_len/4)->imag = -3*one_over_sqrt_10;
                break;
            }
        }
        break;
	case 3:
		// 3GPP TS 36.211 v10.7.0 section 7.1.4
        for(i=0; i<(b_len/6); i++)
        {
            switch((input_bits[i*6+0] << 5) |
                   (input_bits[i*6+1] << 4) |
                   (input_bits[i*6+2] << 3) |
                   (input_bits[i*6+3] << 2) |
                   (input_bits[i*6+4] << 1) |
                   input_bits[i*6+5])
            {
            case 0:
                (symbols+i)->real = +3*one_over_sqrt_42;
                (symbols+i)->imag = +3*one_over_sqrt_42;
                break;
            case 1:
                (symbols+i)->real = +3*one_over_sqrt_42;
                (symbols+i)->imag = +1*one_over_sqrt_42;
                break;
            case 2:
                (symbols+i)->real = +1*one_over_sqrt_42;
                (symbols+i)->imag = +3*one_over_sqrt_42;
                break;
            case 3:
                (symbols+i)->real = +1*one_over_sqrt_42;
                (symbols+i)->imag = +1*one_over_sqrt_42;
                break;
            case 4:
                (symbols+i)->real = +3*one_over_sqrt_42;
                (symbols+i)->imag = +5*one_over_sqrt_42;
                break;
            case 5:
                (symbols+i)->real = +3*one_over_sqrt_42;
                (symbols+i)->imag = +7*one_over_sqrt_42;
                break;
            case 6:
                (symbols+i)->real = +1*one_over_sqrt_42;
                (symbols+i)->imag = +5*one_over_sqrt_42;
                break;
            case 7:
                (symbols+i)->real = +1*one_over_sqrt_42;
                (symbols+i)->imag = +7*one_over_sqrt_42;
                break;
            case 8:
                (symbols+i)->real = +5*one_over_sqrt_42;
                (symbols+i)->imag = +3*one_over_sqrt_42;
                break;
            case 9:
                (symbols+i)->real = +5*one_over_sqrt_42;
                (symbols+i)->imag = +1*one_over_sqrt_42;
                break;
            case 10:
                (symbols+i)->real = +7*one_over_sqrt_42;
                (symbols+i)->imag = +3*one_over_sqrt_42;
                break;
            case 11:
                (symbols+i)->real = +7*one_over_sqrt_42;
                (symbols+i)->imag = +1*one_over_sqrt_42;
                break;
            case 12:
                (symbols+i)->real = +5*one_over_sqrt_42;
                (symbols+i)->imag = +5*one_over_sqrt_42;
                break;
            case 13:
                (symbols+i)->real = +5*one_over_sqrt_42;
                (symbols+i)->imag = +7*one_over_sqrt_42;
                break;
            case 14:
                (symbols+i)->real = +7*one_over_sqrt_42;
                (symbols+i)->imag = +5*one_over_sqrt_42;
                break;
            case 15:
                (symbols+i)->real = +7*one_over_sqrt_42;
                (symbols+i)->imag = +7*one_over_sqrt_42;
                break;
            case 16:
                (symbols+i)->real = +3*one_over_sqrt_42;
                (symbols+i)->imag = -3*one_over_sqrt_42;
                break;
            case 17:
                (symbols+i)->real = +3*one_over_sqrt_42;
                (symbols+i)->imag = -1*one_over_sqrt_42;
                break;
            case 18:
                (symbols+i)->real = +1*one_over_sqrt_42;
                (symbols+i)->imag = -3*one_over_sqrt_42;
                break;
            case 19:
                (symbols+i)->real = +1*one_over_sqrt_42;
                (symbols+i)->imag = -1*one_over_sqrt_42;
                break;
            case 20:
                (symbols+i)->real = +3*one_over_sqrt_42;
                (symbols+i)->imag = -5*one_over_sqrt_42;
                break;
            case 21:
                (symbols+i)->real = +3*one_over_sqrt_42;
                (symbols+i)->imag = -7*one_over_sqrt_42;
                break;
            case 22:
                (symbols+i)->real = +1*one_over_sqrt_42;
                (symbols+i)->imag = -5*one_over_sqrt_42;
                break;
            case 23:
                (symbols+i)->real = +1*one_over_sqrt_42;
                (symbols+i)->imag = -7*one_over_sqrt_42;
                break;
            case 24:
                (symbols+i)->real = +5*one_over_sqrt_42;
                (symbols+i)->imag = -3*one_over_sqrt_42;
                break;
            case 25:
                (symbols+i)->real = +5*one_over_sqrt_42;
                (symbols+i)->imag = -1*one_over_sqrt_42;
                break;
            case 26:
                (symbols+i)->real = +7*one_over_sqrt_42;
                (symbols+i)->imag = -3*one_over_sqrt_42;
                break;
            case 27:
                (symbols+i)->real = +7*one_over_sqrt_42;
                (symbols+i)->imag = -1*one_over_sqrt_42;
                break;
            case 28:
                (symbols+i)->real = +5*one_over_sqrt_42;
                (symbols+i)->imag = -5*one_over_sqrt_42;
                break;
            case 29:
                (symbols+i)->real = +5*one_over_sqrt_42;
                (symbols+i)->imag = -7*one_over_sqrt_42;
                break;
            case 30:
                (symbols+i)->real = +7*one_over_sqrt_42;
                (symbols+i)->imag = -5*one_over_sqrt_42;
                break;
            case 31:
                (symbols+i)->real = +7*one_over_sqrt_42;
                (symbols+i)->imag = -7*one_over_sqrt_42;
                break;
            case 32:
                (symbols+i)->real = -3*one_over_sqrt_42;
                (symbols+i)->imag = +3*one_over_sqrt_42;
                break;
            case 33:
                (symbols+i)->real = -3*one_over_sqrt_42;
                (symbols+i)->imag = +1*one_over_sqrt_42;
                break;
            case 34:
                (symbols+i)->real = -1*one_over_sqrt_42;
                (symbols+i)->imag = +3*one_over_sqrt_42;
                break;
            case 35:
                (symbols+i)->real = -1*one_over_sqrt_42;
                (symbols+i)->imag = +1*one_over_sqrt_42;
                break;
            case 36:
                (symbols+i)->real = -3*one_over_sqrt_42;
                (symbols+i)->imag = +5*one_over_sqrt_42;
                break;
            case 37:
                (symbols+i)->real = -3*one_over_sqrt_42;
                (symbols+i)->imag = +7*one_over_sqrt_42;
                break;
            case 38:
                (symbols+i)->real = -1*one_over_sqrt_42;
                (symbols+i)->imag = +5*one_over_sqrt_42;
                break;
            case 39:
                (symbols+i)->real = -1*one_over_sqrt_42;
                (symbols+i)->imag = +7*one_over_sqrt_42;
                break;
            case 40:
                (symbols+i)->real = -5*one_over_sqrt_42;
                (symbols+i)->imag = +3*one_over_sqrt_42;
                break;
            case 41:
                (symbols+i)->real = -5*one_over_sqrt_42;
                (symbols+i)->imag = +1*one_over_sqrt_42;
                break;
            case 42:
                (symbols+i)->real = -7*one_over_sqrt_42;
                (symbols+i)->imag = +3*one_over_sqrt_42;
                break;
            case 43:
                (symbols+i)->real = -7*one_over_sqrt_42;
                (symbols+i)->imag = +1*one_over_sqrt_42;
                break;
            case 44:
                (symbols+i)->real = -5*one_over_sqrt_42;
                (symbols+i)->imag = +5*one_over_sqrt_42;
                break;
            case 45:
                (symbols+i)->real = -5*one_over_sqrt_42;
                (symbols+i)->imag = +7*one_over_sqrt_42;
                break;
            case 46:
                (symbols+i)->real = -7*one_over_sqrt_42;
                (symbols+i)->imag = +5*one_over_sqrt_42;
                break;
            case 47:
                (symbols+i)->real = -7*one_over_sqrt_42;
                (symbols+i)->imag = +7*one_over_sqrt_42;
                break;
            case 48:
                (symbols+i)->real = -3*one_over_sqrt_42;
                (symbols+i)->imag = -3*one_over_sqrt_42;
                break;
            case 49:
                (symbols+i)->real = -3*one_over_sqrt_42;
                (symbols+i)->imag = -1*one_over_sqrt_42;
                break;
            case 50:
                (symbols+i)->real = -1*one_over_sqrt_42;
                (symbols+i)->imag = -3*one_over_sqrt_42;
                break;
            case 51:
                (symbols+i)->real = -1*one_over_sqrt_42;
                (symbols+i)->imag = -1*one_over_sqrt_42;
                break;
            case 52:
                (symbols+i)->real = -3*one_over_sqrt_42;
                (symbols+i)->imag = -5*one_over_sqrt_42;
                break;
            case 53:
                (symbols+i)->real = -3*one_over_sqrt_42;
                (symbols+i)->imag = -7*one_over_sqrt_42;
                break;
            case 54:
                (symbols+i)->real = -1*one_over_sqrt_42;
                (symbols+i)->imag = -5*one_over_sqrt_42;
                break;
            case 55:
                (symbols+i)->real = -1*one_over_sqrt_42;
                (symbols+i)->imag = -7*one_over_sqrt_42;
                break;
            case 56:
                (symbols+i)->real = -5*one_over_sqrt_42;
                (symbols+i)->imag = -3*one_over_sqrt_42;
                break;
            case 57:
                (symbols+i)->real = -5*one_over_sqrt_42;
                (symbols+i)->imag = -1*one_over_sqrt_42;
                break;
            case 58:
                (symbols+i)->real = -7*one_over_sqrt_42;
                (symbols+i)->imag = -3*one_over_sqrt_42;
                break;
            case 59:
                (symbols+i)->real = -7*one_over_sqrt_42;
                (symbols+i)->imag = -1*one_over_sqrt_42;
                break;
            case 60:
                (symbols+i)->real = -5*one_over_sqrt_42;
                (symbols+i)->imag = -5*one_over_sqrt_42;
                break;
            case 61:
                (symbols+i)->real = -5*one_over_sqrt_42;
                (symbols+i)->imag = -7*one_over_sqrt_42;
                break;
            case 62:
                (symbols+i)->real = -7*one_over_sqrt_42;
                (symbols+i)->imag = -5*one_over_sqrt_42;
                break;
            case 63:
                (symbols+i)->real = -7*one_over_sqrt_42;
                (symbols+i)->imag = -7*one_over_sqrt_42;
                break;
            }
        }
        //*M_symb = (b_len/6);
        if((b_len % 6) != 0)
        {
            //*M_symb = (b_len/6) + 1;
            if((b_len % 6) == 1)
            {
                temp = input_bits[b_len-1] << 5;
            }else if((b_len % 6) == 2){
                temp = ((input_bits[b_len-2] << 5) |
                         (input_bits[b_len-1] << 4));
            }else if((b_len % 6) == 3){
                temp = ((input_bits[b_len-3] << 5) |
                         (input_bits[b_len-2] << 4) |
                         (input_bits[b_len-1] << 3));
            }else if((b_len % 6) == 4){
                temp = ((input_bits[b_len-4] << 5) |
                         (input_bits[b_len-3] << 4) |
                         (input_bits[b_len-2] << 3) |
                         (input_bits[b_len-1] << 2));
            }else if((b_len % 6) == 5){
                temp = ((input_bits[b_len-5] << 5) |
                         (input_bits[b_len-4] << 4) |
                         (input_bits[b_len-3] << 3) |
                         (input_bits[b_len-2] << 2) |
                         (input_bits[b_len-1] << 1));
            }
            switch(temp)
            {
            case 0:
                (symbols+b_len/6)->real = +3*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +3*one_over_sqrt_42;
                break;
            case 1:
                (symbols+b_len/6)->real = +3*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +1*one_over_sqrt_42;
                break;
            case 2:
                (symbols+b_len/6)->real = +1*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +3*one_over_sqrt_42;
                break;
            case 3:
                (symbols+b_len/6)->real = +1*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +1*one_over_sqrt_42;
                break;
            case 4:
                (symbols+b_len/6)->real = +3*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +5*one_over_sqrt_42;
                break;
            case 5:
                (symbols+b_len/6)->real = +3*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +7*one_over_sqrt_42;
                break;
            case 6:
                (symbols+b_len/6)->real = +1*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +5*one_over_sqrt_42;
                break;
            case 7:
                (symbols+b_len/6)->real = +1*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +7*one_over_sqrt_42;
                break;
            case 8:
                (symbols+b_len/6)->real = +5*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +3*one_over_sqrt_42;
                break;
            case 9:
                (symbols+b_len/6)->real = +5*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +1*one_over_sqrt_42;
                break;
            case 10:
                (symbols+b_len/6)->real = +7*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +3*one_over_sqrt_42;
                break;
            case 11:
                (symbols+b_len/6)->real = +7*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +1*one_over_sqrt_42;
                break;
            case 12:
                (symbols+b_len/6)->real = +5*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +5*one_over_sqrt_42;
                break;
            case 13:
                (symbols+b_len/6)->real = +5*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +7*one_over_sqrt_42;
                break;
            case 14:
                (symbols+b_len/6)->real = +7*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +5*one_over_sqrt_42;
                break;
            case 15:
                (symbols+b_len/6)->real = +7*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +7*one_over_sqrt_42;
                break;
            case 16:
                (symbols+b_len/6)->real = +3*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -3*one_over_sqrt_42;
                break;
            case 17:
                (symbols+b_len/6)->real = +3*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -1*one_over_sqrt_42;
                break;
            case 18:
                (symbols+b_len/6)->real = +1*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -3*one_over_sqrt_42;
                break;
            case 19:
                (symbols+b_len/6)->real = +1*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -1*one_over_sqrt_42;
                break;
            case 20:
                (symbols+b_len/6)->real = +3*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -5*one_over_sqrt_42;
                break;
            case 21:
                (symbols+b_len/6)->real = +3*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -7*one_over_sqrt_42;
                break;
            case 22:
                (symbols+b_len/6)->real = +1*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -5*one_over_sqrt_42;
                break;
            case 23:
                (symbols+b_len/6)->real = +1*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -7*one_over_sqrt_42;
                break;
            case 24:
                (symbols+b_len/6)->real = +5*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -3*one_over_sqrt_42;
                break;
            case 25:
                (symbols+b_len/6)->real = +5*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -1*one_over_sqrt_42;
                break;
            case 26:
                (symbols+b_len/6)->real = +7*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -3*one_over_sqrt_42;
                break;
            case 27:
                (symbols+b_len/6)->real = +7*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -1*one_over_sqrt_42;
                break;
            case 28:
                (symbols+b_len/6)->real = +5*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -5*one_over_sqrt_42;
                break;
            case 29:
                (symbols+b_len/6)->real = +5*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -7*one_over_sqrt_42;
                break;
            case 30:
                (symbols+b_len/6)->real = +7*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -5*one_over_sqrt_42;
                break;
            case 31:
                (symbols+b_len/6)->real = +7*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -7*one_over_sqrt_42;
                break;
            case 32:
                (symbols+b_len/6)->real = -3*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +3*one_over_sqrt_42;
                break;
            case 33:
                (symbols+b_len/6)->real = -3*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +1*one_over_sqrt_42;
                break;
            case 34:
                (symbols+b_len/6)->real = -1*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +3*one_over_sqrt_42;
                break;
            case 35:
                (symbols+b_len/6)->real = -1*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +1*one_over_sqrt_42;
                break;
            case 36:
                (symbols+b_len/6)->real = -3*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +5*one_over_sqrt_42;
                break;
            case 37:
                (symbols+b_len/6)->real = -3*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +7*one_over_sqrt_42;
                break;
            case 38:
                (symbols+b_len/6)->real = -1*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +5*one_over_sqrt_42;
                break;
            case 39:
                (symbols+b_len/6)->real = -1*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +7*one_over_sqrt_42;
                break;
            case 40:
                (symbols+b_len/6)->real = -5*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +3*one_over_sqrt_42;
                break;
            case 41:
                (symbols+b_len/6)->real = -5*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +1*one_over_sqrt_42;
                break;
            case 42:
                (symbols+b_len/6)->real = -7*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +3*one_over_sqrt_42;
                break;
            case 43:
                (symbols+b_len/6)->real = -7*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +1*one_over_sqrt_42;
                break;
            case 44:
                (symbols+b_len/6)->real = -5*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +5*one_over_sqrt_42;
                break;
            case 45:
                (symbols+b_len/6)->real = -5*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +7*one_over_sqrt_42;
                break;
            case 46:
                (symbols+b_len/6)->real = -7*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +5*one_over_sqrt_42;
                break;
            case 47:
                (symbols+b_len/6)->real = -7*one_over_sqrt_42;
                (symbols+b_len/6)->imag = +7*one_over_sqrt_42;
                break;
            case 48:
                (symbols+b_len/6)->real = -3*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -3*one_over_sqrt_42;
                break;
            case 49:
                (symbols+b_len/6)->real = -3*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -1*one_over_sqrt_42;
                break;
            case 50:
                (symbols+b_len/6)->real = -1*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -3*one_over_sqrt_42;
                break;
            case 51:
                (symbols+b_len/6)->real = -1*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -1*one_over_sqrt_42;
                break;
            case 52:
                (symbols+b_len/6)->real = -3*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -5*one_over_sqrt_42;
                break;
            case 53:
                (symbols+b_len/6)->real = -3*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -7*one_over_sqrt_42;
                break;
            case 54:
                (symbols+b_len/6)->real = -1*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -5*one_over_sqrt_42;
                break;
            case 55:
                (symbols+b_len/6)->real = -1*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -7*one_over_sqrt_42;
                break;
            case 56:
                (symbols+b_len/6)->real = -5*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -3*one_over_sqrt_42;
                break;
            case 57:
                (symbols+b_len/6)->real = -5*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -1*one_over_sqrt_42;
                break;
            case 58:
                (symbols+b_len/6)->real = -7*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -3*one_over_sqrt_42;
                break;
            case 59:
                (symbols+b_len/6)->real = -7*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -1*one_over_sqrt_42;
                break;
            case 60:
                (symbols+b_len/6)->real = -5*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -5*one_over_sqrt_42;
                break;
            case 61:
                (symbols+b_len/6)->real = -5*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -7*one_over_sqrt_42;
                break;
            case 62:
                (symbols+b_len/6)->real = -7*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -5*one_over_sqrt_42;
                break;
            case 63:
                (symbols+b_len/6)->real = -7*one_over_sqrt_42;
                (symbols+b_len/6)->imag = -7*one_over_sqrt_42;
                break;
            }
        }
        break;

	}

}


#endif
