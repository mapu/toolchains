//File Name    :  APCTest.sv
//Creating Date:  2012-10-16
//Author       :  leizu.yin@ia.ac.cn
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
  bit[7:0] i;
  
  initial begin
    cEnv =new();
    Mon  =new();

    // Initilize the APE0 Memory: IM and MIMX and MIMY
    APE0InitInstrMem();
    APE0InitDataMem();

    // initialize the M RegFile
    // for Sim version, write the MBankData; for SYN version, write the single reg  
    for(i=0; i<128; i++)
        Mon.M_Write(i,{249'b0,i,249'b0,i});

    ResetAndStartSPU();        //Start the SPU
    $display("====================================");   // 288 ns
     
     fork
       // Run the Verification Environment
       cEnv.run();
     join_none

    // add your code here
    fork
      // check M condition Execution
      CheckMCondExe(32'h8, 0,0,0,0);
      CheckMCondExe(32'h9, 1,0,0,0);
      CheckMCondExe(32'ha, 0,0,0,0);
      CheckMCondExe(32'hb, 0,1,0,0);
      CheckMCondExe(32'hc, 0,0,0,0);
      CheckMCondExe(32'hd, 0,0,1,0);
      CheckMCondExe(32'he, 0,0,0,0);
      CheckMCondExe(32'hf, 0,0,0,1);

     // check SHU condition Execution
     CheckSHUCondExe(32'h10, 0,0);
     CheckSHUCondExe(32'h11, 1,0);
     CheckSHUCondExe(32'h12, 0,0);
     CheckSHUCondExe(32'h13, 0,1);

     CheckBIUCondExe(32'h14, 0,0,0);
     CheckBIUCondExe(32'h15, 1,0,0);
     CheckBIUCondExe(32'h16, 0,0,0);
     CheckBIUCondExe(32'h17, 0,1,0);
     CheckBIUCondExe(32'h18, 0,0,0);
     CheckBIUCondExe(32'h19, 0,0,1);

     CheckMACCCondExe(32'h1a, 0,0,0,0);
     CheckMACCCondExe(32'h1b, 1,0,0,0);
     CheckMACCCondExe(32'h1c, 0,0,0,0);
     CheckMACCCondExe(32'h1d, 0,1,0,0);
     CheckMACCCondExe(32'h1e, 0,0,0,0);
     CheckMACCCondExe(32'h1f, 0,0,1,0);
     CheckMACCCondExe(32'h20, 0,0,0,0);
     CheckMACCCondExe(32'h22, 0,0,0,1);

      // check M condition Execution
      CheckMCondExe(32'h25, 1,0,0,0);
      CheckMCondExe(32'h26, 0,0,0,0);
      CheckMCondExe(32'h27, 0,1,0,0);
      CheckMCondExe(32'h28, 0,0,0,0);
      CheckMCondExe(32'h29, 0,0,1,0);
      CheckMCondExe(32'h2a, 0,0,0,0);
      CheckMCondExe(32'h2b, 0,0,0,1);
      CheckMCondExe(32'h2c, 0,0,0,0);

     // check SHU condition Execution
     CheckSHUCondExe(32'h2d, 1,0);
     CheckSHUCondExe(32'h2e, 0,0);
     CheckSHUCondExe(32'h2f, 0,1);
     CheckSHUCondExe(32'h30, 0,0);

     CheckBIUCondExe(32'h31, 1,0,0);
     CheckBIUCondExe(32'h32, 0,0,0);
     CheckBIUCondExe(32'h33, 0,1,0);
     CheckBIUCondExe(32'h34, 0,0,0);
     CheckBIUCondExe(32'h35, 0,0,1);
     CheckBIUCondExe(32'h36, 0,0,0);

     CheckMACCCondExe(32'h37, 1,0,0,0);
     CheckMACCCondExe(32'h38, 0,0,0,0);
     CheckMACCCondExe(32'h39, 0,1,0,0);
     CheckMACCCondExe(32'h3a, 0,0,0,0);
     CheckMACCCondExe(32'h3b, 0,0,1,0);
     CheckMACCCondExe(32'h3c, 0,0,0,0);
     CheckMACCCondExe(32'h3d, 0,0,0,1);
     CheckMACCCondExe(32'h3e, 0,0,0,0);

    join


    #100;

    WaitSPUCycles(10);
    $display("====================================");       
       
    // count the coverage of the CoverGroup
    cEnv.wrap_up();
    
  end

 ///////////////////////////////////////////////
  //Chechk the regisiter value. 
  `define MPU_EX1_PC  $root.TestTop.uAPC.uAPE0.uMPU.uMFetch.nMIAddrDP
  `define MRCode    $root.TestTop.uAPC.uAPE0.uMPU.uMFetch.oMCodeLine
  `define SHU0Code  $root.TestTop.uAPC.uAPE0.uMPU.uMFetch.oSHU0CodeLine
  `define SHU1Code  $root.TestTop.uAPC.uAPE0.uMPU.uMFetch.oSHU1CodeLine
  `define IALUCode  $root.TestTop.uAPC.uAPE0.uMPU.uMFetch.oIALUCodeLine
  `define IMACCode  $root.TestTop.uAPC.uAPE0.uMPU.uMFetch.oIMACCodeLine
  `define FALUCode  $root.TestTop.uAPC.uAPE0.uMPU.uMFetch.oFALUCodeLine
  `define FMACCode  $root.TestTop.uAPC.uAPE0.uMPU.uMFetch.oFMACCodeLine
  `define BIU0Code  $root.TestTop.uAPC.uAPE0.uMPU.uMFetch.oBIU0CodeLine
  `define BIU1Code  $root.TestTop.uAPC.uAPE0.uMPU.uMFetch.oBIU1CodeLine
  `define BIU2Code  $root.TestTop.uAPC.uAPE0.uMPU.uMFetch.oBIU2CodeLine
   task automatic CheckMCondExe(input int MPUPC, input bit MR0TrueExe, input bit MR1TrueExe, input bit MR2TrueExe, input bit MR3TrueExe);
     wait(`MPU_EX1_PC == MPUPC);
     fork
       if(MR0TrueExe)begin
         if(`MRCode[22] == 0)  $display("Error for MR0 Condtion Execution @MPUEX1_PC = %h", MPUPC);
         else                  $display("Passed for MR0 Condtion Execution @MPUEX1_PC = %h", MPUPC);
       end else if(MR1TrueExe) begin
         if(`MRCode[42] == 0)  
               $display("Error for MR1 Condtion Execution @MPUEX1_PC = %h", MPUPC);
         else  $display("Passed for MR1 Condtion Execution @MPUEX1_PC = %h", MPUPC);
       end else if(MR2TrueExe)begin
         if(`MRCode[63] == 0)  
               $display("Error for MR2 Condtion Execution @MPUEX1_PC = %h", MPUPC);
         else  $display("Passed for MR2 Condtion Execution  @MPUEX1_PC = %h", MPUPC);
       end else if(MR3TrueExe)begin
         if(`MRCode[78] == 0)  
               $display("Error for MR3 Condtion Execution @MPUEX1_PC = %h", MPUPC);
         else  $display("Passed for MR3 Condtion Execution @MPUEX1_PC = %h", MPUPC);
       end else begin
         if((`MRCode[22] != 0) | (`MRCode[42] != 0) | (`MRCode[63] != 0) | (`MRCode[78] != 0))
                           $display("Error for M Condtion Execution @MPUEX1_PC = %h", MPUPC);
         else              $display("Passed for M Condtion Execution @MPUEX1_PC = %h", MPUPC);         
       end
     join_none
   endtask

  task automatic CheckSHUCondExe(input int MPUPC, input bit SHU0TrueExe, input bit SHU1TrueExe);
     wait(`MPU_EX1_PC == MPUPC);
     
       if(SHU0TrueExe)begin
         if(`SHU0Code[27:25] == 0)  $display("Error for  SHU0 Condtion Execution @MPUEX1_PC = %h", MPUPC);
         else                $display("Passed for SHU0 Condtion Execution @MPUEX1_PC = %h", MPUPC);
       end else if(SHU1TrueExe) begin
         if(`SHU1Code[27:25] == 0)  
               $display("Error for  SHU1 Condtion Execution @MPUEX1_PC = %h", MPUPC);
         else  $display("Passed for SHU1 Condtion Execution @MPUEX1_PC = %h", MPUPC);
       end else begin
         if((`SHU0Code[27:25] != 0) | (`SHU1Code[27:25] != 0))  $display("Error for  SHU Condtion Execution @MPUEX1_PC = %h", MPUPC);
         else                                     $display("Passed for SHU Condtion Execution @MPUEX1_PC = %h", MPUPC);         
       end
       
  endtask

  task automatic CheckBIUCondExe(input int MPUPC, input bit BIU0TrueExe, input bit BIU1TrueExe, input bit BIU2TrueExe);
     wait(`MPU_EX1_PC == MPUPC);
     fork
       if(BIU0TrueExe)begin
         if(`BIU0Code[19:17] == 0)  $display("Error for  BIU0 Condtion Execution @MPUEX1_PC = %h", MPUPC);
         else                $display("Passed for BIU0 Condtion Execution @MPUEX1_PC = %h", MPUPC);
       end else if(BIU1TrueExe) begin
         if(`BIU1Code[19:17] == 0)  
               $display("Error for  BIU1 Condtion Execution @MPUEX1_PC = %h", MPUPC);
         else  $display("Passed for BIU1 Condtion Execution @MPUEX1_PC = %h", MPUPC);
       end else if(BIU2TrueExe) begin
         if(`BIU2Code[19:17] == 0)  
               $display("Error for  BIU2 Condtion Execution @MPUEX1_PC = %h", MPUPC);
         else  $display("Passed for BIU2 Condtion Execution @MPUEX1_PC = %h", MPUPC);
       end else begin
         if((`BIU0Code[19:17] != 0) | (`BIU1Code[19:17] != 0) | (`BIU2Code[19:17] != 0))  $display("Error for  BIU Condtion Execution @MPUEX1_PC = %h", MPUPC);
         else                                                        $display("Passed for BIU Condtion Execution @MPUEX1_PC = %h", MPUPC);         
       end
     join_none    
  endtask

  task automatic CheckMACCCondExe(input int MPUPC, input bit IALUTrueExe, input bit IMACTrueExe, input bit FALUTrueExe, input bit FMACTrueExe);
     wait(`MPU_EX1_PC == MPUPC);
     fork
       if(IALUTrueExe)begin
         if(`IALUCode[30:26] == 0)  $display("Error for  IALU Condtion Execution @MPUEX1_PC = %h", MPUPC);
         else                $display("Passed for IALU Condtion Execution @MPUEX1_PC = %h", MPUPC);
       end else if(IMACTrueExe) begin
         if(`IMACCode[28:25] == 0)  
               $display("Error for  IMAC Condtion Execution @MPUEX1_PC = %h", MPUPC);
         else  $display("Passed for IMAC Condtion Execution @MPUEX1_PC = %h", MPUPC);
       end else if(FALUTrueExe) begin
         if(`FALUCode[25:21] == 0)  
               $display("Error for  FALU Condtion Execution @MPUEX1_PC = %h", MPUPC);
         else  $display("Passed for FALU Condtion Execution @MPUEX1_PC = %h", MPUPC);
       end else if(FMACTrueExe) begin
         if(`FMACCode[23:21] == 0)  
               $display("Error for  FMAC Condtion Execution @MPUEX1_PC = %h", MPUPC);
         else  $display("Passed for FMAC Condtion Execution @MPUEX1_PC = %h", MPUPC);
       end else begin
         if((`IALUCode[30:26] != 0) | (`IMACCode[28:25] != 0) | (`FALUCode[25:21] != 0) | (`FMACCode[23:21] != 0))  $display("Error for  MACC Condtion Execution @MPUEX1_PC = %h", MPUPC);
         else                                                        $display("Passed for MACC Condtion Execution @MPUEX1_PC = %h", MPUPC);         
       end
     join_none    
  endtask

  `undef MPU_EX1_PC
endprogram

