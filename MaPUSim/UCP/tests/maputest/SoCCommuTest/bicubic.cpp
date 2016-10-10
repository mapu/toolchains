//
//  main.c
//  Bicubic
//
//  Created by Lei Wang on 13-4-27.
//  Copyright (c) 2013年 Lei Wang. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
//#include "Bicubic.h"
//#include "Bicubic_opt.h"
//#include "Bicubic_mapu.h"
#include "BicubicDemo.h"
#include "csu_object.hh"
#include "elf_object.hh"

#include "math.h"



int min(int A,int B)
{
  return((A)<(B)?(A):(B));
}
int max(int A,int B)
{
  return((A)>(B)?(A):(B));
}

void Bicubic(unsigned char** inY, unsigned char** outY_temp,unsigned char** outY,int inW,int inH, int outW,int outH)
{

  int nBits = 10;
  int nUnit = 1 << nBits;
  int nHalf = 1 << (nBits - 1);
  int nDBits = nBits + 1;
  int nDHalf = 1 << (nDBits - 1);
  int nMaxValue = 255;


  //horizontally interpolate
  for (int i = 0; i < outH; i++)
  {
    // location Y in input image (use integer programming)
    int nY10Bits = (i << nBits) * inH / outH + 1;
    int nY = nY10Bits >> nBits;
    int nYm1 = min(max((nY - 1),0),inH-1);
    int nYp1 = min(max((nY + 1),0),inH-1);
    int nYp2 = min(max((nY + 2),0),inH-1);

    int nAlphaY = nY10Bits - (nY << nBits);
    int nAlphaY2 = (nAlphaY * nAlphaY + nHalf) >> nBits;
    int nAlphaY3 = (nAlphaY2 * nAlphaY + nHalf) >> nBits;

    int nC0 = -nAlphaY3 + (nAlphaY2 << 1) - nAlphaY;
    int nC1 = nAlphaY3 * 3 - nAlphaY2 * 5 + (nUnit << 1);
    int nC2 = -nAlphaY3 * 3 + (nAlphaY2 << 2) + nAlphaY;
    int nC3 = nAlphaY3 - nAlphaY2;

    for (int j = 0; j < inW; j++)
    {

      int outY0 = (inY[nYm1][j] * nC0 + inY[nY][j] * nC1 + inY[nYp1][j] * nC2 + inY[nYp2][j]*nC3 + nDHalf) >> nDBits;
      outY_temp[i][j] = min(max(outY0, 0), nMaxValue);
    }
  }

  //vertically interpolate
  for (int j = 0; j < outW; j++)
  {
    int nX10Bits = (j << nBits) * inW / outW + 1;
    int nX = nX10Bits >> nBits;
    int nXm1 = min(max((nX - 1),0),inW-1);
    int nXp1 = min(max((nX + 1),0),inW-1);
    int nXp2 = min(max((nX + 2),0),inW-1);

    int nAlphaX = nX10Bits - (nX << nBits);
    int nAlphaX2 = (nAlphaX * nAlphaX + nHalf) >> nBits;
    int nAlphaX3 = (nAlphaX2 * nAlphaX + nHalf) >> nBits;

    int nW0 = -nAlphaX3 + (nAlphaX2 << 1) - nAlphaX;
    int nW1 = nAlphaX3 * 3 - nAlphaX2 * 5 + (nUnit << 1);
    int nW2 = -nAlphaX3 * 3 + (nAlphaX2 << 2) + nAlphaX;
    int nW3 = nAlphaX3 - nAlphaX2;
    for (int i = 0; i < outH; i++)
    {

      int outY0 = (outY_temp[i][nXm1] * nW0 + outY_temp[i][nX] * nW1 + outY_temp[i][nXp1] * nW2 + outY_temp[i][nXp2]*nW3 + nDHalf) >> nDBits;
      outY[i][j] = min(max(outY0, 0), nMaxValue);

    }
  }
}

#define CLIP1(x) (x & ~255) ? (-x >> 31) : x

unsigned int BuffIndex = 0;  // JPEGŒƒº˛µ±«∞∂¡»°µƒŒª÷√
unsigned int BuffSize = 0;

