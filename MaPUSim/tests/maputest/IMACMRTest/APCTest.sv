//File Name    :  APCTest.sv
//Creating Date:  2013-03-25
//Author       :  tao.wang@ia.ac.cn
//Description  :  The TestCase the Test the System Start/Stop
//Last Commit  :  $Id: APCTest.sv 1385 2012-12-06 04:50:55Z yinlz $
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

  // Verification Componets of APC
  Environment  cEnv;
  Monitor      Mon;
  integer i;
  
  initial begin
    cEnv =new();
    Mon  =new();

    // Initilize the APE0 Memory: IM and MIMX and MIMY
    APE0InitInstrMem();
    APE0InitDataMem();

    ResetAndStartSPU();        //Start the SPU
    $display("====================================");   // 288 ns
     
     fork
       // Run the Verification Environment
       cEnv.run();
     join_none
     
    // initialize the M RegFile
    // for Sim version, write the MBankData; for SYN version, write the single reg
    fork
       for(i=0; i<128; i++)  Mon.M_Write(i,i);     
    join_none
    // add your code here
    #100;
    // XY Cluster Test
    // IMAC Mul test
    CheckTReg(32'h9,3'b000,3'b00,512'h00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400,32'h8);
    CheckTReg(32'ha,3'b000,3'b01,512'h00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400,32'h8);
    CheckTReg(32'hf,3'b000,3'b10,512'h00040800_00040800_00040800_00040800_00040800_00040800_00040800_00040800_00040800_00040800_00040800_00040800_00040800_00040800_00040800_00040800,32'h8);
    CheckTReg(32'h10,3'b000,3'b11,512'h00060c00_00060c00_00060c00_00060c00_00060c00_00060c00_00060c00_00060c00_00060c00_00060c00_00060c00_00060c00_00060c00_00060c00_00060c00_00060c00,32'h8);
    CheckTReg(32'h11,3'b000,3'b11,512'h00081000_00081000_00081000_00081000_00081000_00081000_00081000_00081000_00081000_00081000_00081000_00081000_00081000_00081000_00081000_00081000,32'h8);
    
    CheckTReg(32'h13,3'b010,3'b00,512'h00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400_00020400,32'h8);
    CheckTReg(32'h14,3'b010,3'b01,512'h00040800_00040800_00040800_00040800_00040800_00040800_00040800_00040800_00040800_00040800_00040800_00040800_00040800_00040800_00040800_00040800,32'h8);
    CheckTReg(32'h15,3'b010,3'b10,512'h00060c00_00060c00_00060c00_00060c00_00060c00_00060c00_00060c00_00060c00_00060c00_00060c00_00060c00_00060c00_00060c00_00060c00_00060c00_00060c00,32'h8);
    CheckTReg(32'h16,3'b010,3'b00,512'h00001000_00001000_00001000_00001000_00001000_00001000_00001000_00001000_00001000_00001000_00001000_00001000_00001000_00001000_00001000_00001000,32'h8);
    CheckTReg(32'h16,3'b010,3'b01,512'h00000008_00000008_00000008_00000008_00000008_00000008_00000008_00000008_00000008_00000008_00000008_00000008_00000008_00000008_00000008_00000008,32'h9);

    CheckTReg(32'h20,3'b000,3'b00,512'h00091200_00091200_00091200_00091200_00091200_00091200_00091200_00091200_00091200_00091200_00091200_00091200_00091200_00091200_00091200_00091200,32'h4);
    CheckTReg(32'h21,3'b000,3'b01,512'h000a1400_000a1400_000a1400_000a1400_000a1400_000a1400_000a1400_000a1400_000a1400_000a1400_000a1400_000a1400_000a1400_000a1400_000a1400_000a1400,32'h4);
    CheckTReg(32'h22,3'b000,3'b10,512'h000b1600_000b1600_000b1600_000b1600_000b1600_000b1600_000b1600_000b1600_000b1600_000b1600_000b1600_000b1600_000b1600_000b1600_000b1600_000b1600,32'h4);
    //CheckTReg(32'h23,3'b000,3'b00,512'h00001600_00001600_00001600_00001600_00001600_00001600_00001600_00001600_00001600_00001600_00001600_00001600_00001600_00001600_00001600_00001600,32'h4);
    //CheckTReg(32'h23,3'b000,3'b01,512'h0001020b_0001020b_0001020b_0001020b_0001020b_0001020b_0001020b_0001020b_0001020b_0001020b_0001020b_0001020b_0001020b_0001020b_0001020b_0001020b,32'h5);

    WaitSPUCycles(10);
    $display("====================================");       
       
    // count the coverage of the CoverGroup
    cEnv.wrap_up();
    
  end

 ///////////////////////////////////////////////
  //Chechk the regisiter value. 
  `define X_EX1_PC  $root.TestTop.uAPC.uAPE0.uMPU.uMFetch.nMIAddr
   
   task automatic CheckTReg(input int PC, input int GroupID, input int TRegID,input bit[511:0] TRegValue,input int WBStage);
     int result ,i ;
     bit[511:0]    nTValue ;

     bit[31:0]     nTExpectedValue, nTResultValue;     
    
     wait(PC == `X_EX1_PC) ;
     
     fork: CHECK_XYT
       begin
         WaitMPUCycles(WBStage);
         Mon.T_Read(GroupID,TRegID,nTValue);
                
         result = (TRegValue == nTValue);

         if(result == 1) $display("@time %0t,@EX1_PC=%h, success",$time,PC);
         else begin
           $display("**********************************************************************");
           $display("@time %0t,@EX1_PC=%h, error:",$time,PC);
           for(i=0; i<16; i++) begin
             nTExpectedValue = TRegValue[i*32 +: 32];
             nTResultValue   = nTValue[i*32 +: 32];
             $display ("TReg[%2d]: Expected, %h; Result, %h\n",i,nTExpectedValue,nTResultValue);
           end
           $display("**********************************************************************");
         end
       end
     join_none:CHECK_XYT
   endtask

  `define X_TB  $root.TestTop.uAPC.uAPE0.uMPU.uSHU0.rSHUXTB



  `undef X_EX1_PC
endprogram
