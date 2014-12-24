//////////////////////////////////////////////////////////////////////////
//  Design unit	:  BIU Test						//
//  		:							//
//  File name	:  app.m.asm						//
//		:							//
//  Author	:  yanglp@zklc.com 					//
//		:							//
//  Description	:  The file to test the function of BIU Function	//
//                 Point						//
//  									//
//  Revision	:  Version 1.0	08/16/2013				//
//////////////////////////////////////////////////////////////////////////

.text
    .global _start    
    .global _BIUTest0
    .global _BIUTest1
    .global _BIUTest2

_start:
    NOP;;
    NOP;;
    NOP;;
_BIUTest0:
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x004
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x006               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x008               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x00A               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x00C               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x00E               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x010               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x012               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x014               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x016               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x018               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x01A               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x01C               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x01E               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x020               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x022               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x024               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x026               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x028               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x02A               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x02C               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x02E               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x030          
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x032               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x034               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x036               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x038               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x03A               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x03C               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x03E               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x040               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x042               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x044               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x046               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x048               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x04A               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x04C               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x04E               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x050               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x052               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x054               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x056               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x058               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x05A               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x05C               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x05E               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x060               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x062               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x064               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x066               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x068               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x06A               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x06C               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x06E               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x070               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x072               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x074               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x076               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x078               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x07A               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x07C               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x07E               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x080               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x082               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x084
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x086
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x088
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x08A
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x08C
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x08E
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x090
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x092
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x094
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x096
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x098
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x09A
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x09C
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x09E
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0A0
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0A2
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0A4
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0A6
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0A8
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0AA
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0AC
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0AE
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0B0
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0B2
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0B4
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0B6
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0B8
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0BA
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0BC
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0BE
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0C0
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0C2
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0C4
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0C6
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0C8
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0CA
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0CC
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0CE
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0D0
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0D2
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0D4
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0D6
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0D8
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0DA
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0DC 
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0DE
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0E0
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0E2
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0E4
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0E6
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0E8
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0EA
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0EC
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0EE
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0F0
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0F2
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0F4
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0F6
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0F8
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0FA
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0FC
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x0FE
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x100
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x102
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x104
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x106
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x108
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x10A
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x10C
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x10E
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x110
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x112



    M3.w0 = BIU1.KG ;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    BIU1.KG = M7.r3;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;



_BIUTest1:
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x128
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x12a               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x12c               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x12e               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x130               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x132               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x134               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x136              
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x138               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x13a               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x13c               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x13e               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x140               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x142               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x144               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x146               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x148             
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x14a               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x14c               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x14e               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x150               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x152               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x154          
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x156               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x158               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x15a               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x15c               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x15e               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x160               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x162               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x164               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x166               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x168               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x16a               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x16c               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x16e               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x170               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x172               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x174               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x176               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x178               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x17a               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x17c               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x17e               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x180               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x182               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x184               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x186               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x188               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x18a               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x18c               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x18e               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x190               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x192               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x194               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x196               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x198               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x19a               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x19c               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x19e               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1a0               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1a2               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1a4               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1a6               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1a8
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1aa
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1ac
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1ae
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1b0
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1b2
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1b4
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1b6
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1b8
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1ba
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1bc
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1be
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1c0
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1c2
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1c4
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1c6
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1c8
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1ca
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1cc
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1ce
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1d0
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1d2
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1d4
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1d6
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1d8
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1da
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1dc
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1de
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1e0
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1e2
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1e4
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1e6
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1e8
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1ea
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1ec
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1ee
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1f0
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1f2
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1f4
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1f6
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1f8
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1fa
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1fc
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x1fe
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x200 
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x202
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x204
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x206
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x208
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x20a
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x20c
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x20e
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x210
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x212
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x214
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x216
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x218
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x21a
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x21c
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x21e
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x220
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x222
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x224
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x226
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x228
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x22a
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x22c 
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x22e
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x230
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x232
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x234
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //        // PC = 0x236
    NOP;;


    BIU1.KG = M3.r3   ;;

    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;  




    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //242      
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //244        
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    //246      
    
    
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    MPU.Stop;;




_BIUTest2:
    NOP;;
    M7.w3 = SHU0.T0 IND TSQ ;;
    NOP;;
    NOP;;
    NOP;;
    MPU.Stop;;
