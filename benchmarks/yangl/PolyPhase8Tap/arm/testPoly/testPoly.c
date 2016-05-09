//  Date: 2016-03-15
//  Author: Yanglei
//  PolyPhase 8 Tap, Width, (240 + 7) * (135 + 7)-> (480) * (270), Modification for multi-tap

#include "PolyPhase.h"
#include "common.h"
#include "exports.h"


unsigned int BuffIndex = 0;  // JPEGŒƒº˛µ±«∞∂¡»°µƒŒª÷√
unsigned int BuffSize = 0;
int i, j;
int readbmpheader(unsigned char* fileName, int* pImageW, int *pImageH);
int readbmp(unsigned char* fileName, unsigned char* Buff, int* pImageW, int *pImageH);
int clip(int n, int Min, int Max);

#define inW 256
#define inH 256
#define _nTap 8
#define PadNum (_nTap - 1)
unsigned char pPicBuff[inW * inH];
short int DMInitData[inH + PadNum][inW + PadNum];
short int ResultMatrix[2 * inW][2 * inH];

//*************************************************************************************************
int main()
{
  int _inW = inW;
  int _inH = inH;
  unsigned char* picName = (unsigned char *)0x72000000; //image location in DDR
  int nTap = _nTap;

  readbmpheader(picName, &_inW, &_inH);
  //pPicBuff = (unsigned char*) malloc(inW * inH * sizeof(unsigned char));
  
  readbmp(picName, pPicBuff, &_inW, &_inH);   
  
  //Matrix adding pad
  int PadNum1 = nTap / 2 - 1;
  for(i = 0; i < inH + PadNum; i++)
    for(j = 0; j < inW + PadNum; j++)
      DMInitData[i][j] = *(pPicBuff + (clip(i,PadNum1,inH+PadNum1-1) - PadNum1)*inW + (clip(j,PadNum1,inW+PadNum1-1) - PadNum1) );

  printf("sending matrx...\n");
  send_matrix(0x400000, (unsigned int)DMInitData, inW + PadNum, inH + PadNum, DT_HALF, 0);
  ape_dma_wait(DATA_E2I_GP, 0);
  printf("calculating...\n");
  PolyPhase(NULL, 0);
  ape_wait(0);
  printf("calculated done!\n");
  //fetch_matrix((uint32_t)ResultMatrix, 0xC00000, 2 * inW, 2 * inH, DT_HALF, 0);
  ape_dma_wait(DATA_I2E_GP, 0);
  /*for(i = 0; i < inW; i++) {
    for(j = 0; j < inH; j++) 
      printf("%d ", ResultMatrix[i][j]);
    printf("\n");
  }*/
  
  /*for(i = 0; i < inH + nTap - 1; i++) {
    for(j = 0; j < inW + nTap - 1; j++) 
      printf("%d ", DMInitData[i][j]);
    printf("\n");
  }*/
  /*for(i = 0; i < inH; i++) {
    for(j = 0; j < inW; j++) 
      printf("%d ", *( pPicBuff + i * inW + j));
    printf("\n");
  }*/
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
