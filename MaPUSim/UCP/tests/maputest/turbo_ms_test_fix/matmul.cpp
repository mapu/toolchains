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
#include "csu_object.hh"
#include "elf_object.hh"

#include "time.h"
#include <math.h>

#include "channel.h"
#include "crc.h"
#include "define.h"
#include "modulation.h"
#include "qpp_interleaver.h"
#include "turbo_enc_dec.h"
#include "ULRecvTurbodec.h"

char fixed_point(float input, int numBit, int p);

CsuObject *sim_init() {
  std::string apename = "/dev/ape";
  CsuObject *csuobj = CsuObject::tryFile(apename);

  csuobj->setID(0);

  std::string spuname = "/home/wujn/Simulator/SDK/sdk/template/turbo_test_fix/slib/bin/turbo_test_fix.s.elf";

  ElfObject *spuobj = ElfObject::tryFile(spuname);

  spuobj->loadSections(csuobj->get_fd(), 0);

  delete spuobj;

  return csuobj;
}

void sim_close(CsuObject *csuobj) {
  csuobj->Stop();
}

int main() {
  int i, j, en;
  int blk_num, blk_len, coded_blk_len, sym_len, error_blk_num, error_bit_num,
      error_bit;
  int modu_order;
  int *source_bit, *sys_bit, *parity1_bit, *parity2_bit, *info_bit, *coded_bit;
  char *dec_bits_sys_fix, *dec_bits_par1_fix, *dec_bits_par2_fix;
  float EbN0, BER, BLER, sigma_2, sigma;
  float code_rate;

  Complex *sym, *received_sym;
  float *sys_llr, *parity1_llr, *parity2_llr, *dem_llr;
  int iter_num;
  float avr_iter_num = 0;
  clock_t time_start;
  float time_operation;
  unsigned char decoded_byte[CB_LEN / 8] = { 0 };
  int decoded_bit[CB_LEN];

  struct param {
    unsigned int sys;
    unsigned int par1;
    unsigned int par2;
    unsigned int output;
    unsigned int iter;
    unsigned int cb_len;
    unsigned int sb_num;
    unsigned int sw_len1;
    unsigned int scale;
    unsigned int crc_en;
    unsigned int crc_type;
    unsigned int crc_num;
    unsigned int final_iter;
  } p;

  FILE *fp,*fp_debug;
  //FILE *fp_sys,*fp_par1,*fp_par2,*fp_hardbit;

  time_start = clock();

  blk_num = CB_NUM;
  blk_len = CB_LEN;
  code_rate = 1.0 / 3;
  modu_order = MODU_ORDER;
  coded_blk_len = 3 * blk_len + 12;
  sym_len = coded_blk_len / modu_order;

  fp = fopen("debug.txt", "a");
  // fp_sys=fopen("sys.txt","w");
  // fp_par1=fopen("par1.txt","w");
  // fp_par2=fopen("par2.txt","w");
  // fp_hardbit=fopen("hardbit.txt","w");

  //Initiate the data[][] Randomly:0 or 1
  //srand((unsigned int)(time(NULL)));

  CsuObject *csuobj = sim_init();

  //Memory allocate
  source_bit = (int*) malloc((blk_len - 24) * sizeof(int));
  info_bit = (int*) malloc(blk_len * sizeof(int));
  sys_bit = (int*) malloc((blk_len + 4) * sizeof(int));
  parity1_bit = (int*) malloc((blk_len + 4) * sizeof(int));
  parity2_bit = (int*) malloc((blk_len + 4) * sizeof(int));
  coded_bit = (int*) malloc(coded_blk_len * sizeof(int));
  // decoded_bit = (int*)malloc(blk_len*sizeof(int));

  sym = (Complex *) malloc(sym_len * sizeof(Complex));
  received_sym = (Complex *) malloc(sym_len * sizeof(Complex));
  dem_llr = (float*) malloc(coded_blk_len * sizeof(float));
  sys_llr = (float *) malloc((blk_len + 4) * sizeof(float));
  parity1_llr = (float *) malloc((blk_len + 4) * sizeof(float));
  parity2_llr = (float *) malloc((blk_len + 4) * sizeof(float));
  dec_bits_sys_fix = (char *) malloc((blk_len + 4) * sizeof(char));
  dec_bits_par1_fix = (char *) malloc((blk_len + 4) * sizeof(char));
  dec_bits_par2_fix = (char *) malloc((blk_len + 4) * sizeof(char));

  code_rate = code_rate * modu_order;

  fprintf(fp, "Qm_type = %d.CRC_SW = %d.CB_LEN=%d.SW_LEN = %d.CB_NUM=%d.\n"
          "====================================\n",
          MODU_ORDER, CRC_SW, CB_LEN, SW_LEN, CB_NUM);

  for (en = 0; en < EbN0_NUM; en++) {
		fp_debug = fopen("fp_debug.txt", "a");
    EbN0 = EbN0_START + EbN0_STEP * en;
    //  sigma_2 = 1/(pow(10.0, EbN0/10)*2.0*code_rate);
    sigma_2 = 1.0 / (pow(10.0, EbN0 / 10.0) * 2.0);
    sigma = sqrt(sigma_2);

    error_blk_num = 0;
    error_bit_num = 0;
    iter_num = 0;

    for (i = 0; i < blk_num; i++) {
      error_bit = 0;
      srand((unsigned int)(time(NULL)));
      for (j = 0; j < blk_len - 24; j++) {
        source_bit[j] = rand() % 2;
      }

      crc_add(info_bit, source_bit, blk_len - 24, CRC_TYPE);

      turbo_encoder(sys_bit, parity1_bit, parity2_bit, info_bit, blk_len);

      //PS
      for (j = 0; j < blk_len + 4; j++) {
        coded_bit[j] = sys_bit[j];
        coded_bit[(blk_len + 4) + j] = parity1_bit[j];
        coded_bit[2 * (blk_len + 4) + j] = parity2_bit[j];
      }

      //  modulation(sym, coded_bit, sym_len, modu_order);
      modulation_s(sym, coded_bit, coded_blk_len, modu_order);

      //  AWGN(received_sym, sym, sigma, sym_len);
      AWGNgen(received_sym, sym, sym_len, EbN0);

      //  demodulation(dem_llr, received_sym, 1/sigma_2, sym_len, modu_order);
      ULRecvDemodulaton(dem_llr, received_sym, sigma_2, sym_len, modu_order);

      for (j = 0; j < blk_len + 4; j++) {
        sys_llr[j] = dem_llr[j];
        parity1_llr[j] = dem_llr[(blk_len + 4) + j];
        parity2_llr[j] = dem_llr[2 * (blk_len + 4) + j];
      }

      for (j = 0; j < blk_len + 4; j++) {
        dec_bits_sys_fix[j] = fixed_point(sys_llr[j], 8, 2);
        dec_bits_par1_fix[j] = fixed_point(parity1_llr[j], 8, 2);
        dec_bits_par2_fix[j] = fixed_point(parity2_llr[j], 8, 2);
      }
        //LOG FILE
        //   for(j=0;j<blk_len;j++)
        //   {
        //    fprintf(fp_sys,"  %#010x,\n",*(unsigned int *)&sys_llr[j]);
        //    fprintf(fp_par1,"  %#010x,\n",*(unsigned int *)&parity1_llr[j]);
        //    fprintf(fp_par2,"  %#010x,\n",*(unsigned int *)&parity2_llr[j]);
        //   }

        //  iter_num += turbo_decoder(decoded_bit, sys_llr, parity1_llr, parity2_llr);
        //  iter_num += turbo_decoder_serial(decoded_bit,sys_llr, parity1_llr, parity2_llr,blk_len+4, MAX_ITERATION, SCALING_FACTOR,1);//serial
        //parameters config
      p.sys = 0x400000;
      p.par1 = p.sys + (CB_LEN + 4) * sizeof(char);
      p.par2 = p.par1 + (CB_LEN + 4) * sizeof(char);
      p.output = 0xC00000;
      p.iter = MAX_ITERATION;
      p.cb_len = CB_LEN;
      p.sb_num = PU_NUM;
      p.sw_len1 = SW_LEN;
      p.scale = SCALING_FACTOR;
      p.crc_en = CRC_SW;
      p.crc_type = CRC_TYPE;
      p.crc_num = CRC_CHECK_NUM;
      csuobj->sendData(dec_bits_sys_fix, (CB_LEN + 4) * sizeof(char),
                       p.sys);
      csuobj->sendData(dec_bits_par1_fix, (CB_LEN + 4) * sizeof(char),
                       p.par1);
      csuobj->sendData(dec_bits_par2_fix, (CB_LEN + 4) * sizeof(char),
                       p.par2);
      csuobj->sendData(&p, sizeof(p), 0x800000);

      csuobj->Wait();
      csuobj->Startup();
      do {
        csuobj->update();
      } while (csuobj->isAPEOn());
      csuobj->getData(decoded_byte, sizeof(decoded_byte), 0xC00000);
      csuobj->getData(&p, sizeof(p), 0x800000);
      csuobj->Wait();

      iter_num = iter_num + p.final_iter;
      byte2bit(decoded_bit, decoded_byte, CB_LEN / 8);

      for (j = 0; j < blk_len; j++) {
        if (decoded_bit[j] != info_bit[j]) {
          error_bit++;
        }
      }

      //   for(j=0;j<blk_len; j++)
      // fprintf(fp_hardbit,"  %#010x,\n",*(unsigned int *)&decoded_bit[j]);

      if (error_bit) {
        error_blk_num++;
        error_bit_num += error_bit;
      }
    }//end of blk_num

    avr_iter_num = (float) iter_num / blk_num;

    BER = ((float) error_bit_num) / ((float) (blk_len * blk_num));
    BLER = ((float) error_blk_num) / ((float) blk_num);

    fprintf(fp, "EbN0(%d) = %f; BER(%d) = %d/%d = %f; BLER(%d) = %d/%d =  %f; avr_iter = %f;\n",
            en + 1, EbN0, en + 1, error_bit_num, blk_len * blk_num, BER, en + 1, error_blk_num, blk_num, BLER, avr_iter_num);
    fprintf(fp_debug, "EbN0(%d) = %f; BER(%d) = %d/%d = %f; BLER(%d) = %d/%d =  %f; avr_iter = %f;\n",
            en + 1, EbN0, en + 1, error_bit_num, blk_len * blk_num, BER, en + 1, error_blk_num, blk_num, BLER, avr_iter_num);
    printf("EbN0(%d) = %f; BER(%d) = %d/%d = %f; BLER(%d) = %d/%d =  %f; avr_iter = %f;\n",
           en + 1, EbN0, en + 1, error_bit_num, blk_len * blk_num, BER, en + 1, error_blk_num, blk_num, BLER, avr_iter_num);

    if (BLER < 0.001) break;
    //if (BER < 1.0/MAX_VALUE)
    //break;
    fclose(fp_debug);
  }

  time_operation = ((float) (clock() - time_start) / CLOCKS_PER_SEC) / 3600;
  fprintf(fp, "Time_Operation = %f Hour\n", time_operation);

  fclose(fp);
  // fclose(fp_sys);
  // fclose(fp_par1);
  // fclose(fp_par2);
  // fclose(fp_hardbit);

  free(source_bit);
  free(info_bit);
  free(parity1_bit);
  free(sys_bit);
  free(parity2_bit);
  // free(decoded_bit);
  free(coded_bit);
  free(sym);
  free(parity1_llr);
  free(received_sym);
  free(dem_llr);
  free(parity2_llr);
  free(sys_llr);
  free(dec_bits_sys_fix);
  free(dec_bits_par1_fix);
  free(dec_bits_par2_fix);

  csuobj->Stop();
  return 0;
}

