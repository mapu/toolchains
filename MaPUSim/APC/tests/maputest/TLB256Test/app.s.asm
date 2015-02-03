//  Date: 2013-05-24
//  Name: app.s.asm
//  Author: tao.wang@ia.ac.cn
//  The configuration Unit for MPU for Bicubic Interpolation

// begin program code */
    .text
    .global _start

//*************************************************/ 
_start:

// parameter definition
////////////////////////////////////////////////////////////////////
// the following need not to modify    
    SDA0DM0_START = 0x400000
    SDA1DM0_START = 0x800000 
    SDA2DM0_START = 0xc00000  
    SDA2DM1_START = 0xe00000  
    SDASize       = 0x40000  
///////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// the following need to modify
    BLen          = 1 << 12 
    BS            = 64               // Data type
    DataSize      = 1                //  Byte 1, Short 2, Word 4, Long 8
    DataSizeGran  = 0         
///////////////////////////////////////////////////////////////////////  
     
    BankNum       = BS  
    BlockSize     = 64                        // the total Rd size, for Row Rd is 512/8  
    BlockGran     = 6                         // log2(BlockSize)    
     
    BBSTailNum    = BLen % BS 
    BBSTailEn     = BBSTailNum !=0 
    BBSNum        = BLen / BS + BBSTailEn

    MaskTailNum   = BBSTailNum 

    .if MaskTailNum  < 8
        ShiftNum   = MaskTailNum * 4
        MaskLTemp  = 0xffffffff << ShiftNum 
        MaskL      = ~ MaskLTemp
        MaskH      = 0
    .else 
        ShiftNum   = MaskTailNum *4 - 32
        MaskHTemp  = 0xffffffff << ShiftNum
        MaskL      = 0xffffffff
        MaskH      = ~ MaskHTemp
    .endif
    
    
    ACirNum       = 1                         // the number of circle
    BCirNum       = 1
    CCirNum       = 1

    // for MC Config   
    MIndexStart   = 0x0
    MStepMode     = 0x1 
    MLen          = 0x4
    ISize         = 0x4

    MIndexOffset  = 0
    MStepOffset   = 8
    MLenOffset    = 24
    ISizeOffset   = 12

    MCIndexStart  = MIndexStart << MIndexOffset 
    MCStepMode    = MStepMode   << MStepOffset
    MCLen         = MLen        << MLenOffset
    ICSize        = ISize       << ISizeOffset
    MConfig       = MCIndexStart | MCStepMode | MCLen | ICSize
/////////////////////////////////////////////////////////////////////////////////////////////////
      

    
 m.s     NOP ;;
 m.s     NOP ;;
 m.s     NOP ;;
 m.s     NOP ;;
   
    // MC
 m.s     R2 = MConfig        ;;
 m.s     MC.r0 = R2          ;;
 m.s     MC.w0 = R2          ;;
    

    //////////////////////////////////////////////////////////////////////
    // BIU Configure   
    // for BIU0    
 m.s     R1 =  SDA0DM0_START          ;; // KB for BIU0  
 m.s     R2 =  BlockSize              ;; // KS0
 m.s     R3 =  4                      ;; // KC0 
 m.s     R4 =  4                      ;; // KI0  
   
 m.s     R14=  BlockGran              ;; // KG0
 m.s     R15=  ACirNum                ;; // KL0 
  
    
 m.s     KB0 = R1 ;;
 m.s     KS0 = R2 ;;
 //   // m.s KC0 = R3 ;;
 m.s     KI0 = R4 ;;   
    
 m.s     KG0 = R14;;
 m.s     KL0 = R15;; 

    // for BIU1   
 m.s     R1 =  SDA1DM0_START          ;; // KB for BIU0  
 m.s     R2 =  BlockSize              ;; // KS0
 m.s     R3 =  BBSNum                 ;; // KC0 
 m.s     R4 =  BBSNum                 ;; // KI0  
   
 m.s     R14=  BlockGran              ;; // KG0
 m.s     R15=  BCirNum                ;; // KL0 
  
    
 m.s     KB4 = R1 ;;
 m.s     KS4 = R2 ;;
  // m.s KC4 = R3 ;;
 m.s     KI4 = R4 ;;   
    
 m.s     KG1 = R14;;
 m.s     KL1 = R15;; 
    
    // for BIU1   
 m.s     R1 =  SDA2DM0_START          ;; // KB for BIU0  
 m.s     R2 =  BlockSize              ;; // KS0
 m.s     R3 =  BBSNum                 ;; // KC0 
 m.s     R4 =  BBSNum                 ;; // KI0  
   
 m.s     R14=  BlockGran              ;; // KG0
 m.s     R15=  CCirNum                ;; // KL0 
  
    
 m.s     KB8 = R1 ;;
 m.s     KS8 = R2 ;;
  // m.s KC8 = R3 ;;
 m.s     KI8 = R4 ;;   
    
 m.s     KG2 = R14;;
 m.s     KL2 = R15;; 

    //LOOP 
 m.s     R0   = BBSNum  ;;
 m.s     NOP ;;
 m.s     NOP ;;
 m.s     KI12 = R0      ;;

    // End BIU Configrure   
    //////////////////////////////////////////////////////////////////

 m.s     NOP  ;;
 m.s     NOP  ;;
 m.s     NOP  ;;
 m.s     CallM GammarTest(B);;

 m.s     NOP ;;
 m.s     NOP ;;
 m.s     SPU.Stop ;;

   ///////////////////////////////////////////////////////////////////////////////
   //Data Segment
   //SDA0DM0: Table Value 256 Bytes
   .section .SDA0.DM0, "aw"
