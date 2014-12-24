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
    NOP;;
    NOP;;
_BIUTest1:
    NOP;;
    BIU1 = M0.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x008
    BIU1 = M1.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x00B
    BIU1 = M2.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x00E
    BIU1 = M3.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x011
    BIU1 = M4.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x014
    BIU1 = M5.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x017
    BIU1 = M6.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x01A
    BIU1 = M7.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x01D
    BIU1 = M8.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x020
    BIU1 = M9.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x023
    BIU1 = M10.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x026
    BIU1 = M11.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x029
    BIU1 = M12.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x02C
    BIU1 = M13.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x02F
    BIU1 = M14.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x032
    BIU1 = M15.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x035
    BIU1 = M16.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x038
    BIU1 = M17.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x03B
    BIU1 = M18.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x03E
    BIU1 = M19.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x041
    BIU1 = M20.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x044
    BIU1 = M21.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x047
    BIU1 = M22.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x04A
    BIU1 = M23.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x04D
    BIU1 = M24.r3;;	NOP;;    DM = BIU1 (A++) (K++);;    	        // PC = 0x050
    BIU1 = M25.r3;;	NOP;;    DM = BIU1 (A++) (K++);;      	        // PC = 0x053
    BIU1 = M26.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x056
    BIU1 = M27.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x059
    BIU1 = M28.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x05C
    BIU1 = M29.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x05F
    BIU1 = M30.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x062
    BIU1 = M31.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x065
    BIU1 = M32.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x068
    BIU1 = M33.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x06B
    BIU1 = M34.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x06E
    BIU1 = M35.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x071
    BIU1 = M36.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x074
    BIU1 = M37.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x077
    BIU1 = M38.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x07A
    BIU1 = M39.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x07D
    BIU1 = M40.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x080
    BIU1 = M41.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x083
    BIU1 = M42.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x086
    BIU1 = M43.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x089
    BIU1 = M44.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x08C
    BIU1 = M45.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x08F
    BIU1 = M46.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x092
    BIU1 = M47.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x095
    BIU1 = M48.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x098
    BIU1 = M49.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x09B
    BIU1 = M50.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x09E
    BIU1 = M51.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0A1
    BIU1 = M52.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0A4
    BIU1 = M53.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0A7
    BIU1 = M54.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0AA
    BIU1 = M55.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0AD
    BIU1 = M56.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0B0
    BIU1 = M57.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0B3
    BIU1 = M58.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0B6
    BIU1 = M59.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0B9
    BIU1 = M60.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0BC
    BIU1 = M61.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0BF
    BIU1 = M62.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0C2
    BIU1 = M63.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0C5
    BIU1 = M64.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0C8
    BIU1 = M65.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0CB
    BIU1 = M66.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0CE
    BIU1 = M67.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0D1
    BIU1 = M68.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0D4
    BIU1 = M69.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0D7
    BIU1 = M70.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0DA
    BIU1 = M71.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0DD
    BIU1 = M72.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0E0
    BIU1 = M73.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0E3
    BIU1 = M74.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0E6
    BIU1 = M75.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0E9
    BIU1 = M76.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0EC
    BIU1 = M77.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0EF
    BIU1 = M78.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0F2
    BIU1 = M79.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0F5
    BIU1 = M80.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0F8
    BIU1 = M81.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0FB
    BIU1 = M82.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x0FE
    BIU1 = M83.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x101
    BIU1 = M84.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x104
    BIU1 = M85.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x107
    BIU1 = M86.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x10A
    BIU1 = M87.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x10D
    BIU1 = M88.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x110
    BIU1 = M89.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x113
    BIU1 = M90.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x116
    BIU1 = M91.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x119
    BIU1 = M92.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x11C
    BIU1 = M93.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x11F
    BIU1 = M94.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x122
    BIU1 = M95.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x125
    BIU1 = M96.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x128
    BIU1 = M97.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x12B
    BIU1 = M98.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x12E
    BIU1 = M99.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x131
    BIU1 = M100.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x134
    BIU1 = M101.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x137
    BIU1 = M102.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x13A
    BIU1 = M103.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x13D
    BIU1 = M104.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x140
    BIU1 = M105.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x143
    BIU1 = M106.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x146
    BIU1 = M107.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x149
    BIU1 = M108.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x14C
    BIU1 = M109.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x14F
    BIU1 = M110.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x152
    BIU1 = M111.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x155
    BIU1 = M112.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x158
    BIU1 = M113.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x15B
    BIU1 = M114.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x15E
    BIU1 = M115.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x161
    BIU1 = M116.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x164
    BIU1 = M117.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x167
    BIU1 = M118.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x16A
    BIU1 = M119.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x16D
    BIU1 = M120.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x170



    M3.w0 = BIU1.KG ;;
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


