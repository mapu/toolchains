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
    NOP; 
    NOP; 
_BIUTest1:
    NOP; 
 M[0]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x008
 M[1]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x00B
 M[2]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x00E
 M[3]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x011
 M[4]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x014
 M[5]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x017
 M[6]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x01A
 M[7]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x01D
 M[8]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x020
 M[9]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x023
 M[10]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x026
 M[11]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x029
 M[12]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x02C
 M[13]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x02F
 M[14]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x032
 M[15]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x035
 M[16]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x038
 M[17]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x03B
 M[18]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x03E
 M[19]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x041
 M[20]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x044
 M[21]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x047
 M[22]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x04A
 M[23]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x04D
 M[24]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);     	        // PC = 0x050
 M[25]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);       	        // PC = 0x053
 M[26]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x056
 M[27]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x059
 M[28]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x05C
 M[29]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x05F
 M[30]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x062
 M[31]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x065
 M[32]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x068
 M[33]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x06B
 M[34]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x06E
 M[35]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x071
 M[36]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x074
 M[37]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x077
 M[38]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x07A
 M[39]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x07D
 M[40]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x080
 M[41]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x083
 M[42]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x086
 M[43]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x089
 M[44]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x08C
 M[45]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x08F
 M[46]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x092
 M[47]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x095
 M[48]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x098
 M[49]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x09B
 M[50]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x09E
 M[51]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0A1
 M[52]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0A4
 M[53]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0A7
 M[54]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0AA
 M[55]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0AD
 M[56]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0B0
 M[57]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0B3
 M[58]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0B6
 M[59]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0B9
 M[60]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0BC
 M[61]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0BF
 M[62]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0C2
 M[63]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0C5
 M[64]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0C8
 M[65]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0CB
 M[66]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0CE
 M[67]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0D1
 M[68]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0D4
 M[69]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0D7
 M[70]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0DA
 M[71]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0DD
 M[72]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0E0
 M[73]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0E3
 M[74]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0E6
 M[75]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0E9
 M[76]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0EC
 M[77]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0EF
 M[78]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0F2
 M[79]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0F5
 M[80]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0F8
 M[81]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0FB
 M[82]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x0FE
 M[83]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x101
 M[84]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x104
 M[85]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x107
 M[86]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x10A
 M[87]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x10D
 M[88]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x110
 M[89]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x113
 M[90]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x116
 M[91]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x119
 M[92]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x11C
 M[93]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x11F
 M[94]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x122
 M[95]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x125
 M[96]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x128
 M[97]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x12B
 M[98]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x12E
 M[99]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x131
 M[100]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x134
 M[101]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x137
 M[102]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x13A
 M[103]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x13D
 M[104]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x140
 M[105]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x143
 M[106]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x146
 M[107]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x149
 M[108]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x14C
 M[109]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x14F
 M[110]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x152
 M[111]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x155
 M[112]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x158
 M[113]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x15B
 M[114]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x15E
 M[115]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x161
 M[116]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x164
 M[117]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x167
 M[118]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x16A
 M[119]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x16D
 M[120]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x170



 BIU1 (KG) ->    M[3] ; 
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
    NOP; 


