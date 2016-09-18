//===-- UCPSInstrFormats.td - UCPS Instruction Formats ---*- tablegen -*-===//
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
def IPath : Operand<i32> {
  let PrintMethod = "printIPath";
}
def Flags : Operand<i32> {
  let PrintMethod = "printFlags";
  let DecoderMethod = "DecodeFlags";
}
def MACCTI : Operand<i32> {
  let PrintMethod = "printMACCTI";
  let MIOperandInfo = (ops MACC, TPort, IPath);
  let EncoderMethod = "getMACCTIEncoding";
  let DecoderMethod = "DecodeMACCTI";
}
def SHUT : Operand<i32> {
  let PrintMethod = "printSHUT";
  let MIOperandInfo = (ops SHU, TPort);
  let EncoderMethod = "getSHUTEncoding";
  let DecoderMethod = "DecodeSHUT";
}
def MRegO: Operand<i32> {
  let MIOperandInfo = (ops MReg);
  let DecoderMethod = "DecodeMRegO";
  let EncoderMethod = "getMRegOEncoding";
}
def BIUO: Operand<i32> {
  let DecoderMethod = "DecodeBIURegisterClass";
  let MIOperandInfo = (ops BIU);
}

def imm32i : Operand<i32>;

/* a common instruction template base for UCPS */
class UCPSInst<dag ins, dag outs,
                   string asmstr,
                   list<dag> pattern = []> : Instruction {
  let Namespace = "UCPS";
  let OutOperandList = outs;
  let InOperandList = ins;
  let Pattern = pattern;
  let AsmString = asmstr;
  let Size = 6;
  let DecoderNamespace = "UCPSDecode";
  field bits<48> Inst;
  field bits<48> SoftFail;
}

class CtrlInstr<string header, string ctrl>
        : UCPSInst<(ins), (outs),
                       !strconcat(header, ".", ctrl)> {}

/* instruction template for the class of assignment instructions */
class CommonAssignInstr<dag ins, dag outs, 
                        string header, string rclause, string assign, string lclause,
                        list<dag> pattern = []>
        : UCPSInst<ins, outs,
                       !strconcat(header, lclause, " ", assign, " ", rclause),
                       pattern>;

class AssignInstr<dag ins, string header, string lclause, Operand DestTy,
                  list<dag> pattern = []>
        : CommonAssignInstr<ins, (outs DestTy: $d, Condition: $c),
                            header, "$d$c", "->", lclause, pattern> {
  bits<9>  d;
  bits<2>  c;
}

class ToMRwoMovInstr<dag ins, string mrassign, string lclause>
        : CommonAssignInstr<ins, (outs Condition: $c), !strconcat("MR ", mrassign, " "),
                            "$c", "", lclause> {
  bits<2> c;
}

class ToMRwMovInstr<dag ins, string mrassign, string lclause, Operand DestTy>
        : CommonAssignInstr<ins, (outs DestTy: $d, Condition: $c),
                            !strconcat("MR ", mrassign, " "),
                            "$d$c", "->", lclause> {
  bits<9> d;
  bits<2> c;
}


class TParaInstr<string header, bit mode, string fop,
                 RegisterClass OprtorTy, Operand DestTy>
        : AssignInstr<(ins OprtorTy: $op, TPort: $tm, TPort: $tn, TPort: $tp, Flags: $f),
                      header,
                      !if(mode, "$op($tm, $tn, $tp)$f",
                          !strconcat(fop, " $tm $op $tn$f")), DestTy> {
  bits<4>  tm;
  bits<4>  tn;
  bits<2>  tp;
  bits<32> f;
  bits<8>  op;
}

class BinaryInstr<string header, bit mode,
                  RegisterClass OprtorTy, Operand DestTy>
        : AssignInstr<(ins OprtorTy: $op, TEPort: $tm, TEPort: $tn, TPort: $tp, Flags: $f),
                      header,
                      !if(mode, "$tm $op $tn$f", "$op($tm, $tn)$f"), DestTy> {
  bits<4>  tm;
  bits<4>  tn;
  bits<2>  tp;
  bits<32> f;
  bits<8>  op;
}

class UnaryInstr<string header, RegisterClass OprtorTy, Operand DestTy>
        : AssignInstr<(ins OprtorTy: $op, TPort: $tm, TEPort: $tn, TPort: $tp, Flags: $f),
                      header,
                      "$op $tm$f", DestTy> {
  bits<4>  tm;
  bits<4>  tn;
  bits<2>  tp;
  bits<32> f;
  bits<8>  op;
}

class SEQInst<dag ins, 
          string opstr, 
          string label, 
          string rclause>
	: UCPSInst <ins, (outs),
                    !strconcat(opstr, label, " @(", rclause, ")")> {
  let Inst{47-44} = 13;
}

