//  Date: 2013-08-15
//  Name: app.s.asm
//  Author: tao.wang@ia.ac.cn
//  Some common variables for app.s.asm and app.m.asm

    BS             = 16 

    DataLen        = 1024
    EffiLen        = 64 
    PadDataLen     = DataLen + EffiLen - 1
    
    DataTailNum    = DataLen % 	BS
    DataTailEn     = DataTailNum != 0
    DataBSNum      = DataLen /	BS + DataTailEn

    PadDataTailNum = PadDataLen % 	BS
    PadDataTailEn  = PadDataTailNum != 0
    PadDataBSNum   = PadDataLen /	BS + PadDataTailEn

    EffiTailNum    = EffiLen % 	BS
    EffiTailEn     = EffiTailNum != 0
    EffiBSNum      = EffiLen /	BS + EffiTailEn

    MDataLen       =  EffiLen + BS -1
    MTailLen       =  MDataLen % BS
    MTailEn        =  MTailLen !=0
    MBSNum         =  MDataLen / BS +  MTailEn 
