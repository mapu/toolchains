/* Function Name: 8 Taps Interpolation
 * Author       : yangl
 * Time         : 2016/04/26
 * Description  : grep image, 1920*1080 -> 3840*2160.
 *              : individual sections: (160+7)*(60+7) -> 320*120, 12*18 sections in total
 */

// The header file below defines API function to call APC programs
#include "PolyFull.h"
//source signal and factor
#include <apc/ape.h>
#include <apc/loader.h>
#include <common.h>
#include <exports.h>
 
unsigned int BuffIndex = 0;  // JPEGŒƒº˛µ±«∞∂¡»°µƒŒª÷√
unsigned int BuffSize = 0;
int i, j, k;
int readbmpheader(unsigned char* fileName, int* pImageW, int *pImageH);
int readbmp(unsigned char* fileName, unsigned char* Buff, int* pImageW, int *pImageH);
int clip(int n, int Min, int Max);

#define inW 160
#define inH 60
#define _nTap 8
#define PadNum (_nTap - 1)
unsigned char pPicBuff[inW * inH];
short int DMInitData[inH + PadNum][inW + PadNum];
short int ResultMatrix[2 * inW][2 * inH];

//definations for global (full) image
#define GinW 1920
#define GinH 1080

#define GoutW (GinW * 2)
#define GoutH (GinH * 2)

#define SectionOutW (2 * inW) //320
#define SectionOutH (2 * inH) //120

#define WSectionNum (GinW / inW)//12
#define HSectionNum (GinH / inH)//18

#define WSectionPadLen (inW + PadNum)//160 + 7
#define HSectionPadLen (inH + PadNum)//60 + 7

//static short int GDMInitData[GinH + PadNum][GinW + PadNum];//(1920 + 7) * (1080 + 7)


//short int tempSectionMarix[HSectionPadLen][WSectionPadLen];

