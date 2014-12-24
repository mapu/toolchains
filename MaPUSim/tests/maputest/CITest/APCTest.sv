//File Name    :  APCTest.sv
//Creating Date:  2013-8-20
//Author       :  tao.wang@ia.ac.cn
//Description  :  TestCase for Bypass
//Last Commit  :  $Id: APCTest.sv 1443 2013-01-14 12:42:52Z yinlz $

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
  
  initial begin
    cEnv =new();
    Mon  =new();

    // Initilize the APE0 Memory: IM and MIMX and MIMY
    APE0InitInstrMem();
    APE0InitDataMem();

    ResetAndStartSPU();        //Start the SPU
    $display("====================================");
     
     fork
       // Run the Verification Environment
       cEnv.run();
     join_none
     
     //R Stall test
     CheckBypassOnPC (32'h0,32'h0,32'h0);
     CheckBypassOnPC (32'h4,32'h1,32'h2);
     CheckBypassOnPC (32'h8,32'h0,32'h0);
     CheckBypassOnPC (32'hC,32'h1,32'h1);
     CheckBypassOnPC (32'h10,32'h0,32'h0);
     CheckBypassOnPC (32'h14,32'h0,32'h0);
     CheckBypassOnPC (32'h18,32'h1,32'h2);
     CheckBypassOnPC (32'h1c,32'h0,32'h0);
     CheckBypassOnPC (32'h20,32'h1,32'h2);
     CheckBypassOnPC (32'h24,32'h0,32'h0);
     CheckBypassOnPC (32'h28,32'h0,32'h0);
     CheckBypassOnPC (32'h2c,32'h0,32'h0);
     CheckBypassOnPC (32'h30,32'h0,32'h0);
     CheckBypassOnPC (32'h34,32'h0,32'h0);
     CheckBypassOnPC (32'h38,32'h1,32'h2);
     CheckBypassOnPC (32'h40,32'h1,32'h1);

     CheckSROnPC(32'h58, 5,  32'h1,2); 
     CheckSROnPC(32'h5c, 6,  32'h8,2); 
     CheckSROnPC(32'h68, 7,  32'h1,2); 
     CheckSROnPC(32'h6c, 8,  32'h8,2); 
     CheckSROnPC(32'h78, 9,  32'h1,2); 
     CheckSROnPC(32'h7c, 10, 32'h8,2); 
     CheckSROnPC(32'h88, 11, 32'hffffffff,2); 
     CheckSROnPC(32'h8c, 12, 32'h8,2); 

     CheckSROnPC(32'ha8, 5,  32'hffffffff,2); 
     CheckSROnPC(32'hac, 6,  32'h3,2); 
     CheckSROnPC(32'hb8, 7,  32'hffffffff,2); 
     CheckSROnPC(32'hbc, 8,  32'h3,2); 
     CheckSROnPC(32'hc8, 9,  32'hffffffff,2); 
     CheckSROnPC(32'hcc, 10, 32'h3,2); 
     CheckSROnPC(32'hd8, 11, 32'h0,2); 
     CheckSROnPC(32'hdc, 12, 32'h3,2); 


     
    
   
    

    #160;

    WaitSPUCycles(10);
    $display("====================================");       
       
    // count the coverage of the CoverGroup
    cEnv.wrap_up();

  end

 ///////////////////////////////////////////////
  //Chechk the resiter value. 
  `define DC_PC  $root.TestTop.uAPC.uAPE0.uSPU.uDecode.iCodeLinePCInDP
  `define BYPASS_STALL $root.TestTop.uAPC.uAPE0.uSPU.uBypass.oDPStall
  task automatic CheckBypassOnPC( input int PC, input int StallValid, input int StallCycle);
    int i ;    
  
    wait(PC == `DC_PC );
    fork: CHECK_STALL_DELAY
      begin
        @(negedge iSPUClk ) begin
          if(`BYPASS_STALL != StallValid)   // not equal, report error at once
            $display( "@time %0t Error @DC_PC = %h, ExpectedStall =%0h , ResultStall =%0h", $time, PC, StallValid, `BYPASS_STALL);

          else if(StallValid == 1) begin   // Stalled and check if has the right stalled cycles
            if (StallCycle == 1) begin     // expected: 1 cycles
              @ (negedge iSPUClk) begin    // the next cycle
                if((`BYPASS_STALL == 1) && (PC == `DC_PC))  // if BYPASS_STALL=1 and the PC doesn't change, report error.
                  $display( "@time %0t Error @DC_PC = %h, ExpectedStall =%0h , ResultStall =%0h", $time, PC, !StallValid,`BYPASS_STALL); 
                else $display("@time %0t, @DC_PC=%h,success!",$time,PC);                 // BPASS_STALL=0,or PC change change, report success
              end 
            end
            else begin                     // stalled more than 2 cycles
              for(i=0; i<StallCycle-1; i=i+1) begin 
                @ (negedge iSPUClk) begin  // the next cycle
                  if((`BYPASS_STALL == 0) || (PC != `DC_PC))     // BYPASS_STALL !=1 or PC != DC_PC 
                      $display( "@time %0t Error @DC_PC = %h, ExpectedStall =%0h , ResultStall =%0h", $time, PC, StallValid, `BYPASS_STALL); 
                  else                        $display("@time %0t, @DC_PC=%h,success!",$time,PC);
                end
              end
            end
          end else  $display("@time %0t, @DC_PC=%h,success!",$time,PC);
        end        
      end
    join_none:CHECK_STALL_DELAY
  endtask

  ///////////////////////////////////////////////
  //Chechk the resiter value. 
  `define EX1_PC  $root.TestTop.uAPC.uAPE0.uSPU.uFetch.rRRPC
  task automatic CheckSROnPC( input int PC, input int RegID, input int Value, input int WBStage=2);
    int Result;
    
    wait(PC == `EX1_PC );
    fork: CHECK_SR_DELAY
      begin
        WaitSPUCycles(WBStage);
        Mon.R_Read(RegID, Result); 
        if( Result != Value ) $display( "@time %0t Error @EX1_PC = %h, Expected =%h , Result =%h", $time, PC, Value, Result);
        else $display("@time %0t, @EX1_PC = %h, success!",$time,PC);
      end
    join_none:CHECK_SR_DELAY
  endtask

  task automatic CheckDROnPC( input int PC, input int RegID, input longint Value, input int WBStage=2);
    int Result[2];   
    
    wait(PC == `EX1_PC );
    fork: CHECK_DR_DELAY
      begin
        WaitSPUCycles(WBStage);
        Mon.R_Read(RegID<<1,   Result[0]); 
        Mon.R_Read((RegID<<1)+1, Result[1]);
        if( (Result[0] != Value[31:0] )  || (Result[1] != Value[63:32] ) ) 
          $display( "@time %0t Error @EX1_PC = %h, Expected =%h , Result =%h_%h", 
                    $time,              PC, Value,            Result[1],Result[0]);
        else $display("@time %0t, @EX1_PC = %h, success!",$time,PC);
      end
    join_none:CHECK_DR_DELAY
  endtask

  `undef EX1_PC

   `undef BYPASS_STALL
   `undef DC_PC   
endprogram
