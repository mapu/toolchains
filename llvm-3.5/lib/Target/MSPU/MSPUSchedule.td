//===- SparcSchedule.td - Sparc Scheduling Definitions -*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
/* Note:
   A chipset is a group of ICs to perform some related functions. 
   Chipset are often the core parts of a motherboard. 
   A 'target' may include one or more chipsets. 
*/

//===----------------------------------------------------------------------===//
// Processor functional unit - These values represent the function units
// available across all chip sets for the target.  Eg., IntUnit, FPUnit, ...
// These may be independent values for each chip set or may be shared across
// all chip sets of the target.  Each functional unit is treated as a resource
// during scheduling and has an effect on instruction order based on availability
// during a time interval.
// Functional Units, i.e., pipeline slots and computing units
def FU_SCU: FuncUnit;
def FU_AGU: FuncUnit;
def FU_SEQ: FuncUnit;
def FU_SYN: FuncUnit;
def FU_IMM: FuncUnit;
def FU_NOP: FuncUnit;

//===----------------------------------------------------------------------===//
/* Itinerary classes.
   An itinerary represents a sequential series of steps
   required to complete an instruction.  Itineraries are represented as lists of
   instruction stages. 
   
   Instruction itinerary class - This value represents a 'named' instruction
   itinerary. Using named itineraries simplifies managing groups of
   instructions across chip sets defined in the target.  An instruction uses 
   the same itinerary class across all chip sets (each chipset corresponds to a 
   subtarget???). Thus a new chip set can be added without modifying instruction 
   information.

   A named itinerary class represents a group of instructions with the
   same itinerary.

   A def of InstrItinClass only assigns the name to an instruction group.
   The detailed information is given by InstrItinData.
*/
def ItinSCU_2_1_1		:	InstrItinClass;
def ItinSCU_4_1_1		:	InstrItinClass;
def ItinSCU_5_1  		:	InstrItinClass;
def ItinSCU_5_1_1		:	InstrItinClass;

def ItinAGU_1_1_1		:	InstrItinClass;
def ItinAGU_ImmExt_1_1_1   : InstrItinClass;
def ItinAGU_1_1  		:	InstrItinClass;
def ItinAGU_2_1	    	:	InstrItinClass;
def ItinAGU_ImmExt_2_1       : InstrItinClass;
def ItinAGU_2_1_1		:	InstrItinClass;
def ItinAGU_5_1_1		:	InstrItinClass;
def ItinAGU_ImmExt_5_1_1   : InstrItinClass;
def ItinAGU_5_1_1_1		:	InstrItinClass;

def ItinSEQ_1	    	:	InstrItinClass;
def ItinSEQ_ImmExt_1       : InstrItinClass;

def ItinSYN_1	    	:	InstrItinClass;
def ItinSYN_1_1	    	:	InstrItinClass;
def ItinSYN_2_1	    	:	InstrItinClass;
def ItinSYN_2	    	:	InstrItinClass;

def ItinSEQNoReg		:	InstrItinClass;
def ItinSEQNoReg_ImmExt    : InstrItinClass;
def ItinSYNNoReg		:	InstrItinClass;

def ItinPseudo  		:	InstrItinClass;

def ItinNop      : InstrItinClass;

//===----------------------------------------------------------------------===//
// Instruction stage - This value represents a non-pipelined step in
// 			   the execution of an instruction.
/*
class InstrStage<int cycles, list<FuncUnit> units,
                 int timeinc = -1,
                 ReservationKind kind = Required> {
  int Cycles          = cycles;       // length of stage in machine cycles
  list<FuncUnit> Units = units;       // choice of functional units
  int TimeInc         = timeinc;      // cycles till start of next stage
  int Kind            = kind.Value;   // kind of FU reservation
}
*/
// 'Cycles' represents the number of discrete time slots needed to 
// 			complete the stage.  
// 'Units' represents the choice of functional units that can be used to complete
// 		   the stage (ie, choose one among many).  Eg. IntUnit1, IntUnit2.
// 'TimeInc' indicates how many cycles should elapse from the start of 
// 			 this stage to the start of the next stage in the itinerary.

// Instruction itinerary data - These values provide a runtime map of an 
// 			   instruction itinerary class (name) to its itinerary data.
/*
class InstrItinData<InstrItinClass Class, list<InstrStage> stages,
                    list<int> operandcycles = [],
                    list<Bypass> bypasses = []> {
  InstrItinClass TheClass = Class;
  list<InstrStage> Stages = stages;
  list<int> OperandCycles = operandcycles;
  list<Bypass> Bypasses = bypasses;
}
*/

