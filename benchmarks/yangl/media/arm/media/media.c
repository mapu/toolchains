#include "medias.h"
#include "common.h"
#include "exports.h"

#define SDA0DM0_START 0x400000
#define SDA1DM0_START 0x800000
#define SDA2DM0_START 0xc00000
#define PRCM_BASEADDR 0x50340000
#define DIS_BASEADDR 0x50504000
#define APE0_START 0
#define APE1_START 480
#define APE2_START 960
#define APE3_START 1440
#define E_STEP_H 115800
#define T_STEP_H 115620
#define STEP_W 160
//int32_t send_vector(uint32_t dst, uint32_t src, uint32_t len, uint32_t size, uint32_t cpuid);
//int32_t ape_dma_wait(uint32_t group, uint32_t cpuid);
//int32_t fetch_vector(uint32_t dst, uint32_t src, uint32_t len, uint32_t size, uint32_t cpuid);

void display(void)
{
	unsigned char chr;
	unsigned char chr1;
	unsigned char ptr;
	unsigned int intxy;
//	unsigned int *CAM  = (int*) CAM_BASEADDR;
	unsigned int *DIS = (int*) DIS_BASEADDR;
	unsigned int *PRCM = (int*) PRCM_BASEADDR;
	unsigned int tmp11;
//	printf("hello world\n\r");
//	scanf("%d", &tmp);
	chr=65;
	//DIS[6]=0X01000010;
	//DIS[16]=0x60000000;
	//DIS[21]=0x100;
	//DIS[6]=0X11000010;
	//while(1);
	//30fps
	//PRCM[2] = 0x02001502;
	//PRCM[2] = 0x00001502;
	//PRCM[2] = 0x02012a02;
	//PRCM[2] = 0x00012a02;
	//16fps
	//PRCM[2] = 0x02001503;
	//PRCM[2] = 0x00001503;
	//50fps_ok_shake
	//PRCM[2] = 0x02002402;
	//PRCM[2] = 0x00002402;
	//PRCM[2] = 0x02012401;
	//PRCM[2] = 0x00012401;
	//50fps_ok_stable
	//PRCM[2] = 0x02014802;
	//PRCM[2] = 0x00014802;
	//60fps
	PRCM[2] = 0x02012a01;
	PRCM[2] = 0x00012a01;
	//TV_Cfg();
	DIS[0]=0x001E000A;//30,10
	DIS[1]=0x00040438;//4,1080
	DIS[2]=0x0094002C;//148,44
	DIS[3]=0x00580780;//88,1920
	//DIS[3]=0x005803c0;//120,960
	DIS[7]=0x04380780;
	DIS[4]=0x70000000;//
	DIS[7]=0x04380780;
	DIS[6]=0x00000000;
	DIS[8]=0x60000000;
	DIS[16]=0x76000000;
	DIS[24]=0x60000000;
	DIS[32]=0x60000000;
	DIS[40]=0x60000000;
	DIS[48]=0x60000000;
	DIS[17]=0X00000000;
	DIS[18]=0X04300780;
	DIS[19]=0X00000000;
	DIS[20]=0X04300780;
	DIS[21]=0X00000100;
	if(1){
		//4k
		DIS[0]=0x001E000A;//30,10
		DIS[1]=0x00040870;//4,2160
		DIS[2]=0x0094002C;//148,44
		DIS[3]=0x00580f00;//88,3840
		//DIS[3]=0x00580780;//120,1920
		DIS[7]=0x08700f00;
		DIS[18]=0x08700f00;
		DIS[20]=0x08700eff;
		//4k^
	}
	DIS[6]=0X11000010;
	//while(1);
}
int32_t my_send_sub_matrix(uint32_t dst, uint32_t src, uint32_t width, uint32_t sub_width, uint32_t height,
                         enum DATA_TYPE size, uint32_t cpuid) {
  struct dma_if conf;
  uint32_t bank_size = 0x1000;
  uint32_t bank_num = 64;
  if (size > 6) return -1;
  bank_num >>= size;
  size = 1 << size;
  bank_size *= size;
  conf.DMAGlobalAddr = src;
  conf.DMAGlobalXNum = size * sub_width;
  conf.DMAGlobalYAllNum = height;
  conf.DMAGlobalYNum = bank_num;
  conf.DMAGlobalYStep = size * width;
  conf.DMAGlobalZStep = size * width * bank_num;
  conf.DMALocalAddr = dst;
  conf.DMALocalXNum = size * sub_width;
  conf.DMALocalYAllNum = height;
  conf.DMALocalYNum = bank_num;
  conf.DMALocalYStep = bank_size;
  conf.DMALocalZStep = size * sub_width;
  conf.DMACmd = CMD_EXT2INT;
  conf.DMAGroupNum = DATA_E2I_GP;
  return ape_dma_request(&conf, cpuid);
}

