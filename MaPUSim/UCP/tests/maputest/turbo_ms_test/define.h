#ifndef DEFINE_H
#define DEFINE_H

#define MAX_VALUE		2147483647.0 
#define MIN_VALUE		-2147483647.0 

#define log1_8			0 //-2.079441541679836

//no modification 
#define CRC_TYPE		0
#define MAX_INFO_LEN	        6120
#define MAX_CB_LEN		6144

#define MAX_SB_LEN		1536
#define MAX_SW_LEN		128 

#define MAX_MEM_LEN		192 
#define MAX_ITERATION	        9


//can be modified  
#define CRC_SW        1 //CRC switch
#define MODU_ORDER		1
#define CB_LEN			768
#define CB_NUM      1
#define PU_NUM			1 //fixed
#define SW_LEN			128
#define SW_NUM			6  // (CB_LEN/PU_NUM/SW_NUM) should be less than MAX_SW_LEN
#define CRC_CHECK_NUM   2

#define N_STATE			8 // state number 

#define EbN0_START		-3.0
#define EbN0_STEP		0.1
#define EbN0_NUM		1

#define SCALING_FACTOR	        0x18

#define MAX(a, b) (a >= b ? a : b)
typedef float state_metric[8];
typedef float branch_metric[2];


typedef struct{
	float real;
	float imag;
}Complex;

#endif
