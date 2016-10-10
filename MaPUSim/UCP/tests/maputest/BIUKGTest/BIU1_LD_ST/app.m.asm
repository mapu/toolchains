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
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x004
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x006               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x008               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x00A               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x00C               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x00E               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x010               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x012               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x014               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x016               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x018               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x01A               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x01C               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x01E               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x020               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x022               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x024               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x026               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x028               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x02A               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x02C               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x02E               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x030          
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x032               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x034               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x036               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x038               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x03A               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x03C               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x03E               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x040               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x042               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x044               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x046               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x048               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x04A               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x04C               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x04E               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x050               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x052               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x054               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x056               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x058               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x05A               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x05C               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x05E               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x060               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x062               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x064               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x066               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x068               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x06A               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x06C               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x06E               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x070               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x072               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x074               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x076               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x078               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x07A               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x07C               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x07E               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x080               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x082               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x084
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x086
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x088
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x08A
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x08C
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x08E
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x090
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x092
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x094
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x096
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x098
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x09A
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x09C
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x09E
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0A0
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0A2
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0A4
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0A6
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0A8
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0AA
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0AC
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0AE
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0B0
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0B2
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0B4
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0B6
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0B8
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0BA
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0BC
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0BE
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0C0
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0C2
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0C4
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0C6
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0C8
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0CA
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0CC
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0CE
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0D0
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0D2
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0D4
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0D6
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0D8
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0DA
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0DC 
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0DE
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0E0
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0E2
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0E4
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0E6
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0E8
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0EA
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0EC
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0EE
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0F0
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0F2
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0F4
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0F6
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0F8
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0FA
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0FC
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x0FE
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x100
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x102
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x104
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x106
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x108
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x10A
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x10C
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x10E
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x110
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //        // PC = 0x112



 BIU1 (KG) ->    M[3] ; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 M[7]->    BIU1 (KG) ; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 