//∂¡»°8Bits
unsigned char get_byte(unsigned char *buff) {
  if (BuffIndex >= BuffSize) return 0;
  return buff[BuffIndex++];
}
void WriteBMP(const char* pFileName, unsigned char** ppBuff, int nW, int nH) {
  BmpHead headBMP;
  InfoHead infoHead;
  RGBMixPlate mainColor[256];

  FILE *p1 = fopen(pFileName, "wb");
  //FILE *p1 = fopen("c:\\grey.bmp","wb");
  if (p1 == NULL)

  {

    printf("open new file failed.\n");

    // return -1;

  }

  //write BMPHead
  headBMP.imageSize = 14 + 40 + 1024 + nW * nH;
  headBMP.blank = 0;
  headBMP.startPosition = 14 + 40 + 1024;

  //write InfoHead

  infoHead.Length = 40;
  infoHead.width = nW;
  infoHead.height = nH;
  infoHead.colorPlane = 1;
  infoHead.bitColor = 8;
  infoHead.zipFormat = 0;
  infoHead.realSize = nW * nH;
  infoHead.xPels = 0;  // nH;
  infoHead.yPels = 0;  //nW;

  infoHead.colorUse = 256;
  infoHead.colorImportant = 256;

  for (int i = 0; i < 256; i++) {
    mainColor[i].rgbBlue = i;
    mainColor[i].rgbGreen = i;
    mainColor[i].rgbRed = i;
    mainColor[i].rgbReserved = 0;
  }

  char hh[2] = { 0x42, 0x4D };

  fwrite(hh, 1, 2, p1);                //BMPŒƒº˛ø™Õ∑¡Ω◊÷Ω⁄, 0x4d42 = "BM"

  fwrite(&headBMP, 1, 12, p1);    //BMPŒƒº˛Õ∑

  fwrite(&infoHead, 1, 40, p1);    //BMPŒƒº˛Õ∑–≈œ¢

  fwrite(mainColor, 1, sizeof(mainColor), p1);    //–¥µ˜…´≈Ã–≈œ¢;

  for (int i = 0; i < nH; i++) {
    for (int j = 0; j < nW; j++) {
      fwrite((unsigned char*) (&ppBuff[nH - 1 - i][j]), 1, 1, p1);  //—’…´ ˝æ›
    }
  }

  fclose(p1);

}
void readbmpheader(char* fileName, int* pImageW, int *pImageH) {
  unsigned char buff[4];
  unsigned int x, y, i;
  unsigned int t1 = 0, t2 = 0;

  FILE *fp;

  if ((fp = fopen(fileName, "rb")) == NULL) {
    perror(0);
    exit(0);
  }

  rewind(fp); // Ω´Œƒº˛÷∏’Î÷∏œÚŒƒº˛Õ∑
  //fp = fp+0x12;
  fseek(fp, 0x12, 0);
  fread(buff, 1, 4, fp);  // ∂¡Œƒº˛µΩbuff

  t1 = 0;
  for (i = 0; i < 4; i++) {
    t2 = buff[i];
    t2 <<= i * 8;
    t1 = t1 | t2;
  }
  x = t1;

  //fp = fp+0x16;
  fseek(fp, 0x16, 0);
  fread(buff, 1, 4, fp);  // ∂¡Œƒº˛µΩbuff
  t1 = 0;

  for (i = 0; i < 4; i++) {
    t2 = buff[i];
    t2 <<= i * 8;
    t1 = t1 | t2;
  }
  y = t1;

  *pImageW = x;
  *pImageH = y;
  fclose(fp);

}

void readbmp(char* fileName, unsigned char* Buff, int* pImageW, int *pImageH) {
  unsigned char *buff = NULL;
  unsigned char str;
  unsigned int length, x, y, i, j;
  unsigned int t1 = 0, t2 = 0;

  FILE *fp;

  if ((fp = fopen(fileName, "rb")) == NULL) {
    perror(0);
    exit(0);
  }

  // ªÒ»°Œƒº˛≥§∂»
  BuffSize = 0;
  while (!feof(fp)) {
    fgetc(fp);
    BuffSize++;
  }
  BuffSize--;
  buff = (unsigned char *) malloc(BuffSize * sizeof(char));
  rewind(fp); // Ω´Œƒº˛÷∏’Î÷∏œÚŒƒº˛Õ∑
  fread(buff, 1, BuffSize, fp);  // ∂¡Œƒº˛µΩbuff

  //∂¡»°Œƒº˛ ˝æ›«¯ ◊µÿ÷∑
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
  fclose(fp);
  free(buff);

}

