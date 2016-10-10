/*============================================================================*/
//  Information
/*============================================================================*/
/* 
*  @file    demodulation_serial.h
*  @brief   
*  @author  wujunning
*  @date    2014/03/14
/*----------------------------------------------------------------------------*/

#ifndef DEMODULATION_SERIAL_H
#define DEMODULATION_SERIAL_H

/*============================================================================*/
//  Header files
/*============================================================================*/
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "define.h"

void demodulation_serial(float *output_soft_bit,Complex * input_symbol,int len, int qm);

#endif