_BIUTest1:
    NOP; 
 M[0]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x128
 M[1]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x12B
 M[2]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x12E
 M[3]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x131
 M[4]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x134
 M[5]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x137
 M[6]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x13A
 M[7]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x13D
 M[8]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x140
 M[9]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x143
 M[10]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x146
 M[11]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x149
 M[12]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x14C
 M[13]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x14F
 M[14]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x152
 M[15]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x155
 M[16]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x158
 M[17]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	       // PC = 0x15B
 M[18]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	       // PC = 0x15E
 M[19]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	       // PC = 0x161
 M[20]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	       // PC = 0x164
 M[21]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	       // PC = 0x167
 M[22]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	       // PC = 0x16A
 M[23]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	       // PC = 0x16D
 M[24]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	      // PC = 0x170
 M[25]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	      // PC = 0x173
 M[26]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	       // PC = 0x176
 M[27]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	       // PC = 0x179
 M[28]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	       // PC = 0x17C
 M[29]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	       // PC = 0x17F
 M[30]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	      // PC = 0x182
 M[31]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x185
 M[32]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x188
 M[33]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x18B
 M[34]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x18E
 M[35]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x191
 M[36]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x194
 M[37]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x197
 M[38]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x19A
 M[39]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x19D
 M[40]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1A0
 M[41]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1A3
 M[42]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1A6
 M[43]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1A9
 M[44]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1AC
 M[45]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1AF
 M[46]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1B2
 M[47]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1B5
 M[48]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1B8
 M[49]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1BB
 M[50]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1BE
 M[51]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1C1
 M[52]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1C4
 M[53]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1C7
 M[54]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1CA
 M[55]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1CD
 M[56]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1D0
 M[57]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1D3
 M[58]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1D6
 M[59]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1D9
 M[60]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1DC
 M[61]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1DF
 M[62]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1E2
 M[63]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1E5
 M[64]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1E8
 M[65]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1EB
 M[66]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1EE
 M[67]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1F1
 M[68]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1F4
 M[69]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1F7
 M[70]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1FA
 M[71]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x1FD
 M[72]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x200
 M[73]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x203
 M[74]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x206
 M[75]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x209
 M[76]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x20C
 M[77]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x20F
 M[78]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x212
 M[79]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x215
 M[80]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x218
 M[81]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	      // PC = 0x21B
 M[82]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x21E
 M[83]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x221
 M[84]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x224
 M[85]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x227
 M[86]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x22A
 M[87]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x22D
 M[88]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x230
 M[89]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x233
 M[90]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x236
 M[91]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x239
 M[92]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x23C
 M[93]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x23F
 M[94]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x242
 M[95]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x245
 M[96]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x248
 M[97]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x24B
 M[98]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x24E
 M[99]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x251
 M[100]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x254
 M[101]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x257
 M[102]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x25A
 M[103]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x25D
 M[104]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x260
 M[105]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x263
 M[106]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x266
 M[107]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x269
 M[108]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x26C
 M[109]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x26F
 M[110]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x272
 M[111]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x275
 M[112]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x278
 M[113]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x27B
 M[114]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x27E
 M[115]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x281
 M[116]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x284
 M[117]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x287
 M[118]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x28A
 M[119]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x28D
 M[120]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x290
 M[121]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x293
 M[122]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x296
 M[123]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x299
 M[124]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x29C
 M[125]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x29F
 M[126]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2A2
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2A5
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2A8
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2AB
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2AE
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2B1
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2B4
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2B7
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2BA
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2BD
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2C0
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2C3
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2C6
    


    NOP; 
 M[3]   ->    BIU1 (KG) ; 
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



    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //2D4        // PC = 0x114
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //2D6        // PC = 0x116
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     //2D8        // PC = 0x118
    
    

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
    NOP; BIU1.DM  (A++,K++) ->  M[0]; 	// PC = 0x004
    NOP; BIU1.DM  (A++,K++) ->  M[1]; 	// PC = 0x006               
    NOP; BIU1.DM  (A++,K++) ->  M[2]; 	// PC = 0x008               
    NOP; BIU1.DM  (A++,K++) ->  M[3]; 	// PC = 0x00A               
    NOP; BIU1.DM  (A++,K++) ->  M[4]; 	// PC = 0x00C               
    NOP; BIU1.DM  (A++,K++) ->  M[5]; 	// PC = 0x00E               
    NOP; BIU1.DM  (A++,K++) ->  M[6]; 	// PC = 0x010               
    NOP; BIU1.DM  (A++,K++) ->  M[7]; 	// PC = 0x012               
    NOP; BIU1.DM  (A++,K++) ->  M[8]; 	// PC = 0x014               
    NOP; BIU1.DM  (A++,K++) ->  M[9]; 	// PC = 0x016               
    NOP; BIU1.DM  (A++,K++) ->  M[10];   	// PC = 0x018               
    NOP; BIU1.DM  (A++,K++) ->  M[11];   	// PC = 0x01A               
    NOP; BIU1.DM  (A++,K++) ->  M[12];   	// PC = 0x01C               
    NOP; BIU1.DM  (A++,K++) ->  M[13];   	// PC = 0x01E               
    NOP; BIU1.DM  (A++,K++) ->  M[14];   	// PC = 0x020               
    NOP; BIU1.DM  (A++,K++) ->  M[15];   	// PC = 0x022               
    NOP; BIU1.DM  (A++,K++) ->  M[16];   	// PC = 0x024               
    NOP; BIU1.DM  (A++,K++) ->  M[17];   	// PC = 0x026               
    NOP; BIU1.DM  (A++,K++) ->  M[18];   	// PC = 0x028               
    NOP; BIU1.DM  (A++,K++) ->  M[19];   	// PC = 0x02A               
    NOP; BIU1.DM  (A++,K++) ->  M[20];   	// PC = 0x02C               
    NOP; BIU1.DM  (A++,K++) ->  M[21];   	// PC = 0x02E               
    NOP; BIU1.DM  (A++,K++) ->  M[22];   	// PC = 0x030          
    NOP; BIU1.DM  (A++,K++) ->  M[23];   //        // PC = 0x032               
    NOP; BIU1.DM  (A++,K++) ->  M[24];   //        // PC = 0x034               
    NOP; BIU1.DM  (A++,K++) ->  M[25];   //        // PC = 0x036               
    NOP; BIU1.DM  (A++,K++) ->  M[26];   //        // PC = 0x038               
    NOP; BIU1.DM  (A++,K++) ->  M[27];   //        // PC = 0x03A               
    NOP; BIU1.DM  (A++,K++) ->  M[28];   //        // PC = 0x03C               
    NOP; BIU1.DM  (A++,K++) ->  M[29];   //        // PC = 0x03E               
    NOP; BIU1.DM  (A++,K++) ->  M[30];   //        // PC = 0x040               
    NOP; BIU1.DM  (A++,K++) ->  M[31];   //        // PC = 0x042               
    NOP; BIU1.DM  (A++,K++) ->  M[32];   //        // PC = 0x044               
    NOP; BIU1.DM  (A++,K++) ->  M[33];   //        // PC = 0x046               
    NOP; BIU1.DM  (A++,K++) ->  M[34];   //        // PC = 0x048               
    NOP; BIU1.DM  (A++,K++) ->  M[35];   //        // PC = 0x04A               
    NOP; BIU1.DM  (A++,K++) ->  M[36];   //        // PC = 0x04C               
    NOP; BIU1.DM  (A++,K++) ->  M[37];   //        // PC = 0x04E               
    NOP; BIU1.DM  (A++,K++) ->  M[38];   //        // PC = 0x050               
    NOP; BIU1.DM  (A++,K++) ->  M[39];   //        // PC = 0x052               
    NOP; BIU1.DM  (A++,K++) ->  M[40];   //        // PC = 0x054               
    NOP; BIU1.DM  (A++,K++) ->  M[41];   //        // PC = 0x056               
    NOP; BIU1.DM  (A++,K++) ->  M[42];   //        // PC = 0x058               
    NOP; BIU1.DM  (A++,K++) ->  M[43];   //        // PC = 0x05A               
    NOP; BIU1.DM  (A++,K++) ->  M[44];   //        // PC = 0x05C               
    NOP; BIU1.DM  (A++,K++) ->  M[45];   //        // PC = 0x05E               
    NOP; BIU1.DM  (A++,K++) ->  M[46];   //        // PC = 0x060               
    NOP; BIU1.DM  (A++,K++) ->  M[47];   //        // PC = 0x062               
    NOP; BIU1.DM  (A++,K++) ->  M[48];   //        // PC = 0x064               
    NOP; BIU1.DM  (A++,K++) ->  M[49];   //        // PC = 0x066               
    NOP; BIU1.DM  (A++,K++) ->  M[50];   //        // PC = 0x068               
    NOP; BIU1.DM  (A++,K++) ->  M[51];   //        // PC = 0x06A               
    NOP; BIU1.DM  (A++,K++) ->  M[52];   //        // PC = 0x06C               
    NOP; BIU1.DM  (A++,K++) ->  M[53];   //        // PC = 0x06E               
    NOP; BIU1.DM  (A++,K++) ->  M[54];   //        // PC = 0x070               
    NOP; BIU1.DM  (A++,K++) ->  M[55];   //        // PC = 0x072               
    NOP; BIU1.DM  (A++,K++) ->  M[56];   //        // PC = 0x074               
    NOP; BIU1.DM  (A++,K++) ->  M[57];   //        // PC = 0x076               
    NOP; BIU1.DM  (A++,K++) ->  M[58];   //        // PC = 0x078               
    NOP; BIU1.DM  (A++,K++) ->  M[59];   //        // PC = 0x07A               
    NOP; BIU1.DM  (A++,K++) ->  M[60];   	// PC = 0x004
    NOP; BIU1.DM  (A++,K++) ->  M[61];   	// PC = 0x006               
    NOP; BIU1.DM  (A++,K++) ->  M[62];   	// PC = 0x008               
    NOP; BIU1.DM  (A++,K++) ->  M[63];   	// PC = 0x00A               
    NOP; BIU1.DM  (A++,K++) ->  M[64];   	// PC = 0x00C               
    NOP; BIU1.DM  (A++,K++) ->  M[65];   	// PC = 0x00E               
    NOP; BIU1.DM  (A++,K++) ->  M[66];   	// PC = 0x010               
    NOP; BIU1.DM  (A++,K++) ->  M[67];   	// PC = 0x012               
    NOP; BIU1.DM  (A++,K++) ->  M[68];   	// PC = 0x014               
    NOP; BIU1.DM  (A++,K++) ->  M[69];   	// PC = 0x016               
    NOP; BIU1.DM  (A++,K++) ->  M[70];   	// PC = 0x018               
    NOP; BIU1.DM  (A++,K++) ->  M[71];   	// PC = 0x01A               
    NOP; BIU1.DM  (A++,K++) ->  M[72];   	// PC = 0x01C               
    NOP; BIU1.DM  (A++,K++) ->  M[73];   	// PC = 0x01E               
    NOP; BIU1.DM  (A++,K++) ->  M[74];   	// PC = 0x020               
    NOP; BIU1.DM  (A++,K++) ->  M[75];   	// PC = 0x022               
    NOP; BIU1.DM  (A++,K++) ->  M[76];   	// PC = 0x024               
    NOP; BIU1.DM  (A++,K++) ->  M[77];   	// PC = 0x026               
    NOP; BIU1.DM  (A++,K++) ->  M[78];   	// PC = 0x028               
    NOP; BIU1.DM  (A++,K++) ->  M[79];   	// PC = 0x02A               
    NOP; BIU1.DM  (A++,K++) ->  M[80];   	// PC = 0x02C               
    NOP; BIU1.DM  (A++,K++) ->  M[81];   	// PC = 0x02E               
    NOP; BIU1.DM  (A++,K++) ->  M[82];   	// PC = 0x030          
    NOP; BIU1.DM  (A++,K++) ->  M[83];   //        // PC = 0x032               
    NOP; BIU1.DM  (A++,K++) ->  M[84];   //        // PC = 0x034               
    NOP; BIU1.DM  (A++,K++) ->  M[85];   //        // PC = 0x036               
    NOP; BIU1.DM  (A++,K++) ->  M[86];   //        // PC = 0x038               
    NOP; BIU1.DM  (A++,K++) ->  M[87];   //        // PC = 0x03A               
    NOP; BIU1.DM  (A++,K++) ->  M[88];   //        // PC = 0x03C               
    NOP; BIU1.DM  (A++,K++) ->  M[89];   //        // PC = 0x03E               
    NOP; BIU1.DM  (A++,K++) ->  M[90];   //        // PC = 0x040               
    NOP; BIU1.DM  (A++,K++) ->  M[91];   //        // PC = 0x042               
    NOP; BIU1.DM  (A++,K++) ->  M[92];   //        // PC = 0x044               
    NOP; BIU1.DM  (A++,K++) ->  M[93];   //        // PC = 0x046               
    NOP; BIU1.DM  (A++,K++) ->  M[94];   //        // PC = 0x048               
    NOP; BIU1.DM  (A++,K++) ->  M[95];   //        // PC = 0x04A               
    NOP; BIU1.DM  (A++,K++) ->  M[96];   //        // PC = 0x04C               
    NOP; BIU1.DM  (A++,K++) ->  M[97];   //        // PC = 0x04E               
    NOP; BIU1.DM  (A++,K++) ->  M[98];   //        // PC = 0x050               
    NOP; BIU1.DM  (A++,K++) ->  M[99];   //        // PC = 0x052  
    NOP; BIU1.DM  (A++,K++) ->  M[100];   //        // PC = 0x068 
    NOP; BIU1.DM  (A++,K++) ->  M[101];   //        // PC = 0x056               
    NOP; BIU1.DM  (A++,K++) ->  M[102];   //        // PC = 0x058               
    NOP; BIU1.DM  (A++,K++) ->  M[103];   //        // PC = 0x05A               
    NOP; BIU1.DM  (A++,K++) ->  M[104];   //        // PC = 0x05C               
    NOP; BIU1.DM  (A++,K++) ->  M[105];   //        // PC = 0x05E               
    NOP; BIU1.DM  (A++,K++) ->  M[106];   //        // PC = 0x060               
    NOP; BIU1.DM  (A++,K++) ->  M[107];   //        // PC = 0x062               
    NOP; BIU1.DM  (A++,K++) ->  M[108];   //        // PC = 0x064               
    NOP; BIU1.DM  (A++,K++) ->  M[109];   //        // PC = 0x066                          
    NOP; BIU1.DM  (A++,K++) ->  M[110];   //        // PC = 0x054               
    NOP; BIU1.DM  (A++,K++) ->  M[111];   //        // PC = 0x056               
    NOP; BIU1.DM  (A++,K++) ->  M[112];   //        // PC = 0x058               
    NOP; BIU1.DM  (A++,K++) ->  M[113];   //        // PC = 0x05A               
    NOP; BIU1.DM  (A++,K++) ->  M[114];   //        // PC = 0x05C               
    NOP; BIU1.DM  (A++,K++) ->  M[115];   //        // PC = 0x05E               
    NOP; BIU1.DM  (A++,K++) ->  M[116];   //        // PC = 0x060               
    NOP; BIU1.DM  (A++,K++) ->  M[117];   //        // PC = 0x062               
    NOP; BIU1.DM  (A++,K++) ->  M[118];   //        // PC = 0x064               
    NOP; BIU1.DM  (A++,K++) ->  M[119];   //        // PC = 0x066               
    NOP; BIU1.DM  (A++,K++) ->  M[120];   //        // PC = 0x068               
    NOP; BIU1.DM  (A++,K++) ->  M[121];   //        // PC = 0x06A               
    NOP; BIU1.DM  (A++,K++) ->  M[122];   //        // PC = 0x06C               
    NOP; BIU1.DM  (A++,K++) ->  M[123];   //        // PC = 0x06E               
    NOP; BIU1.DM  (A++,K++) ->  M[124];   //        // PC = 0x070               
    NOP; BIU1.DM  (A++,K++) ->  M[125];   //        // PC = 0x072               
    NOP; BIU1.DM  (A++,K++) ->  M[126];   //        // PC = 0x074               
    NOP; BIU1.DM  (A++,K++) ->  M[127];   //        // PC = 0x076               
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    SHU0.T0 IND TSQ -> M[7] ;
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