char fixed_point(float input, int numBit, int p) {
  char result;
  float temp, scale_factor;
  float max, min;
  scale_factor = 1 << p;
  temp = input * scale_factor; //quantization
  max = (1 << (numBit - 3)) - 1;
  min = -(1 << (numBit - 3));
  if(temp >= max) {
    result = (1 << (numBit - 3)) - 1;
    std::cout << "upper boundary" << std::endl;
  } else if (temp <= min) {
    result = -(1 << (numBit - 3));
    std::cout << "lower boundary" << std::endl;
  } else
    result = temp;

  return result;
}
/*
 int main() {
 std::string apename = "/dev/ape";
 CsuObject *csuobj = CsuObject::tryFile(apename);

 csuobj->setID(0);

 std::string spuname = "app.s.out";

 ElfObject *spuobj = ElfObject::tryFile(spuname);

 spuobj->loadSections(csuobj->get_fd(), 0);

 std::string mpuname = "app.m.out";

 ElfObject *mpuobj = ElfObject::tryFile(mpuname);

 mpuobj->loadSections(csuobj->get_fd(), 0);

 uint8_t *buf = (uint8_t *)csuobj->dmalloc(1024*1024);

 std::string dmname = "SDA0DM0.dat";

 std::ifstream in(dmname.c_str());

 if (!in)
 std::cout << "SDA0DM0 is not found.\n";

 int i = 0;
 std::string t;
 while(in && i < 0x40000) {
 in >> t;
 buf[i++] = std::stoi(t, 0, 16);
 }
 in.close();
 in.clear();
 std::cout << "SDA0DM0 is loaded, size is " << i << std::endl;

 dmname = "SDA1DM0.dat";

 in.open(dmname.c_str());

 if (!in)
 std::cout << "SDA1DM0 is not found.\n";

 while(in && i < 0x80000) {
 in >> t;
 buf[i++] = std::stoi(t, 0, 16);
 }
 in.close();
 in.clear();
 std::cout << "SDA1DM0 is loaded, size is " << i << std::endl;

 struct dma_if conf;

 conf.DMAGlobalAddr = 0x70000000;
 conf.DMAGlobalXNum = 1024;
 conf.DMAGlobalYAllNum = 256;
 conf.DMAGlobalYNum = 256;
 conf.DMAGlobalYStep = 1024;
 conf.DMAGlobalZStep = 256 * 1024;
 conf.DMALocalAddr = 0x400000;
 conf.DMALocalXNum = 1024;
 conf.DMALocalYAllNum = 256;
 conf.DMALocalYNum = 256;
 conf.DMALocalYStep = 1024;
 conf.DMALocalZStep = 256 * 1024;
 conf.DMAGroupNum = 3;
 conf.DMACmd = 0;

 while (!csuobj->update() || csuobj->isCSUFull()) ;
 csuobj->DMARequest(&conf);

 conf.DMAGlobalAddr = 0x70000000 + 256 * 1024;
 conf.DMAGlobalXNum = 1024;
 conf.DMAGlobalYAllNum = 256;
 conf.DMAGlobalYNum = 256;
 conf.DMAGlobalYStep = 1024;
 conf.DMAGlobalZStep = 256 * 1024;
 conf.DMALocalAddr = 0x800000;
 conf.DMALocalXNum = 1024;
 conf.DMALocalYAllNum = 256;
 conf.DMALocalYNum = 256;
 conf.DMALocalYStep = 1024;
 conf.DMALocalZStep = 256 * 1024;
 conf.DMAGroupNum = 3;
 conf.DMACmd = 0;

 while (!csuobj->update() || csuobj->isCSUFull()) ;
 csuobj->DMARequest(&conf);

 csuobj->Wait();

 csuobj->Startup();
 do {
 csuobj->update();
 } while (csuobj->isAPEOn());

 conf.DMAGlobalAddr = 0x70000000;
 conf.DMAGlobalXNum = 256;
 conf.DMAGlobalYAllNum = 64;
 conf.DMAGlobalYNum = 16;
 conf.DMAGlobalYStep = 256;
 conf.DMAGlobalZStep = 256 * 16;
 conf.DMALocalAddr = 0xc00000;
 conf.DMALocalXNum = 256;
 conf.DMALocalYAllNum = 64;
 conf.DMALocalYNum = 16;
 conf.DMALocalYStep = 4096 * 4;
 conf.DMALocalZStep = 256;
 conf.DMAGroupNum = 3;
 conf.DMACmd = 1;

 while (!csuobj->update() || csuobj->isCSUFull()) ;
 csuobj->DMARequest(&conf);

 unsigned int *result = (unsigned int *)buf;
 csuobj->Wait();

 dmname = "StdRes.dat";
 in.open(dmname);

 std::fstream a("../m5out/MatSF64MulTest/MatSF64Mul.out", std::ios::out | std::ios::trunc);

 if (!in) {
 a << "Error: there is no result data to diff! " << std::endl;
 return 0;
 }
 i = 0;
 while (in && i < 256 * 16) {
 in >> t;
 unsigned int code = std::stoul(t, 0, 16);
 if (code != result[i++])
 std::cout << i << code << " : " << result[i-1] << std::endl;
 }

 csuobj->Stop();

 csuobj->dfree(buf);

 return 0;
 }*/