//main function
int FFT_arm(int argc, char * argv[]){
  //volatile int k;
//print head address
  //for(k=0; k<4; k++) {
  int _inW = inW;
  int _inH = inH;
  int _outW = 2 * _inW;
  int _outH = 2 * _inH;
  unsigned char* picName = (unsigned char *)0x72000000; //image location in DDR
  int nTap = _nTap;
  
  //static short int GResultMatrix[GoutW][GoutH];//3840 * 2160
  //init the memory for global result matrix
  //address for the first element of the array &pGResultMatrix[0][0]               : 0x61000000
  //address for the last element of the array &pGResultMatrix[GoutH - 1][GoutW - 1]: 0x61fd1ffe
  short int **ppGResultMatrix;
  short int *pGResultMatrix[GoutH];
  //assign address for 1-d arrays
  for(i = 0; i < GoutH; i++)
    pGResultMatrix[i] = (short int*)0x61000000 + i * GoutW; 
  //assign address for 2-d array
  ppGResultMatrix = &pGResultMatrix[0];
  
  
  //12 * 18 small sections, each of which is (160 + 7) * (60 + 7)
  //address for the first element of the array &pppSectionMatrix[0][0][0]                   : 0x62000000
  //address for the last element of the array \
  //&pppSectionMatrix[HSectionNum * WSectionNum - 1][HSectionPadLen - 1][WSectionPadLen - 1]: 0x6249c16e
  short int ***pppSectionMatrix;
  short int **ppSectionMatrix[WSectionNum * HSectionNum];
  short int *pSectionMatrix[WSectionNum * HSectionNum * HSectionPadLen];
  
  //assign address for every 1-d arrays
  for(i = 0; i < HSectionNum * WSectionNum; i++)
    for(j = 0; j < HSectionPadLen; j++)
      pSectionMatrix[i * HSectionPadLen + j] = (short int*)0x62000000 + i * HSectionPadLen * WSectionPadLen + j * WSectionPadLen;
  //assgin 2-d arrays
  for(i = 0; i < HSectionNum * WSectionNum; i++)
    ppSectionMatrix[i] = &pSectionMatrix[i * HSectionPadLen];
  //assgin 3-d arrays
  pppSectionMatrix = &ppSectionMatrix[0];
  
  
  //12 * 18 small sections, each of which is (320) * (120)
  //address for the first element of the array &pppSectionResultMatrix[0][0][0]                   : 0x63000000
  //address for the last element of the array \
  //&pppSectionResultMatrix[HSectionNum * WSectionNum - 1][SectionOutH - 1][SectionOutW - 1]: 0x63fd1ffe
  short int ***pppSectionResultMatrix;
  short int **ppSectionResultMatrix[WSectionNum * HSectionNum];
  short int *pSectionResultMatrix[WSectionNum * HSectionNum * SectionOutH];
  
  //assign address for every 1-d arrays
  for(i = 0; i < HSectionNum * WSectionNum; i++)
    for(j = 0; j < SectionOutH; j++)
      pSectionResultMatrix[i * SectionOutH + j] = (short int*)0x63000000 + i * SectionOutH * SectionOutW + j * SectionOutW;
  //assgin 2-d arrays
  for(i = 0; i < HSectionNum * WSectionNum; i++)
    ppSectionResultMatrix[i] = &pSectionResultMatrix[i * SectionOutH];
  //assgin 3-d arrays
  pppSectionResultMatrix = &ppSectionResultMatrix[0];
  /***************************************************************************/
  
  readbmpheader(picName, &_inW, &_inH);
  readbmp(picName, pPicBuff, &_inW, &_inH);   
  
  int PadNum1 = nTap / 2 - 1;
  
  
  for(k = 0; k < HSectionNum; k++){ //18 lines
    //APE0
    for(i = 0; i < HSectionPadLen; i++)
      for(j = 0; j < WSectionPadLen; j++)
        pppSectionMatrix[k + 0][i][j] = *(pPicBuff + (clip(i,PadNum1,inH+PadNum1-1) - PadNum1)*GinW
                                    + (clip(j,PadNum1,inW+PadNum1-1) - PadNum1) );
      
    printf("sending matrix...\n");
    send_matrix(0x400000, (unsigned int)pppSectionMatrix[0][0], WSectionPadLen, HSectionPadLen, DT_HALF, 0);
    ape_dma_wait(DATA_E2I_GP, 0);
    printf("calculating...\n");
    PolyFull(NULL, 0);
    ape_wait(0);
    printf("calculated done!\n");
    fetch_matrix((uint32_t)pppSectionResultMatrix[0][0], 0xC00000, SectionOutW, SectionOutH, DT_HALF, 0);
    ape_dma_wait(DATA_I2E_GP, 0);
    
    //APE1
    
    //APE2
    
    //APE3
    
    
    
  }
  
  
  /*
  for(i = 0; i < SectionOutH; i++) {
    for(j = 0; j < SectionOutW; j++)
      printf("%d ",pppSectionResultMatrix[0][i][j]);
    printf("===\n");
  }*/
  
  
  
  
//spu
  /*
  SPULoad(0, 0);printf("spuload_APE0_%d\n",k);
  SPULoad(0, 1);printf("spuload_APE1_%d\n",k);
  SPULoad(0, 2);printf("spuload_APE2_%d\n",k);
  SPULoad(0, 3);printf("spuload_APE3_%d\n",k);
  ape_wait(0);printf("wait_APE0_%d\n",k);
  ape_wait(1);printf("wait_APE1_%d\n",k);
  ape_wait(2);printf("wait_APE2_%d\n",k);
  ape_wait(3);printf("wait_APE3_%d\n",k);
  printf("fft_over_%d\n",k);
  //}
  */

    
  return 0;
}


int clip(int n, int Min, int Max)
{
  int temp;
  if(Max < Min)
    return 0;
  temp = n;
  if(n < Min)
    temp = Min;
  else if(n > Max)
    temp = Max;
  return temp;
}

//bmp operation

//∂¡»°8Bits
unsigned char get_byte(unsigned char *buff) {
  if (BuffIndex >= BuffSize) return 0;
  return buff[BuffIndex++];
}