int32_t my_fetch_sub_matrix(uint32_t dst, uint32_t src, uint32_t width, uint32_t sub_width, uint32_t height,
                     enum DATA_TYPE size, uint32_t cpuid) {
  struct dma_if conf;
  uint32_t bank_size = 0x1000;
  uint32_t bank_num = 64;
  if (size > 6) return -1;
  bank_num >>= size;
  size = 1 << size;
  bank_size *= size;
  conf.DMAGlobalAddr = dst;
  conf.DMAGlobalXNum = size * sub_width;
  conf.DMAGlobalYAllNum = height;
  conf.DMAGlobalYNum = bank_num;
  conf.DMAGlobalYStep = size * width;
  conf.DMAGlobalZStep = size * width * bank_num;
  conf.DMALocalAddr = src;
  conf.DMALocalXNum = size * sub_width;
  conf.DMALocalYAllNum = height;
  conf.DMALocalYNum = bank_num;
  conf.DMALocalYStep = bank_size;
  conf.DMALocalZStep = size * sub_width;
  conf.DMACmd = CMD_INT2EXT;
  conf.DMAGroupNum = DATA_I2E_GP;
  return ape_dma_request(&conf, cpuid);
}







    short Res0[69][170];
    short Res1[67][167];
    short Res2[67][167];


