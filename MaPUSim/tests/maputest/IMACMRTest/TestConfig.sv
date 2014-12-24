//File Name: TestConfig.sv
//Creating Date: 2013-01-23
//Creating Author: tao.wang@ia.ac.cn
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

`define  SPUClk      $root.TestTop.iCvSmpIF.SPUCB
`define  APE0SPUPath $root.TestTop.uAPC.uAPE0.uSPU
// 1: define the Covergroup and Coverpoints
class Coverage;
  //--Declare the class to be Covered
  covergroup CovGrp;
  //-------------------------------
  //---add your coverpoint below---//

  // add your comment of the covergroup
 

 

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
`undef SPUClk
`undef APE0SPUPath

