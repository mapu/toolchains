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
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    	// PC = 0x004
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    	// PC = 0x006               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    	// PC = 0x008               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    	// PC = 0x00A               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    	// PC = 0x00C               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    	// PC = 0x00E               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    	// PC = 0x010               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    	// PC = 0x012               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    	// PC = 0x014               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    	// PC = 0x016               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    	// PC = 0x018               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    	// PC = 0x01A               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    	// PC = 0x01C               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    	// PC = 0x01E               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    	// PC = 0x020               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    	// PC = 0x022               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    	// PC = 0x024               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    	// PC = 0x026               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    	// PC = 0x028               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    	// PC = 0x02A               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    	// PC = 0x02C               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    	// PC = 0x02E               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    	// PC = 0x030          
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x032               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x034               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x036               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x038               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x03A               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x03C               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x03E               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x040               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x042               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x044               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x046               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x048               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x04A               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x04C               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x04E               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x050               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x052               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x054               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x056               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x058               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x05A               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x05C               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x05E               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x060               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x062               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x064               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x066               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x068               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x06A               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x06C               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x06E               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x070               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x072               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x074               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x076               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x078               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x07A               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x07C               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x07E               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x080               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x082               
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x084
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x086
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x088
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x08A
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x08C
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x08E
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x090
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x092
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x094
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x096
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x098
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x09A
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x09C
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x09E
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0A0
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0A2
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0A4
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0A6
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0A8
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0AA
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0AC
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0AE
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0B0
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0B2
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0B4
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0B6
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0B8
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0BA
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0BC
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0BE
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0C0
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0C2
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0C4
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0C6
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0C8
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0CA
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0CC
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0CE
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0D0
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0D2
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0D4
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0D6
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0D8
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0DA
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0DC 
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0DE
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0E0
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0E2
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0E4
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0E6
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0E8
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0EA
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0EC
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0EE
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0F0
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0F2
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0F4
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0F6
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0F8
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0FA
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0FC
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x0FE
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x100
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x102
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x104
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x106
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x108
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x10A
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x10C
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x10E
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x110
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //        // PC = 0x112



    M3.w0 = BIU2.KG ;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    BIU2.KG = M7.r3;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;



