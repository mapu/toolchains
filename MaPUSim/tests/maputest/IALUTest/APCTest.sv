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
    // basic IALU
    CheckTReg(32'h9,3'b001,3'b00,512'h11111021_11111020_1111101f_1111101e_1111101d_1111101c_1111101b_1111101a_11111019_11111018_11111017_11111016_11111015_11111014_11111013_11111012,32'h3);
    CheckTReg(32'ha,3'b001,3'b01,512'heeeeedff_eeeeedfe_eeeeedfd_eeeeedfc_eeeeedfb_eeeeedfa_eeeeedf9_eeeeedf8_eeeeedf7_eeeeedf6_eeeeedf5_eeeeedf4_eeeeedf3_eeeeedf2_eeeeedf1_eeeeedf0,32'h3);
    CheckTReg(32'hb,3'b001,3'b10,512'hffffff10_ffffff0f_ffffff0e_ffffff0d_ffffff0c_ffffff0b_ffffff0a_ffffff09_ffffff08_ffffff07_ffffff06_ffffff05_ffffff04_ffffff03_ffffff02_ffffff01,32'h3);
    CheckTReg(32'hc,3'b001,3'b11,512'h11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111,32'h3);
    CheckTReg(32'hd,3'b001,3'b00,512'h11111201_11111202_11111203_11111204_11111205_11111206_11111207_11111208_11111209_1111120a_1111120b_1111120c_1111120d_1111120e_1111120f_11111210,32'h3);
    CheckTReg(32'he,3'b001,3'b01,512'h11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111,32'h3);
    CheckTReg(32'hf,3'b001,3'b10,512'h11111111_ffffff0f_11111111_ffffff0d_11111111_ffffff0b_11111111_ffffff09_11111111_ffffff07_11111111_ffffff05_11111111_ffffff03_11111111_ffffff01,32'h3);
   
    //logic test
    CheckTReg(32'h10,3'b010,3'b00,512'h11111110_11111101_11111100_11111101_11111100_11111101_11111100_11111101_11111100_11111101_11111100_11111101_11111100_11111101_11111100_11111101,32'h3);
    CheckTReg(32'h11,3'b010,3'b01,512'hffffff11_ffffff1f_ffffff1f_ffffff1d_ffffff1d_ffffff1b_ffffff1b_ffffff19_ffffff19_ffffff17_ffffff17_ffffff15_ffffff15_ffffff13_ffffff13_ffffff11,32'h3);
    CheckTReg(32'h13,3'b010,3'b11,512'heeeeeeee_eeeeeeee_eeeeeeee_eeeeeeee_eeeeeeee_eeeeeeee_eeeeeeee_eeeeeeee_eeeeeeee_eeeeeeee_eeeeeeee_eeeeeeee_eeeeeeee_eeeeeeee_eeeeeeee_eeeeeeee,32'h3);
   
    //shift test
    CheckTReg(32'h14,3'b001,3'b00,512'h04444444_04444444_04444444_04444444_04444444_04444444_04444444_04444444_04444444_04444444_04444444_04444444_04444444_04444444_04444444_04444444,32'h3);
    CheckTReg(32'h15,3'b001,3'b01,512'h44444444_44444444_44444444_44444444_44444444_44444444_44444444_44444444_44444444_44444444_44444444_44444444_44444444_44444444_44444444_44444444,32'h3);
    CheckTReg(32'h16,3'b001,3'b10,512'h04444444_04444444_04444444_04444444_04444444_04444444_04444444_04444444_04444444_04444444_04444444_04444444_04444444_04444444_04444444_04444444,32'h3);
    CheckTReg(32'h17,3'b001,3'b11,512'h44444444_44444444_44444444_44444444_44444444_44444444_44444444_44444444_44444444_44444444_44444444_44444444_44444444_44444444_44444444_44444444,32'h3);
    
    //compare test
    CheckTReg(32'h18,3'b010,3'b00,512'h00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000,32'h3);
    CheckTReg(32'h19,3'b010,3'b01,512'h00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000,32'h3);
    CheckTReg(32'h1a,3'b010,3'b10,512'h00000001_00000001_00000001_00000001_00000001_00000001_00000001_00000001_00000001_00000001_00000001_00000001_00000001_00000001_00000001_00000001,32'h3);
    CheckTReg(32'h1b,3'b010,3'b11,512'h01010100_01010100_01010100_01010100_01010100_01010100_01010100_01010100_01010100_01010100_01010100_01010100_01010100_01010100_01010100_01010100,32'h3);
    
    

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