_BIUTest0:
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x184
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x186               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x188               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x18A               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x18C               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x18E               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x190               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x192               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x194               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x196               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x198               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x19A               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x19C               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x19E               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x1A0               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x1A2               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x1A4               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x1A6               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x1A8               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x1AA               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x1AC               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x1AE               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;     	// PC = 0x1B0          
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1B2               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1B4               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1B6               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1B8               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1BA               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1BC               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1BE               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1C0               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1C2               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1C4               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1C6               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1C8               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1CA               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1CC               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1CE               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1D0               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1D2               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1D4               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1D6               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1D8               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1DA               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1DC               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1DE               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1E0               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1E2               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1E4               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1E6               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1E8               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1EA               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1EC               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1EE               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1F0               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1F2               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1F4               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1F6               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1F8               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1FA               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1FC               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x1FE               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x200               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x202               
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x204
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x206
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x208
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x20A
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x20C
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x20E
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x210
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x212
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x214
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x216
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x218
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x21A
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x21C
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x21E
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x220
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x222
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x224
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x226
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x228
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x22A
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x22C
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x22E
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x230
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x232
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x234
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x236
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x238
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x23A
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x23C
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x23E
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x240
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x242
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x244
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x246
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x248
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x24A
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x24C
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x24E
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x250
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x252
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x254
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x256
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x258
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x25A
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x25C 
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x25E
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x260
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x262
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x264
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x266
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x268
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x26A
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x26C
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x26E
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x270
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x272
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x274
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x276
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x278
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x27A
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x27C
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x27E
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x280
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x282
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x284
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x286
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x288
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x28A
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x28C
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x28E
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x290
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x292
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x294
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x296
    NOP;  BIU1.DM  (A++,K++)-> IALU.T0 (I0) ;          // PC = 0x298

    NOP; 
 M[3]   ->    BIU1 (KG) ; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP;  
 M[121]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2A3
 M[122]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2A6
 M[123]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2A9
 M[124]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2AC
 M[125]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2AF
 M[126]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2B2
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2B5
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2B8
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2BB
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2BE
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2C1
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2C4
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2C7
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2CA
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2CD
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2D0
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2D3
 M[127]->    BIU1 ; 	NOP; BIU1->DM(A++,K++);    	        // PC = 0x2D6


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
    NOP; BIU1.DM (A++,K++) -> M[0];   	// PC = 0x004
    NOP; BIU1.DM (A++,K++) -> M[1];   	// PC = 0x006               
    NOP; BIU1.DM (A++,K++) -> M[2];   	// PC = 0x008               
    NOP; BIU1.DM (A++,K++) -> M[3];   	// PC = 0x00A               
    NOP; BIU1.DM (A++,K++) -> M[4];   	// PC = 0x00C               
    NOP; BIU1.DM (A++,K++) -> M[5];   	// PC = 0x00E               
    NOP; BIU1.DM (A++,K++) -> M[6];   	// PC = 0x010               
    NOP; BIU1.DM (A++,K++) -> M[7];   	// PC = 0x012               
    NOP; BIU1.DM (A++,K++) -> M[8];   	// PC = 0x014               
    NOP; BIU1.DM (A++,K++) -> M[9];   	// PC = 0x016               
    NOP; BIU1.DM (A++,K++) -> M[10];   	// PC = 0x018               
    NOP; BIU1.DM (A++,K++) -> M[11];   	// PC = 0x01A               
    NOP; BIU1.DM (A++,K++) -> M[12];   	// PC = 0x01C               
    NOP; BIU1.DM (A++,K++) -> M[13];   	// PC = 0x01E               
    NOP; BIU1.DM (A++,K++) -> M[14];   	// PC = 0x020               
    NOP; BIU1.DM (A++,K++) -> M[15];   	// PC = 0x022               
    NOP; BIU1.DM (A++,K++) -> M[16];   	// PC = 0x024               
    NOP; BIU1.DM (A++,K++) -> M[17];   	// PC = 0x026               
    NOP; BIU1.DM (A++,K++) -> M[18];   	// PC = 0x028               
    NOP; BIU1.DM (A++,K++) -> M[19];   	// PC = 0x02A               
    NOP; BIU1.DM (A++,K++) -> M[20];   	// PC = 0x02C               
    NOP; BIU1.DM (A++,K++) -> M[21];   	// PC = 0x02E               
    NOP; BIU1.DM (A++,K++) -> M[22];   	// PC = 0x030          
    NOP; BIU1.DM (A++,K++) -> M[23];   //        // PC = 0x032               
    NOP; BIU1.DM (A++,K++) -> M[24];   //        // PC = 0x034               
    NOP; BIU1.DM (A++,K++) -> M[25];   //        // PC = 0x036               
    NOP; BIU1.DM (A++,K++) -> M[26];   //        // PC = 0x038               
    NOP; BIU1.DM (A++,K++) -> M[27];   //        // PC = 0x03A               
    NOP; BIU1.DM (A++,K++) -> M[28];   //        // PC = 0x03C               
    NOP; BIU1.DM (A++,K++) -> M[29];   //        // PC = 0x03E               
    NOP; BIU1.DM (A++,K++) -> M[30];   //        // PC = 0x040               
    NOP; BIU1.DM (A++,K++) -> M[31];   //        // PC = 0x042               
    NOP; BIU1.DM (A++,K++) -> M[32];   //        // PC = 0x044               
    NOP; BIU1.DM (A++,K++) -> M[33];   //        // PC = 0x046               
    NOP; BIU1.DM (A++,K++) -> M[34];   //        // PC = 0x048               
    NOP; BIU1.DM (A++,K++) -> M[35];   //        // PC = 0x04A               
    NOP; BIU1.DM (A++,K++) -> M[36];   //        // PC = 0x04C               
    NOP; BIU1.DM (A++,K++) -> M[37];   //        // PC = 0x04E               
    NOP; BIU1.DM (A++,K++) -> M[38];   //        // PC = 0x050               
    NOP; BIU1.DM (A++,K++) -> M[39];   //        // PC = 0x052               
    NOP; BIU1.DM (A++,K++) -> M[40];   //        // PC = 0x054               
    NOP; BIU1.DM (A++,K++) -> M[41];   //        // PC = 0x056               
    NOP; BIU1.DM (A++,K++) -> M[42];   //        // PC = 0x058               
    NOP; BIU1.DM (A++,K++) -> M[43];   //        // PC = 0x05A               
    NOP; BIU1.DM (A++,K++) -> M[44];   //        // PC = 0x05C               
    NOP; BIU1.DM (A++,K++) -> M[45];   //        // PC = 0x05E               
    NOP; BIU1.DM (A++,K++) -> M[46];   //        // PC = 0x060               
    NOP; BIU1.DM (A++,K++) -> M[47];   //        // PC = 0x062               
    NOP; BIU1.DM (A++,K++) -> M[48];   //        // PC = 0x064               
    NOP; BIU1.DM (A++,K++) -> M[49];   //        // PC = 0x066               
    NOP; BIU1.DM (A++,K++) -> M[50];   //        // PC = 0x068               
    NOP; BIU1.DM (A++,K++) -> M[51];   //        // PC = 0x06A               
    NOP; BIU1.DM (A++,K++) -> M[52];   //        // PC = 0x06C               
    NOP; BIU1.DM (A++,K++) -> M[53];   //        // PC = 0x06E               
    NOP; BIU1.DM (A++,K++) -> M[54];   //        // PC = 0x070               
    NOP; BIU1.DM (A++,K++) -> M[55];   //        // PC = 0x072               
    NOP; BIU1.DM (A++,K++) -> M[56];   //        // PC = 0x074               
    NOP; BIU1.DM (A++,K++) -> M[57];   //        // PC = 0x076               
    NOP; BIU1.DM (A++,K++) -> M[58];   //        // PC = 0x078               
    NOP; BIU1.DM (A++,K++) -> M[59];   //        // PC = 0x07A               
    NOP; BIU1.DM (A++,K++) -> M[60];   	// PC = 0x004
    NOP; BIU1.DM (A++,K++) -> M[61];   	// PC = 0x006               
    NOP; BIU1.DM (A++,K++) -> M[62];   	// PC = 0x008               
    NOP; BIU1.DM (A++,K++) -> M[63];   	// PC = 0x00A               
    NOP; BIU1.DM (A++,K++) -> M[64];   	// PC = 0x00C               
    NOP; BIU1.DM (A++,K++) -> M[65];   	// PC = 0x00E               
    NOP; BIU1.DM (A++,K++) -> M[66];   	// PC = 0x010               
    NOP; BIU1.DM (A++,K++) -> M[67];   	// PC = 0x012               
    NOP; BIU1.DM (A++,K++) -> M[68];   	// PC = 0x014               
    NOP; BIU1.DM (A++,K++) -> M[69];   	// PC = 0x016               
    NOP; BIU1.DM (A++,K++) -> M[70];   	// PC = 0x018               
    NOP; BIU1.DM (A++,K++) -> M[71];   	// PC = 0x01A               
    NOP; BIU1.DM (A++,K++) -> M[72];   	// PC = 0x01C               
    NOP; BIU1.DM (A++,K++) -> M[73];   	// PC = 0x01E               
    NOP; BIU1.DM (A++,K++) -> M[74];   	// PC = 0x020               
    NOP; BIU1.DM (A++,K++) -> M[75];   	// PC = 0x022               
    NOP; BIU1.DM (A++,K++) -> M[76];   	// PC = 0x024               
    NOP; BIU1.DM (A++,K++) -> M[77];   	// PC = 0x026               
    NOP; BIU1.DM (A++,K++) -> M[78];   	// PC = 0x028               
    NOP; BIU1.DM (A++,K++) -> M[79];   	// PC = 0x02A               
    NOP; BIU1.DM (A++,K++) -> M[80];   	// PC = 0x02C               
    NOP; BIU1.DM (A++,K++) -> M[81];   	// PC = 0x02E               
    NOP; BIU1.DM (A++,K++) -> M[82];   	// PC = 0x030          
    NOP; BIU1.DM (A++,K++) -> M[83];   //        // PC = 0x032               
    NOP; BIU1.DM (A++,K++) -> M[84];   //        // PC = 0x034               
    NOP; BIU1.DM (A++,K++) -> M[85];   //        // PC = 0x036               
    NOP; BIU1.DM (A++,K++) -> M[86];   //        // PC = 0x038               
    NOP; BIU1.DM (A++,K++) -> M[87];   //        // PC = 0x03A               
    NOP; BIU1.DM (A++,K++) -> M[88];   //        // PC = 0x03C               
    NOP; BIU1.DM (A++,K++) -> M[89];   //        // PC = 0x03E               
    NOP; BIU1.DM (A++,K++) -> M[90];   //        // PC = 0x040               
    NOP; BIU1.DM (A++,K++) -> M[91];   //        // PC = 0x042               
    NOP; BIU1.DM (A++,K++) -> M[92];   //        // PC = 0x044               
    NOP; BIU1.DM (A++,K++) -> M[93];   //        // PC = 0x046               
    NOP; BIU1.DM (A++,K++) -> M[94];   //        // PC = 0x048               
    NOP; BIU1.DM (A++,K++) -> M[95];   //        // PC = 0x04A               
    NOP; BIU1.DM (A++,K++) -> M[96];   //        // PC = 0x04C               
    NOP; BIU1.DM (A++,K++) -> M[97];   //        // PC = 0x04E               
    NOP; BIU1.DM (A++,K++) -> M[98];   //        // PC = 0x050               
    NOP; BIU1.DM (A++,K++) -> M[99];   //        // PC = 0x052  
    NOP; BIU1.DM (A++,K++) -> M[100];    //        // PC = 0x068 
    NOP; BIU1.DM (A++,K++) -> M[101];    //        // PC = 0x056               
    NOP; BIU1.DM (A++,K++) -> M[102];    //        // PC = 0x058               
    NOP; BIU1.DM (A++,K++) -> M[103];    //        // PC = 0x05A               
    NOP; BIU1.DM (A++,K++) -> M[104];    //        // PC = 0x05C               
    NOP; BIU1.DM (A++,K++) -> M[105];    //        // PC = 0x05E               
    NOP; BIU1.DM (A++,K++) -> M[106];    //        // PC = 0x060               
    NOP; BIU1.DM (A++,K++) -> M[107];    //        // PC = 0x062               
    NOP; BIU1.DM (A++,K++) -> M[108];    //        // PC = 0x064               
    NOP; BIU1.DM (A++,K++) -> M[109];    //        // PC = 0x066                          
    NOP; BIU1.DM (A++,K++) -> M[110];    //        // PC = 0x054               
    NOP; BIU1.DM (A++,K++) -> M[111];    //        // PC = 0x056               
    NOP; BIU1.DM (A++,K++) -> M[112];    //        // PC = 0x058               
    NOP; BIU1.DM (A++,K++) -> M[113];    //        // PC = 0x05A               
    NOP; BIU1.DM (A++,K++) -> M[114];    //        // PC = 0x05C               
    NOP; BIU1.DM (A++,K++) -> M[115];    //        // PC = 0x05E               
    NOP; BIU1.DM (A++,K++) -> M[116];    //        // PC = 0x060               
    NOP; BIU1.DM (A++,K++) -> M[117];    //        // PC = 0x062               
    NOP; BIU1.DM (A++,K++) -> M[118];    //        // PC = 0x064               
    NOP; BIU1.DM (A++,K++) -> M[119];    //        // PC = 0x066               
    NOP; BIU1.DM (A++,K++) -> M[120];    //        // PC = 0x068               
    NOP; BIU1.DM (A++,K++) -> M[121];    //        // PC = 0x06A               
    NOP; BIU1.DM (A++,K++) -> M[122];    //        // PC = 0x06C               
    NOP; BIU1.DM (A++,K++) -> M[123];    //        // PC = 0x06E               
    NOP; BIU1.DM (A++,K++) -> M[124];    //        // PC = 0x070               
    NOP; BIU1.DM (A++,K++) -> M[125];    //        // PC = 0x072               
    NOP; BIU1.DM (A++,K++) -> M[126];    //        // PC = 0x074               
    NOP; BIU1.DM (A++,K++) -> M[127];    //        // PC = 0x076               
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
