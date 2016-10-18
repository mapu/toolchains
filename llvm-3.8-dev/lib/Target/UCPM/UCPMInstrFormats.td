//===-- UCPMInstrFormats.td - UCPM Instruction Formats ---*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
def KIReg : Operand<i32> {
  let PrintMethod = "printKIReg";
}
def Label : Operand<i32> {
  let DecoderMethod = "DecodeLabel";
}
def Flags : Operand<i32> {
  let PrintMethod = "printFlags";
  let DecoderMethod = "DecodeFlags";
}
def IPath : Operand<i32> {
  let PrintMethod = "printIPath";
}
def MACCTI : Operand<i32> {
  let PrintMethod = "printMACCTI";
  let MIOperandInfo = (ops MACC, TPort, IPath);
  let EncoderMethod = "getMACCTIEncoding";
  let DecoderMethod = "DecodeMACCTI";
}
def MACCT : Operand<i32> {
  let PrintMethod = "printMACCTI";
  let MIOperandInfo = (ops MACC, TPort);
  let EncoderMethod = "getMACCTEncoding";
  let DecoderMethod = "DecodeMACCTI";
}
def imm32i : Operand<i32>;



def MR0DestT : Operand<i32> {
  //let PrintMethod = "printMR0DestT";
  let MIOperandInfo = (ops MR0Dest, TPort);
  let EncoderMethod = "getMR0DestTEncoding";
  //let DecoderMethod = "DecodeMR0DestT";  
}
def MR1DestT : Operand<i32> {
  //let PrintMethod = "printMR0DestT";
  let MIOperandInfo = (ops MR1Dest, TPort);
  let EncoderMethod = "getMR1DestTEncoding";
  //let DecoderMethod = "DecodeMR0DestT";  
}
def MR2DestT : Operand<i32> {
  //let PrintMethod = "printMR0DestT";
  let MIOperandInfo = (ops MR2Dest, TPort);
  let EncoderMethod = "getMR2DestTEncoding";
  //let DecoderMethod = "DecodeMR0DestT";  
}
def MR3DestT : Operand<i32> {
  //let PrintMethod = "printMR0DestT";
  let MIOperandInfo = (ops MR3Dest, TPort);
  let EncoderMethod = "getMR3DestTEncoding";
  //let DecoderMethod = "DecodeMR0DestT";  
}
def MR4DestT : Operand<i32> {
  //let PrintMethod = "printMR0DestT";
  let MIOperandInfo = (ops MR4Dest, TPort);
  let EncoderMethod = "getMR4DestTEncoding";
  //let DecoderMethod = "DecodeMR0DestT";  
}
def MR5DestT : Operand<i32> {
  //let PrintMethod = "printMR0DestT";
  let MIOperandInfo = (ops MR5Dest, TPort);
  let EncoderMethod = "getMR5DestTEncoding";
  //let DecoderMethod = "DecodeMR0DestT";  
}



def SHUT : Operand<i32> {
  let PrintMethod = "printSHUT";
  let MIOperandInfo = (ops SHU, TPort);
  let EncoderMethod = "getSHUTEncoding";
  let DecoderMethod = "DecodeSHUT";
}
def BIUO: Operand<i32> {
  //let DecoderMethod = "DecodeBIURegisterClass";
  let MIOperandInfo = (ops BIU, TPort);
  let EncoderMethod = "getBIUTEncoding";
}


// ducx
def BIU0DestTE : Operand<i32> {
  //let PrintMethod = "printBIU0DestTE";
  let MIOperandInfo = (ops BIU0Dest, TEPort);
  let EncoderMethod = "getBIU0DestTEEncoding";
  //let DecoderMethod = "DecodeBIU0DestTE";  
}
def BIU1DestTE : Operand<i32> {
  //let PrintMethod = "printBIU1DestTE";
  let MIOperandInfo = (ops BIU1Dest, TEPort);
  let EncoderMethod = "getBIU1DestTEEncoding";
  //let DecoderMethod = "DecodeBIU1DestTE";  
}
def BIU2DestTE : Operand<i32> {
  //let PrintMethod = "printBIU2DestTE";
  let MIOperandInfo = (ops BIU2Dest, TEPort);
  let EncoderMethod = "getBIU2DestTEEncoding";
  //let DecoderMethod = "DecodeBIU2DestTE";  
}

def SHU0DestTI : Operand<i32> {
  //let PrintMethod = "printSHU0DestT";
  let MIOperandInfo = (ops SHU0Dest, TPort, IPath);
  let EncoderMethod = "getSHU0DestTIEncoding";
  //let DecoderMethod = "DecodeSHU0DestT";  
}

def SHU1DestTI : Operand<i32> {
  //let PrintMethod = "printSHU1DestT";
  let MIOperandInfo = (ops SHU1Dest, TPort, IPath);
  let EncoderMethod = "getSHU1DestTIEncoding";
  //let DecoderMethod = "DecodeSHU1DestT";  
}

def SHU2DestTI : Operand<i32> {
  //let PrintMethod = "printSHU2DestT";
  let MIOperandInfo = (ops SHU2Dest, TPort, IPath);
  let EncoderMethod = "getSHU2DestTIEncoding";
  //let DecoderMethod = "DecodeSHU2DestT";  
}


def MRegO: Operand<i32> {
  //let DecoderMethod = "DecodeMRegO";
  let MIOperandInfo = (ops MReg);
  let EncoderMethod = "getMRegOEncoding";
}