int main(int argc, char * argv[])
{
  short DE_MH[672];
  unsigned char CE_MB[128];
  unsigned short CE_MH[1184];
  int CE_M32[64];
  int expAddr[216];
  int tmpAddr[216];
  const int PixelNum = 320*120;
  unsigned short *Src=(unsigned short *)0x72000000;
  unsigned int *Result=(unsigned int *)0x76000000;
  unsigned char *ResultByte=(unsigned char *)0x76000000;
  unsigned short *ResultShort=(unsigned short *)0x76000000;
//  unsigned short *de=(unsigned short *)0x76000000;
//  short *ce=(short *)0x76000000;
  int i,j;
  int *p = (int *)0x40402000;
  *p = 0;
  int BIU[3][3];
  BIU[0][0]=0x400000;
  BIU[0][1]=0x600000;
  BIU[1][0]=0x800000;
  BIU[1][1]=0xA00000;
  BIU[2][0]=0xc00000;
  *p = 0;
  /* initialze the source data */

  for(i=0;i<32;i++)
  {
    DE_MH[ 0*32+i] =-128;
    DE_MH[ 1*32+i] =-56;
    DE_MH[ 2*32+i] =-176;
    DE_MH[ 3*32+i] =720;
    DE_MH[ 4*32+i] =-176;
    DE_MH[ 5*32+i] =-56;
    DE_MH[ 6*32+i] =-128;
    DE_MH[ 7*32+i] =85;
    DE_MH[ 8*32+i] =0;
    DE_MH[ 9*32+i] =-1;
    DE_MH[10*32+i] =12;
    DE_MH[11*32+i] =8192;
    DE_MH[12*32+i] =8;
    DE_MH[13*32+i] =-256;
    DE_MH[14*32+i] =-256;
    DE_MH[15*32+i] =1024;
    DE_MH[16*32+i] =-256;
    DE_MH[17*32+i] =-256;
    DE_MH[18*32+i] =1023;
    DE_MH[19*32+i] =100;
    DE_MH[20*32+i] =-100;
  }
  for(i=0;i<64;i++)
  {
    CE_MB[0*64+i]=i%2;
    CE_MB[1*64+i]=(i/2)*2;
  }
  for(i=0;i<32;i++)
  {
    CE_MH[0*32+i] =31;
    CE_MH[1056+i] =128;
    CE_MH[1088+i] =2;
    CE_MH[1120+i] =0;
    CE_MH[1152+i] =255;
  }
  for(i=0;i<1024;i++)
  {
    CE_MH[32+i] =i;
  }
  for(i=0;i<16;i++)
  {
    CE_M32[0*16+i] =0x1C501C5;
    CE_M32[1*16+i] =0x580058;
    CE_M32[2*16+i] =182;
    CE_M32[3*16+i] =358;
  }
  
  send_vector(SDA2DM0_START+0x3EB00, (uint32_t)DE_MH, 672, DT_HALF, 0);
  ape_dma_wait(DATA_E2I_GP, 0);//DE

  send_vector(SDA2DM0_START+0x3F040, (uint32_t)CE_MB, 128, DT_BYTE, 0);
  ape_dma_wait(DATA_E2I_GP, 0);//CE

  send_vector(SDA2DM0_START+0x3F040+128, (uint32_t)CE_MH, 1184, DT_HALF, 0);
  ape_dma_wait(DATA_E2I_GP, 0);//CE

  send_vector(SDA2DM0_START+0x3F040+2496, (uint32_t)CE_M32, 64, DT_WORD, 0);
  ape_dma_wait(DATA_E2I_GP, 0);//CE

  send_vector(SDA2DM0_START+0x3FB00, (uint32_t)0x78000000, 1280, DT_BYTE, 0);
  ape_dma_wait(DATA_E2I_GP, 0);//CC

for(i=0; i<18; i++)
for(j=0; j<12; j++)
{
   expAddr[i*12+j]=i*E_STEP_H+j*STEP_W;
   tmpAddr[i*12+j]=i*T_STEP_H+j*STEP_W;
}
 //   display();
///////////////////////////////////////////////////////////////////////////////////
/*
BIU[0][0]=0x400000;
  BIU[0][1]=0x600000;
  BIU[1][0]=0x800000;
  BIU[1][1]=0xA00000;
int32_t send_sub_matrix(uint32_t dst, uint32_t src, uint32_t width, uint32_t sub_width, uint32_t height,
                         enum DATA_TYPE size, uint32_t cpuid)
*/
    my_send_sub_matrix(BIU[0][0], (uint32_t)(Src), 1930, 170, 69, DT_HALF, 0);
    ape_dma_wait(DATA_E2I_GP, 0);//y
   /* send_vector(SDA2DM0_START+0x3F040+192, (uint32_t)(Src+6291068), 1024, DT_HALF, 0);
    ape_dma_wait(DATA_E2I_GP, 0);//CE
*/

    /*my_send_sub_matrix(BIU[0][0]+1020, (uint32_t)(Src+2101770), 1927, 167, 67, DT_HALF, 0);
    ape_dma_wait(DATA_E2I_GP, 0);//u
    my_send_sub_matrix(BIU[0][0]+2022, (uint32_t)(Src+4196419), 1927, 167, 67, DT_HALF, 0);
    ape_dma_wait(DATA_E2I_GP, 0);//v
*/

    medias(NULL, 0);
    //ape_wait(0);
 
    printf("1st-post\n");

    fetch_matrix((uint32_t)Res0, BIU[0][0], 170, 69, DT_HALF, 0);
    ape_dma_wait(DATA_I2E_GP, 0);
/*
    fetch_matrix((uint32_t)Res1, BIU[0][0]+1020, 167, 67, DT_HALF, 0);
    ape_dma_wait(DATA_I2E_GP, 0);
    fetch_matrix((uint32_t)Res2, BIU[0][0]+2022, 167, 67, DT_HALF, 0);
    ape_dma_wait(DATA_I2E_GP, 0);*/
    //printf("1st-pre\n");
    int i0, i1;
    for(i0 = 0; i0 < 69; i0++) {
      for(i1 = 0; i1 < 170; i1++)
        printf("%d ",Res0[i0][i1]);
      printf("==================================\n");
    }
    printf("#######################################\n");
    printf("#######################################\n");
    printf("#######################################\n");
/*
    for(i0 = 0; i0 < 67; i0++) {
      for(i1 = 0; i1 < 167; i1++)
        printf("%d ",Res1[i0][i1]);
      printf("==================================\n");
    }


    printf("#######################################\n");
    printf("#######################################\n");
    printf("#######################################\n");
    for(i0 = 0; i0 < 67; i0++) {
      for(i1 = 0; i1 < 167; i1++)
        printf("%d ",Res2[i0][i1]);
      printf("==================================\n");
    }*/


    

    /*my_send_sub_matrix(BIU[0][!(*p)], (uint32_t)(Src+expAddr[1]), 1930, 170, 69, DT_HALF, 0);
    ape_dma_wait(DATA_E2I_GP, 0);//y
    my_send_sub_matrix(BIU[0][!(*p)]+1020, (uint32_t)(Src+2101770+tmpAddr[1]), 1927, 167, 67, DT_HALF, 0);
    ape_dma_wait(DATA_E2I_GP, 0);//u
    my_send_sub_matrix(BIU[0][!(*p)]+2022, (uint32_t)(Src+4196419+tmpAddr[1]), 1927, 167, 67, DT_HALF, 0);
    ape_dma_wait(DATA_E2I_GP, 0);//v
*/

    ape_wait(0);

    fetch_matrix((uint32_t)(ResultShort+0), BIU[2][0], 326, 124, DT_HALF, 0);
    ape_dma_wait(DATA_I2E_GP, 0);//wait DMA to finish
    /*fetch_matrix((uint32_t)(ResultShort+40424), BIU[1][0]+2560, 320, 120, DT_HALF, 0);
    ape_dma_wait(DATA_I2E_GP, 0);//wait DMA to finish
    fetch_matrix((uint32_t)(ResultShort+78824), BIU[1][0]+5120, 320, 120, DT_HALF, 0);
    ape_dma_wait(DATA_I2E_GP, 0);//wait DMA to finish*/
    for(i=0;i<124;i++) {
      for(j=0;j<326;j++)
        printf("%d\t",*(ResultShort+(i)*326+j));
        //printf("%d\t",*(ResultShort+i*320+j+40424));
        //printf("%d\n",*(ResultShort+i*320+j+78824));
      printf("==================================\n");
    }


/*
///////////////////////////////////////////////////////////////////////////////////////
for(i=1; i<215; i++)
{
    *p = !(*p);
    printf("%d-pre\n",i+1);
    medias(NULL, 0);

    fetch_sub_matrix((uint32_t)(Result+tmpAddr[i-1]), BIU[1][!(*p)], 3840, 640, 120, DT_HALF, 0);
    ape_dma_wait(DATA_I2E_GP, 0);//wait DMA to finish 
    printf("%d-fetch\n",i+1);
    send_sub_matrix(BIU[0][!(*p)], (uint32_t)(Src+expAddr[i+1]), 1930, 170, 69, DT_HALF, 0);
    ape_dma_wait(DATA_E2I_GP, 0);//y
    send_sub_matrix(BIU[0][!(*p)]+1020, (uint32_t)(Src+2101770+tmpAddr[i+1]), 1927, 167, 67, DT_HALF, 0);
    ape_dma_wait(DATA_E2I_GP, 0);//u
    send_sub_matrix(BIU[0][!(*p)]+2022, (uint32_t)(Src+4196419+tmpAddr[i+1]), 1927, 167, 67, DT_HALF, 0);
    ape_dma_wait(DATA_E2I_GP, 0);//v
    printf("%d-fetch\n",i+1); 
    ape_wait(0);
    printf("%d-post\n",i+1);
}
///////////////////////////////////////////////////////////////////////////////////////////////
    *p = !(*p);
    printf("last-pre\n");
    medias(NULL, 0);
    printf("last-post\n");    
    fetch_sub_matrix((uint32_t)(Result+tmpAddr[i-1]), BIU[1][0], 3840, 640, 120, DT_HALF, 0);
    ape_dma_wait(DATA_I2E_GP, 0);//wait DMA to finish

    ape_wait(0);

    fetch_sub_matrix((uint32_t)(Result+tmpAddr[i]), BIU[1][1], 3840, 640, 120, DT_HALF, 0);
    ape_dma_wait(DATA_I2E_GP, 0);//wait DMA to finish
*/
/////////////////////////////////////////////////////////////////////////////////

    display();
  
////////////////////////////

  return 0;
}

