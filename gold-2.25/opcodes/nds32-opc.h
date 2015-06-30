/* NDS32-specific support for 32-bit ELF.
   Copyright (C) 2012-2014 Free Software Foundation, Inc.
   Contributed by Andes Technology Corporation.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.*/


#ifndef NDS32_OPC_H
#define NDS32_OPC_H

/* This was the enum used for 32/16 conversion.  */

enum
{
  NDS32_INSN_INVALID, NDS32_INSN_MOVI, NDS32_INSN_SETHI, NDS32_INSN_ADDI,
  NDS32_INSN_ADD, NDS32_INSN_SLTSI, NDS32_INSN_SLTS, NDS32_INSN_SLTI,
  NDS32_INSN_SLT, NDS32_INSN_SUBRI, NDS32_INSN_SUB, NDS32_INSN_ANDI,
  NDS32_INSN_AND, NDS32_INSN_XORI, NDS32_INSN_XOR, NDS32_INSN_ORI,
  NDS32_INSN_OR, NDS32_INSN_NOR, NDS32_INSN_SVA, NDS32_INSN_SVS,
  NDS32_INSN_SEB, NDS32_INSN_SEH, NDS32_INSN_ZEH, NDS32_INSN_WSBH,
  NDS32_INSN_SLLI, NDS32_INSN_SLL, NDS32_INSN_SRAI, NDS32_INSN_SRA,
  NDS32_INSN_ROTRI, NDS32_INSN_ROTR, NDS32_INSN_SRLI, NDS32_INSN_SRL,
  NDS32_INSN_MUL, NDS32_INSN_MULTS64, NDS32_INSN_MULT64, NDS32_INSN_MADDS64,
  NDS32_INSN_MADD64, NDS32_INSN_MSUBS64, NDS32_INSN_MSUB64,
  NDS32_INSN_MULT32, NDS32_INSN_MADD32, NDS32_INSN_MSUB32, NDS32_INSN_MFUSR,
  NDS32_INSN_MTUSR, NDS32_INSN_LBI, NDS32_INSN_LBI_BI, NDS32_INSN_LB,
  NDS32_INSN_LB_BI, NDS32_INSN_LHI, NDS32_INSN_LHI_BI, NDS32_INSN_LH,
  NDS32_INSN_LH_BI, NDS32_INSN_LWI, NDS32_INSN_LWI_BI, NDS32_INSN_LW,
  NDS32_INSN_LW_BI, NDS32_INSN_LWUP, NDS32_INSN_SWUP, NDS32_INSN_LBSI,
  NDS32_INSN_LBSI_BI, NDS32_INSN_LBS, NDS32_INSN_LBS_BI, NDS32_INSN_LHSI,
  NDS32_INSN_LHSI_BI, NDS32_INSN_LHS, NDS32_INSN_LHS_BI, NDS32_INSN_SBI,
  NDS32_INSN_SBI_BI, NDS32_INSN_SB, NDS32_INSN_SB_BI, NDS32_INSN_SHI,
  NDS32_INSN_SHI_BI, NDS32_INSN_SH, NDS32_INSN_SH_BI, NDS32_INSN_SWI,
  NDS32_INSN_SWI_BI, NDS32_INSN_SW, NDS32_INSN_SW_BI, NDS32_INSN_LMW_BI,
  NDS32_INSN_LMW_BIM, NDS32_INSN_LMW_BD, NDS32_INSN_LMW_BDM,
  NDS32_INSN_LMW_AI, NDS32_INSN_LMW_AIM, NDS32_INSN_LMW_AD,
  NDS32_INSN_LMW_ADM, NDS32_INSN_SMW_BI, NDS32_INSN_SMW_BIM,
  NDS32_INSN_SMW_BD, NDS32_INSN_SMW_BDM, NDS32_INSN_SMW_AI,
  NDS32_INSN_SMW_AIM, NDS32_INSN_SMW_AD, NDS32_INSN_SMW_ADM, NDS32_INSN_LLW,
  NDS32_INSN_SCW, NDS32_INSN_J, NDS32_INSN_JAL, NDS32_INSN_JR,
  NDS32_INSN_RET, NDS32_INSN_JR_ITOFF, NDS32_INSN_JR_TOFF,
  NDS32_INSN_RET_ITOFF, NDS32_INSN_RET_TOFF, NDS32_INSN_JRAL,
  NDS32_INSN_JRAL_ITON, NDS32_INSN_JRAL_TON, NDS32_INSN_BEQ, NDS32_INSN_BNE,
  NDS32_INSN_BEQZ, NDS32_INSN_BNEZ, NDS32_INSN_BGEZ, NDS32_INSN_BLTZ,
  NDS32_INSN_BGTZ, NDS32_INSN_BLEZ, NDS32_INSN_BGEZAL, NDS32_INSN_BLTZAL,
  NDS32_INSN_MFSR, NDS32_INSN_MTSR, NDS32_INSN_SETEND_L,
  NDS32_INSN_SETEND_B, NDS32_INSN_SETGIE_D, NDS32_INSN_SETGIE_E,
  NDS32_INSN_CMOVZ, NDS32_INSN_CMOVN, NDS32_INSN_DPREFI_D,
  NDS32_INSN_DPREFI_W, NDS32_INSN_DPREF, NDS32_INSN_ISYNC, NDS32_INSN_MSYNC,
  NDS32_INSN_ISB, NDS32_INSN_DSB, NDS32_INSN_STANDBY, NDS32_INSN_TRAP,
  NDS32_INSN_TEQZ, NDS32_INSN_TNEZ, NDS32_INSN_BREAK, NDS32_INSN_SYSCALL,
  NDS32_INSN_IRET, NDS32_INSN_TLBOP, NDS32_INSN_CCTL, NDS32_INSN_DIVS,
  NDS32_INSN_DIV, NDS32_INSN_ABS, NDS32_INSN_AVE, NDS32_INSN_MIN,
  NDS32_INSN_MAX, NDS32_INSN_BSET, NDS32_INSN_BTGL, NDS32_INSN_BCLR,
  NDS32_INSN_BTST, NDS32_INSN_CLIPS, NDS32_INSN_CLIP, NDS32_INSN_CLZ,
  NDS32_INSN_CLO, NDS32_INSN_BSE, NDS32_INSN_BSP, NDS32_INSN_PBSAD,
  NDS32_INSN_PBSADA, NDS32_INSN_MOV55, NDS32_INSN_MOVI55, NDS32_INSN_ADDI45,
  NDS32_INSN_ADD45, NDS32_INSN_SUBI45, NDS32_INSN_SUB45, NDS32_INSN_SRAI45,
  NDS32_INSN_SRLI45, NDS32_INSN_SLLI333, NDS32_INSN_SEB33, NDS32_INSN_SEH33,
  NDS32_INSN_ZEB33, NDS32_INSN_ZEH33, NDS32_INSN_XLSB33, NDS32_INSN_X11B33,
  NDS32_INSN_ADDI333, NDS32_INSN_ADD333, NDS32_INSN_SUBI333,
  NDS32_INSN_SUB333, NDS32_INSN_LWI333, NDS32_INSN_LWI333_BI,
  NDS32_INSN_LHI333, NDS32_INSN_LBI333, NDS32_INSN_SWI333,
  NDS32_INSN_SWI333_BI, NDS32_INSN_SHI333, NDS32_INSN_SBI333,
  NDS32_INSN_LWI450, NDS32_INSN_SWI450, NDS32_INSN_LWI37, NDS32_INSN_SWI37,
  NDS32_INSN_BEQZ38, NDS32_INSN_BNEZ38, NDS32_INSN_BEQS38,
  NDS32_INSN_BNES38, NDS32_INSN_J8, NDS32_INSN_JR5, NDS32_INSN_RET5,
  NDS32_INSN_JRAL5, NDS32_INSN_SLTI45, NDS32_INSN_SLTSI45, NDS32_INSN_SLT45,
  NDS32_INSN_SLTS45, NDS32_INSN_BEQZS8, NDS32_INSN_BNEZS8,
  NDS32_INSN_BREAK16, NDS32_INSN_ADDI10_SP, NDS32_INSN_LWI37_SP,
  NDS32_INSN_SWI37_SP, NDS32_INSN_BMSKI33, NDS32_INSN_FEXTI33,
  NDS32_INSN_ADDRI36_SP, NDS32_INSN_LWI45_FE, NDS32_INSN_NEG33,
  NDS32_INSN_NOT33, NDS32_INSN_MUL33, NDS32_INSN_XOR33, NDS32_INSN_AND33,
  NDS32_INSN_OR33, NDS32_INSN_MOVPI45, NDS32_INSN_PUSH25, NDS32_INSN_POP25,
  NDS32_INSN_MOVD44, NDS32_INSN_ADD5_PC, NDS32_INSN_BREAK16V3,
  NDS32_INSN_ADDI_GP, NDS32_INSN_MADDR32, NDS32_INSN_MSUBR32,
  NDS32_INSN_MULR64, NDS32_INSN_MULSR64, NDS32_INSN_SBI_GP,
  NDS32_INSN_SHI_GP, NDS32_INSN_SWI_GP, NDS32_INSN_LBI_GP,
  NDS32_INSN_LBSI_GP, NDS32_INSN_LHI_GP, NDS32_INSN_LHSI_GP,
  NDS32_INSN_LWI_GP, NDS32_INSN_DIVR, NDS32_INSN_DIVSR, NDS32_INSN_LMWA_BI,
  NDS32_INSN_LMWA_BIM, NDS32_INSN_LMWA_BD, NDS32_INSN_LMWA_BDM,
  NDS32_INSN_LMWA_AI, NDS32_INSN_LMWA_AIM, NDS32_INSN_LMWA_AD,
  NDS32_INSN_LMWA_ADM, NDS32_INSN_SMWA_BI, NDS32_INSN_SMWA_BIM,
  NDS32_INSN_SMWA_BD, NDS32_INSN_SMWA_BDM, NDS32_INSN_SMWA_AI,
  NDS32_INSN_SMWA_AIM, NDS32_INSN_SMWA_AD, NDS32_INSN_SMWA_ADM,
  NDS32_INSN_LBUP, NDS32_INSN_SBUP, NDS32_INSN_LMWZB_B, NDS32_INSN_LMWZB_BM,
  NDS32_INSN_LMWZB_A, NDS32_INSN_LMWZB_AM, NDS32_INSN_SMWZB_B,
  NDS32_INSN_SMWZB_BM, NDS32_INSN_SMWZB_A, NDS32_INSN_SMWZB_AM,
  NDS32_INSN_BEQC, NDS32_INSN_BNEC, NDS32_INSN_JRALNEZ, NDS32_INSN_JRNEZ,
  NDS32_INSN_ADD_SLLI, NDS32_INSN_ADD_SRLI, NDS32_INSN_SUB_SLLI,
  NDS32_INSN_SUB_SRLI, NDS32_INSN_AND_SLLI, NDS32_INSN_AND_SRLI,
  NDS32_INSN_OR_SLLI, NDS32_INSN_OR_SRLI, NDS32_INSN_XOR_SLLI,
  NDS32_INSN_XOR_SRLI, NDS32_INSN_BITC, NDS32_INSN_BITCI, NDS32_INSN_AADDL,
  NDS32_INSN_ASUBL, NDS32_INSN_ALA, NDS32_INSN_ALR, NDS32_INSN_ALR2,
  NDS32_INSN_ASA, NDS32_INSN_ASR, NDS32_INSN_AUPI, NDS32_INSN_AMFAR,
  NDS32_INSN_AMTAR, NDS32_INSN_AMTARI, NDS32_INSN_ASATS48, NDS32_INSN_AWEXT,
  NDS32_INSN_AMADD, NDS32_INSN_AMADDL_S, NDS32_INSN_AMADDL2_S,
  NDS32_INSN_AMADDL_L, NDS32_INSN_AMADDL2_L, NDS32_INSN_AMADDSA,
  NDS32_INSN_AMSUB, NDS32_INSN_AMSUBL_S, NDS32_INSN_AMSUBL2_S,
  NDS32_INSN_AMSUBL_L, NDS32_INSN_AMSUBL2_L, NDS32_INSN_AMSUBSA,
  NDS32_INSN_AMADDS, NDS32_INSN_AMADDSL_S, NDS32_INSN_AMADDSL2_S,
  NDS32_INSN_AMADDSL_L, NDS32_INSN_AMADDSL2_L, NDS32_INSN_AMADDSSA,
  NDS32_INSN_AMSUBS, NDS32_INSN_AMSUBSL_S, NDS32_INSN_AMSUBSL2_S,
  NDS32_INSN_AMSUBSL_L, NDS32_INSN_AMSUBSL2_L, NDS32_INSN_AMSUBSSA,
  NDS32_INSN_AMNEGS, NDS32_INSN_AMNEGSL_S, NDS32_INSN_AMNEGSL2_S,
  NDS32_INSN_AMNEGSL_L, NDS32_INSN_AMNEGSL2_L, NDS32_INSN_AMNEGSSA,
  NDS32_INSN_AMULTS, NDS32_INSN_AMULTSL_S, NDS32_INSN_AMULTSL2_S,
  NDS32_INSN_AMULTSL_L, NDS32_INSN_AMULTSL2_L, NDS32_INSN_AMULTSSA,
  NDS32_INSN_AMULT, NDS32_INSN_AMULTL_S, NDS32_INSN_AMULTL2_S,
  NDS32_INSN_AMULTL_L, NDS32_INSN_AMULTL2_L, NDS32_INSN_AMULTSA,
  NDS32_INSN_AZOL, NDS32_INSN_AMABBS, NDS32_INSN_AMABTS, NDS32_INSN_AMATBS,
  NDS32_INSN_AMATTS, NDS32_INSN_AMBBS, NDS32_INSN_AMBTS, NDS32_INSN_AMTBS,
  NDS32_INSN_AMTTS, NDS32_INSN_AMABBSL_S, NDS32_INSN_AMABBSL_L,
  NDS32_INSN_AMABBSL2_S, NDS32_INSN_AMABBSL2_L, NDS32_INSN_AMABBSSA,
  NDS32_INSN_AMABTSL_S, NDS32_INSN_AMABTSL_L, NDS32_INSN_AMABTSL2_S,
  NDS32_INSN_AMABTSL2_L, NDS32_INSN_AMABTSSA, NDS32_INSN_AMATBSL_S,
  NDS32_INSN_AMATBSL_L, NDS32_INSN_AMATBSL2_S, NDS32_INSN_AMATBSL2_L,
  NDS32_INSN_AMATBSSA, NDS32_INSN_AMATTSL_S, NDS32_INSN_AMATTSL_L,
  NDS32_INSN_AMATTSL2_S, NDS32_INSN_AMATTSL2_L, NDS32_INSN_AMATTSSA,
  NDS32_INSN_AMBBSL_S, NDS32_INSN_AMBBSL_L, NDS32_INSN_AMBBSL2_S,
  NDS32_INSN_AMBBSL2_L, NDS32_INSN_AMBBSSA, NDS32_INSN_AMBTSL_S,
  NDS32_INSN_AMBTSL_L, NDS32_INSN_AMBTSL2_S, NDS32_INSN_AMBTSL2_L,
  NDS32_INSN_AMBTSSA, NDS32_INSN_AMTBSL_S, NDS32_INSN_AMTBSL_L,
  NDS32_INSN_AMTBSL2_S, NDS32_INSN_AMTBSL2_L, NDS32_INSN_AMTBSSA,
  NDS32_INSN_AMTTSL_S, NDS32_INSN_AMTTSL_L, NDS32_INSN_AMTTSL2_S,
  NDS32_INSN_AMTTSL2_L, NDS32_INSN_AMTTSSA, NDS32_INSN_AMAWBS,
  NDS32_INSN_AMAWTS, NDS32_INSN_AMWBS, NDS32_INSN_AMWTS,
  NDS32_INSN_AMAWBSL_S, NDS32_INSN_AMAWBSL_L, NDS32_INSN_AMAWBSL2_S,
  NDS32_INSN_AMAWBSL2_L, NDS32_INSN_AMAWBSSA, NDS32_INSN_AMAWTSL_S,
  NDS32_INSN_AMAWTSL_L, NDS32_INSN_AMAWTSL2_S, NDS32_INSN_AMAWTSL2_L,
  NDS32_INSN_AMAWTSSA, NDS32_INSN_AMWBSL_S, NDS32_INSN_AMWBSL_L,
  NDS32_INSN_AMWBSL2_S, NDS32_INSN_AMWBSL2_L, NDS32_INSN_AMWBSSA,
  NDS32_INSN_AMWTSL_S, NDS32_INSN_AMWTSL_L, NDS32_INSN_AMWTSL2_S,
  NDS32_INSN_AMWTSL2_L, NDS32_INSN_AMWTSSA, NDS32_INSN_AMFAR2,
  NDS32_INSN_AMTAR2, NDS32_INSN_FLS, NDS32_INSN_FLS_BI, NDS32_INSN_FLSI,
  NDS32_INSN_FLSI_BI, NDS32_INSN_FMFCFG, NDS32_INSN_FMFCSR,
  NDS32_INSN_FMTCSR, NDS32_INSN_FMFSR, NDS32_INSN_FMTSR, NDS32_INSN_FSS,
  NDS32_INSN_FSS_BI, NDS32_INSN_FSSI, NDS32_INSN_FSSI_BI, NDS32_INSN_FS2D,
  NDS32_INSN_FABSS, NDS32_INSN_FADDS, NDS32_INSN_FCMOVNS,
  NDS32_INSN_FCMOVZS, NDS32_INSN_FCMPEQS, NDS32_INSN_FCMPEQS_E,
  NDS32_INSN_FCMPLTS, NDS32_INSN_FCMPLTS_E, NDS32_INSN_FCMPLES,
  NDS32_INSN_FCMPLES_E, NDS32_INSN_FCMPUNS, NDS32_INSN_FCMPUNS_E,
  NDS32_INSN_FCPYNSS, NDS32_INSN_FCPYSS, NDS32_INSN_FDIVS,
  NDS32_INSN_FMADDS, NDS32_INSN_FMULS, NDS32_INSN_FMSUBS,
  NDS32_INSN_FNMADDS, NDS32_INSN_FNMSUBS, NDS32_INSN_FS2SI,
  NDS32_INSN_FS2SI_Z, NDS32_INSN_FS2UI, NDS32_INSN_FS2UI_Z,
  NDS32_INSN_FSI2S, NDS32_INSN_FSQRTS, NDS32_INSN_FSUBS, NDS32_INSN_FUI2S,
  NDS32_INSN_FABSD, NDS32_INSN_FADDD, NDS32_INSN_FCMOVND,
  NDS32_INSN_FCMOVZD, NDS32_INSN_FCMPEQD, NDS32_INSN_FCMPEQD_E,
  NDS32_INSN_FCMPLTD, NDS32_INSN_FCMPLTD_E, NDS32_INSN_FCMPLED,
  NDS32_INSN_FCMPLED_E, NDS32_INSN_FCMPUND, NDS32_INSN_FCMPUND_E,
  NDS32_INSN_FCPYNSD, NDS32_INSN_FCPYSD, NDS32_INSN_FD2S, NDS32_INSN_FD2SI,
  NDS32_INSN_FD2SI_Z, NDS32_INSN_FD2UI, NDS32_INSN_FD2UI_Z,
  NDS32_INSN_FDIVD, NDS32_INSN_FLD, NDS32_INSN_FLD_BI, NDS32_INSN_FLDI,
  NDS32_INSN_FLDI_BI, NDS32_INSN_FMADDD, NDS32_INSN_FMFDR,
  NDS32_INSN_FMSUBD, NDS32_INSN_FMTDR, NDS32_INSN_FMULD, NDS32_INSN_FNMADDD,
  NDS32_INSN_FNMSUBD, NDS32_INSN_FSD, NDS32_INSN_FSD_BI, NDS32_INSN_FSDI,
  NDS32_INSN_FSDI_BI, NDS32_INSN_FSI2D, NDS32_INSN_FSQRTD, NDS32_INSN_FSUBD,
  NDS32_INSN_FUI2D, NDS32_INSN_CPE1_CP1, NDS32_INSN_CPE1_CP2,
  NDS32_INSN_CPE1_CP3, NDS32_INSN_CPE2_CP1, NDS32_INSN_CPE2_CP2,
  NDS32_INSN_CPE2_CP3, NDS32_INSN_CPE3_CP1, NDS32_INSN_CPE3_CP2,
  NDS32_INSN_CPE3_CP3, NDS32_INSN_CPE4_CP1, NDS32_INSN_CPE4_CP2,
  NDS32_INSN_CPE4_CP3, NDS32_INSN_CPLD_CP1, NDS32_INSN_CPLD_BI_CP1,
  NDS32_INSN_CPLDI_CP1, NDS32_INSN_CPLDI_BI_CP1, NDS32_INSN_CPLD_CP2,
  NDS32_INSN_CPLD_BI_CP2, NDS32_INSN_CPLDI_CP2, NDS32_INSN_CPLDI_BI_CP2,
  NDS32_INSN_CPLD_CP3, NDS32_INSN_CPLD_BI_CP3, NDS32_INSN_CPLDI_CP3,
  NDS32_INSN_CPLDI_BI_CP3, NDS32_INSN_CPLW_CP1, NDS32_INSN_CPLW_BI_CP1,
  NDS32_INSN_CPLWI_CP1, NDS32_INSN_CPLWI_BI_CP1, NDS32_INSN_CPLW_CP2,
  NDS32_INSN_CPLW_BI_CP2, NDS32_INSN_CPLWI_CP2, NDS32_INSN_CPLWI_BI_CP2,
  NDS32_INSN_CPLW_CP3, NDS32_INSN_CPLW_BI_CP3, NDS32_INSN_CPLWI_CP3,
  NDS32_INSN_CPLWI_BI_CP3, NDS32_INSN_CPSD_CP1, NDS32_INSN_CPSD_BI_CP1,
  NDS32_INSN_CPSDI_CP1, NDS32_INSN_CPSDI_BI_CP1, NDS32_INSN_CPSD_CP2,
  NDS32_INSN_CPSD_BI_CP2, NDS32_INSN_CPSDI_CP2, NDS32_INSN_CPSDI_BI_CP2,
  NDS32_INSN_CPSD_CP3, NDS32_INSN_CPSD_BI_CP3, NDS32_INSN_CPSDI_CP3,
  NDS32_INSN_CPSDI_BI_CP3, NDS32_INSN_CPSW_CP1, NDS32_INSN_CPSW_BI_CP1,
  NDS32_INSN_CPSWI_CP1, NDS32_INSN_CPSWI_BI_CP1, NDS32_INSN_CPSW_CP2,
  NDS32_INSN_CPSW_BI_CP2, NDS32_INSN_CPSWI_CP2, NDS32_INSN_CPSWI_BI_CP2,
  NDS32_INSN_CPSW_CP3, NDS32_INSN_CPSW_BI_CP3, NDS32_INSN_CPSWI_CP3,
  NDS32_INSN_CPSWI_BI_CP3, NDS32_INSN_MFCPD_CP1, NDS32_INSN_MTCPD_CP1,
  NDS32_INSN_MFCPD_CP2, NDS32_INSN_MTCPD_CP2, NDS32_INSN_MFCPD_CP3,
  NDS32_INSN_MTCPD_CP3, NDS32_INSN_MFCPW_CP1, NDS32_INSN_MTCPW_CP1,
  NDS32_INSN_MFCPW_CP2, NDS32_INSN_MTCPW_CP2, NDS32_INSN_MFCPW_CP3,
  NDS32_INSN_MTCPW_CP3, NDS32_INSN_MFCPPW_CP1, NDS32_INSN_MTCPPW_CP1,
  NDS32_INSN_MFCPPW_CP2, NDS32_INSN_MTCPPW_CP2, NDS32_INSN_MFCPPW_CP3,
  NDS32_INSN_MTCPPW_CP3, NDS32_INSN_FFB, NDS32_INSN_FFBI, NDS32_INSN_FFMISM,
  NDS32_INSN_FLMISM, NDS32_INSN_FFZMISM, NDS32_INSN_KADDW, NDS32_INSN_KSUBW,
  NDS32_INSN_KSLRAW, NDS32_INSN_KADDH, NDS32_INSN_KSUBH, NDS32_INSN_KDMBB,
  NDS32_INSN_KDMBT, NDS32_INSN_KDMTB, NDS32_INSN_KDMTT, NDS32_INSN_KHMBB,
  NDS32_INSN_KHMBT, NDS32_INSN_KHMTB, NDS32_INSN_KHMTT, NDS32_INSN_RDOV,
  NDS32_INSN_CLROV, NDS32_INSN_IFCALL9, NDS32_INSN_IFCALL, NDS32_INSN_IFRET,
  NDS32_INSN_EX5_IT, NDS32_INSN_EX9_IT
};

#endif
