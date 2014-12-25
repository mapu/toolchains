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
    NOP; 
    NOP; 
    NOP; 
_BIUTest0:
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x004
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x006               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x008               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x00A               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x00C               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x00E               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x010               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x012               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x014               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x016               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x018               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x01A               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x01C               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x01E               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x020               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x022               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x024               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x026               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x028               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x02A               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x02C               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x02E               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x030          
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x032               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x034               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x036               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x038               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x03A               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x03C               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x03E               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x040               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x042               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x044               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x046               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x048               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x04A               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x04C               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x04E               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x050               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x052               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x054               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x056               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x058               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x05A               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x05C               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x05E               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x060               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x062               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x064               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x066               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x068               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x06A               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x06C               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x06E               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x070               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x072               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x074               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x076               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x078               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x07A               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x07C               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x07E               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x080               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x082               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x084
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x086
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x088
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x08A
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x08C
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x08E
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x090
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x092
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x094
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x096
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x098
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x09A
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x09C
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x09E
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0A0
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0A2
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0A4
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0A6
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0A8
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0AA
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0AC
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0AE
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0B0
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0B2
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0B4
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0B6
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0B8
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0BA
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0BC
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0BE
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0C0
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0C2
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0C4
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0C6
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0C8
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0CA
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0CC
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0CE
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0D0
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0D2
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0D4
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0D6
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0D8
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0DA
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0DC 
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0DE
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0E0
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0E2
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0E4
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0E6
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0E8
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0EA
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0EC
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0EE
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0F0
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0F2
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0F4
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0F6
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0F8
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0FA
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0FC
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0FE
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x100
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x102
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x104
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x106
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x108
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x10A
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x10C
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x10E
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x110
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x112



 BIU2 (KG) ->    M[3] ; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 M[7]->    BIU2 (KG) ; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 



_BIUTest1:
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x128
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x12a               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x12c               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x12e               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x130               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x132               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x134               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x136              
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x138               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x13a               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x13c               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x13e               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x140               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x142               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x144               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x146               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x148             
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x14a               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x14c               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x14e               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x150               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x152               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x154          
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x156               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x158               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x15a               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x15c               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x15e               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x160               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x162               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x164               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x166               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x168               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x16a               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x16c               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x16e               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x170               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x172               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x174               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x176               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x178               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x17a               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x17c               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x17e               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x180               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x182               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x184               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x186               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x188               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x18a               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x18c               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x18e               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x190               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x192               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x194               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x196               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x198               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x19a               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x19c               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x19e               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1a0               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1a2               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1a4               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1a6               
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1a8
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1aa
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1ac
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1ae
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1b0
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1b2
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1b4
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1b6
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1b8
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1ba
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1bc
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1be
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1c0
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1c2
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1c4
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1c6
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1c8
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1ca
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1cc
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1ce
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1d0
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1d2
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1d4
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1d6
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1d8
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1da
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1dc
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1de
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1e0
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1e2
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1e4
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1e6
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1e8
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1ea
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1ec
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1ee
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1f0
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1f2
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1f4
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1f6
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1f8
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1fa
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1fc
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x1fe
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x200 
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x202
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x204
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x206
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x208
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x20a
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x20c
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x20e
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x210
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x212
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x214
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x216
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x218
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x21a
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x21c
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x21e
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x220
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x222
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x224
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x226
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x228
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x22a
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x22c 
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x22e
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x230
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x232
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x234
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x236
    NOP; 


 M[3]   ->    BIU2 (KG) ; 

    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP;   




    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //242      
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //244        
    NOP;  BIU2.DM  (A++,K++)-> IALU.T0 (I0) ;     //246      
    
    
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    MPUSTOP;




_BIUTest2:
    NOP;
    SHU0.T0 IND TSQ -> M[7];
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    MPUSTOP;