_BIUTest0:
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x184
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x186               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x188               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x18A               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x18C               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x18E               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x190               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x192               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x194               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x196               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x198               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x19A               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x19C               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x19E               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x1A0               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x1A2               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x1A4               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x1A6               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x1A8               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x1AA               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x1AC               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x1AE               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;    	// PC = 0x1B0          
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1B2               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1B4               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1B6               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1B8               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1BA               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1BC               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1BE               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1C0               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1C2               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1C4               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1C6               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1C8               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1CA               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1CC               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1CE               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1D0               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1D2               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1D4               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1D6               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1D8               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1DA               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1DC               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1DE               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1E0               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1E2               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1E4               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1E6               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1E8               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1EA               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1EC               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1EE               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1F0               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1F2               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1F4               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1F6               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1F8               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1FA               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1FC               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x1FE               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x200               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x202               
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x204
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x206
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x208
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x20A
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x20C
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x20E
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x210
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x212
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x214
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x216
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x218
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x21A
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x21C
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x21E
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x220
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x222
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x224
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x226
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x228
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x22A
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x22C
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x22E
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x230
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x232
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x234
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x236
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x238
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x23A
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x23C
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x23E
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x240
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x242
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x244
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x246
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x248
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x24A
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x24C
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x24E
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x250
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x252
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x254
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x256
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x258
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x25A
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x25C 
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x25E
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x260
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x262
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x264
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x266
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x268
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x26A
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x26C
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x26E
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x270
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x272
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x274
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x276
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x278
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x27A
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x27C
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x27E
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x280
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x282
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x284
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x286
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x288
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x28A
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x28C
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x28E
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x290
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x292
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x294
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x296
    NOP;; IALU.T0 (I0) = BIU1.DM  (A++) (K++);;         // PC = 0x298

    NOP;;
    BIU1.KG = M3.r3   ;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;; 
    BIU1 = M121.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x2A3
    BIU1 = M122.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x2A6
    BIU1 = M123.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x2A9
    BIU1 = M124.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x2AC
    BIU1 = M125.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x2AF
    BIU1 = M126.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x2B2
    BIU1 = M127.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x2B5
    BIU1 = M127.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x2B8
    BIU1 = M127.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x2BB
    BIU1 = M127.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x2BE
    BIU1 = M127.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x2C1
    BIU1 = M127.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x2C4
    BIU1 = M127.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x2C7
    BIU1 = M127.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x2CA
    BIU1 = M127.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x2CD
    BIU1 = M127.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x2D0
    BIU1 = M127.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x2D3
    BIU1 = M127.r3;;	NOP;;    DM = BIU1 (A++) (K++);;   	        // PC = 0x2D6


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
    NOP;; M0.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x004
    NOP;; M1.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x006               
    NOP;; M2.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x008               
    NOP;; M3.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x00A               
    NOP;; M4.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x00C               
    NOP;; M5.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x00E               
    NOP;; M6.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x010               
    NOP;; M7.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x012               
    NOP;; M8.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x014               
    NOP;; M9.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x016               
    NOP;; M10.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x018               
    NOP;; M11.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x01A               
    NOP;; M12.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x01C               
    NOP;; M13.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x01E               
    NOP;; M14.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x020               
    NOP;; M15.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x022               
    NOP;; M16.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x024               
    NOP;; M17.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x026               
    NOP;; M18.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x028               
    NOP;; M19.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x02A               
    NOP;; M20.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x02C               
    NOP;; M21.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x02E               
    NOP;; M22.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x030          
    NOP;; M23.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x032               
    NOP;; M24.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x034               
    NOP;; M25.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x036               
    NOP;; M26.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x038               
    NOP;; M27.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x03A               
    NOP;; M28.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x03C               
    NOP;; M29.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x03E               
    NOP;; M30.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x040               
    NOP;; M31.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x042               
    NOP;; M32.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x044               
    NOP;; M33.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x046               
    NOP;; M34.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x048               
    NOP;; M35.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x04A               
    NOP;; M36.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x04C               
    NOP;; M37.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x04E               
    NOP;; M38.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x050               
    NOP;; M39.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x052               
    NOP;; M40.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x054               
    NOP;; M41.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x056               
    NOP;; M42.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x058               
    NOP;; M43.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x05A               
    NOP;; M44.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x05C               
    NOP;; M45.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x05E               
    NOP;; M46.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x060               
    NOP;; M47.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x062               
    NOP;; M48.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x064               
    NOP;; M49.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x066               
    NOP;; M50.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x068               
    NOP;; M51.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x06A               
    NOP;; M52.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x06C               
    NOP;; M53.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x06E               
    NOP;; M54.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x070               
    NOP;; M55.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x072               
    NOP;; M56.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x074               
    NOP;; M57.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x076               
    NOP;; M58.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x078               
    NOP;; M59.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x07A               
    NOP;; M60.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x004
    NOP;; M61.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x006               
    NOP;; M62.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x008               
    NOP;; M63.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x00A               
    NOP;; M64.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x00C               
    NOP;; M65.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x00E               
    NOP;; M66.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x010               
    NOP;; M67.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x012               
    NOP;; M68.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x014               
    NOP;; M69.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x016               
    NOP;; M70.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x018               
    NOP;; M71.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x01A               
    NOP;; M72.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x01C               
    NOP;; M73.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x01E               
    NOP;; M74.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x020               
    NOP;; M75.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x022               
    NOP;; M76.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x024               
    NOP;; M77.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x026               
    NOP;; M78.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x028               
    NOP;; M79.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x02A               
    NOP;; M80.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x02C               
    NOP;; M81.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x02E               
    NOP;; M82.w0 = BIU1.DM  (A++) (K++);;    	// PC = 0x030          
    NOP;; M83.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x032               
    NOP;; M84.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x034               
    NOP;; M85.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x036               
    NOP;; M86.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x038               
    NOP;; M87.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x03A               
    NOP;; M88.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x03C               
    NOP;; M89.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x03E               
    NOP;; M90.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x040               
    NOP;; M91.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x042               
    NOP;; M92.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x044               
    NOP;; M93.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x046               
    NOP;; M94.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x048               
    NOP;; M95.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x04A               
    NOP;; M96.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x04C               
    NOP;; M97.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x04E               
    NOP;; M98.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x050               
    NOP;; M99.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x052  
    NOP;; M100.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x068 
    NOP;; M101.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x056               
    NOP;; M102.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x058               
    NOP;; M103.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x05A               
    NOP;; M104.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x05C               
    NOP;; M105.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x05E               
    NOP;; M106.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x060               
    NOP;; M107.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x062               
    NOP;; M108.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x064               
    NOP;; M109.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x066                          
    NOP;; M110.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x054               
    NOP;; M111.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x056               
    NOP;; M112.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x058               
    NOP;; M113.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x05A               
    NOP;; M114.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x05C               
    NOP;; M115.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x05E               
    NOP;; M116.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x060               
    NOP;; M117.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x062               
    NOP;; M118.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x064               
    NOP;; M119.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x066               
    NOP;; M120.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x068               
    NOP;; M121.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x06A               
    NOP;; M122.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x06C               
    NOP;; M123.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x06E               
    NOP;; M124.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x070               
    NOP;; M125.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x072               
    NOP;; M126.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x074               
    NOP;; M127.w0 = BIU1.DM  (A++) (K++);;    //        // PC = 0x076               
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