// 'OperandCycles' are optional "cycle counts". They specify the cycles (after
// instruction issue) when the values (corresponding to specific operand indices)
// are defined (for output operands) or used (for input operands).

// 'Bypasses' are optional "pipeline forwarding pathes", if
// a specific bypass is available for a def of an instruction and the use 
// of this instruction can read from the same bypass, then the 
// operand use latency is reduced by one.

// Processor itineraries - These values represent the set of all itinerary
// classes for a given chip set. Each chip set (a chip set = a processor )
/*
class ProcessorItineraries<list<FuncUnit> fu, list<Bypass> bp,
                           list<InstrItinData> iid> {
  list<FuncUnit> FU = fu;
  list<Bypass> BP = bp;
  list<InstrItinData> IID = iid;
}
*/
def MSPUItineraries : ProcessorItineraries<
  [FU_SCU, FU_AGU, FU_SEQ, FU_SYN, FU_IMM, FU_NOP], [],
  [InstrItinData<ItinSCU_5_1, [InstrStage<1, [FU_SCU]>], [5,1]>,
   InstrItinData<ItinSCU_2_1_1, [InstrStage<1, [FU_SCU]>], [2,1,1]>,
   InstrItinData<ItinSCU_4_1_1, [InstrStage<1, [FU_SCU]>], [4,1,1]>,
   InstrItinData<ItinSCU_5_1_1, [InstrStage<1, [FU_SCU]>], [5,1,1]>,

   InstrItinData<ItinAGU_1_1, [InstrStage<1, [FU_AGU]>], [1,1,1]>,
   InstrItinData<ItinAGU_2_1, [InstrStage<1, [FU_AGU]>], [2,1]>,
   InstrItinData<ItinAGU_1_1_1, [InstrStage<1, [FU_AGU]>], [1,1,1]>,
   InstrItinData<ItinAGU_2_1_1, [InstrStage<1, [FU_AGU]>], [2,1,1]>,
   InstrItinData<ItinAGU_5_1_1, [InstrStage<1, [FU_AGU]> ], [5,1,1]>,
   InstrItinData<ItinAGU_5_1_1_1, [InstrStage<1, [FU_AGU]>], [5,1,1,1]>,
   InstrItinData<ItinAGU_ImmExt_2_1, [InstrStage<1, [FU_AGU]>, 
                                      InstrStage<1, [FU_IMM]>], [2,1]>,
   InstrItinData<ItinAGU_ImmExt_1_1_1, [InstrStage<1, [FU_AGU]>, 
                                        InstrStage<1, [FU_IMM]>], [1,1,1]>,
   InstrItinData<ItinAGU_ImmExt_5_1_1, [InstrStage<1, [FU_AGU]>, 
                                        InstrStage<1, [FU_IMM]>], [5,1,1]>,

   InstrItinData<ItinSEQ_1, [InstrStage<1, [FU_SEQ]>], [1]>,
   InstrItinData<ItinSEQ_ImmExt_1, [InstrStage<1, [FU_SEQ]>,
                                    InstrStage<1, [FU_IMM]>], [1]>,

   InstrItinData<ItinSEQNoReg, [InstrStage<1, [FU_SEQ]>], []>,
   InstrItinData<ItinSEQNoReg_ImmExt, [InstrStage<1, [FU_SEQ]>,
                                       InstrStage<1, [FU_IMM]>], []>,

   InstrItinData<ItinSYN_1, [InstrStage<1, [FU_SYN]>], [1]>,
   InstrItinData<ItinSYN_2, [InstrStage<1, [FU_SYN]>], [2]>,
   InstrItinData<ItinSYN_1_1, [InstrStage<1, [FU_SYN]>], [1,1]>,
   InstrItinData<ItinSYN_2_1, [InstrStage<1, [FU_SYN]>], [2,1]>,
   InstrItinData<ItinSYNNoReg, [InstrStage<1, [FU_SYN]>], []>,

   InstrItinData<ItinPseudo, [InstrStage<1, [FU_SCU]>, 
                              InstrStage<1, [FU_AGU]>, 
                              InstrStage<1, [FU_SEQ]>, 
                              InstrStage<1, [FU_SYN]>], []>,
   InstrItinData<ItinNop, [InstrStage<1, [FU_SCU, FU_AGU, FU_SEQ, FU_SYN]>], []>
  ]
>;

def MSPUSchedModel : SchedMachineModel {
  // IssueWidth is not for restricting packet size, but is helpful for scheduler to
  // organize a number of independent insts together before VLIW packetizing.
  let IssueWidth = 4;
  let Itineraries = MSPUItineraries;
}
