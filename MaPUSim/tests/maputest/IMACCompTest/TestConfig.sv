//File Name: TestConfig.sv
//Creating Date: 2012-12.3
//Creating Author: leizu.yin@ia.ac.cn
//Description: The Verification Informatino Configuration definition for Testing, including: Verification interface, Coverage Coverpoint
//Last Commit: $Id$
//

// define the clocking block
interface CovSampleIF(input  iSoCClk,
                      input  iSPUClk,
                      input  iMPUClk
);

  clocking SoCCB @(posedge iSoCClk);  endclocking
  clocking SPUCB @(posedge iSPUClk);  endclocking
  clocking MPUCB @(posedge iMPUClk);  endclocking
  
endinterface

// 1: define the Covergroup and Coverpoints
`define  SPUPath0 $root.TestTop.uAPC.uAPE0.uSPU

class Coverage;
  //--Declare the class to be Covered
  covergroup CovGrp;
  //-------------------------------
  //---add your coverpoint below---//

  // add your comment of the covergroup
  type_option.comment = "CovGrp:MRegTest";



  //----end of adding coverpoint---//
  //-------------------------------//
  endgroup:CovGrp
  
  function new;
  CovGrp = new();
  endfunction

  extern task run();

endclass
task Coverage::run();

    while(1) begin
      // 2: decide the sample condition. default is at the posedge of SPUClk
      @($root.TestTop.iCvSmpIF.SPUCB) CovGrp.sample();

    end
endtask
`undef SPUPath0
