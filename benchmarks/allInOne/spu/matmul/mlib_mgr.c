/*
 * mlib_mgr.c
 *
 *  Created on: Feb 13, 2014
 *      Author: wangl
 */

#include "mlib_mgr.h"

enum CSU_CH {
  CH_EventStat    =   0,
  CH_EventWMask   =   1,
  CH_EventAck     =   2,
  CH_EventRMask   =   3,

  CH_lAddr        =   4,
  CH_lXNum        =   5,
  CH_lYStep       =   6,
  CH_lYNum        =   7,
  CH_lZStep       =   8,
  CH_lAllNum      =   9, 

  CH_gAddr        =   10,
  CH_gXNum        =   11,
  CH_gYStep       =   12,
  CH_gYNum        =   13,
  CH_gZStep       =   14,
  CH_gAllNum      =   15,

  CH_DMAGrp       =   16,
  CH_DMACmd       =   17,
  
  CH_DMAGrpMaskW  =   18,
  CH_DMAGrpMaskR  =   19,
  CH_DMAGrpStatU  =   20,
  CH_DMAGrpStat   =   21,
};

static int mlib_table = 0x40400000;
static int current_id = -1;
static int ape_id = -1;
void load_mcode(int id) {
  unsigned int size, base;
  if (current_id == id) return;
  current_id = id;
  if (ape_id == -1) {
    ape_id = __mspu_getCh(0x1B);
    mlib_table += ape_id * MAX_MC_NUM * 4 * 2;
  }

  base = *(((volatile unsigned int *)mlib_table) + id * 2);
  size = *(((volatile unsigned int *)mlib_table) + id * 2 + 1);

  __mspu_setCh(CH_lAddr, 0x200000);
  __mspu_setCh(CH_lXNum, 41);
  __mspu_setCh(CH_lYStep, 0x40);
  __mspu_setCh(CH_lYNum, size);
  __mspu_setCh(CH_lZStep, size);
  __mspu_setCh(CH_lAllNum, size);

  __mspu_setCh(CH_gAddr, base);
  __mspu_setCh(CH_gXNum, 41);
  __mspu_setCh(CH_gYStep, 41);
  __mspu_setCh(CH_gYNum, size);
  __mspu_setCh(CH_gZStep, size);
  __mspu_setCh(CH_gAllNum, size);
  
  __mspu_setCh(CH_DMAGrp, 0x0);
  __mspu_setCh(CH_DMACmd, 0x0);

  while (__mspu_getChNum(CH_DMACmd) < 8);

  __mspu_setCh(CH_DMAGrpMaskW, -1UL);
  __mspu_setCh(CH_DMAGrpStatU, 2);
  __mspu_getCh(CH_DMAGrpStat);
}

