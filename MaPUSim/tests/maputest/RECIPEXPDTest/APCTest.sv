//  Date: 2014-02-20
//  Name: APCTest.sv
//  Author: yanglp@zklc.com
//  The file to test the function of IALU 

program APCTest(

  input  bit              iSPUClk,          //The SPU Clock
  input  bit              iSoCClk,          //The SoC Clock
  input  bit              iMainRst_n,       //The SoC Reset

  AXIBusIF.TestSlave      iSMBus0,           //The SM Bus port.
  AXIBusIF.TestMaster     iExtToCSU0,        //The ExtToCSU port.
  AXIBusIF.TestSlave      iCSU0ToExt,        //The CSU Master to CBus.

  AXIBusIF.TestSlave      iSMBus1,           //The SM Bus port.
  AXIBusIF.TestMaster     iExtToCSU1,        //The ExtToCSU port.
  AXIBusIF.TestSlave      iCSU1ToExt,        //The CSU Master to CBus.
  CovSampleIF             iCvSmpIF           //The interface for coverpoint
);

  `include "TestLib.sv"
  `include "SPUDef.v"
  `include "MPUDef.v"
  `include "MACCDef.v" 
  `include "APEDef.v"

  localparam  DDRSize =32'h400000;

  // Virtual Componets of MaPU
  ARM                                               hARM;    // [Optional]
  ShareMem #(.BASE(32'h7000000),.SIZE(32'h20000))   hSM;     // [Optional]
  DDR      #(.BASE(`DDR_ADDR_BASE),.SIZE(DDRSize))  hDDR;    // [Optional]

  byte SDA0DM0[(1<<18):0];
  byte SDA0DM1[(1<<18):0];
  byte SDA1DM0[(1<<18):0];
  byte SDA1DM1[(1<<18):0];
  byte SDA2DM0[(1<<18):0];
  byte SDA2DM1[(1<<18):0];

  // Verification Componets of APC
  Environment  cEnv;
  Monitor      Mon;
  bit[15:0] i;

  integer HandSDA0DM0;
  bit[7:0]   j,k,Temp,g;
  
  initial begin
    cEnv =new();
    Mon  =new();
    hDDR =new(); 
     
    // Initilize the APE0 Memory: IM and MIMX and MIMY
    APE0InitInstrMem();
    APE0InitDataMem();
    hDDR.Run();
    
    // initialize the M RegFile
    // for Sim version, write the MBankData; for SYN version, write the single reg  
    for(k=0; k<128; k++)
      Mon.M_Write(k,{{k+8'd64},{k+8'd63},{k+8'd62},{k+8'd61},{k+8'd60},{k+8'd59},{k+8'd58},{k+8'd57},{k+8'd56},{k+8'd55},{k+8'd54},{k+8'd53},{k+8'd52},{k+8'd51},
                     {k+8'd50},{k+8'd49},{k+8'd48},{k+8'd47},{k+8'd46},{k+8'd45},{k+8'd44},{k+8'd43},{k+8'd42},{k+8'd41},{k+8'd40},{k+8'd39},{k+8'd38},{k+8'd37},
                     {k+8'd36},{k+8'd35},{k+8'd34},{k+8'd33},{k+8'd32},{k+8'd31},{k+8'd30},{k+8'd29},{k+8'd28},{k+8'd27},{k+8'd26},{k+8'd25},{k+8'd24},{k+8'd23},
                     {k+8'd22},{k+8'd21},{k+8'd20},{k+8'd19},{k+8'd18},{k+8'd17},{k+8'd16},{k+8'd15},{k+8'd14},{k+8'd13},{k+8'd12},{k+8'd11},{k+8'd10},{k+8'd09},
                     {k+8'd08},{k+8'd07},{k+8'd06},{k+8'd05},{k+8'd04},{k+8'd03},{k+8'd02},{k+8'd01}});
    
    Mon.M_Write(0,512'h00003e3d_00003a39_00003635_00003231_00002e2d_00002a29_00002625_00002221_00001e1d_00001a19_00001615_00001211_00000e0d_00000a09_00000605_00000201);
    Mon.M_Write(1,512'h00003f3e_00003b3a_00003736_00003332_00002f2e_00002b2a_00002726_00002322_00001f1e_00001b1a_00001716_00001312_00000f0e_00000b0a_00000706_00000302);
    Mon.M_Write(2,512'h0041003f_003d003b_00390037_00350033_0031002f_002d002b_00290027_00250023_0021001f_001d001b_00190017_00150013_0011000f_000d000b_00090007_00050003);
    Mon.M_Write(3,512'h00420040_003e003c_003a0038_00360034_00320030_002e002c_002a0028_00260024_00220020_001e001c_001a0018_00160014_00120010_000e000c_000a0008_00060004);
    
    ResetAndStartSPU();        //Start the SPU
    $display("====================================");
     
    fork
      // Run the Verification Environment
      cEnv.run();
    join_none
    
    // add your code here
    fork
        CheckTValue(32'h006, 3'b000, 2'b00, 512'h3f3e3d3c_3b3a3938_37363534_33323130_2f2e2d2c_2b2a2928_27262524_23222120_1f1e1d1c_1b1a1918_17161514_13121110_0f0e0d0c_0b0a0908_07060504_03020100);        
        
        CheckTValue(32'h008, 3'b000, 2'b00, 512'h00003d3c_00003938_00003534_00003130_00002d2c_00002928_00002524_00002120_00001d1c_00001918_00001514_00001110_00000d0c_00000908_00000504_00000100);        
        CheckTValue(32'h009, 3'b000, 2'b01, 512'h00003f3e_00003b3a_00003736_00003332_00002f2e_00002b2a_00002726_00002322_00001f1e_00001b1a_00001716_00001312_00000f0e_00000b0a_00000706_00000302);        
        
        CheckTValue(32'h00d, 3'b000, 2'b00, 512'h3f3e3d3c_3b3a3938_37363534_33323130_2f2e2d2c_2b2a2928_27262524_23222120_1f1e1d1c_1b1a1918_17161514_13121110_0f0e0d0c_0b0a0908_07060504_03020100);        
        
        CheckTValue(32'h012, 3'b000, 2'b00, 512'h003d003c_00390038_00350034_00310030_002d002c_00290028_00250024_00210020_001d001c_00190018_00150014_00110010_000d000c_00090008_00050004_00010000);        
        CheckTValue(32'h013, 3'b000, 2'b01, 512'h003f003e_003b003a_00370036_00330032_002f002e_002b002a_00270026_00230022_001f001e_001b001a_00170016_00130012_000f000e_000b000a_00070006_00030002);        
        
        CheckTValue(32'h017, 3'b000, 2'b00, 512'h3f3e3d3c_3b3a3938_37363534_33323130_2f2e2d2c_2b2a2928_27262524_23222120_1f1e1d1c_1b1a1918_17161514_13121110_0f0e0d0c_0b0a0908_07060504_03020100);        
                                  
    join

    #100;
    
    WaitSPUCycles(10);
    $display("====================================");       
  
    // count the coverage of the CoverGroup
    cEnv.wrap_up();
    end





 ///////////////////////////////////////////////
  //Chechk the regisiter value. 
  `define EX1_PC  $root.TestTop.uAPC.uAPE0.uSPU.uFetch.rRRPC
  `define MPU_EX1_PC  $root.TestTop.uAPC.uAPE0.uMPU.uMFetch.nMIAddr

  // GrpID: 000-IALU,001-IMAC,010-FALU,011-FMAC,100-SHU0,101-SHU1;  TID: 00~11
  task automatic CheckTValue(input int MPUPC, input bit[2:0] GrpID, input bit[1:0] TID, input bit[511:0] ExpValue); 
      bit[511:0]   TValue;
      wait(`MPU_EX1_PC == MPUPC);
      
      WaitMPUCycles(3);
      Mon.T_Read(GrpID, TID,TValue);
      if(TValue == ExpValue) $display("%0t Read T  OK @MPUEX1_PC = %h!!", $time, MPUPC);
      else   
        begin
          $display("%0t Error @MPUEX1_PC = %h, Expected Value and Real Result are :", $time, MPUPC);
          Mon.display_512(ExpValue);
          Mon.display_512(TValue);
          $display(" ");
        end
  endtask

  task automatic CheckMValue(input int MPUPC, input int WaitCycle, input bit[6:0] RID, input bit[511:0] ExpValue); 
      bit[511:0]   MValue;
      wait(`MPU_EX1_PC == MPUPC);

      WaitMPUCycles(WaitCycle);

      Mon.M_Read(RID, MValue);
      if(MValue == ExpValue) $display("%0t Read M of OK @MPUEX1_PC = %h!!", $time, MPUPC);
      else  begin
        $display("%0t Error @MPUEX1_PC = %h, Expected Value and real value are : ", $time, MPUPC);
        Mon.display_512(ExpValue);            
        Mon.display_512(MValue);
        $display(" ");        
      end
  endtask
					       //  WaitCycle = 7    
  task automatic CheckDMValue(input int MPUPC, input int WaitCycle, input bit[2:0] KG, input int Addr, input bit[511:0] ExpValue);
      bit[511:0]   DMValue512;
      byte DMValue[];
      DMValue  = new[64];
      
      wait(`MPU_EX1_PC == MPUPC);
      
      WaitMPUCycles(WaitCycle);
      
      case(KG)
        3'b000: begin
          for(i=0 ; i<64 ; i++) begin
            TestTop.uAPC.uAPE0.uSDA2DM0.DMReadBytes(Addr+4096*i,DMValue);
            DMValue512[8*i +:8] = DMValue[0] ;
          end
        end
        3'b001: begin
          for(i=0 ; i<32 ; i++) begin
            TestTop.uAPC.uAPE0.uSDA2DM0.DMReadBytes(Addr+4096*2*i,DMValue);
            DMValue512[16*i   +: 8] = DMValue[0] ;
            DMValue512[16*i+8 +: 8] = DMValue[1] ;
          end
        end
        3'b010: begin
          for(i=0 ; i<16 ; i++) begin
            TestTop.uAPC.uAPE0.uSDA2DM0.DMReadBytes(Addr+4096*4*i,DMValue);
            DMValue512[32*i   +: 8] = DMValue[0] ;
            DMValue512[32*i+8 +: 8] = DMValue[1] ;
            DMValue512[32*i+16+: 8] = DMValue[2] ;
            DMValue512[32*i+24+: 8] = DMValue[3] ;
          end
        end
        3'b011: begin
          for(i=0 ; i<8 ; i++) begin
            TestTop.uAPC.uAPE0.uSDA2DM0.DMReadBytes(Addr+4096*8*i,DMValue);
            DMValue512[64*i   +: 8] = DMValue[0] ;
            DMValue512[64*i+8 +: 8] = DMValue[1] ;
            DMValue512[64*i+16+: 8] = DMValue[2] ;
            DMValue512[64*i+24+: 8] = DMValue[3] ;
            DMValue512[64*i+32+: 8] = DMValue[4] ;
            DMValue512[64*i+40+: 8] = DMValue[5] ;
            DMValue512[64*i+48+: 8] = DMValue[6] ;
            DMValue512[64*i+56+: 8] = DMValue[7] ;
          end
        end
        3'b100: begin
          for(i=0 ; i<4 ; i++) begin
            TestTop.uAPC.uAPE0.uSDA2DM0.DMReadBytes(Addr+4096*16*i,DMValue);
            DMValue512[128*i    +: 8] = DMValue[0] ;
            DMValue512[128*i+8  +: 8] = DMValue[1] ;
            DMValue512[128*i+16 +: 8] = DMValue[2] ;
            DMValue512[128*i+24 +: 8] = DMValue[3] ;
            DMValue512[128*i+32 +: 8] = DMValue[4] ;
            DMValue512[128*i+40 +: 8] = DMValue[5] ;
            DMValue512[128*i+48 +: 8] = DMValue[6] ;
            DMValue512[128*i+56 +: 8] = DMValue[7] ;
            DMValue512[128*i+64 +: 8] = DMValue[8] ;
            DMValue512[128*i+72 +: 8] = DMValue[9] ;
            DMValue512[128*i+80 +: 8] = DMValue[10] ;
            DMValue512[128*i+88 +: 8] = DMValue[11] ;
            DMValue512[128*i+96 +: 8] = DMValue[12] ;
            DMValue512[128*i+104+: 8] = DMValue[13] ;
            DMValue512[128*i+112+: 8] = DMValue[14] ;
            DMValue512[128*i+120+: 8] = DMValue[15] ;
          end
        end
        3'b101: begin
          for(i=0 ; i<2 ; i++) begin
            TestTop.uAPC.uAPE0.uSDA2DM0.DMReadBytes(Addr+4096*32*i,DMValue);
            DMValue512[256*i    +: 8] = DMValue[0] ;
            DMValue512[256*i+8  +: 8] = DMValue[1] ;
            DMValue512[256*i+16 +: 8] = DMValue[2] ;
            DMValue512[256*i+24 +: 8] = DMValue[3] ;
            DMValue512[256*i+32 +: 8] = DMValue[4] ;
            DMValue512[256*i+40 +: 8] = DMValue[5] ;
            DMValue512[256*i+48 +: 8] = DMValue[6] ;
            DMValue512[256*i+56 +: 8] = DMValue[7] ;
            DMValue512[256*i+64 +: 8] = DMValue[8] ;
            DMValue512[256*i+72 +: 8] = DMValue[9] ;
            DMValue512[256*i+80 +: 8] = DMValue[10] ;
            DMValue512[256*i+88 +: 8] = DMValue[11] ;
            DMValue512[256*i+96 +: 8] = DMValue[12] ;
            DMValue512[256*i+104+: 8] = DMValue[13] ;
            DMValue512[256*i+112+: 8] = DMValue[14] ;
            DMValue512[256*i+120+: 8] = DMValue[15] ;
            DMValue512[256*i+128+: 8] = DMValue[16] ;
            DMValue512[256*i+136+: 8] = DMValue[17] ;
            DMValue512[256*i+144+: 8] = DMValue[18] ;
            DMValue512[256*i+152+: 8] = DMValue[19] ;
            DMValue512[256*i+160+: 8] = DMValue[20] ;
            DMValue512[256*i+168+: 8] = DMValue[21] ;
            DMValue512[256*i+176+: 8] = DMValue[22] ;
            DMValue512[256*i+184+: 8] = DMValue[23] ;
            DMValue512[256*i+192+: 8] = DMValue[24] ;
            DMValue512[256*i+200+: 8] = DMValue[25] ;
            DMValue512[256*i+208+: 8] = DMValue[26] ;
            DMValue512[256*i+216+: 8] = DMValue[27] ;
            DMValue512[256*i+224+: 8] = DMValue[28] ;
            DMValue512[256*i+232+: 8] = DMValue[29] ;
            DMValue512[256*i+240+: 8] = DMValue[30] ;
            DMValue512[256*i+248+: 8] = DMValue[31] ;
          end
        end
        3'b110: begin
          for(i=0 ; i<1 ; i++) begin
            TestTop.uAPC.uAPE0.uSDA2DM0.DMReadBytes(Addr+4096*64*i,DMValue);
            DMValue512[512*i    +: 8] = DMValue[0] ;
            DMValue512[512*i+8  +: 8] = DMValue[1] ;
            DMValue512[512*i+16 +: 8] = DMValue[2] ;
            DMValue512[512*i+24 +: 8] = DMValue[3] ;
            DMValue512[512*i+32 +: 8] = DMValue[4] ;
            DMValue512[512*i+40 +: 8] = DMValue[5] ;
            DMValue512[512*i+48 +: 8] = DMValue[6] ;
            DMValue512[512*i+56 +: 8] = DMValue[7] ;
            DMValue512[512*i+64 +: 8] = DMValue[8] ;
            DMValue512[512*i+72 +: 8] = DMValue[9] ;
            DMValue512[512*i+80 +: 8] = DMValue[10] ;
            DMValue512[512*i+88 +: 8] = DMValue[11] ;
            DMValue512[512*i+96 +: 8] = DMValue[12] ;
            DMValue512[512*i+104+: 8] = DMValue[13] ;
            DMValue512[512*i+112+: 8] = DMValue[14] ;
            DMValue512[512*i+120+: 8] = DMValue[15] ;
            DMValue512[512*i+128+: 8] = DMValue[16] ;
            DMValue512[512*i+136+: 8] = DMValue[17] ;
            DMValue512[512*i+144+: 8] = DMValue[18] ;
            DMValue512[512*i+152+: 8] = DMValue[19] ;
            DMValue512[512*i+160+: 8] = DMValue[20] ;
            DMValue512[512*i+168+: 8] = DMValue[21] ;
            DMValue512[512*i+176+: 8] = DMValue[22] ;
            DMValue512[512*i+184+: 8] = DMValue[23] ;
            DMValue512[512*i+192+: 8] = DMValue[24] ;
            DMValue512[512*i+200+: 8] = DMValue[25] ;
            DMValue512[512*i+208+: 8] = DMValue[26] ;
            DMValue512[512*i+216+: 8] = DMValue[27] ;
            DMValue512[512*i+224+: 8] = DMValue[28] ;
            DMValue512[512*i+232+: 8] = DMValue[29] ;
            DMValue512[512*i+240+: 8] = DMValue[30] ;
            DMValue512[512*i+248+: 8] = DMValue[31] ;
            DMValue512[512*i+256+: 8] = DMValue[32] ;
            DMValue512[512*i+264+: 8] = DMValue[33] ;
            DMValue512[512*i+272+: 8] = DMValue[34] ;
            DMValue512[512*i+280+: 8] = DMValue[35] ;
            DMValue512[512*i+288+: 8] = DMValue[36] ;
            DMValue512[512*i+296+: 8] = DMValue[37] ;
            DMValue512[512*i+304+: 8] = DMValue[38] ;
            DMValue512[512*i+312+: 8] = DMValue[39] ;
            DMValue512[512*i+320+: 8] = DMValue[40] ;
            DMValue512[512*i+328+: 8] = DMValue[41] ;
            DMValue512[512*i+336+: 8] = DMValue[42] ;
            DMValue512[512*i+344+: 8] = DMValue[43] ;
            DMValue512[512*i+352+: 8] = DMValue[44] ;
            DMValue512[512*i+360+: 8] = DMValue[45] ;
            DMValue512[512*i+368+: 8] = DMValue[46] ;
            DMValue512[512*i+376+: 8] = DMValue[47] ;
            DMValue512[512*i+384+: 8] = DMValue[48] ;
            DMValue512[512*i+392+: 8] = DMValue[49] ;
            DMValue512[512*i+400+: 8] = DMValue[50] ;
            DMValue512[512*i+408+: 8] = DMValue[51] ;
            DMValue512[512*i+416+: 8] = DMValue[52] ;
            DMValue512[512*i+424+: 8] = DMValue[53] ;
            DMValue512[512*i+432+: 8] = DMValue[54] ;
            DMValue512[512*i+440+: 8] = DMValue[55] ;
            DMValue512[512*i+448+: 8] = DMValue[56] ;
            DMValue512[512*i+456+: 8] = DMValue[57] ;
            DMValue512[512*i+464+: 8] = DMValue[58] ;
            DMValue512[512*i+472+: 8] = DMValue[59] ;
            DMValue512[512*i+480+: 8] = DMValue[60] ;
            DMValue512[512*i+488+: 8] = DMValue[61] ;
            DMValue512[512*i+496+: 8] = DMValue[62] ;
            DMValue512[512*i+504+: 8] = DMValue[63] ;
          end
        end
        default:  $display("The input of KG is invalid !\n\n");
      endcase
      
     
      if(DMValue512 == ExpValue) $display("%0t Read DM OK @MPUEX1_PC = %h!!", $time, MPUPC);
      else  begin
        $display("%0t Error @MPUEX1_PC = %h, Expected Value and real value are : ", $time, MPUPC);
        Mon.display_512(ExpValue);            
        Mon.display_512(DMValue512);
        $display(" ");
      end
  endtask

  task  automatic CheckBIUValue(input int MPUPC, input int WaitCycle, input bit[1:0] BIUID, input bit[511:0] ExpValue);
      bit[511:0]   BIUValue;
      wait(`MPU_EX1_PC == MPUPC);

      WaitMPUCycles(WaitCycle);
      
      case(BIUID)
        2'b00:    BIUValue = $root.TestTop.uAPC.uAPE0.uMPU.uIPath.uBIU0.WDataLatch;
        2'b01:    BIUValue = $root.TestTop.uAPC.uAPE0.uMPU.uIPath.uBIU1.WDataLatch;
        2'b10:    BIUValue = $root.TestTop.uAPC.uAPE0.uMPU.uIPath.uBIU2.WDataLatch;
        default:  $display("The input of BIUID is invalid !\n\n");
      endcase
      
      if(BIUValue == ExpValue) $display("%0t Read BIU of OK @MPUEX1_PC = %h!!", $time, MPUPC);
      else  begin
        $display("%0t Error @MPUEX1_PC = %h, Expected Value and real value are : ", $time, MPUPC);
        Mon.display_512(ExpValue);            
        Mon.display_512(BIUValue);
        $display(" ");        
      end
  endtask

  task  automatic CheckCondReg(input int MPUPC, input bit ExpValue);
      bit   CondValue;
      wait(`MPU_EX1_PC == MPUPC);

      WaitMPUCycles(3);
      
      CondValue = $root.TestTop.uAPC.uAPE0.uMPU.uMFetch.rCondReg;
      
      if(CondValue == ExpValue) $display("%0t Read CondReg of OK @MPUEX1_PC = %h!!", $time, MPUPC);
      else  begin
        $display("%0t Error @MPUEX1_PC = %h, Expected Value and real value are : %h and %h", $time, MPUPC, ExpValue, CondValue);
        $display(" ");        
      end
  endtask

  `undef EX1_PC
endprogram