.int  0x63098124
.int  0x12658d0d
.int  0x3d760d01
.int  0xc6f98ced
.int  0x77e5aac5
.int  0xcef28f12
.int  0xbd5cc5e8
.int  0x0a63652d
.int  0x9daa2080
.int  0x530d1396
.int  0xae02d56b
.int  0x0a23cf1d
.int  0x8af23cca
.int  0x8978d841
.int  0xaec6b6eb
.int  0x710b2abc
.int  0x3a3b4f85
.int  0xd9f1157e
.int  0x8f9f4c62
.int  0x5c9fb7f8
.int  0xd049895b
.int  0x0c9651d7
.int  0x3d77c8c2
.int  0x396d7e12
.int  0x7885d31f
.int  0x2a3f495b
.int  0x9c8e8658
.int  0xa37326fa
.int  0x445fb32f
.int  0x5ae6cbf7
.int  0x65daed29
.int  0x4479dfb5
.int  0x0eab2ad0
.int  0xc3fd9adc
.int  0x0a674e56
.int  0xb87938b6
.int  0x59049394
.int  0x6dd94ddb
.int  0x95b6ca76
.int  0x69f70446
.int  0x2d2888b4
.int  0x1c082ec7
.int  0x861c29fd
.int  0x70663dda
.int  0xfa5eba73
.int  0x37b91ad5
.int  0xb626c096
.int  0x7886dc7d
.int  0x79cfdb7e
.int  0xa11761fa
.int  0x35f55086
.int  0x98c5c129
.int  0x8aec734b
.int  0xa1a9a84e
.int  0x2a9fe60e
.int  0x389e8d2a
.int  0x13cac879
.int  0xbab6c76b
.int  0xb492b9c4
.int  0xf2fa867f
.int  0xe484bd32
.int  0x8ea1a9ca
.int  0xc9ef0bfb
.int  0x6b8f7536



//SDA1DM0: Table Index, 4096 Bytes
    .section .SDA1.DM0, "aw"
