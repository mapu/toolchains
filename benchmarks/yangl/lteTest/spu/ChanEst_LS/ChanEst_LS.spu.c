#include <mspu-intrin.h>
#include <mlib_mgr.h>

extern void ChEstLS_Tail1();
extern void ChEstLS_Tail2();
extern void ChEstLS_Grp1T1();
extern void ChEstLS_Grp1T2();
extern void ChEstLS_Grp2T1();
extern void ChEstLS_Grp2T2();
extern void ChEstLS_Grp3();
extern void ChEstLS_Grp3T1();
extern void ChEstLS_Grp3T2();
extern void ChEstLS_Grp4T1();
extern void ChEstLS_Grp4T2();
extern void ChEstLS_Grp();
extern void ChEstLS_GrpT1();
extern void ChEstLS_GrpT2();

void main()
{
  unsigned int SRC_ADDR = 0xa00000;
  unsigned int DMRS_ADDR = 0xc00000;
  unsigned int DST_ADDR = 0x400000;
  unsigned int DataLen = 9600*3;
    unsigned int    BS          = 64; 
    unsigned int    DataTailNum = DataLen % BS;
    unsigned int    DataGrpNum  = DataLen / BS;
    unsigned int    DataTailEn  = DataTailNum != 0;
    unsigned int    DataBSNum   = (DataTailNum <= 32) ? (2 * DataGrpNum + DataTailEn) : (2 * (DataGrpNum + DataTailEn));
    unsigned int    TailNum     = DataLen % 32;
    unsigned int    BlockSize   = 64;                        // the total Rd size, for Row Rd is 512/8  
    unsigned int    BlockGran   = 6 ;                        // log2(BlockSize)    
	unsigned int    TailMask_L      ;
	unsigned int    TailMask_H      ;
	unsigned int    ShiftNum        ;
	unsigned int    MaskLTemp       ;
	unsigned int    MaskHTemp       ;
	    
    if (TailNum == 0){
        TailMask_L = 0xffffffff;
        TailMask_H = 0xffffffff; 
    }else if (TailNum < 16){
        ShiftNum   = TailNum * 2;
        MaskLTemp  = 0xffffffff << ShiftNum; 
        TailMask_L = ~ MaskLTemp;
        TailMask_H = 0;
    }else{ 
        ShiftNum   = TailNum * 2 - 32;
        MaskHTemp  = 0xffffffff << ShiftNum;
        TailMask_L = 0xffffffff;
        TailMask_H = ~ MaskHTemp;
    }

	__mspu_setKI(KI12,DataGrpNum);
	
	__mspu_setKB(KB4,SRC_ADDR);
	__mspu_setKS(KS4,0x20000);
	__mspu_setKI(KI4,2);
	__mspu_setKB(KB5,SRC_ADDR);
	__mspu_setKS(KS5,BlockSize);
	__mspu_setKI(KI5,DataBSNum);
	__mspu_setKM(KM1,TailMask_L);
	__mspu_setHiKM(KM1,TailMask_H);	
	__mspu_setKG(KG1,BlockGran);
	__mspu_setKL(KL1,2);
	
	__mspu_setKB(KB8,DMRS_ADDR);
	__mspu_setKS(KS8,0x20000);
	__mspu_setKI(KI8,2);
	__mspu_setKB(KB9,DMRS_ADDR);
	__mspu_setKS(KS9,BlockSize);
	__mspu_setKI(KI9,DataBSNum);
	__mspu_setKM(KM2,TailMask_L);
	__mspu_setHiKM(KM2,TailMask_H);
	__mspu_setKG(KG2,BlockGran);
	__mspu_setKL(KL2,2);
	
	__mspu_setKB(KB0,DST_ADDR);
	__mspu_setKS(KS0,0x20000);
	__mspu_setKI(KI0,2);
	__mspu_setKB(KB1,DST_ADDR);
	__mspu_setKS(KS1,BlockSize);
	__mspu_setKI(KI1,DataBSNum);
	__mspu_setKM(KM0,TailMask_L);
	__mspu_setHiKM(KM0,TailMask_H);	
	__mspu_setKG(KG0,BlockGran);
	__mspu_setKL(KL0,2);	

    if (DataGrpNum > 4){
        if (DataTailNum == 0){
        	MLOAD(ChEstLS_Grp);
			__mspu_callmb(ChEstLS_Grp);
		}else if (DataTailNum <= 32){
			MLOAD(ChEstLS_GrpT1);
			__mspu_callmb(ChEstLS_GrpT1);
        }else{
			MLOAD(ChEstLS_GrpT2);
			__mspu_callmb(ChEstLS_GrpT2);
        }
    }else if (DataGrpNum == 4){
		if (DataTailNum <= 32){
			MLOAD(ChEstLS_Grp4T1);
			__mspu_callmb(ChEstLS_Grp4T1);
        }else{
			MLOAD(ChEstLS_Grp4T2);
			__mspu_callmb(ChEstLS_Grp4T2);
        }
    }else if (DataGrpNum == 3){
		if (DataTailNum == 0){
        	MLOAD(ChEstLS_Grp3);
			__mspu_callmb(ChEstLS_Grp3);
		}else if (DataTailNum <= 32){
			MLOAD(ChEstLS_Grp3T1);
			__mspu_callmb(ChEstLS_Grp3T1);
        }else{
			MLOAD(ChEstLS_Grp3T2);
			__mspu_callmb(ChEstLS_Grp3T2);
        }
    }else if (DataGrpNum == 2){
		if (DataTailNum <= 32){
			MLOAD(ChEstLS_Grp2T1);
			__mspu_callmb(ChEstLS_Grp2T1);
        }else{
			MLOAD(ChEstLS_Grp2T2);
			__mspu_callmb(ChEstLS_Grp2T2);
        }
    }else if (DataGrpNum == 1){
		if (DataTailNum <= 32){
			MLOAD(ChEstLS_Grp1T1);
			__mspu_callmb(ChEstLS_Grp1T1);
        }else{
			MLOAD(ChEstLS_Grp1T2);
			__mspu_callmb(ChEstLS_Grp1T2);
        }
	}else{
		if (DataTailNum <= 32){
			MLOAD(ChEstLS_Tail1);
			__mspu_callmb(ChEstLS_Tail1);
        }else{
			MLOAD(ChEstLS_Tail2);
			__mspu_callmb(ChEstLS_Tail2);
        }
    }
}
