//  Date: 2014-01-20
//  Name: app.s.asm
//  Author: tao.wang@ia.ac.cn
//  The include file for fix pint FFT




// parameter definition
////////////////////////////////////////////////////////////////////
// the following need not to modify    
    SDA0DM0_START = 0x400000
    SDA0DM1_START = 0x600000
    SDA1DM0_START = 0x800000 
    SDA1DM1_START = 0xa00000 
    SDA2DM0_START = 0xc00000  
    SDA2DM1_START = 0xe00000  
    SDASize       = 0x40000  
///////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// the following need to modify
    ALen                = 1024                         // the FFT Length   
    StagesNum           = 10    
    ABitRevLen          = 8                            // the bit reversed number = log2(ALen)-2   
//////////////////////////////////////////////////////////////////////// 



/////////////////////////////////////////////////////////////////////////
// the following need not changing    
    BS            = 16      
    BankNum       = BS
   
    
    BankSize      = SDASize / BS
    BlockSize     = 64                       // the total Rd size, for Row Rd is 512/8     
    BlockGran     = 6                        //  Block Granularity = log2(BlockSize)
    DataSize      = 4                        //  Byte 1, Short 2, Word 4, Long 8
    DataSizeGran  = 2                        //  log2 (DataSize)
    
    
    ABSLen              = ALen   /  BS
    BankGrpNum          = ABSLen / BankNum    // the num of group in each Bank  
    //BitRevAddrInc       = 1                   // the address increment in bit reversed mode
    BitRevAddrInc       = ALen   / 8          // the address increment in bit reversed mode

    
    GrpNum              = ABSLen 
    PipeGrp             = 12
    PipeGrpNum          = GrpNum / PipeGrp  
    PipeGrpTailNum      = GrpNum % PipeGrp 
    PipeGrpTailEn       = PipeGrpTailNum !=0
    PipeNum             = PipeGrpNum + PipeGrpTailEn  

    EpochStages      = 4
    EpochTailStages  = StagesNum % EpochStages
    EpochTailStagesEn= EpochTailStages !=0 
    EpochNum         = StagesNum / EpochStages + EpochTailStagesEn 
   
    .if EpochTailStages == 0
        LastEpochDataSizeGran = 2
        LastEpochDataSize     = 4
    .elseif EpochTailStages == 1
        LastEpochDataSizeGran = 5
        LastEpochDataSize     = 32
    .elseif EpochTailStages == 2
        LastEpochDataSizeGran = 4
        LastEpochDataSize     = 16
    .else  
        LastEpochDataSizeGran = 3
        LastEpochDataSize     = 8
    .endif
   
    ACirBRNum       = 1                         // the cycle number of load input with BR in Epoch 0  
    ACirNBRNum      = 2                         // the cycle number of load input without BR in other Epoch except Epoch 0                   
    BCirNum         = EpochNum                  // the cycle number of load twiddle
    CCirNum         = 1                         // the cycle number of store result    

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // for MC Config
    // M.w0/r0/r1 
    MIndexStartW0   = 0x0
    MStepModeW0     = 0x1 
    MLenW0          = PipeGrp 
    ISizeW0         = PipeGrp 

    MIndexOffset    = 0
    MStepOffset     = 8
    MLenOffset      = 24
    ISizeOffset     = 12

    MCIndexStartW0  = MIndexStartW0 << MIndexOffset 
    MCStepModeW0    = MStepModeW0   << MStepOffset
    MCLenW0         = MLenW0        << MLenOffset
    ICSizeW0        = ISizeW0       << ISizeOffset
    MConfigW0       = MCIndexStartW0 | MCStepModeW0 | MCLenW0 | ICSizeW0


    // M.w0/r0/r1 
    MIndexStartW1   = 0xc
    MStepModeW1     = 0x1 
    MLenW1          = PipeGrp 
    ISizeW1         = PipeGrp 
 
    MCIndexStartW1  = MIndexStartW1 << MIndexOffset 
    MCStepModeW1    = MStepModeW1   << MStepOffset
    MCLenW1         = MLenW1        << MLenOffset
    ICSizeW1        = ISizeW1       << ISizeOffset
    MConfigW1       = MCIndexStartW1 | MCStepModeW1 | MCLenW1 | ICSizeW1
////////////////////////////////////////////////////////////////////////////