_BIUTest1:
    NOP;;
    BIU2 = M0.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x128
    BIU2 = M1.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x12B
    BIU2 = M2.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x12E
    BIU2 = M3.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x131
    BIU2 = M4.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x134
    BIU2 = M5.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x137
    BIU2 = M6.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x13A
    BIU2 = M7.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x13D
    BIU2 = M8.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x140
    BIU2 = M9.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x143
    BIU2 = M10.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x146
    BIU2 = M11.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x149
    BIU2 = M12.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x14C
    BIU2 = M13.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x14F
    BIU2 = M14.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x152
    BIU2 = M15.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x155
    BIU2 = M16.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x158
    BIU2 = M17.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	       // PC = 0x15B
    BIU2 = M18.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	       // PC = 0x15E
    BIU2 = M19.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	       // PC = 0x161
    BIU2 = M20.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	       // PC = 0x164
    BIU2 = M21.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	       // PC = 0x167
    BIU2 = M22.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	       // PC = 0x16A
    BIU2 = M23.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	       // PC = 0x16D
    BIU2 = M24.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	      // PC = 0x170
    BIU2 = M25.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	      // PC = 0x173
    BIU2 = M26.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	       // PC = 0x176
    BIU2 = M27.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	       // PC = 0x179
    BIU2 = M28.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	       // PC = 0x17C
    BIU2 = M29.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	       // PC = 0x17F
    BIU2 = M30.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	      // PC = 0x182
    BIU2 = M31.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x185
    BIU2 = M32.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x188
    BIU2 = M33.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x18B
    BIU2 = M34.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x18E
    BIU2 = M35.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x191
    BIU2 = M36.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x194
    BIU2 = M37.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x197
    BIU2 = M38.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x19A
    BIU2 = M39.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x19D
    BIU2 = M40.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1A0
    BIU2 = M41.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1A3
    BIU2 = M42.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1A6
    BIU2 = M43.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1A9
    BIU2 = M44.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1AC
    BIU2 = M45.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1AF
    BIU2 = M46.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1B2
    BIU2 = M47.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1B5
    BIU2 = M48.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1B8
    BIU2 = M49.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1BB
    BIU2 = M50.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1BE
    BIU2 = M51.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1C1
    BIU2 = M52.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1C4
    BIU2 = M53.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1C7
    BIU2 = M54.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1CA
    BIU2 = M55.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1CD
    BIU2 = M56.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1D0
    BIU2 = M57.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1D3
    BIU2 = M58.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1D6
    BIU2 = M59.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1D9
    BIU2 = M60.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1DC
    BIU2 = M61.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1DF
    BIU2 = M62.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1E2
    BIU2 = M63.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1E5
    BIU2 = M64.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1E8
    BIU2 = M65.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1EB
    BIU2 = M66.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1EE
    BIU2 = M67.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1F1
    BIU2 = M68.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1F4
    BIU2 = M69.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1F7
    BIU2 = M70.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1FA
    BIU2 = M71.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x1FD
    BIU2 = M72.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x200
    BIU2 = M73.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x203
    BIU2 = M74.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x206
    BIU2 = M75.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x209
    BIU2 = M76.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x20C
    BIU2 = M77.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x20F
    BIU2 = M78.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x212
    BIU2 = M79.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x215
    BIU2 = M80.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x218
    BIU2 = M81.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	      // PC = 0x21B
    BIU2 = M82.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x21E
    BIU2 = M83.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x221
    BIU2 = M84.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x224
    BIU2 = M85.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x227
    BIU2 = M86.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x22A
    BIU2 = M87.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x22D
    BIU2 = M88.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x230
    BIU2 = M89.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x233
    BIU2 = M90.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x236
    BIU2 = M91.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x239
    BIU2 = M92.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x23C
    BIU2 = M93.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x23F
    BIU2 = M94.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x242
    BIU2 = M95.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x245
    BIU2 = M96.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x248
    BIU2 = M97.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x24B
    BIU2 = M98.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x24E
    BIU2 = M99.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x251
    BIU2 = M100.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x254
    BIU2 = M101.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x257
    BIU2 = M102.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x25A
    BIU2 = M103.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x25D
    BIU2 = M104.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x260
    BIU2 = M105.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x263
    BIU2 = M106.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x266
    BIU2 = M107.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x269
    BIU2 = M108.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x26C
    BIU2 = M109.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x26F
    BIU2 = M110.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x272
    BIU2 = M111.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x275
    BIU2 = M112.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x278
    BIU2 = M113.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x27B
    BIU2 = M114.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x27E
    BIU2 = M115.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x281
    BIU2 = M116.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x284
    BIU2 = M117.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x287
    BIU2 = M118.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x28A
    BIU2 = M119.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x28D
    BIU2 = M120.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x290
    BIU2 = M121.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x293
    BIU2 = M122.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x296
    BIU2 = M123.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x299
    BIU2 = M124.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x29C
    BIU2 = M125.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x29F
    BIU2 = M126.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x2A2
    BIU2 = M127.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x2A5
    BIU2 = M127.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x2A8
    BIU2 = M127.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x2AB
    BIU2 = M127.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x2AE
    BIU2 = M127.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x2B1
    BIU2 = M127.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x2B4
    BIU2 = M127.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x2B7
    BIU2 = M127.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x2BA
    BIU2 = M127.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x2BD
    BIU2 = M127.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x2C0
    BIU2 = M127.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x2C3
    BIU2 = M127.r3;;	NOP;;    DM = BIU2 (A++) (K++);;   	        // PC = 0x2C6
    


    NOP;;
    BIU2.KG = M3.r3   ;;
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



    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //2D4        // PC = 0x114
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //2D6        // PC = 0x116
    NOP;; IALU.T0 (I0) = BIU2.DM  (A++) (K++);;    //2D8        // PC = 0x118
    
    

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
    NOP;; M0.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x004
    NOP;; M1.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x006               
    NOP;; M2.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x008               
    NOP;; M3.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x00A               
    NOP;; M4.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x00C               
    NOP;; M5.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x00E               
    NOP;; M6.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x010               
    NOP;; M7.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x012               
    NOP;; M8.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x014               
    NOP;; M9.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x016               
    NOP;; M10.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x018               
    NOP;; M11.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x01A               
    NOP;; M12.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x01C               
    NOP;; M13.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x01E               
    NOP;; M14.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x020               
    NOP;; M15.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x022               
    NOP;; M16.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x024               
    NOP;; M17.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x026               
    NOP;; M18.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x028               
    NOP;; M19.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x02A               
    NOP;; M20.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x02C               
    NOP;; M21.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x02E               
    NOP;; M22.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x030          
    NOP;; M23.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x032               
    NOP;; M24.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x034               
    NOP;; M25.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x036               
    NOP;; M26.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x038               
    NOP;; M27.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x03A               
    NOP;; M28.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x03C               
    NOP;; M29.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x03E               
    NOP;; M30.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x040               
    NOP;; M31.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x042               
    NOP;; M32.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x044               
    NOP;; M33.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x046               
    NOP;; M34.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x048               
    NOP;; M35.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x04A               
    NOP;; M36.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x04C               
    NOP;; M37.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x04E               
    NOP;; M38.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x050               
    NOP;; M39.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x052               
    NOP;; M40.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x054               
    NOP;; M41.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x056               
    NOP;; M42.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x058               
    NOP;; M43.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x05A               
    NOP;; M44.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x05C               
    NOP;; M45.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x05E               
    NOP;; M46.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x060               
    NOP;; M47.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x062               
    NOP;; M48.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x064               
    NOP;; M49.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x066               
    NOP;; M50.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x068               
    NOP;; M51.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x06A               
    NOP;; M52.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x06C               
    NOP;; M53.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x06E               
    NOP;; M54.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x070               
    NOP;; M55.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x072               
    NOP;; M56.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x074               
    NOP;; M57.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x076               
    NOP;; M58.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x078               
    NOP;; M59.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x07A               
    NOP;; M60.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x004
    NOP;; M61.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x006               
    NOP;; M62.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x008               
    NOP;; M63.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x00A               
    NOP;; M64.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x00C               
    NOP;; M65.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x00E               
    NOP;; M66.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x010               
    NOP;; M67.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x012               
    NOP;; M68.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x014               
    NOP;; M69.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x016               
    NOP;; M70.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x018               
    NOP;; M71.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x01A               
    NOP;; M72.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x01C               
    NOP;; M73.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x01E               
    NOP;; M74.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x020               
    NOP;; M75.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x022               
    NOP;; M76.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x024               
    NOP;; M77.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x026               
    NOP;; M78.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x028               
    NOP;; M79.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x02A               
    NOP;; M80.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x02C               
    NOP;; M81.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x02E               
    NOP;; M82.w0 = BIU2.DM  (A++) (K++);;    	// PC = 0x030          
    NOP;; M83.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x032               
    NOP;; M84.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x034               
    NOP;; M85.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x036               
    NOP;; M86.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x038               
    NOP;; M87.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x03A               
    NOP;; M88.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x03C               
    NOP;; M89.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x03E               
    NOP;; M90.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x040               
    NOP;; M91.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x042               
    NOP;; M92.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x044               
    NOP;; M93.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x046               
    NOP;; M94.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x048               
    NOP;; M95.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x04A               
    NOP;; M96.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x04C               
    NOP;; M97.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x04E               
    NOP;; M98.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x050               
    NOP;; M99.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x052  
    NOP;; M100.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x068 
    NOP;; M101.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x056               
    NOP;; M102.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x058               
    NOP;; M103.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x05A               
    NOP;; M104.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x05C               
    NOP;; M105.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x05E               
    NOP;; M106.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x060               
    NOP;; M107.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x062               
    NOP;; M108.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x064               
    NOP;; M109.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x066                          
    NOP;; M110.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x054               
    NOP;; M111.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x056               
    NOP;; M112.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x058               
    NOP;; M113.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x05A               
    NOP;; M114.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x05C               
    NOP;; M115.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x05E               
    NOP;; M116.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x060               
    NOP;; M117.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x062               
    NOP;; M118.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x064               
    NOP;; M119.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x066               
    NOP;; M120.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x068               
    NOP;; M121.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x06A               
    NOP;; M122.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x06C               
    NOP;; M123.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x06E               
    NOP;; M124.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x070               
    NOP;; M125.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x072               
    NOP;; M126.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x074               
    NOP;; M127.w0 = BIU2.DM  (A++) (K++);;    //        // PC = 0x076               
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    M7.w3 = SHU0.T0 IND TSQ ;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    MPU.Stop;;
