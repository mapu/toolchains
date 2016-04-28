/* Function Name: Cooley-Tuky fft arithmetic in 4 APE
 * Author       : zhangshanshan 
 * Description  : This ARM function configures DMA and call SPU program.                
 */

// The header file below defines API function to call APC programs
#include "SPULoad.h"
#include "SPULoad_2.h"
//source signal and factor
#include "data.h"
#include <apc/ape.h>
#include <apc/loader.h>
#include <common.h>
#include <exports.h>
 
   float data_out_col[1024][2048];//col_fft
   float data_out_row[1024][2048];//col_row
//main function
int FFT_arm(int argc, char * argv[]){
   volatile int k;
//print head address
    printf("input_data_head\n");   printf("%x\n", (int)data_in) ;
    printf("input_factor_head\n"); printf("%x\n", (int)factor_in) ;
    printf("output_col_head\n");   printf("%x\n", (int)data_out_col) ;
    printf("output_row_head\n");   printf("%x\n", (int)data_out_row) ;

    int  ape0_base=0x41000000;
    int  ape1_base=0x41400000;
    int  ape2_base=0x41800000;
    int  ape3_base=0x41c00000; 
    
    int  dmac_base=0x50000000;//DMAC reg

    volatile    int *ape0_addr=(int *)ape0_base;
    volatile    int *ape1_addr=(int *)ape1_base;
    volatile    int *ape2_addr=(int *)ape2_base;
    volatile    int *ape3_addr=(int *)ape3_base;  
    
    volatile    int *dmac_addr=(int *)dmac_base;//DMAC reg
// FFT Rows Calculate times is 4
// Per data Size: 0x400000 / 4 (Byte)
//////////////////////////////////////////////////////////////////////////////
//col_fft
/////////////////////////////////////////////////////////////////////////////
 for(k=0; k<4; k++)
{
//////////////////////////////////////////////////////////////////////////
//data_in
    // APE's DMA, CSU configuration -- data
    // CSU, cDDR--->APE0.DM0     
    // Size: 256/2 KB =0x20000B=0x200*0x100
    // APE's DMA configuration
    ape0_addr[0x10/4]=0x400000   ;   //APE's start Addr
    ape0_addr[0x18/4]=0x200      ;   //XNum   
    ape0_addr[0x20/4]=0x200      ;   //YStep  
    ape0_addr[0x28/4]=0x100      ;   //YNUM   
    ape0_addr[0x30/4]=0x1        ;   //ZStep  
    ape0_addr[0x38/4]=0x100      ;   //YAllNUM  

    //DDR's DMA configuration
    ape0_addr[0x40/4]=(int ) & data_in[0][256*k];
    ape0_addr[0x48/4]=0x04      ;  //XNum
    ape0_addr[0x50/4]=0x1000    ;  //YStep
    ape0_addr[0x58/4]=0x400     ;  //YNUM
    ape0_addr[0x60/4]=0x04      ;  //ZStep
    ape0_addr[0x68/4]=0x8000    ;  //AllNum=1024*32

    //DMA's control word
    ape0_addr[0x78/4]=0x0       ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape0_addr[0x70/4]=0x0       ;
	a=ape0_addr[0x80/4]	;	//DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape0_addr[0x80/4];	//0:success 1:fail
    ape0_addr[0x90/4]=0x1       ;
    ape0_addr[0x98/4]=0x1       ;
    a=ape0_addr[0xA0/4]         ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape0_addr[0xA0/4] ;                 //1: Finished, 0:transport in    

printf("APE0_DM0_col_in_%d\n",k);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//data_in
    // APE's DMA, CSU configuration -- data
    // CSU, cDDR--->APE0.DM0     
    // Size:  256/2 KB =0x20000B=0x200*0x100
    // APE's DMA configuration
    ape0_addr[0x10/4]=0x420000   ;   //APE's start Addr
    ape0_addr[0x18/4]=0x200      ;   //XNum   
    ape0_addr[0x20/4]=0x200      ;   //YStep  
    ape0_addr[0x28/4]=0x100      ;   //YNUM   
    ape0_addr[0x30/4]=0x1        ;   //ZStep  
    ape0_addr[0x38/4]=0x100      ;   //YAllNUM  

    //DDR's DMA configuration
    ape0_addr[0x40/4]=(int ) & data_in[0][32+256*k];
    ape0_addr[0x48/4]=0x04       ;  //XNum
    ape0_addr[0x50/4]=0x1000     ;  //YStep
    ape0_addr[0x58/4]=0x400      ;  //YNUM
    ape0_addr[0x60/4]=0x04       ;  //ZStep
    ape0_addr[0x68/4]=0x8000     ;  //AllNum=1024*32

    //DMA's control word
    ape0_addr[0x78/4]=0x0        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape0_addr[0x70/4]=0x0        ;
	a=ape0_addr[0x80/4]	 ;	//DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape0_addr[0x80/4];	//0:success 1:fail
    ape0_addr[0x90/4]=0x1        ;
    ape0_addr[0x98/4]=0x1        ;
    a=ape0_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape0_addr[0xA0/4] ;                 //1: Finished, 0:transport in    

printf("APE0_DM0_col_in_%d\n",k);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//factor_in
    // APE's DMA, CSU configuration -- data
    // CSU, cDDR--->APE0.DM1
    // Size: 256 KB 
    // APE's DMA configuration
    ape0_addr[0x10/4]=0x600000   ;   //APE's start Addr
    ape0_addr[0x18/4]=0x400      ;   //XNum
    ape0_addr[0x20/4]=0x400      ;   //YStep
    ape0_addr[0x28/4]=0x100      ;   //YNUM
    ape0_addr[0x30/4]=0x1        ;   //ZStep
    ape0_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape0_addr[0x40/4]=(int ) factor_in [256*k];
    ape0_addr[0x48/4]=0x400      ;  //XNum
    ape0_addr[0x50/4]=0x400      ;  //YStep
    ape0_addr[0x58/4]=0x100      ;  //YNUM
    ape0_addr[0x60/4]=0x1        ;  //ZStep
    ape0_addr[0x68/4]=0x100      ;  //AllNum

    //DMA's control word
    ape0_addr[0x78/4]=0x0        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape0_addr[0x70/4]=0x0        ;
	a=ape0_addr[0x80/4]	 ;	//DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape0_addr[0x80/4];	//0:success 1:fail
    ape0_addr[0x90/4]=0x1        ;
    ape0_addr[0x98/4]=0x1        ;
    a=ape0_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape0_addr[0xA0/4] ;                 //1: Finished, 0:transport in    

printf("factor_APE0_DM1_in_%d\n",k);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//factor_in
    // APE's DMA, CSU configuration -- data
    // CSU, cDDR--->APE0.DM2
    // Size: 256 KB 
    // APE's DMA configuration
    ape0_addr[0x10/4]=0x800000   ;   //APE's start Addr
    ape0_addr[0x18/4]=0x400      ;   //XNum
    ape0_addr[0x20/4]=0x400      ;   //YStep
    ape0_addr[0x28/4]=0x100      ;   //YNUM
    ape0_addr[0x30/4]=0x1        ;   //ZStep
    ape0_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration

    ape0_addr[0x40/4]=(int ) factor_in [32+256*k];
    ape0_addr[0x48/4]=0x400      ;  //XNum
    ape0_addr[0x50/4]=0x400      ;  //YStep
    ape0_addr[0x58/4]=0x100      ;  //YNUM
    ape0_addr[0x60/4]=0x1        ;  //ZStep
    ape0_addr[0x68/4]=0x100      ;  //AllNum

    //DMA's control word
    ape0_addr[0x78/4]=0x0        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape0_addr[0x70/4]=0x0        ;
	a=ape0_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape0_addr[0x80/4];//0:success 1:fail
    ape0_addr[0x90/4]=0x1        ;
    ape0_addr[0x98/4]=0x1        ;
    a=ape0_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape0_addr[0xA0/4] ;   //1: Finished, 0:transport in    

printf("factor_APE0_DM2_in_%d\n",k);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//data_in
    // APE's DMA, CSU configuration -- data
    // CSU,cDDR--->APE1.DM0
    // Size:  256/2 KB =0x20000B=0x200*0x100 
    // APE's DMA configuration
    ape1_addr[0x10/4]=0x400000   ;   //APE's start Addr
    ape1_addr[0x18/4]=0x200      ;   //XNum
    ape1_addr[0x20/4]=0x200      ;   //YStep
    ape1_addr[0x28/4]=0x100      ;   //YNUM
    ape1_addr[0x30/4]=0x1        ;   //ZStep
    ape1_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape1_addr[0x40/4]=(int ) & data_in[0][64+256*k];
    ape1_addr[0x48/4]=0x04     	 ;  //XNum
    ape1_addr[0x50/4]=0x1000   	 ;  //YStep
    ape1_addr[0x58/4]=0x400    	 ;  //YNUM
    ape1_addr[0x60/4]=0x04     	 ;  //ZStep
    ape1_addr[0x68/4]=0x8000   	 ;  //AllNum

    //DMA's control word
    ape1_addr[0x78/4]=0x0        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape1_addr[0x70/4]=0x0        ;
	a=ape1_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape1_addr[0x80/4];//0:success 1:fail
    ape1_addr[0x90/4]=0x1        ;
    ape1_addr[0x98/4]=0x1        ;
    a=ape1_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape1_addr[0xA0/4] ;  //1: Finished, 0:transport in        

printf("APE1_DM0_col_in_%d\n",k);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//data_in
    // APE's DMA, CSU configuration -- data
    // CSU,cDDR--->APE1.DM0
    // Size: 256 KB 
    // APE's DMA configuration
    ape1_addr[0x10/4]=0x420000   ;   //APE's start Addr
    ape1_addr[0x18/4]=0x200      ;   //XNum
    ape1_addr[0x20/4]=0x200      ;   //YStep
    ape1_addr[0x28/4]=0x100      ;   //YNUM
    ape1_addr[0x30/4]=0x1        ;   //ZStep
    ape1_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape1_addr[0x40/4]=(int ) & data_in[0][96+256*k];
    ape1_addr[0x48/4]=0x04       ;  //XNum
    ape1_addr[0x50/4]=0x1000     ;  //YStep
    ape1_addr[0x58/4]=0x400      ;  //YNUM
    ape1_addr[0x60/4]=0x04       ;  //ZStep
    ape1_addr[0x68/4]=0x8000     ;  //AllNum

    //DMA's control word
    ape1_addr[0x78/4]=0x0        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape1_addr[0x70/4]=0x0        ;
	a=ape1_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape1_addr[0x80/4];	//0:success 1:fail
    ape1_addr[0x90/4]=0x1        ;
    ape1_addr[0x98/4]=0x1        ;
    a=ape1_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape1_addr[0xA0/4] ;   //1: Finished, 0:transport in        

printf("APE1_DM0_col_in_%d\n",k);
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//factor_in 
   // APE's DMA, CSU configuration -- data
    // CSU,cDDR--->APE1.DM1
    // Size: 256 KB 
    // APE's DMA configuration
    ape1_addr[0x10/4]=0x600000   ;   //APE's start Addr
    ape1_addr[0x18/4]=0x400      ;   //XNum
    ape1_addr[0x20/4]=0x400      ;   //YStep
    ape1_addr[0x28/4]=0x100      ;   //YNUM
    ape1_addr[0x30/4]=0x1        ;   //ZStep
    ape1_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape1_addr[0x40/4]=(int) factor_in [64+256*k];
    ape1_addr[0x48/4]=0x400     ;  //XNum
    ape1_addr[0x50/4]=0x400     ;  //YStep
    ape1_addr[0x58/4]=0x100     ;  //YNUM
    ape1_addr[0x60/4]=0x1       ;  //ZStep
    ape1_addr[0x68/4]=0x100     ;  //AllNum

    //DMA's control word
    ape1_addr[0x78/4]=0x0       ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape1_addr[0x70/4]=0x0       ;
	a=ape1_addr[0x80/4]	; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape1_addr[0x80/4];	//0:success 1:fail
    ape1_addr[0x90/4]=0x1       ;
    ape1_addr[0x98/4]=0x1       ;
    a=ape1_addr[0xA0/4]         ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape1_addr[0xA0/4] ;   //1: Finished, 0:transport in        

printf("factor_APE1_DM1_in_%d\n",k);
////////////////////////////////////////////////////////////////////////////////
//factor_in 
   // APE's DMA, CSU configuration -- data
    // CSU,cDDR--->APE1.DM2
    // Size: 256 KB 
    // APE's DMA configuration
    ape1_addr[0x10/4]=0x800000   ;   //APE's start Addr
    ape1_addr[0x18/4]=0x400      ;   //XNum
    ape1_addr[0x20/4]=0x400      ;   //YStep
    ape1_addr[0x28/4]=0x100      ;   //YNUM
    ape1_addr[0x30/4]=0x1        ;   //ZStep
    ape1_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape1_addr[0x40/4]=(int ) factor_in [96+256*k];
    ape1_addr[0x48/4]=0x400      ;  //XNum
    ape1_addr[0x50/4]=0x400      ;  //YStep
    ape1_addr[0x58/4]=0x100      ;  //YNUM
    ape1_addr[0x60/4]=0x1        ;  //ZStep
    ape1_addr[0x68/4]=0x100      ;  //AllNum

    //DMA's control word
    ape1_addr[0x78/4]=0x0        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape1_addr[0x70/4]=0x0        ;
	a=ape1_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape1_addr[0x80/4];	//0:success 1:fail
    ape1_addr[0x90/4]=0x1        ;
    ape1_addr[0x98/4]=0x1        ;
    a=ape1_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape1_addr[0xA0/4] ;    //1: Finished, 0:transport in        

printf("factor_APE1_DM2_in_%d\n",k);
 //////////////////////////////////////////////////////////////////////////////
//data_in 
   // APE's DMA, CSU configuration -- data
    // CSU,cDDR--->APE2.DM0
    // Size:  256/2 KB =0x20000B=0x200*0x100 
    // APE's DMA configuration
    ape2_addr[0x10/4]=0x400000   ;   //APE's start Addr
    ape2_addr[0x18/4]=0x200      ;   //XNum
    ape2_addr[0x20/4]=0x200      ;   //YStep
    ape2_addr[0x28/4]=0x100      ;   //YNUM
    ape2_addr[0x30/4]=0x1        ;   //ZStep
    ape2_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape2_addr[0x40/4]=(int )& data_in[0][128+256*k];
    ape2_addr[0x48/4]=0x04       ;  //XNum
    ape2_addr[0x50/4]=0x1000     ;  //YStep
    ape2_addr[0x58/4]=0x400      ;  //YNUM
    ape2_addr[0x60/4]=0x04       ;  //ZStep
    ape2_addr[0x68/4]=0x8000     ;  //AllNum

    //DMA's control word
    ape2_addr[0x78/4]=0x0        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape2_addr[0x70/4]=0x0        ;
	a=ape2_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape2_addr[0x80/4];	//0:success 1:fail
    ape2_addr[0x90/4]=0x1        ;
    ape2_addr[0x98/4]=0x1        ;
    a=ape2_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape2_addr[0xA0/4] ;  //1: Finished, 0:transport in        

printf("APE2_DM0_col_in_%d\n",k);
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//data_in 
   // APE's DMA, CSU configuration -- data
    // CSU,cDDR--->APE2.DM0
    // Size:  256/2 KB =0x20000B=0x200*0x100 
    // APE's DMA configuration
    ape2_addr[0x10/4]=0x420000   ;   //APE's start Addr
    ape2_addr[0x18/4]=0x200      ;   //XNum
    ape2_addr[0x20/4]=0x200      ;   //YStep
    ape2_addr[0x28/4]=0x100      ;   //YNUM
    ape2_addr[0x30/4]=0x1        ;   //ZStep
    ape2_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape2_addr[0x40/4]=(int )& data_in[0][160+256*k];
    ape2_addr[0x48/4]=0x04       ;  //XNum
    ape2_addr[0x50/4]=0x1000     ;  //YStep
    ape2_addr[0x58/4]=0x400      ;  //YNUM
    ape2_addr[0x60/4]=0x04       ;  //ZStep
    ape2_addr[0x68/4]=0x8000     ;  //AllNum

    //DMA's control word
    ape2_addr[0x78/4]=0x0        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape2_addr[0x70/4]=0x0        ;
	a=ape2_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape2_addr[0x80/4];	//0:success 1:fail
    ape2_addr[0x90/4]=0x1        ;
    ape2_addr[0x98/4]=0x1        ;
    a=ape2_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape2_addr[0xA0/4] ;    //1: Finished, 0:transport in        

printf("APE2_DM0_col_in_%d\n",k);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//factor_in
    // APE's DMA, CSU configuration -- data
    // CSU,cDDR--->APE2.DM1
    // Size: 256 KB 
    // APE's DMA configuration
    ape2_addr[0x10/4]=0x600000   ;   //APE's start Addr
    ape2_addr[0x18/4]=0x400      ;   //XNum
    ape2_addr[0x20/4]=0x400      ;   //YStep
    ape2_addr[0x28/4]=0x100      ;   //YNUM
    ape2_addr[0x30/4]=0x1        ;   //ZStep
    ape2_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape2_addr[0x40/4]=(int ) factor_in [128+256*k];
    ape2_addr[0x48/4]=0x400      ;  //XNum
    ape2_addr[0x50/4]=0x400      ;  //YStep
    ape2_addr[0x58/4]=0x100      ;  //YNUM
    ape2_addr[0x60/4]=0x1        ;  //ZStep
    ape2_addr[0x68/4]=0x100      ;  //AllNum

    //DMA's control word
    ape2_addr[0x78/4]=0x0        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape2_addr[0x70/4]=0x0        ;
	a=ape2_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape2_addr[0x80/4];	//0:success 1:fail
    ape2_addr[0x90/4]=0x1        ;
    ape2_addr[0x98/4]=0x1        ;
    a=ape2_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape2_addr[0xA0/4] ;  //1: Finished, 0:transport in        

printf("factor_APE2_DM1_in_%d\n",k);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//factor_in
    // APE's DMA, CSU configuration -- data
    // CSU,cDDR--->APE2.DM2
    // Size: 256 KB 
    // APE's DMA configuration
    ape2_addr[0x10/4]=0x800000   ;   //APE's start Addr
    ape2_addr[0x18/4]=0x400      ;   //XNum
    ape2_addr[0x20/4]=0x400      ;   //YStep
    ape2_addr[0x28/4]=0x100      ;   //YNUM
    ape2_addr[0x30/4]=0x1        ;   //ZStep
    ape2_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape2_addr[0x40/4]=(int ) factor_in [160+256*k];
    ape2_addr[0x48/4]=0x400      ;  //XNum
    ape2_addr[0x50/4]=0x400      ;  //YStep
    ape2_addr[0x58/4]=0x100      ;  //YNUM
    ape2_addr[0x60/4]=0x1        ;  //ZStep
    ape2_addr[0x68/4]=0x100      ;  //AllNum

    //DMA's control word
    ape2_addr[0x78/4]=0x0        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape2_addr[0x70/4]=0x0        ;
	a=ape2_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape2_addr[0x80/4];	//0:success 1:fail
    ape2_addr[0x90/4]=0x1        ;
    ape2_addr[0x98/4]=0x1        ;
    a=ape2_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape2_addr[0xA0/4] ;  //1: Finished, 0:transport in        

printf("factor_APE2_DM2_col_in_%d\n",k);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//data_in
    // APE's DMA, CSU configuration -- data
    // CSU,cDDR--->APE3.DM0
    // Size:  256/2 KB =0x20000B=0x200*0x100 
    // APE's DMA configuration
    ape3_addr[0x10/4]=0x400000   ;   //APE's start Addr
    ape3_addr[0x18/4]=0x200      ;   //XNum
    ape3_addr[0x20/4]=0x200      ;   //YStep
    ape3_addr[0x28/4]=0x100      ;   //YNUM
    ape3_addr[0x30/4]=0x1        ;   //ZStep
    ape3_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape3_addr[0x40/4]=(int )& data_in[0][192+256*k];
    ape3_addr[0x48/4]=0x04       ;  //XNum
    ape3_addr[0x50/4]=0x1000     ;  //YStep
    ape3_addr[0x58/4]=0x400      ;  //YNUM
    ape3_addr[0x60/4]=0x04       ;  //ZStep
    ape3_addr[0x68/4]=0x8000     ;  //AllNum

    //DMA's control word
    ape3_addr[0x78/4]=0x0        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape3_addr[0x70/4]=0x0        ;
	a=ape3_addr[0x80/4]	 ;	//DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape3_addr[0x80/4];	//0:success 1:fail
    ape3_addr[0x90/4]=0x1        ;
    ape3_addr[0x98/4]=0x1        ;
    a=ape3_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape3_addr[0xA0/4] ;   //1: Finished, 0:transport in    
    
printf("APE3_DM0_col_in_%d\n",k);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//data_in
    // APE's DMA, CSU configuration -- data
    // CSU,cDDR--->APE3.DM0
    // Size:  256/2 KB =0x20000B=0x200*0x100 
    // APE's DMA configuration
    ape3_addr[0x10/4]=0x420000   ;   //APE's start Addr
    ape3_addr[0x18/4]=0x200      ;   //XNum
    ape3_addr[0x20/4]=0x200      ;   //YStep
    ape3_addr[0x28/4]=0x100      ;   //YNUM
    ape3_addr[0x30/4]=0x1        ;   //ZStep
    ape3_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape3_addr[0x40/4]=(int )& data_in[0][224+256*k];
    ape3_addr[0x48/4]=0x04       ;  //XNum
    ape3_addr[0x50/4]=0x1000     ;  //YStep
    ape3_addr[0x58/4]=0x400      ;  //YNUM
    ape3_addr[0x60/4]=0x04       ;  //ZStep
    ape3_addr[0x68/4]=0x8000     ;  //AllNum

    //DMA's control word
    ape3_addr[0x78/4]=0x0        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape3_addr[0x70/4]=0x0        ;
	a=ape3_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape3_addr[0x80/4];	//0:success 1:fail
    ape3_addr[0x90/4]=0x1        ;
    ape3_addr[0x98/4]=0x1        ;
    a=ape3_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape3_addr[0xA0/4] ; //1: Finished, 0:transport in    
    
printf("APE3_DM0_col_in_%d\n",k);
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//factor_in
    // APE's DMA, CSU configuration -- data
    // CSU,cDDR--->APE3.DM1
    // Size: 256 KB 
    // APE's DMA configuration
    ape3_addr[0x10/4]=0x600000   ;   //APE's start Addr
    ape3_addr[0x18/4]=0x400      ;   //XNum
    ape3_addr[0x20/4]=0x400      ;   //YStep
    ape3_addr[0x28/4]=0x100      ;   //YNUM
    ape3_addr[0x30/4]=0x1        ;   //ZStep
    ape3_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape3_addr[0x40/4]=(int ) factor_in [192+256*k];
    ape3_addr[0x48/4]=0x400      ;  //XNum
    ape3_addr[0x50/4]=0x400      ;  //YStep
    ape3_addr[0x58/4]=0x100      ;  //YNUM
    ape3_addr[0x60/4]=0x1        ;  //ZStep
    ape3_addr[0x68/4]=0x100      ;  //AllNum

    //DMA's control word
    ape3_addr[0x78/4]=0x0        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape3_addr[0x70/4]=0x0        ;
	a=ape3_addr[0x80/4]	 ;	//DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape3_addr[0x80/4];	//0:success 1:fail
    ape3_addr[0x90/4]=0x1        ;
    ape3_addr[0x98/4]=0x1        ;
    a=ape3_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape3_addr[0xA0/4] ;  //1: Finished, 0:transport in    
    
printf("factor_APE3_DM1_in_%d\n",k);
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//factor_in
    // APE's DMA, CSU configuration -- data
    // CSU,cDDR--->APE3.DM2
    // Size: 256 KB 
    // APE's DMA configuration
    ape3_addr[0x10/4]=0x800000   ;   //APE's start Addr
    ape3_addr[0x18/4]=0x400      ;   //XNum
    ape3_addr[0x20/4]=0x400      ;   //YStep
    ape3_addr[0x28/4]=0x100      ;   //YNUM
    ape3_addr[0x30/4]=0x1        ;   //ZStep
    ape3_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape3_addr[0x40/4]=(int ) factor_in [224+256*k];
    ape3_addr[0x48/4]=0x400      ;  //XNum
    ape3_addr[0x50/4]=0x400      ;  //YStep
    ape3_addr[0x58/4]=0x100      ;  //YNUM
    ape3_addr[0x60/4]=0x1        ;  //ZStep
    ape3_addr[0x68/4]=0x100      ;  //AllNum

    //DMA's control word
    ape3_addr[0x78/4]=0x0        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape3_addr[0x70/4]=0x0        ;
	a=ape3_addr[0x80/4]	 ;	//DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape3_addr[0x80/4];	//0:success 1:fail
    ape3_addr[0x90/4]=0x1        ;
    ape3_addr[0x98/4]=0x1        ;
    a=ape3_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape3_addr[0xA0/4] ;   //1: Finished, 0:transport in    
    
printf("factor_APE3_DM2_in_%d\n",k);

//spu
    SPULoad(0, 0);printf("spuload_APE0_%d\n",k);
    SPULoad(0, 1);printf("spuload_APE1_%d\n",k);
    SPULoad(0, 2);printf("spuload_APE2_%d\n",k);
    SPULoad(0, 3);printf("spuload_APE3_%d\n",k);
    ape_wait(0);printf("wait_APE0_%d\n",k);
    ape_wait(1);printf("wait_APE1_%d\n",k);
    ape_wait(2);printf("wait_APE2_%d\n",k);
    ape_wait(3);printf("wait_APE3_%d\n",k);
    printf("fft_over_%d\n",k);
// data_out_col
   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU,APE0.DM4--->cDDR
    // Size: 256 KB 
    // APE's DMA configuration
    ape0_addr[0x10/4]=0xc00000   ;   //APE's start Addr
    ape0_addr[0x18/4]=0x400      ;   //XNum
    ape0_addr[0x20/4]=0x400      ;   //YStep
    ape0_addr[0x28/4]=0x100      ;   //YNUM
    ape0_addr[0x30/4]=0x1        ;   //ZStep
    ape0_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape0_addr[0x40/4]=(int ) & data_out_col[0][512*k];  //cDDR's start Addr
    ape0_addr[0x48/4]=0x8        ;  //XNum
    ape0_addr[0x50/4]=0x2000     ;  //YStep
    ape0_addr[0x58/4]=0x400      ;  //YNUM
    ape0_addr[0x60/4]=0x8        ;  //ZStep
    ape0_addr[0x68/4]=0x8000     ;  //AllNum

    //DMA's control word
    ape0_addr[0x78/4]=0x1        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape0_addr[0x70/4]=0x0        ;
	a=ape0_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape0_addr[0x80/4];//0:success 1:fail
    ape0_addr[0x90/4]=0x1        ;
    ape0_addr[0x98/4]=0x1        ;
    a=ape0_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape0_addr[0xA0/4] ;  //1: Finished, 0:transport in  

    printf("APE0_DM4_col_out_%d\n",k);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU,APE0.DM3--->cDDR
    // Size: 256 KB 
    // APE's DMA configuration
    ape0_addr[0x10/4]=0xa00000   ;   //APE's start Addr
    ape0_addr[0x18/4]=0x400      ;   //XNum
    ape0_addr[0x20/4]=0x400      ;   //YStep
    ape0_addr[0x28/4]=0x100      ;   //YNUM
    ape0_addr[0x30/4]=0x1        ;   //ZStep
    ape0_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape0_addr[0x40/4]=(int )& data_out_col[0][64+512*k];  //cDDR's start Addr
    ape0_addr[0x48/4]=0x8     	 ;  //XNum
    ape0_addr[0x50/4]=0x2000     ;  //YStep
    ape0_addr[0x58/4]=0x400      ;  //YNUM
    ape0_addr[0x60/4]=0x8        ;  //ZStep
    ape0_addr[0x68/4]=0x8000     ;  //AllNum

    //DMA's control word
    ape0_addr[0x78/4]=0x1       ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape0_addr[0x70/4]=0x0       ;
	a=ape0_addr[0x80/4]	; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape0_addr[0x80/4];//0:success 1:fail
    ape0_addr[0x90/4]=0x1       ;
    ape0_addr[0x98/4]=0x1       ;
    a=ape0_addr[0xA0/4]         ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape0_addr[0xA0/4] ; //1: Finished, 0:transport in        
     
printf("APE0_DM3_col_out_%d\n",k);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU,APE1.DM4--->cDDR
    // Size: 256 KB 
    // APE's DMA configuration
    ape1_addr[0x10/4]=0xc00000   ;   //APE's start Addr
    ape1_addr[0x18/4]=0x400      ;   //XNum
    ape1_addr[0x20/4]=0x400      ;   //YStep
    ape1_addr[0x28/4]=0x100      ;   //YNUM
    ape1_addr[0x30/4]=0x1        ;   //ZStep
    ape1_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape1_addr[0x40/4]=(int ) & data_out_col[0][128+512*k];  //cDDR's start Addr
    ape1_addr[0x48/4]=0x8     	 ;  //XNum
    ape1_addr[0x50/4]=0x2000     ;  //YStep
    ape1_addr[0x58/4]=0x400      ;  //YNUM
    ape1_addr[0x60/4]=0x8        ;  //ZStep
    ape1_addr[0x68/4]=0x8000     ;  //AllNum

    //DMA's control word
    ape1_addr[0x78/4]=0x1        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape1_addr[0x70/4]=0x0        ;
	a=ape1_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape1_addr[0x80/4]; //0:success 1:fail
    ape1_addr[0x90/4]=0x1        ;
    ape1_addr[0x98/4]=0x1        ;
    a=ape1_addr[0xA0/4]          ; //DMA Status, 0
     
    while((a&0x1)==0) a=ape1_addr[0xA0/4] ; //1: Finished, 0:transport in  

printf("APE1_DM4_col_out_%d\n",k);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU,APE1.DM3--->cDDR
    // Size: 256 KB 
    // APE's DMA configuration
    ape1_addr[0x10/4]=0xa00000   ;   //APE's start Addr
    ape1_addr[0x18/4]=0x400      ;   //XNum
    ape1_addr[0x20/4]=0x400      ;   //YStep
    ape1_addr[0x28/4]=0x100      ;   //YNUM
    ape1_addr[0x30/4]=0x1        ;   //ZStep
    ape1_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape1_addr[0x40/4]=(int ) & data_out_col[0][192+512*k];  //cDDR's start Addr
    ape1_addr[0x48/4]=0x8     	 ;  //XNum
    ape1_addr[0x50/4]=0x2000     ;  //YStep
    ape1_addr[0x58/4]=0x400      ;  //YNUM
    ape1_addr[0x60/4]=0x8        ;  //ZStep
    ape1_addr[0x68/4]=0x8000     ;  //AllNum

    //DMA's control word
    ape1_addr[0x78/4]=0x1        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape1_addr[0x70/4]=0x0        ;
	a=ape1_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape1_addr[0x80/4];//0:success 1:fail
    ape1_addr[0x90/4]=0x1        ;
    ape1_addr[0x98/4]=0x1        ;
    a=ape1_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape1_addr[0xA0/4] ; //1: Finished, 0:transport in        
     
printf("APE1_DM3_col_out_%d\n",k);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU,APE2.DM4--->cDDR
    // Size: 256 KB 
    // APE's DMA configuration
    ape2_addr[0x10/4]=0xc00000   ;   //APE's start Addr
    ape2_addr[0x18/4]=0x400      ;   //XNum
    ape2_addr[0x20/4]=0x400      ;   //YStep
    ape2_addr[0x28/4]=0x100      ;   //YNUM
    ape2_addr[0x30/4]=0x1        ;   //ZStep
    ape2_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape2_addr[0x40/4]=(int ) & data_out_col[0][256+512*k];  //cDDR's start Addr
    ape2_addr[0x48/4]=0x8     	 ;  //XNum
    ape2_addr[0x50/4]=0x2000     ;  //YStep
    ape2_addr[0x58/4]=0x400      ;  //YNUM
    ape2_addr[0x60/4]=0x8        ;  //ZStep
    ape2_addr[0x68/4]=0x8000     ;  //AllNum

    //DMA's control word
    ape2_addr[0x78/4]=0x1        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape2_addr[0x70/4]=0x0        ;
	a=ape2_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape2_addr[0x80/4];//0:success 1:fail
    ape2_addr[0x90/4]=0x1        ;
    ape2_addr[0x98/4]=0x1        ;
    a=ape2_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape2_addr[0xA0/4] ; //1: Finished, 0:transport in     

printf("APE2_DM4_col_out_%d\n",k);
    ////////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU,APE2.DM3--->cDDR
    // Size: 256 KB 
    // APE's DMA configuration
    ape2_addr[0x10/4]=0xa00000   ;   //APE's start Addr
    ape2_addr[0x18/4]=0x400      ;   //XNum
    ape2_addr[0x20/4]=0x400      ;   //YStep
    ape2_addr[0x28/4]=0x100      ;   //YNUM
    ape2_addr[0x30/4]=0x1        ;   //ZStep
    ape2_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape2_addr[0x40/4]=(int ) & data_out_col[0][320+512*k];  //cDDR's start Addr
    ape2_addr[0x48/4]=0x8     	 ;  //XNum
    ape2_addr[0x50/4]=0x2000     ;  //YStep
    ape2_addr[0x58/4]=0x400      ;  //YNUM
    ape2_addr[0x60/4]=0x8        ;  //ZStep
    ape2_addr[0x68/4]=0x8000     ;  //AllNum

    //DMA's control word
    ape2_addr[0x78/4]=0x1        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape2_addr[0x70/4]=0x0        ;
	a=ape2_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape2_addr[0x80/4];	//0:success 1:fail
    ape2_addr[0x90/4]=0x1        ;
    ape2_addr[0x98/4]=0x1        ;
    a=ape2_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape2_addr[0xA0/4] ;  //1: Finished, 0:transport in        
   
printf("APE2_DM3_col_out_%d\n",k);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU,APE3.DM4--->cDDR
    // Size: 256 KB 
    // APE's DMA configuration
    ape3_addr[0x10/4]=0xc00000   ;   //APE's start Addr
    ape3_addr[0x18/4]=0x400      ;   //XNum
    ape3_addr[0x20/4]=0x400      ;   //YStep
    ape3_addr[0x28/4]=0x100      ;   //YNUM
    ape3_addr[0x30/4]=0x1        ;   //ZStep
    ape3_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape3_addr[0x40/4]=(int ) & data_out_col[0][384+512*k];  //cDDR's start Addr
    ape3_addr[0x48/4]=0x8     	 ;  //XNum
    ape3_addr[0x50/4]=0x2000     ;  //YStep
    ape3_addr[0x58/4]=0x400      ;  //YNUM
    ape3_addr[0x60/4]=0x8        ;  //ZStep
    ape3_addr[0x68/4]=0x8000     ;  //AllNum

    //DMA's control word
    ape3_addr[0x78/4]=0x1        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape3_addr[0x70/4]=0x0        ;
	a=ape3_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape3_addr[0x80/4]; //0:success 1:fail
    ape3_addr[0x90/4]=0x1        ;
    ape3_addr[0x98/4]=0x1        ;
    a=ape3_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape3_addr[0xA0/4] ;  //1: Finished, 0:transport in    

printf("APE3_DM4_col_out_%d\n",k);    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU,APE3.DM3--->cDDR
    // Size: 256 KB 
    // APE's DMA configuration
    ape3_addr[0x10/4]=0xa00000   ;   //APE's start Addr
    ape3_addr[0x18/4]=0x400      ;   //XNum
    ape3_addr[0x20/4]=0x400      ;   //YStep
    ape3_addr[0x28/4]=0x100      ;   //YNUM
    ape3_addr[0x30/4]=0x1        ;   //ZStep
    ape3_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape3_addr[0x40/4]=(int ) & data_out_col[0][448+512*k];  //cDDR's start Addr
    ape3_addr[0x48/4]=0x8     	 ;  //XNum
    ape3_addr[0x50/4]=0x2000     ;  //YStep
    ape3_addr[0x58/4]=0x400      ;  //YNUM
    ape3_addr[0x60/4]=0x8        ;  //ZStep
    ape3_addr[0x68/4]=0x8000     ;  //AllNum

    //DMA's control word
    ape3_addr[0x78/4]=0x1        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape3_addr[0x70/4]=0x0        ;
	a=ape3_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape3_addr[0x80/4]; //0:success 1:fail
    ape3_addr[0x90/4]=0x1        ;
    ape3_addr[0x98/4]=0x1        ;
    a=ape3_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape3_addr[0xA0/4] ;   //1: Finished, 0:transport in        

printf("APE3_DM3_col_out_%d\n",k);
}

//////////////////////////////////////////////////////////////////////////////
//row_fft
/////////////////////////////////////////////////////////////////////////////
 for(k=0; k<4; k++)
{
    //////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU, cDDR--->APE0.DM0
    // Size: 256 KB =0x40000B=0x400*0x100
    // APE's DMA configuration
    ape0_addr[0x10/4]=0x400000   ;   //APE's start Addr
    ape0_addr[0x18/4]=0x400      ;   //XNum   
    ape0_addr[0x20/4]=0x400      ;   //YStep  
    ape0_addr[0x28/4]=0x100      ;   //YNUM   
    ape0_addr[0x30/4]=0x1        ;   //ZStep  
    ape0_addr[0x38/4]=0x100      ;   //YAllNUM  

    //DDR's DMA configuration
    ape0_addr[0x40/4]=(int )  data_out_col[256*k];
    ape0_addr[0x48/4]=0x400      ;  //XNum
    ape0_addr[0x50/4]=0x400      ;  //YStep
    ape0_addr[0x58/4]=0x100      ;  //YNUM
    ape0_addr[0x60/4]=0x1        ;  //ZStep
    ape0_addr[0x68/4]=0x100      ;  //AllNum

    //DMA's control word
    ape0_addr[0x78/4]=0x0        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape0_addr[0x70/4]=0x0        ;
	a=ape0_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape0_addr[0x80/4];	//0:success 1:fail
    ape0_addr[0x90/4]=0x1        ;
    ape0_addr[0x98/4]=0x1        ;
    a=ape0_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape0_addr[0xA0/4] ;  //1: Finished, 0:transport in    

printf("APE0_DM0_row_in_%d\n",k);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU, cDDR--->APE0.DM1
    // Size: 256 KB 
    // APE's DMA configuration
    ape0_addr[0x10/4]=0x600000   ;   //APE's start Addr
    ape0_addr[0x18/4]=0x400      ;   //XNum
    ape0_addr[0x20/4]=0x400      ;   //YStep
    ape0_addr[0x28/4]=0x100      ;   //YNUM
    ape0_addr[0x30/4]=0x1        ;   //ZStep
    ape0_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape0_addr[0x40/4]=(int ) data_out_col[32+256*k];
    ape0_addr[0x48/4]=0x400      ;  //XNum
    ape0_addr[0x50/4]=0x400      ;  //YStep
    ape0_addr[0x58/4]=0x100      ;  //YNUM
    ape0_addr[0x60/4]=0x1        ;  //ZStep
    ape0_addr[0x68/4]=0x100      ;  //AllNum

    //DMA's control word
    ape0_addr[0x78/4]=0x0        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape0_addr[0x70/4]=0x0        ;
	a=ape0_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape0_addr[0x80/4];//0:success 1:fail
    ape0_addr[0x90/4]=0x1        ;
    ape0_addr[0x98/4]=0x1        ;
    a=ape0_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape0_addr[0xA0/4] ; //1: Finished, 0:transport in    

printf("APE0_DM1_row_in_%d\n",k);
    //////////////////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU,cDDR--->APE1.DM0
    // Size: 256 KB 
    // APE's DMA configuration
    ape1_addr[0x10/4]=0x400000   ;   //APE's start Addr
    ape1_addr[0x18/4]=0x400      ;   //XNum
    ape1_addr[0x20/4]=0x400      ;   //YStep
    ape1_addr[0x28/4]=0x100      ;   //YNUM
    ape1_addr[0x30/4]=0x1        ;   //ZStep
    ape1_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape1_addr[0x40/4]=(int ) data_out_col[64+256*k];
    ape1_addr[0x48/4]=0x400      ;  //XNum
    ape1_addr[0x50/4]=0x400      ;  //YStep
    ape1_addr[0x58/4]=0x100      ;  //YNUM
    ape1_addr[0x60/4]=0x1        ;  //ZStep
    ape1_addr[0x68/4]=0x100      ;  //AllNum

    //DMA's control word
    ape1_addr[0x78/4]=0x0        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape1_addr[0x70/4]=0x0        ;
	a=ape1_addr[0x80/4]	 ;	//DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape1_addr[0x80/4];	//0:success 1:fail
    ape1_addr[0x90/4]=0x1        ;
    ape1_addr[0x98/4]=0x1        ;
    a=ape1_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape1_addr[0xA0/4] ;                 //1: Finished, 0:transport in        

printf("APE1_DM0_row_in_%d\n",k);
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU,cDDR--->APE1.DM1
    // Size: 256 KB 
    // APE's DMA configuration
    ape1_addr[0x10/4]=0x600000   ;   //APE's start Addr
    ape1_addr[0x18/4]=0x400      ;   //XNum
    ape1_addr[0x20/4]=0x400      ;   //YStep
    ape1_addr[0x28/4]=0x100      ;   //YNUM
    ape1_addr[0x30/4]=0x1        ;   //ZStep
    ape1_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape1_addr[0x40/4]=(int ) data_out_col[96+256*k];
    ape1_addr[0x48/4]=0x400      ;  //XNum
    ape1_addr[0x50/4]=0x400      ;  //YStep
    ape1_addr[0x58/4]=0x100      ;  //YNUM
    ape1_addr[0x60/4]=0x1        ;  //ZStep
    ape1_addr[0x68/4]=0x100      ;  //AllNum

    //DMA's control word
    ape1_addr[0x78/4]=0x0        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape1_addr[0x70/4]=0x0        ;
	a=ape1_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape1_addr[0x80/4]; //0:success 1:fail
    ape1_addr[0x90/4]=0x1        ;
    ape1_addr[0x98/4]=0x1        ;
    a=ape1_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape1_addr[0xA0/4] ; //1: Finished, 0:transport in        

printf("APE1_DM1_row_in_%d\n",k);
    //////////////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU,cDDR--->APE2.DM0
    // Size: 256 KB 
    // APE's DMA configuration
    ape2_addr[0x10/4]=0x400000   ;   //APE's start Addr
    ape2_addr[0x18/4]=0x400      ;   //XNum
    ape2_addr[0x20/4]=0x400      ;   //YStep
    ape2_addr[0x28/4]=0x100      ;   //YNUM
    ape2_addr[0x30/4]=0x1        ;   //ZStep
    ape2_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape2_addr[0x40/4]=(int ) data_out_col[128+256*k];
    ape2_addr[0x48/4]=0x400      ;  //XNum
    ape2_addr[0x50/4]=0x400      ;  //YStep
    ape2_addr[0x58/4]=0x100      ;  //YNUM
    ape2_addr[0x60/4]=0x1        ;  //ZStep
    ape2_addr[0x68/4]=0x100      ;  //AllNum

    //DMA's control word
    ape2_addr[0x78/4]=0x0        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape2_addr[0x70/4]=0x0        ;
	a=ape2_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape2_addr[0x80/4];	//0:success 1:fail
    ape2_addr[0x90/4]=0x1        ;
    ape2_addr[0x98/4]=0x1        ;
    a=ape2_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape2_addr[0xA0/4] ;  //1: Finished, 0:transport in        

printf("APE2_DM0_row_in_%d\n",k);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU,cDDR--->APE2.DM1
    // Size: 256 KB 
    // APE's DMA configuration
    ape2_addr[0x10/4]=0x600000   ;   //APE's start Addr
    ape2_addr[0x18/4]=0x400      ;   //XNum
    ape2_addr[0x20/4]=0x400      ;   //YStep
    ape2_addr[0x28/4]=0x100      ;   //YNUM
    ape2_addr[0x30/4]=0x1        ;   //ZStep
    ape2_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape2_addr[0x40/4]=(int ) data_out_col[160+256*k];
    ape2_addr[0x48/4]=0x400      ;  //XNum
    ape2_addr[0x50/4]=0x400      ;  //YStep
    ape2_addr[0x58/4]=0x100      ;  //YNUM
    ape2_addr[0x60/4]=0x1        ;  //ZStep
    ape2_addr[0x68/4]=0x100      ;  //AllNum

    //DMA's control word
    ape2_addr[0x78/4]=0x0        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape2_addr[0x70/4]=0x0        ;
	a=ape2_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape2_addr[0x80/4]; //0:success 1:fail
    ape2_addr[0x90/4]=0x1        ;
    ape2_addr[0x98/4]=0x1        ;
    a=ape2_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape2_addr[0xA0/4] ;  //1: Finished, 0:transport in        

printf("APE2_DM1_row_in_%d\n",k);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU,cDDR--->APE3.DM0
    // Size: 256 KB 
    // APE's DMA configuration
    ape3_addr[0x10/4]=0x400000   ;   //APE's start Addr
    ape3_addr[0x18/4]=0x400      ;   //XNum
    ape3_addr[0x20/4]=0x400      ;   //YStep
    ape3_addr[0x28/4]=0x100      ;   //YNUM
    ape3_addr[0x30/4]=0x1        ;   //ZStep
    ape3_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape3_addr[0x40/4]=(int ) data_out_col[192+256*k];
    ape3_addr[0x48/4]=0x400      ;  //XNum
    ape3_addr[0x50/4]=0x400      ;  //YStep
    ape3_addr[0x58/4]=0x100      ;  //YNUM
    ape3_addr[0x60/4]=0x1        ;  //ZStep
    ape3_addr[0x68/4]=0x100      ;  //AllNum

    //DMA's control word
    ape3_addr[0x78/4]=0x0        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape3_addr[0x70/4]=0x0        ;
	a=ape3_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape3_addr[0x80/4]; //0:success 1:fail
    ape3_addr[0x90/4]=0x1        ;
    ape3_addr[0x98/4]=0x1        ;
    a=ape3_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape3_addr[0xA0/4] ;  //1: Finished, 0:transport in    
    
printf("APE3_DM0_row_in_%d\n",k);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU,cDDR--->APE3.DM1
    // Size: 256 KB 
    // APE's DMA configuration
    ape3_addr[0x10/4]=0x600000   ;   //APE's start Addr
    ape3_addr[0x18/4]=0x400      ;   //XNum
    ape3_addr[0x20/4]=0x400      ;   //YStep
    ape3_addr[0x28/4]=0x100      ;   //YNUM
    ape3_addr[0x30/4]=0x1        ;   //ZStep
    ape3_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape3_addr[0x40/4]=(int ) data_out_col[224+256*k];
    ape3_addr[0x48/4]=0x400      ;  //XNum
    ape3_addr[0x50/4]=0x400      ;  //YStep
    ape3_addr[0x58/4]=0x100      ;  //YNUM
    ape3_addr[0x60/4]=0x1        ;  //ZStep
    ape3_addr[0x68/4]=0x100      ;  //AllNum

    //DMA's control word
    ape3_addr[0x78/4]=0x0        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape3_addr[0x70/4]=0x0        ;
	a=ape3_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape3_addr[0x80/4]; //0:success 1:fail
    ape3_addr[0x90/4]=0x1        ;
    ape3_addr[0x98/4]=0x1        ;
    a=ape3_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape3_addr[0xA0/4] ; //1: Finished, 0:transport in    
    
printf("APE3_DM1_row_in_%d\n",k);
//spu
    SPULoad_2(0, 0);printf("spuload_APE0_%d\n",k);
    SPULoad_2(0, 1);printf("spuload_APE1_%d\n",k);
    SPULoad_2(0, 2);printf("spuload_APE2_%d\n",k);
    SPULoad_2(0, 3);printf("spuload_APE3_%d\n",k);
	ape_wait(0);printf("wait_APE0_%d\n",k);
	ape_wait(1);printf("wait_APE1_%d\n",k);
	ape_wait(2);printf("wait_APE2_%d\n",k);
	ape_wait(3);printf("wait_APE3_%d\n",k);
printf("fft_over_%d\n",k);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU,APE0.DM0--->cDDR
    // Size: 256 KB 
    // APE's DMA configuration
    ape0_addr[0x10/4]=0x400000   ;   //APE's start Addr
    ape0_addr[0x18/4]=0x400      ;   //XNum
    ape0_addr[0x20/4]=0x400      ;   //YStep
    ape0_addr[0x28/4]=0x100      ;   //YNUM
    ape0_addr[0x30/4]=0x1        ;   //ZStep
    ape0_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape0_addr[0x40/4]=(int ) data_out_row[256*k];  //cDDR's start Addr
    ape0_addr[0x48/4]=0x400      ;  //XNum
    ape0_addr[0x50/4]=0x400      ;  //YStep
    ape0_addr[0x58/4]=0x100      ;  //YNUM
    ape0_addr[0x60/4]=0x1        ;  //ZStep
    ape0_addr[0x68/4]=0x100      ;  //AllNum

    //DMA's control word
    ape0_addr[0x78/4]=0x1        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape0_addr[0x70/4]=0x0        ;
	a=ape0_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape0_addr[0x80/4]; //0:success 1:fail
    ape0_addr[0x90/4]=0x1        ;
    ape0_addr[0x98/4]=0x1        ;
    a=ape0_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape0_addr[0xA0/4] ;  //1: Finished, 0:transport in  

printf("APE0_DM0_row_out_%d\n",k);
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU,APE0.DM1--->cDDR
    // Size: 256 KB 
    // APE's DMA configuration
    ape0_addr[0x10/4]=0x600000   ;   //APE's start Addr
    ape0_addr[0x18/4]=0x400      ;   //XNum
    ape0_addr[0x20/4]=0x400      ;   //YStep
    ape0_addr[0x28/4]=0x100      ;   //YNUM
    ape0_addr[0x30/4]=0x1        ;   //ZStep
    ape0_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape0_addr[0x40/4]=(int ) data_out_row[32+256*k];  //cDDR's start Addr
    ape0_addr[0x48/4]=0x400      ;  //XNum
    ape0_addr[0x50/4]=0x400      ;  //YStep
    ape0_addr[0x58/4]=0x100      ;  //YNUM
    ape0_addr[0x60/4]=0x1        ;  //ZStep
    ape0_addr[0x68/4]=0x100      ;  //AllNum

    //DMA's control word
    ape0_addr[0x78/4]=0x1        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape0_addr[0x70/4]=0x0        ;
	a=ape0_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape0_addr[0x80/4]; //0:success 1:fail
    ape0_addr[0x90/4]=0x1        ;
    ape0_addr[0x98/4]=0x1        ;
    a=ape0_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape0_addr[0xA0/4] ;  //1: Finished, 0:transport in        
     
printf("APE0_DM1_row_out_%d\n",k);
   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU,APE1.DM0--->cDDR
    // Size: 256 KB 
    // APE's DMA configuration
    ape1_addr[0x10/4]=0x400000   ;   //APE's start Addr
    ape1_addr[0x18/4]=0x400      ;   //XNum
    ape1_addr[0x20/4]=0x400      ;   //YStep
    ape1_addr[0x28/4]=0x100      ;   //YNUM
    ape1_addr[0x30/4]=0x1        ;   //ZStep
    ape1_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape1_addr[0x40/4]=(int ) data_out_row[64+256*k];  //cDDR's start Addr
    ape1_addr[0x48/4]=0x400      ;  //XNum
    ape1_addr[0x50/4]=0x400      ;  //YStep
    ape1_addr[0x58/4]=0x100      ;  //YNUM
    ape1_addr[0x60/4]=0x1        ;  //ZStep
    ape1_addr[0x68/4]=0x100      ;  //AllNum

    //DMA's control word
    ape1_addr[0x78/4]=0x1        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape1_addr[0x70/4]=0x0        ;
	a=ape1_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape1_addr[0x80/4]; //0:success 1:fail
    ape1_addr[0x90/4]=0x1        ;
    ape1_addr[0x98/4]=0x1        ;
    a=ape1_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape1_addr[0xA0/4] ;   //1: Finished, 0:transport in  

printf("APE1_DM0_row_out_%d\n",k);
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU,APE1.DM1--->cDDR
    // Size: 256 KB 
    // APE's DMA configuration
    ape1_addr[0x10/4]=0x600000   ;   //APE's start Addr
    ape1_addr[0x18/4]=0x400      ;   //XNum
    ape1_addr[0x20/4]=0x400      ;   //YStep
    ape1_addr[0x28/4]=0x100      ;   //YNUM
    ape1_addr[0x30/4]=0x1        ;   //ZStep
    ape1_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape1_addr[0x40/4]=(int ) data_out_row[96+256*k];  //cDDR's start Addr
    ape1_addr[0x48/4]=0x400      ;  //XNum
    ape1_addr[0x50/4]=0x400      ;  //YStep
    ape1_addr[0x58/4]=0x100      ;  //YNUM
    ape1_addr[0x60/4]=0x1        ;  //ZStep
    ape1_addr[0x68/4]=0x100      ;  //AllNum

    //DMA's control word
    ape1_addr[0x78/4]=0x1        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape1_addr[0x70/4]=0x0        ;
	a=ape1_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape1_addr[0x80/4]; //0:success 1:fail
    ape1_addr[0x90/4]=0x1        ;
    ape1_addr[0x98/4]=0x1        ;
    a=ape1_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape1_addr[0xA0/4] ;  //1: Finished, 0:transport in        
     
printf("APE1_DM1_row_out_%d\n",k);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU,APE2.DM0--->cDDR
    // Size: 256 KB 
    // APE's DMA configuration
    ape2_addr[0x10/4]=0x400000   ;   //APE's start Addr
    ape2_addr[0x18/4]=0x400      ;   //XNum
    ape2_addr[0x20/4]=0x400      ;   //YStep
    ape2_addr[0x28/4]=0x100      ;   //YNUM
    ape2_addr[0x30/4]=0x1        ;   //ZStep
    ape2_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape2_addr[0x40/4]=(int )data_out_row[128+256*k];  //cDDR's start Addr
    ape2_addr[0x48/4]=0x400      ;  //XNum
    ape2_addr[0x50/4]=0x400      ;  //YStep
    ape2_addr[0x58/4]=0x100      ;  //YNUM
    ape2_addr[0x60/4]=0x1        ;  //ZStep
    ape2_addr[0x68/4]=0x100      ;  //AllNum

    //DMA's control word
    ape2_addr[0x78/4]=0x1        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape2_addr[0x70/4]=0x0        ;
	a=ape2_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape2_addr[0x80/4]; //0:success 1:fail
    ape2_addr[0x90/4]=0x1        ;
    ape2_addr[0x98/4]=0x1        ;
    a=ape2_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape2_addr[0xA0/4] ; //1: Finished, 0:transport in     

printf("APE2_DM0_row_out_%d\n",k);
   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU,APE2.DM1--->cDDR
    // Size: 256 KB 
    // APE's DMA configuration
    ape2_addr[0x10/4]=0x600000   ;   //APE's start Addr
    ape2_addr[0x18/4]=0x400      ;   //XNum
    ape2_addr[0x20/4]=0x400      ;   //YStep
    ape2_addr[0x28/4]=0x100      ;   //YNUM
    ape2_addr[0x30/4]=0x1        ;   //ZStep
    ape2_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape2_addr[0x40/4]=(int ) data_out_row[160+256*k];  //cDDR's start Addr
    ape2_addr[0x48/4]=0x400      ;  //XNum
    ape2_addr[0x50/4]=0x400      ;  //YStep
    ape2_addr[0x58/4]=0x100      ;  //YNUM
    ape2_addr[0x60/4]=0x1        ;  //ZStep
    ape2_addr[0x68/4]=0x100      ;  //AllNum

    //DMA's control word
    ape2_addr[0x78/4]=0x1        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape2_addr[0x70/4]=0x0        ;
	a=ape2_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape2_addr[0x80/4]; //0:success 1:fail
    ape2_addr[0x90/4]=0x1        ;
    ape2_addr[0x98/4]=0x1        ;
    a=ape2_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape2_addr[0xA0/4] ;   //1: Finished, 0:transport in        
 
printf("APE2_DM1_row_out_%d\n",k);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU,APE3.DM0--->cDDR
    // Size: 256 KB 
    // APE's DMA configuration
    ape3_addr[0x10/4]=0x400000   ;   //APE's start Addr
    ape3_addr[0x18/4]=0x400      ;   //XNum
    ape3_addr[0x20/4]=0x400      ;   //YStep
    ape3_addr[0x28/4]=0x100      ;   //YNUM
    ape3_addr[0x30/4]=0x1        ;   //ZStep
    ape3_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape3_addr[0x40/4]=(int ) data_out_row[192+256*k];  //cDDR's start Addr
    ape3_addr[0x48/4]=0x400      ;  //XNum
    ape3_addr[0x50/4]=0x400      ;  //YStep
    ape3_addr[0x58/4]=0x100      ;  //YNUM
    ape3_addr[0x60/4]=0x1        ;  //ZStep
    ape3_addr[0x68/4]=0x100      ;  //AllNum

    //DMA's control word
    ape3_addr[0x78/4]=0x1        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape3_addr[0x70/4]=0x0        ;
	a=ape3_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape3_addr[0x80/4]; //0:success 1:fail
    ape3_addr[0x90/4]=0x1        ;
    ape3_addr[0x98/4]=0x1        ;
    a=ape3_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape3_addr[0xA0/4] ; //1: Finished, 0:transport in    

printf("APE3_DM0_row_out_%d\n",k);    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // APE's DMA, CSU configuration -- data
    // CSU,APE3.DM1--->cDDR
    // Size: 256 KB 
    // APE's DMA configuration
    ape3_addr[0x10/4]=0x600000   ;   //APE's start Addr
    ape3_addr[0x18/4]=0x400      ;   //XNum
    ape3_addr[0x20/4]=0x400      ;   //YStep
    ape3_addr[0x28/4]=0x100      ;   //YNUM
    ape3_addr[0x30/4]=0x1        ;   //ZStep
    ape3_addr[0x38/4]=0x100      ;   //YAllNUM

    //DDR's DMA configuration
    ape3_addr[0x40/4]=(int ) data_out_row[224+256*k];  //cDDR's start Addr
    ape3_addr[0x48/4]=0x400      ;  //XNum
    ape3_addr[0x50/4]=0x400      ;  //YStep
    ape3_addr[0x58/4]=0x100      ;  //YNUM
    ape3_addr[0x60/4]=0x1        ;  //ZStep
    ape3_addr[0x68/4]=0x100      ;  //AllNum

    //DMA's control word
    ape3_addr[0x78/4]=0x1        ; //0:cDDR->pDDR0, 1:pDDR0->cDDR, 2:Global->Local
    ape3_addr[0x70/4]=0x0        ;
	a=ape3_addr[0x80/4]	 ; //DMACommandStatus 
    while((a|0xfffe)==0xffff)	a=ape3_addr[0x80/4]; //0:success 1:fail
    ape3_addr[0x90/4]=0x1        ;
    ape3_addr[0x98/4]=0x1        ;
    a=ape3_addr[0xA0/4]          ; //DMA Status, 0
    
    while((a&0x1)==0) a=ape3_addr[0xA0/4] ; //1: Finished, 0:transport in        

printf("APE3_DM1_row_out_%d\n",k);
 }

  return 0;
}