int main() {
  FILE *fpout, *fpargs;

  char picName[256] = "in.bmp";
  unsigned char** ppPicBuff;
  unsigned char** ppOutBuff_temp;
  unsigned char** ppOutBuff;

  unsigned char* pPicBuff;
  unsigned char* pOutBuff_temp;
  unsigned char* pOutBuff;

  int inW = 80;
  int inH = 80;
  int outW = 240;
  int outH = 180;

  int i, j;

  //demo 16x16->40x40

  readbmpheader(picName, &inW, &inH);

  ppPicBuff = (unsigned char**) malloc(inH * sizeof(unsigned char*));
  pPicBuff = (unsigned char*) malloc(inW * inH * sizeof(unsigned char));

  for (i = 0; i < inH; i++) {
    ppPicBuff[i] = pPicBuff + i * inW;
  }
  readbmp(picName, pPicBuff, &inW, &inH);

  ppOutBuff_temp = (unsigned char **) malloc(outH * sizeof(unsigned char*));
  pOutBuff_temp = (unsigned char *) malloc(outH * inW * sizeof(unsigned char));

  for (i = 0; i < outH; i++) {
    ppOutBuff_temp[i] = pOutBuff_temp + i * inW;
  }

  ppOutBuff = (unsigned char **) malloc(outH * sizeof(unsigned char*));
  pOutBuff = (unsigned char *) malloc(outH * outW * sizeof(unsigned char));

  for (i = 0; i < outH; i++) {
    ppOutBuff[i] = pOutBuff + i * outW;
  }

  //Bicubic(ppPicBuff,ppOutBuff_temp, ppOutBuff, inW,inH, outW,outH);


  std::string apename = "/dev/ape";
  CsuObject *csuobj = CsuObject::tryFile(apename);

  uint8_t *buf = (uint8_t *)csuobj->dmalloc(1024*1024);

  memset(buf, 0, 1024*1024);

  csuobj->setID(0);

  std::string spuname0 = "app.s.ape0.out";

  ElfObject *spuobj0 = ElfObject::tryFile(spuname0);

  spuobj0->loadSections(csuobj->get_fd(), 0);

  csuobj->setID(3);

  std::string spuname3 = "app.s.ape3.out";

  ElfObject *spuobj3 = ElfObject::tryFile(spuname3);

  spuobj3->loadSections(csuobj->get_fd(), 3);

  csuobj->setID(0);

  std::string mpuname0 = "app.m.ape0.out";

  ElfObject *mpuobj0 = ElfObject::tryFile(mpuname0);

  //mpuobj->loadSections(csuobj->get_fd(), 0);
  mpuobj0->copySections(buf + 0x10000, csuobj->get_fd(), 0);

  csuobj->setID(3);

  std::string mpuname3 = "app.m.ape3.out";

  ElfObject *mpuobj3 = ElfObject::tryFile(mpuname3);

  mpuobj3->copySections(buf + 0x40000, csuobj->get_fd(), 3);

  uint8_t *pad = (uint8_t *)malloc((inW + 3) * (inH + 3));
  for (int i = 0; i < inH + 3; i++)
    for (int j = 0; j < inW + 3; j++) {
      int ri = i - 1;
      int ci = j - 1;
      if (ri < 0) ri = 0;
      else if (ri >= inH) ri = inH - 1;
      if (ci < 0) ci = 0;
      else if (ci >= inW) ci = inW - 1;
      pad[i * (inW + 3) + j] = pPicBuff[(ri * inW + ci)];
    }

  for (int i=0; i < (inH + 3) * (inW + 3); i++)
    ((int *)buf)[0x50000/4 + i] = pad[i];
  struct dma_if conf;

  csuobj->setID(0);

  conf.DMAGlobalAddr = 0x70050000;
  conf.DMAGlobalXNum = (inH + 3) * (inW + 3) * 4;
  conf.DMAGlobalYAllNum = 1;
  conf.DMAGlobalYNum = 1;
  conf.DMAGlobalYStep = 1;
  conf.DMAGlobalZStep = 1;
  conf.DMALocalAddr = 0x80010000;
  conf.DMALocalXNum = (inH + 3) * (inW + 3) * 4;
  conf.DMALocalYAllNum = 1;
  conf.DMALocalYNum = 1;
  conf.DMALocalYStep = 1;
  conf.DMALocalZStep = 1;
  conf.DMAGroupNum = 3;
  conf.DMACmd = 0;

  csuobj->DMARequest(&conf);

  std::string dmname = "ape3.SDA0DM0.dat";

  std::ifstream in(dmname.c_str());

  if (!in)
    std::cout << "SDA0DM0 is not found.\n";

  i = 0;
  std::string t;
  while(in && i < 0x1000) {
    in >> t;
    buf[0x60000 + i++] = std::stoi(t, 0, 16);
  }
  in.close();
  in.clear();
  std::cout << "SDA0DM0 is loaded, size is " << i << std::endl;

  csuobj->setID(3);

  conf.DMAGlobalAddr = 0x70060000;
  conf.DMAGlobalXNum = 256;
  conf.DMAGlobalYAllNum = 1;
  conf.DMAGlobalYNum = 1;
  conf.DMAGlobalYStep = 1;
  conf.DMAGlobalZStep = 1;
  conf.DMALocalAddr = 0xA0010000;
  conf.DMALocalXNum = 256;
  conf.DMALocalYAllNum = 1;
  conf.DMALocalYNum = 1;
  conf.DMALocalYStep = 1;
  conf.DMALocalZStep = 1;
  conf.DMAGroupNum = 3;
  conf.DMACmd = 0;
  csuobj->DMARequest(&conf);

  csuobj->setID(0);

  csuobj->Wait();

  csuobj->setID(3);

  csuobj->Wait();

  csuobj->setID(0);

  csuobj->Startup();

  csuobj->setID(3);

  csuobj->Startup();

  csuobj->setID(0);
  do {
    csuobj->update();
  } while (csuobj->isAPEOn());

  csuobj->setID(3);
  do {
    csuobj->update();
  } while (csuobj->isAPEOn());

  memset(buf, 0, outW * outH);

  /*conf.DMAGlobalAddr = 0x70000000;
  conf.DMAGlobalXNum = 1;
  conf.DMAGlobalYAllNum = outW * 16;
  conf.DMAGlobalYNum = outW;
  conf.DMAGlobalYStep = 1;
  conf.DMAGlobalZStep = outW;
  conf.DMALocalAddr = 0xc00000;
  conf.DMALocalXNum = 1;
  conf.DMALocalYAllNum = outW * 16;
  conf.DMALocalYNum = outW;
  conf.DMALocalYStep = 4;
  conf.DMALocalZStep = 4 * 4096;
  conf.DMAGroupNum = 4;
  conf.DMACmd = 1;

  for (int i = 0; i < outH / 16 + 1; i++) {
    while (!csuobj->update() || csuobj->isCSUFull()) ;
    conf.DMAGlobalAddr = 0x70000000 + outW * 16 * i;
    conf.DMALocalAddr = 0xc00000 + 4 * outW * i;
    csuobj->DMARequest(&conf);
  }*/

  conf.DMAGlobalAddr = 0x70000000;
  conf.DMAGlobalXNum = 1;
  conf.DMAGlobalYAllNum = 180 * 240;
  conf.DMAGlobalYNum = 180;
  conf.DMAGlobalYStep = 1;
  conf.DMAGlobalZStep = 180;
  conf.DMALocalAddr = 0xc00000;
  conf.DMALocalXNum = 1;
  conf.DMALocalYAllNum = 180 * 240;
  conf.DMALocalYNum = 180;
  conf.DMALocalYStep = 4;
  conf.DMALocalZStep = 4 * 180;
  conf.DMAGroupNum = 4;
  conf.DMACmd = 1;
  csuobj->DMARequest(&conf);

  csuobj->Wait();

  csuobj->Stop();

  memcpy(pOutBuff, buf, outW * outH);

  csuobj->dfree(buf);

  //Bicubic_opt(ppPicBuff, ppOutBuff_temp, ppOutBuff, inW, inH, outW, outH, 1, 3,
              //4, 9);
  //Bicubic_mapu(ppPicBuff,ppOutBuff_temp, ppOutBuff, inW,inH, outW,outH, 1, 3, 4, 9);

  if ((fpout = fopen("BicubicOut.dat",
                     "wb"))
      == NULL) {
    perror(0);
    exit(0);
  }
  fprintf(fpout, "À´»˝¥Œ≤Â÷µ—› æ\n");

  //Ω´ÕºœÒ¥Ê»ÎŒƒº˛
  fprintf(fpout, "\n‘≠ ºÕºŒƒº˛ ˝æ›\n");
  for (i = 0; i < inH; i++) {
    for (j = 0; j < inW; j++) {
      fprintf(fpout, "%2x ", ppPicBuff[i][j]);
    }
    fprintf(fpout, "\n");
  }

  fprintf(fpout, "\n≤Â÷µ∫ÛÕºŒƒº˛ ˝æ›\n");
  for (i = 0; i < outH; i++) {
    for (j = 0; j < outW; j++) {
      fprintf(fpout, "%2x ", ppOutBuff[i][j]);
    }
    fprintf(fpout, "\n");
  }

  //Ω´≤Â÷µÕºœÒ¥Ê»ÎbmpŒƒº˛
  WriteBMP("out2.bmp", ppOutBuff, outW,
           outH);

  fclose(fpout);
  free(pPicBuff);
  free(ppPicBuff);
  free(pOutBuff);
  free(ppOutBuff);
  //
  return 0;
}