.int 0x929bae88
.int 0xc24b2d28
.int 0x18ec0d1e
.int 0x3b4186d1
.int 0x5b5653d8
.int 0xd87304e2
.int 0xe539b812
.int 0xc8812ba1
.int 0x041fa127
.int 0x12149658
.int 0xed55b196
.int 0x27adf52b
.int 0xb9e7a784
.int 0x51c9db49
.int 0x45fa2aa1
.int 0xfe727c83
.int 0xf0866f68
.int 0xf6284038
.int 0x3974c0c5
.int 0x622a3cb0
.int 0x4317e115
.int 0xc12586c9
.int 0x2c075a8d
.int 0xb63b710c
.int 0x555c9ef7
.int 0xb472a020
.int 0x794b0ddd
.int 0x8f7bfd9e
.int 0xb11de303
.int 0xede09544
.int 0x528df852
.int 0x908c4684
.int 0xaa846a17
.int 0x8bba607c
.int 0xa23225a6
.int 0x4b3d1449
.int 0x3986475c
.int 0x8c2cd0b4
.int 0xe8116a07
.int 0x23ec734b
.int 0xdf39c9a4
.int 0xb09d67d4
.int 0x20d94982
.int 0x2ae3931c
.int 0x843a5dc8
.int 0x5212392b
.int 0x976ed059
.int 0x00bba6db
.int 0x590c690f
.int 0x5190cfc5
.int 0xd84a7769
.int 0xb7b8c49b
.int 0xb5773086
.int 0xe97607d4
.int 0x80db018b
.int 0xaf30c5c2
.int 0xf8e9af66
.int 0x95e1ef06
.int 0x28e560ee
.int 0x77e12964
.int 0x68d5816e
.int 0xf10faaf0
.int 0x3ce28e7d
.int 0xaf5befa9
.int 0x1313b94a
.int 0x99b75165
.int 0xe462713a
.int 0xe5597d2a
.int 0xaf41c9ed
.int 0x0670cddb
.int 0xdc6abda2
.int 0x78257b50
.int 0x79235cf6
.int 0x730ebbc8
.int 0x59d4ce0c
.int 0x75f4b3b9
.int 0x1502390c
.int 0xb8024e05
.int 0xe8af8d08
.int 0x7b4cf590
.int 0x700d2540
.int 0x30bc292d
.int 0x4da150c0
.int 0x945df5f5
.int 0x9eb8bc6a
.int 0x930746fa
.int 0x6345b755
.int 0x1e682659
.int 0xb91c5a90
.int 0xd86fd0ab
.int 0xfc8b8d45
.int 0xa519a4dc
.int 0xc458f33e
.int 0x189a065c
.int 0xc31aaac5
.int 0xc3e90cb2
.int 0x89f2f8eb
.int 0x68ad1b6f
.int 0x1babcf25
.int 0x04825d80
.int 0x79323657
.int 0xc0aa16e4
.int 0xfb718c2e
.int 0xc117dcbc
.int 0x8f89b5eb
.int 0xd6d3b0bf
.int 0xd7711a82
.int 0x05cdf10e
.int 0x2aec981c
.int 0xfe033765
.int 0x74e83efd
.int 0x681206b0
.int 0xf336fec5
.int 0x0f176bfb
.int 0xd4a62a14
.int 0xf4046bcb
.int 0xb985e18d
.int 0x3c4660c9
.int 0x0e735837
.int 0x00d030dc
.int 0x53a1f843
.int 0xfa875f98
.int 0x316b702d
.int 0x150d0c12
.int 0x80549440
.int 0xdcedc313
.int 0x235f2778
.int 0xc92238ab
.int 0xc7ce82c7
.int 0x7508db24
.int 0x82381ab5
.int 0xa4a35058
.int 0x5406ea65
.int 0x3748c85b
.int 0x8351b5f8
.int 0xfbb0359b
.int 0xdd18304f
.int 0x55596146
.int 0xd3de0b2f
.int 0xd558e928
.int 0x7ea4d4c3
.int 0xa0830fd6
.int 0x7745bf82
.int 0xc6f87545
.int 0x9c31506c
.int 0x690eafbc
.int 0xaa85deb2
.int 0xd39ac228
.int 0xc28bfbba
.int 0x619e71cc
.int 0xc091bf7b
.int 0x51547e59
.int 0x8324e749
.int 0x7795b95b
.int 0x04afe6f1
.int 0xc4abfc12
.int 0x5a73c23f
.int 0xbb74fea0
.int 0x98dfe2e2
.int 0xbdf2109f
.int 0x8954b4ab
.int 0x3b35ac13
.int 0x55342f35
.int 0xc1a7f2bc
.int 0xfa6941e4
.int 0xe8b5ffcd
.int 0xae37124b
.int 0x22da20f8
.int 0x1e7a2aec
.int 0x6e0bebe9
.int 0x8b457c5e
.int 0xe1833712
.int 0xcdd54ace
.int 0x1aecf4e4
.int 0x2df3ece3
.int 0x8fecdd5c
.int 0x27ab78ff
.int 0xbb0c13fb
.int 0x0a6a49e6
.int 0x2e099618
.int 0x757666bd
.int 0x7c7b958f
.int 0xb4c9c96f
.int 0x0ddfadda
.int 0x04518370
.int 0xca06f980
.int 0xb084c9df
.int 0xacde10e9
.int 0x0d61d709
.int 0x2f9ad391
.int 0x5dc6b74e
.int 0x9b442f9c
.int 0x8d2ee962
.int 0x37a0aac6
.int 0xd1cde8f0
.int 0x587ddfbb
.int 0x2906979f
.int 0x4d5dd7c4
.int 0xb94e7f73
.int 0x318139e0
.int 0x0f5dfe8f
.int 0x9a566933
.int 0x3ad3495c
.int 0x253978d8
.int 0xd32c5609
.int 0xe8ac001a
.int 0xd11da06c
.int 0xc0d8da00
.int 0x5151d14c
.int 0x807eb345
.int 0x318148fc
.int 0x003f83a7
.int 0x8ef17e64
.int 0x0a49e89e
.int 0x2549515f
.int 0x2f5e586c
.int 0x94d42783
.int 0x6f463c92
.int 0x7216f741
.int 0xcb92e3a2
.int 0x55cb3168
.int 0xb5cd5e71
.int 0xb03f8208
.int 0x5e0532ac
.int 0x8d0a7db8
.int 0xe5b4f1c9
.int 0x312d2334
.int 0x640eb45e
.int 0x9b8a4864
.int 0xe3ab6bcd
.int 0xe0c2d9bd
.int 0x26931d5f
.int 0x6b6c0905
.int 0x57a5f7b0
.int 0x1db44f76
.int 0x9e664687
.int 0x4c54543f
.int 0x862d5006
.int 0xa0efb86a
.int 0x55a8093f
.int 0xd6ceb81e
.int 0x35cfa925
.int 0x5af5b640
.int 0x442725e1
.int 0xc9a89f79
.int 0x88738585
.int 0x513d3496
.int 0xb4bd1c1b
.int 0xec592f4e
.int 0xe3c09d1e
.int 0x84914b64
.int 0x1cb31349
.int 0xee5d2a28
.int 0xca78bbcc
.int 0xcb761e5d
.int 0x0420b4db
.int 0x4592cd79
.int 0xc0dd9ea9
.int 0xb281bb4b
.int 0xe1480bb3
.int 0x0ea0affc
.int 0x270dd975
.int 0x5fbffc74
.int 0xfbfa6881
.int 0xef53a619
.int 0x2164b1ed
.int 0x6f1468f6
.int 0x46f73e2a
.int 0xf4faa03d
.int 0x70dc0033
.int 0xd7960f29
.int 0x4a4a0623
.int 0x71cd8841
.int 0x556c1328
.int 0xc1f46ffe
.int 0xf9ad18d3
.int 0xe34825c8
.int 0x70678ad0
.int 0x72cf4e60
.int 0x98e894f2
.int 0xcb8df89e
.int 0xa1e038af
.int 0x2521a194
.int 0x9c2e3beb
.int 0x95c22016
.int 0xe807052d
.int 0xeb976898
.int 0x86835237
.int 0x72743471
.int 0x4488ccf2
.int 0x8ff48c82
.int 0xcb1581c5
.int 0x3df52a36
.int 0x93ee3b46
.int 0x7272c4e4
.int 0xaca3b551
.int 0x57be3197
.int 0x7300b476
.int 0x6d1e7229
.int 0x1bd7fdcd
.int 0x87ca96ce
.int 0x22252c2d
.int 0xb03d83f3
.int 0x9aa86cb9
.int 0xdffc9d82
.int 0x5cc00cd9
.int 0xa3a84f81
.int 0x18bdf988
.int 0x967d6b1f
.int 0x3f8f110b
.int 0xc52d7b0a
.int 0xd7ca5dff
.int 0xeb1add3d
.int 0x261329d2
.int 0x8ef2d61d
.int 0x45eef792
.int 0x04a76dfe
.int 0x8de65922
.int 0x4ccf3b05
.int 0xe318a7cb
.int 0xd05b0f6c
.int 0xee20972f
.int 0x04b47225
.int 0x1f63b3a0
.int 0xe43514dc
.int 0xa4b9aece
.int 0x54060d5a
.int 0x377aa1dc
.int 0x5e59074b
.int 0xcae51bd6
.int 0x5100485b
.int 0x19f11d7c
.int 0xc26f91bf
.int 0x1875f07f
.int 0x6711ee17
.int 0x42afd80d
.int 0xdc0654ab
.int 0xc837b1c5
.int 0x3b3e3217
.int 0x9f44620d
.int 0xa0f2cf48
.int 0x715937c1
.int 0x868396df
.int 0x6c50134c
.int 0x08ae3bf9
.int 0xedc48d12
.int 0xfd2fc44b
.int 0x15dfe34c
.int 0xf9b76eac
.int 0x3380d336
.int 0x2a4975f1
.int 0x15cb50a2
.int 0x1af3b937
.int 0x39121cec
.int 0x4eeb4a80
.int 0xd81d3b50
.int 0xc00bc8bb
.int 0xd6dc4e97
.int 0x43a8b222
.int 0x966ac72e
.int 0xc1d08401
.int 0xa17e0299
.int 0x59e4c6d8
.int 0x3a343c44
.int 0x6095efdc
.int 0xce3667b7
.int 0x3a3c2cde
.int 0x3e4be52a
.int 0x2397098a
.int 0x123e1c77
.int 0xc5e2f50a
.int 0x892df16c
.int 0xbb54ab0f
.int 0x832f1780
.int 0xf1fb7c2c
.int 0x546551e6
.int 0xe0f36d9c
.int 0x73607ffc
.int 0xae05496a
.int 0x07ed9b25
.int 0x2e9f7feb
.int 0x57438ff5
.int 0xc131f0a7
.int 0x1a8ae9c9
.int 0x0de88f9d
.int 0xa2c976b7
.int 0xa8c7eedf
.int 0xf230b6e2
.int 0x8b24a49e
.int 0xa6c1362f
.int 0x79838f86
.int 0xc660c453
.int 0x3ccf0385
.int 0x7437864c
.int 0xc1f7c8eb
.int 0x7c89f530
.int 0x1cea0bf1
.int 0xc0d5ae60
.int 0x64ec1a96
.int 0x8967301e
.int 0x715b16e9
.int 0xb8416c7c
.int 0x7ec072a8
.int 0xd26440b3
.int 0xa54f5d41
.int 0xef7fcfd3
.int 0x42c3083b
.int 0x8bf7b3d8
.int 0x2c97d0f4
.int 0x34521b1c
.int 0xdba040c6
.int 0x1804bb27
.int 0x610bfc87
.int 0x7699a8eb
.int 0x4cb48874
.int 0xeb3f0d4b
.int 0x65e6ec69
.int 0xa4b2c119
.int 0x4e723c8e
.int 0x6986c161
.int 0x022b0252
.int 0x8ed68420
.int 0x33c382d4
.int 0xc3483357
.int 0x8a02e0a4
.int 0xc07dc27b
.int 0xc4652be2
.int 0xdadde34c
.int 0xb41740c9
.int 0x69bee106
.int 0x204d948a
.int 0x20f054f3
.int 0x81feda96
.int 0x36aaba5f
.int 0xa103873b
.int 0x76d5eded
.int 0x198e3faa
.int 0x32b37947
.int 0xb507175b
.int 0x0cf0d730
.int 0xab45bfcc
.int 0xaa077efc
.int 0xc22c85aa
.int 0x4211e060
.int 0x4da6414e
.int 0x0af03ca4
.int 0x09fd1aa3
.int 0x87d75936
.int 0xdd992c69
.int 0xbc381aa0
.int 0x7d190fd9
.int 0x3a0df6d9
.int 0xe2cd40a3
.int 0x0b8552f8
.int 0xad946558
.int 0x880db145
.int 0x68157828
.int 0x01bcd2b1
.int 0x9e5ac908
.int 0x501da4a5
.int 0xe3cbeb30
.int 0x475d1c35
.int 0xac8a7607
.int 0xffd2eeb6
.int 0x102cae5c
.int 0x07e824ad
.int 0x6ad80f10
.int 0x726c9227
.int 0xd3beaf16
.int 0xc8393266
.int 0x85f185d6
.int 0xd53d56f8
.int 0x17a996f1
.int 0x54eac32b
.int 0x9fd5f464
.int 0x83ffa4db
.int 0xae1caa0a
.int 0x90b265ac
.int 0x32388280
.int 0xe83e6894
.int 0x32adac68
.int 0x66c89cd8
.int 0x70e8430b
.int 0x5fb0f357
.int 0xe6b6b85c
.int 0x993cbd41
.int 0x2323155d
.int 0x129cda2a
.int 0x91b526df
.int 0xb25fd37b
.int 0x8c0280a8
.int 0xdb34d346
.int 0x65b069f4
.int 0xd51e7a79
.int 0x42cf9b58
.int 0x22f89a72
.int 0xda92f007
.int 0xa865c5f2
.int 0x166ce673
.int 0xc116cd6c
.int 0x908a0750
.int 0x2e7b16bd
.int 0xefaf2ef8
.int 0xe0c9ce3d
.int 0x2d6fb22d
.int 0xac670633
.int 0xadc6693e
.int 0xd6baa4a5
.int 0x401b9388
.int 0x854d408f
.int 0x04929c58
.int 0x0c5fd772
.int 0x20cfc733
.int 0x1ccd634c
.int 0x640ab472
.int 0xd65c53e6
.int 0xc991c74a
.int 0xb565e189
.int 0xd19b6d2f
.int 0x5de14912
.int 0xc28c4694
.int 0x50d2e064
.int 0xc9882b0f
.int 0x750e0b3b
.int 0xf67214df
.int 0x8f6d195e
.int 0x1f48e0ca
.int 0x844ff335
.int 0x9adafc64
.int 0x948db9b9
.int 0xe0e9e9b0
.int 0x5fbe39c8
.int 0x0a9fa425
.int 0xdee641d8
.int 0x306ded0d
.int 0x2a81db0b
.int 0xa8436e4d
.int 0x2eeb63a0
.int 0x282dc185
.int 0x382e7cc9
.int 0xbf4d5893
.int 0x632be9d9
.int 0xbb364376
.int 0xec2741dc
.int 0x62a9da94
.int 0x5db38b89
.int 0x94ad3f58
.int 0x9bfbb292
.int 0x490ac939
.int 0xd96dd95b
.int 0xf3294a1b
.int 0x65c89a09
.int 0xb7483513
.int 0x568215f9
.int 0x9119a68a
.int 0x1c98ea90
.int 0x53bf49be
.int 0xf1b61370
.int 0x7f2221a8
.int 0x23c39509
.int 0x0db22d3d
.int 0x47cd1d8e
.int 0x057cf213
.int 0x49507147
.int 0xfea8e260
.int 0x55e7ce43
.int 0x82559f62
.int 0xae51196d
.int 0x45d1271e
.int 0x57e2f7ff
.int 0x4c34d785
.int 0xa348c953
.int 0xe85e54f8
.int 0xa8ba7b16
.int 0x93515427
.int 0x878243de
.int 0xb5e94059
.int 0xa2e7311a
.int 0x3cfbbf0a
.int 0xa87bab67
.int 0x21cf1af4
.int 0x82c1c342
.int 0xc4f8886c
.int 0x253c6a1a
.int 0x2b934f28
.int 0x40da8ec5
.int 0x21e2ce63
.int 0xccbc0b52
.int 0x3640655c
.int 0x7765853b
.int 0x9e88393e
.int 0xf9401dfb
.int 0xefc4df60
.int 0x3e900cfe
.int 0xc050dfe8
.int 0x79b813f5
.int 0x29521dce
.int 0x0fe8db8a
.int 0x22962042
.int 0x69252371
.int 0xc0c5386b
.int 0xa2b7dddc
.int 0x5608878b
.int 0x15f2254f
.int 0x75fbe583
.int 0xce6414d2
.int 0xc80dabb6
.int 0xd75b7883
.int 0xd439564c
.int 0x6ad5708c
.int 0x992110d5
.int 0xfbc6dc72
.int 0x0d891648
.int 0x2ccbb9cc
.int 0x8234026f
.int 0x9a2a5552
.int 0xdb1beea0
.int 0x8b476855
.int 0xb40d7de7
.int 0x7f680a8b
.int 0x5da4c286
.int 0x9ce68b4b
.int 0xb8a605d9
.int 0x02b12717
.int 0xfbbe6f98
.int 0xf543a295
.int 0x4d938b7a
.int 0xede3badf
.int 0x4649b13b
.int 0x88568239
.int 0x4b594bfa
.int 0x57e89521
.int 0xd4cbfd00
.int 0x8cd632c2
.int 0x81d5ade1
.int 0x891a2ec9
.int 0x24d066ff
.int 0x5296ce99
.int 0x96982370
.int 0xd99f7dd8
.int 0x7c47773b
.int 0x2fe7646a
.int 0xe64b0e03
.int 0x8fcbe0be
.int 0xcae71901
.int 0x53f8d884
.int 0x670a966e
.int 0xc07ef6bd
.int 0x828b7145
.int 0xb334be34
.int 0x86c69162
.int 0x3387834a
.int 0x8295bd08
.int 0xdea4402c
.int 0x08a961c4
.int 0x3c034e68
.int 0xee4a38b1
.int 0xe751fbde
.int 0xe96fd217
.int 0xa0bb0210
.int 0x1138fa73
.int 0x3d9de0dc
.int 0x3bccfdd5
.int 0x74870c8a
.int 0x3e90d3a9
.int 0x90ac113d
.int 0x0ac725d3
.int 0xfbac7ece
.int 0xaf883239
.int 0xb0bdc6f7
.int 0x381e999f
.int 0x893aeee9
.int 0x68befff1
.int 0x6d8c2a41
.int 0x5fa595e1
.int 0x60735357
.int 0x12937fea
.int 0x74b43cf5
.int 0xb0a41db8
.int 0x9323ea67
.int 0xdd8f31e5
.int 0x330dadef
.int 0xd43be51f
.int 0xe3051298
.int 0x63bfb2ad
.int 0xb80dc26a
.int 0xccbc284b
.int 0xa1222ffc
.int 0x83169777
.int 0x90263465
.int 0xcf119109
.int 0x8e2e8795
.int 0x45cc5441
.int 0xa8291000
.int 0x2d1a1220
.int 0xbcee24b7
.int 0xb9be1632
.int 0x2ba596da
.int 0x2c09ccaf
.int 0x6892b369
.int 0xd4d0a7fd
.int 0x65431878
.int 0xebcae992
.int 0xe31866c2
.int 0x564a4b3a
.int 0xa12edd9c
.int 0x415b81ba
.int 0x6babd14a
.int 0xa239a4d0
.int 0x096b00ad
.int 0x99317696
.int 0x0e06cc5c
.int 0xa8d38c4b
.int 0x873f4f11
.int 0xd996556c
.int 0x8bcf5483
.int 0xbbcffd42
.int 0xa68c2b91
.int 0x3e0c37bb
.int 0x2ef966dd
.int 0x77cde7b6
.int 0x6d2f68b7
.int 0x3ba6a29e
.int 0xb427e578
.int 0xa1b01a71
.int 0x4667302f
.int 0x5c6f6526
.int 0x2a0978b6
.int 0x02032168
.int 0xd82bdd25
.int 0x23c97cc0
.int 0xcbb968a6
.int 0x4e531f16
.int 0xeb95dfa8
.int 0xecc8ef92
.int 0xe224826e
.int 0xff97b6e3
.int 0x46399908
.int 0xc798afdc
.int 0xaa60f6a2
.int 0x3787e37e
.int 0x8f894237
.int 0x5eda7b57
.int 0x2e1339a7
.int 0x9e03de43
.int 0x2c36c227
.int 0xd205bd94
.int 0x27cfe316
.int 0x4671f992
.int 0x248c982d
.int 0x9bc36a60
.int 0xde758d18
.int 0x861b853a
.int 0xfad8b460
.int 0x6a57e320
.int 0x0d93b8c7
.int 0xde91aad1
.int 0x93c15af6
.int 0x05f8d333
.int 0xbbc3a591
.int 0xd5095015
.int 0x14ad03a9
.int 0x2c311351
.int 0x200e3892
.int 0xd9bef751
.int 0xbf1f3913
.int 0x754486ed
.int 0x834bddfa
.int 0x22519db6
.int 0xd91a7b10
.int 0x2d89fa92
.int 0x1b7d5e6b
.int 0x8c269ac2
.int 0x827a1c1f
.int 0x3bcd0944
.int 0xef36cd55
.int 0x7ccb8299
.int 0x9e5d22bc
.int 0xfab2fae2
.int 0xb4f35c5d
.int 0xc050046b
.int 0xad7b25cb
.int 0x1f0f9d84
.int 0xaf8243b8
.int 0x6bacccd0
.int 0xb28c33eb
.int 0x97532fdc
.int 0x955c9c8d
.int 0xb91b5f2b
.int 0x0ba4abe1
.int 0x65e03894
.int 0x6a094655
.int 0xed7e0007
.int 0x5a9423f3
.int 0x826e6cb0
.int 0x637cc138
.int 0x32bd8b79
.int 0x5e4d4045
.int 0xc36b8aef
.int 0xc2791472
.int 0x79245b81
.int 0xd53c9e41
.int 0xb05a3d27
.int 0xbaeea5b1
.int 0x5dc52465
.int 0x5eaaabf0
.int 0x6b35fbab
.int 0x4d5a2372
.int 0xfed8d419
.int 0x5820895d
.int 0x8dcae6a4
.int 0x2d265b64
.int 0xe5197996
.int 0xbfc4edf5
.int 0x061cab76
.int 0x9b0c22cb
.int 0xe32c1843
.int 0x11b701e5
.int 0xffc5621e
.int 0x48702dd7
.int 0xeef9944b
.int 0x2a7c3f7d
.int 0xb6536f94
.int 0x35c6f80d
.int 0x070d8c09
.int 0x274e598e
.int 0x62ac6327
.int 0x90ef9ea4
.int 0x13fb385b
.int 0x428c11ca
.int 0x1268d9e3
.int 0x9a99c2e2
.int 0xc1bc4d0f
.int 0xb70c1e27
.int 0x8b3757e9
.int 0x81a06881
.int 0x660edc36
.int 0x9f850b38
.int 0x2c3f2fd7
.int 0x436cd709
.int 0xdfd4318f
.int 0xd6ec1596
.int 0xc8795f24
.int 0x8e557d38
.int 0x7278b3e2
.int 0xd5d20e38
.int 0x8c15707f
.int 0x9ffec75b
.int 0xa948c65e
.int 0x94c63ad9
.int 0xddd85832
.int 0x23ba08b0
.int 0x3ad8a7ff
.int 0x6db53254
.int 0x347e557f
.int 0x16e856c7
.int 0xde6250e7
.int 0xf647c106
.int 0x0716d504
.int 0xa4546be3
.int 0x392558c3
.int 0xe846c82f
.int 0x9d674d15
.int 0x09988456
.int 0xb5c5ce2b
.int 0xfaf5184d
.int 0xfb4c2122
.int 0x6e7d41e5
.int 0x63ad20ff
.int 0xc2775882
.int 0xb8167f15
.int 0xce578728
.int 0xdc6cf4c8
.int 0x952de1f2
.int 0xe6ea40af
.int 0xd36a5ce4
.int 0x21181019
.int 0x6b13a220
.int 0xed26caed
.int 0xb745cd75
.int 0x7aaa31ef
.int 0xb725fb54
.int 0x6db9ed64
.int 0x2126c724
.int 0x5863f211
.int 0x4ebaa0f3
.int 0x1d8cc6a2
.int 0x0a59fd8d
.int 0x2f29a96d
.int 0x44ea78e9
.int 0xadda9c91
.int 0x9a95c4ff
.int 0x61e6487d
.int 0xcce880fe
.int 0xa295c3d1
.int 0x0962f5d9
.int 0x12ec3aa3
.int 0x1b63a73f
.int 0x3bc57820
.int 0x7c73cdec
.int 0x104f4a8d
.int 0x41ecad5a
.int 0x4ce6c7b0
.int 0xea09ccd9
.int 0xb04d7a8f
.int 0x114d0afd
.int 0x234b6591
.int 0xf9539675
.int 0xb0a5ea08
.int 0xfdf1c639
.int 0x647b930b
.int 0xdecdd024
.int 0x12ffb6c1
.int 0xf53e5fe1
.int 0xeb94feff
.int 0x3ea1073f
.int 0x0347d24d
.int 0x4af9a4e7
.int 0xaca99e48
.int 0x0d05886c
.int 0xb60ef367
.int 0x8ca49ac0
.int 0x9022961b
.int 0x6ea04506
.int 0x45eb6454
.int 0x6bde57c4
.int 0x5b1d1664
.int 0x96e3b278
.int 0x93dbee43
.int 0x95c3e78a
.int 0x87cd392d
.int 0xa76da99f
.int 0x29a0b7ab
.int 0x9950274c
.int 0x24dce4f5
.int 0x926c4429
.int 0x18221eaf
.int 0xc5bbab44
.int 0xbcc6aa16
.int 0xfbfbc5f5
.int 0xd9cd9f7a
.int 0x06da9610
.int 0x2f41a826
.int 0x11816e76
.int 0x2cf4b0a1
.int 0xca80820d
.int 0x94956651
.int 0x7615650d
.int 0xf930a480
.int 0xdddd5bba
.int 0x1c02b8be
.int 0x1ada9470
.int 0x2ebd88a1
.int 0x4be0411a
.int 0xed1c9eee
.int 0x1d608a03
.int 0xa3e00715
.int 0x3f93670d
.int 0x062c231a
.int 0xae06573f
.int 0xf54046d5
.int 0xe481f4ba
.int 0x1c923f96
.int 0xeb576f0c
.int 0x6b4bc326
.int 0x460fe2e4
.int 0x7c23b22e
.int 0xc46d8b23
.int 0xd0963df6
.int 0x2de1e053
.int 0xec88ec8a
.int 0xd54e8772
.int 0x5a39893d
.int 0x04382e2f
.int 0x88a3e489
.int 0x4c4e4175
.int 0x85347b59
.int 0xb3548163
.int 0xa4e800a5
.int 0xc6707b9b
.int 0xf5c9c276
.int 0x83d2e1fa
.int 0xabc2f04f
.int 0x2ec2bda5
.int 0x54e2c7e8
.int 0xfc0a8b80
.int 0x00f29056
.int 0xa2c12f03
.int 0x36708f8b
.int 0xc575cd66
.int 0xddedc63e
.int 0x4bb87f3a
.int 0xf4c59c26
.int 0x8608eb5a
.int 0x353e66a7
.int 0x451dbd3e
.int 0x90fdcbca
.int 0xc494faae
.int 0x90e70daa
.int 0x7af73dd5
.int 0x93573230
.int 0xc62bc5b5
.int 0xf0bd0639
.int 0x833876f7
.int 0xee98051a
.int 0x7279a923
.int 0xacce2581
.int 0x8a20e52e
.int 0xd96a74b7
.int 0x2c297770
.int 0x56b9ab46
.int 0x289de7f4
.int 0xb3be89fb
.int 0xb8445231
.int 0x93162448
.int 0x45aa9e16
.int 0xe42a0907
.int 0x1c8f9381
.int 0x03d1489e
.int 0xf9b0d011
.int 0xba475783
.int 0x79efa559
.int 0x1d8cdb1c
.int 0x6ae5c674


   


  

    
   






   




