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
  

    CheckTReg(32'h9,3'b000,3'b01,512'h30007000_7fff5000_30007000_7fff5000_30007000_7fff5000_30007000_7fff5000_30007000_7fff5000_30007000_7fff5000_30007000_7fff5000_30007000_7fff5000,32'h7);
   
    
    
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