float pow(float base, float ex){
    // power of 0
    if (ex == 0){
      return 1;
    // negative exponenet
    }
    else if( ex < 0){
      return 1 / pow(base, -ex);
    // even exponenet
    }
    else if ((int)ex % 2 == 0){
        float half_pow = pow(base, ex/2);
        return half_pow * half_pow;
    //integer exponenet
    }
    else{
      return base * pow(base, ex - 1);
    }
}

int readbmpheader(unsigned char* fileName, int* pImageW, int *pImageH) {
  unsigned char buff[4];
  unsigned int x, y, i;
  unsigned int t1 = 0, t2 = 0;

  if (fileName == NULL)
    return 0;

  unsigned char * fp = fileName;
  //fp = fp+0x12;
  //fseek(fp, 0x12, 0);
  //fread(buff, 1, 4, fp);
  fp = fileName + 0x12;
  for(i = 0;i < 4; i++)
    buff[i] = *(fp + i);

  t1 = 0;
  for (i = 0; i < 4; i++) {
    t2 = buff[i];
    t2 <<= i * 8;
    t1 = t1 | t2;
  }
  x = t1;

  //fp = fp+0x16;
  //fseek(fp, 0x16, 0);
  //fread(buff, 1, 4, fp);  // ∂¡Œƒº˛µΩbuff
  fp = fileName + 0x16;
  for(i = 0;i < 4; i++)
    buff[i] = *(fp + i);
  t1 = 0;

  for (i = 0; i < 4; i++) {
    t2 = buff[i];
    t2 <<= i * 8;
    t1 = t1 | t2;
  }
  y = t1;

  *pImageW = x;
  *pImageH = y;

  return 1;
  //fclose(fp);
}

int readbmp(unsigned char* fileName, unsigned char* Buff, int* pImageW, int *pImageH) {
  //unsigned char *buff = NULL;
  unsigned char str;
  unsigned int length, x, y, i, j;
  unsigned int t1 = 0, t2 = 0;

  if (fileName == NULL)
    return 0;

  //FILE *fp;
  unsigned char * fp = fileName;

  BuffSize = 0;
  /*while (!feof(fp)) {
    fgetc(fp);
    BuffSize++;
  }*/
  //BuffSize--;
  int n = 8; //color depth
  BuffSize = 54 + 4 * (int)pow(2,n) + (*pImageW * *pImageH * n)/8;
  //buff = (unsigned char *) malloc(BuffSize * sizeof(char));
  unsigned char buff[BuffSize];
  //rewind(fp);
  //fread(buff, 1, BuffSize, fp);  // ∂¡Œƒº˛µΩbuff
  fp = fileName;
  for(i = 0;i < BuffSize; i++)
    buff[i] = *(fp + i);



  BuffIndex = 0x12;
  t1 = 0;
  for (i = 0; i < 4; i++) {
    str = get_byte(buff);
    t2 = str;
    t2 <<= i * 8;
    t1 = t1 | t2;
  }
  x = t1;

  t1 = 0;
  BuffIndex = 0x16;
  for (i = 0; i < 4; i++) {
    str = get_byte(buff);
    t2 = str;
    t2 <<= i * 8;
    t1 = t1 | t2;
  }
  y = t1;

  t1 = 0;
  BuffIndex = 0x0a;
  for (i = 0; i < 4; i++) {
    str = get_byte(buff);
    t2 = str;
    t2 <<= i * 8;
    t1 = t1 | t2;
  }
  BuffIndex = t1;
  length = x * y;
  //Buff=(short*)malloc(length);//√ø–––Ë≤π∆Î4µƒ’˚ ˝±∂
  for (i = 0; i < length; i++) {
    Buff[i] = get_byte(buff) & 0x00ff;
  }
  //ÕºœÛµπ–Ú
  for (i = 0; i < y / 2; i++) {
    for (j = 0; j < x; j++) {
      str = Buff[i * x + j];
      Buff[i * x + j] = Buff[((y - 1) * x - i * x) + j];
      Buff[((y - 1) * x - i * x) + j] = str;
    }
  }

  *pImageW = x;
  *pImageH = y;
  return 1;
  //fclose(fp);
  //free(buff);

}