/* a common instruction template base for UCPM */
class UCPMInst<dag ins, dag outs,
                   string asmstr,
                   list<dag> pattern = []> : Instruction {
  let Namespace = "UCPM";
  let OutOperandList = outs;
  let InOperandList = ins;
  let Pattern = pattern;
  let AsmString = asmstr;
  let Size = 6;
  let DecoderNamespace = "UCPMDecode";
  field bits<48> Inst;
  field bits<48> SoftFail;
}
class CtrlInstr<string header, string ctrl>
        : UCPMInst<(ins), (outs),
                       !strconcat(header, ".", ctrl)> {}

/* instruction template for the class of assignment instructions */

// ducx start ---------------------------------------------------------------------
class CommonAssignInstr<dag ins, dag outs, 
                        string header, string rclause, string assign, string lclause,
                        list<dag> pattern = []>
        : UCPMInst<ins, outs,
                       !strconcat(header, lclause, " ", assign, " ", rclause),
                       pattern>;

class AssignInstr<dag ins, string header, string lclause, Operand DestTy,
                  list<dag> pattern = []>
        : CommonAssignInstr<ins, (outs DestTy: $d, Condition: $mode),
                            header, "$d$mode", "->", lclause, pattern> {
  bits<8>  d;
  bits<2> mode;
}

class BIUAssignInstr<dag ins, string header, string lclause, Operand DestTy,
                  list<dag> pattern = []>
        : CommonAssignInstr<ins, (outs DestTy: $d, Condition: $mode),
                            header, "$d$mode", "->", lclause, pattern> {
  bits<6>  d;
  bits<2> mode;
}

class MRegAssignInstr<dag ins, string header, string lclause, Operand DestTy,
                  list<dag> pattern = []>
        : CommonAssignInstr<ins, (outs DestTy: $d, Condition: $mode),
                            header, "$d$mode", "->", lclause, pattern> {
  bits<8>  d;
  bits<2> mode;
}

class BinaryInstr<string header, bit mode,
                  RegisterClass OprtorTy, Operand DestTy>
        : AssignInstr<(ins OprtorTy: $op, TPort: $tm, TPort: $tn, Flags: $f),
                      header,
                      !if(mode, "$tm $op $tn$f", "$op($tm, $tn)$f"), DestTy> {
  bits<2>  tm;
  bits<2>  tn;
  bits<32> f;
  bits<8>  op;
}

class IMACBinaryInstr<string header, bit mode,
                  RegisterClass OprtorTy, Operand DestTy>
        : AssignInstr<(ins OprtorTy: $op, TPort: $tm, TPort: $tn, TPort: $tp, Flags: $f, Flags: $ff, Flags: $shift),
                      header,
                      !if(mode, "$tm $op $tn$ff$f", "$op($tm, $tn)$f"), DestTy> {
  bits<2>  tm;
  bits<2>  tn;
  bits<2>  tp;
  bits<32> f;
  bit ff;
  bits<2> shift;
  bits<8>  op;
}

class UnaryInstr<string header, RegisterClass OprtorTy, Operand DestTy>
        : AssignInstr<(ins OprtorTy: $op, TPort: $tm, TPort: $tn, Flags: $f),
                      header,
                      "$op $tm$f", DestTy> {
  bits<2>  tm;
  bits<2>  tn;
  bits<32> f;
  bits<8>  op;
}

class TParaInstr<string header, bit mode, string fop,
                 RegisterClass OprtorTy, Operand DestTy>
        : AssignInstr<(ins OprtorTy: $op, TPort: $tm, TPort: $tn, TPort: $tp, Flags: $f),
                      header,
                      !if(mode, "$op($tm, $tn, $tp)$f",
                          !strconcat(fop, " $tm $op $tn$f")), DestTy> {
  bits<2>  tm;
  bits<2>  tn;
  bits<2>  tp;
  bits<32> f;
  bits<8>  op;
}

class ToMRwoMovInstr<dag ins, string mrassign, string lclause>
        : CommonAssignInstr<ins, (outs Condition: $c), !strconcat("MR ", mrassign, " "),
                            "", "", lclause> {
}

class ToMRwMovInstr<dag ins, string mrassign, string lclause, Operand DestTy>
        : CommonAssignInstr<ins, (outs DestTy: $d, Condition: $c),
                            !strconcat("MR ", mrassign, " "),
                            "$d", "->", lclause> {
  bits<8> d;
}

// ducx end ----------------------------------------------------------------

//yangl
class UCPAssignInstr<dag ins, string header, string lclause, Operand DestTy,
                  list<dag> pattern = []>
        : CommonAssignInstr<ins, (outs DestTy: $d, Condition: $mode),
                            header, "$d$mode", "->", lclause, pattern> {
  bits<4>  d;
  bits<2> mode;
}
class UCPSHUIndexAssignInstr<dag ins, string header, string lclause, Operand DestTy,
                  list<dag> pattern = []>
        : CommonAssignInstr<ins, (outs DestTy: $d),
                            header, "$d", "->", lclause, pattern> {
  //MACCTI: MACC, T, I
  bits<6>  d;
}



class SEQInst<dag ins, 
          string opstr, 
          string label, 
          string rclause>
	: UCPMInst <ins, (outs),
                    !strconcat(opstr, label, " @(", rclause, ")")> {
  let Inst{47-43} = 0;
}
