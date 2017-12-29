# Simulator
Our simulator is based on [Gem5](http://www.gem5.org/Main_Page). 

### usage
The Simulator binary is under **\<install-dir\>/simulator/apc/gem5.opt**. After compiled the source code into *elf* file,
we can use following simple command line to run the simulator.
For example, in **example/1.Hello\_MaPU/**, we can use follow command to generate the *elf* file:
```bash
/home/xiesl/sdk/apc/bin/clang -target mspu Hello_MaPU.c -g -o Hello_MaPU.elf
```
and then use following command to run the simulation:
```bash
gem5.opt <install-dir>/simulator/apc/system/se.py -c Hello_MaPU.elf
```
* **\<install-dir\>/simulator/apc/system/se.py** is the configuration file

### source code base
The original source code is a stable version with tag **stable_2012_06_28**, which can be downloaded using follow commands:
```bash
hg clone http://repo.gem5.org/gem5
cd gem5; hg chekcout -r f75ee4849
```
### Adding a new target in Gem5
For general description of how to add a new ISA in Gem5, please refer to this tutorial:
[Defining ISA](http://www.gem5.org/Defining_ISAs_(as_of_M5_2.0_beta_3))
In our implementation, we modified following files in Gem5 to add the MaPU CPU model:
1. **MaPUSim/APC/build\_opts/MAPU**
```text
TARGET_ISA = 'mapu'
CPU_MODELS = 'InOrderCPU'
PROTOCOL = 'MI_example'
```
The *PROTOCOL* is the used in cache coherence, which is not actully used in our implementation

2. **MaPUSim/APC/utils/compile**

Added the **mapu** isa as other archtectures

3. **/src/cpu/BaseCPU.py**

Added the TLB type in MaPU. 

## Implement the simulator
For how to construct a simulation system (including how to configure system, how to add caches and memory controllers) 
please refer to this tutorial:[Learning gem5](http://learning.gem5.org)

In this section, we will focuses only on how to define the ISA using the Gem5 description languages. 
More reference can be found on the gem5 wiki page: [ISA Parser](http://www.gem5.org/ISA_Parser)

### decoding process
The main purpose of the simulator is to define the *behavior* for the corresponding instruction encoding.
For example, for instruction "R0 = R1 + R2", the *behavior* should be adding R1 and R2, and then write the result
back to R0.

In Gem5 ISA laugage, this is implemented using **decode** keyword:
```text
decode BIT_FILED_NAME {
    format FORMAT_NAME {
{% raw %} 0x0 : add_INST( {{ Rc = Ra + Rb ; }} );
          0x1 : sub_INST( {{ Rc = Ra - Rb ; }} ); {% endraw %}
    }
}
``` 
* **BIT\_FIELD\_NAME:** The bit range of the instruction encoding, defined with **def bitfield** keywords
* **FORMAT\_NAME:** The instruction format, indicates a specific type of the instructions 
(how many operands needed etc). Defined with **def format** keyword 
* **xx\_INST:** The name of the corresponding instruction.
* **\{\{ C++\_CODE \}\}:** This field is the C++ code that descripts the behavior of the instruciton.

The decoding process can be nested, for example :
```text
0x1:decode OPCODE_LO {
  0x1:decode SD {
    format IntCIOp { {% raw %}
      0x0: fixadd( {{ C++ Code }} ); {% endraw %}
    }
  }
}
```
### def bitfield 
This keyword give a name to the specific bit range of the instruction, which can be referred in the decoding process. 
For example:
```text
def bitfield OPCODE <31:26>;
def bitfield IMM <12>;
```
### def operands
This keyword defines the operands that used in the behavior code. 
```text
{% raw %} def operands {{
    'Ra': ('IntReg', 'uq', 'RA', 'IsInteger', 1),
    'Rb': ('IntReg', 'uq', 'RB', 'IsInteger', 2),
    'Rc': ('IntReg', 'uq', 'RC', 'IsInteger', 3),
}} {% endraw %}
```
Each operand contains following fields:
* **name:** for example, the 'Ra'
* **operand class:** for example, 'IntReg'. Can be one of this:
    * IntReg
    * FloatReg
    * Mem
    * NPC
    * ControlReg
* **defaul data type:** one of the pre-defined data type, which is defined with **def operand\_types**:

```text
{% raw %} def operand_types {{
    'sb' : 'int8_t',
    'ub' : 'uint8_t',
    ...
}} {% endraw %}
```
* **bitfield:** The corresponding bitfield name
* **flags:** a string or triple of strings indicating the instruction flags that can be inferred when the operand is used
* **priority:** used in disassembly

### def format
An instruction format is basically a Python function. 
It takes the arguments supplied by an instruction definition that defined by the aformentioned **decode** keywords.
and generates up to four pieces of C++ code.
* **header_output:** Goes to **decoder.hh**, which is typically the C++ class declarations of the instruction
* **decode_output:** Goes to **decoder.cc**, contains the code that do not visible to **execute()** function
* **exec_output:**  contains per-CPU model definition
* **decode_block:** contains the a statement or block of statements that go into the decode function 

An example of the **def format** is shown in following:
```python
{% raw %} def format IntOp(code, *opt_flags) {{
    iop = InstObjParams(name, Name, 'IntOp', code, opt_flags)
    header_output = BasicDeclare.subst(iop)
    decoder_output = BasicConstructor.subst(iop)
    decode_block = RegNopCheckDecode.subst(iop)
    exec_output = BasicExecute.subst(iop)
}}; {% endraw %}
```

The **InstObjParams** is an python class that contains following fields:
* **mnemonic** The mnemonic name for this format
* **class\_name** The C++ class name for this format. *'IntOp'* in the above example
* **snippets** The C++ behavior code of the instruction that defined in **decode** block, *'code'* in the above example
* **opt_args** Additional parameters, *'opt_flags'* in the above example

The **BasicDeclare**, **BasicConstructor**, **BasicExecute** are pre-defined **template**

### def template
A template is used to define a framework that generates C++ code. It takes the parameter **InstObjParams**,
and return the C++ code pieces. 
For example:
```python
{% raw %} def template BasicDecode {{
    return new %(class_name)s(machInst);
}} {% endraw %}
```
The content of the **template** is actally python code, and can use the parameters defined in **InstObjParams**. 
In the above example, if the **class_name** is **IntOp**, the return string will be:
```c++
    return new IntOp( machInst );
```
## ISA description structure
The MaPU ISA description are show in the following:
```text
src/arch/mapu/isa/
   bitfield.isa : definitions of the bitfield
   operands.isa : definitions of the operands
   sdecoder.isa : definitions of the decode process
   formats/
        basic.isa: The template definition 
        int.isa  : The SCU format defintion 
        ...      : The format definition of other FUs
```
Next we will take a tour of how a SCU instruction (R0 = R1 + R2) is implemented in Gem5. 
1. **Decoding**  defined in *sdecoder.isa*
```text
decode OPCODE_HI default Unknown::unknown() {
    ...
    0x1:decode OPCODE_LO {
      0x1:decode SD {
        format IntCIOp { {% raw %}
          0x0: fixadd( {{ 
            if(SCU_U){ ... }
            else{
              uint64_t i, j, k;
              i = Rs;
              j = Rt;
              k = i + j;
              CI = (k >> 32) ;
              int64_t a, b, c;
              a = (int32_t)Rs;
              b = (int32_t)Rt;
              c = a + b;
              if(SCU_T) Rd = c;                             // Rs = Rm + Rn(T)
              else      Rd = c > MAX_INT32 ? MAX_INT32 :    // Rs = Rm + Rn
                             c < MIN_INT32 ? MIN_INT32 : c;
            }
          }} {% endraw %}); 
        }
      }
    }
}
```
2. **OPCODE\_HI,OPCODE\_LO, SD, SCU\_T, SCU\_U**: in *bitfield.isa*
```text
 def bitfield OPCODE_HI  <30:28>;
 def bitfield OPCODE_LO  <27:23>;
 def bitfield SD         <20:19>;
 def bitfield SCU_T      <18:18>;
 def bitfield SCU_U      <21:21>;
```
3. **Rs, Rt, Rd, CI**: in *operands.isa*
```text
 'Rd': ('IntReg', 'uw', 'RD', 'IsInteger', 3),
 'Rs': ('IntReg', 'uw', 'RS', 'IsInteger', 2),
 'Rt': ('IntReg', 'uw', 'RT', 'IsInteger', 3),
 'CI': ('ControlReg', 'ud', 'MISCREG_CI', None,1),
```
4. **format IntCIOp**: Defined in *formats/int.isa*
```text
{% raw %} def format IntCIOp(code, *opt_flags) {{
     iop = InstObjParams(name, Name, 'IntCIOp', code, opt_flags)
     header_output = BasicDeclare.subst(iop)
     decoder_output = BasicConstructor.subst(iop)
     decode_block = RegNopCheckDecode.subst(iop)
     exec_output = BasicExecute.subst(iop)
 }}; {% endraw %}
```
The BasiceDeclare, BasicConstructor, and BasicExecute can be found in *formats/basic.isa*, 
the RegNopCheckDecode can be found in *formats/noop.isa*, which is the same with BasicDecode.

5. **Generated source file** Can be found in *build/MAPU/arch/mapu/generated*
    * **decoder.hh:** containts the class definition of the **fixadd**

    ```c++
    class Fixadd : public IntCIOp {
    public:
        //Constructor.
        Fixadd(ExtMachInst machInst);
    
        Fault execute(CheckerCPU *, Trace::InstRecord *) const;
    
        Fault execute(InOrderDynInst *, Trace::InstRecord *) const;
    
    };
    ```

    * **decoder.cc:** contains the decoding process and the the class implementation:

    ```c++
     inline Fixadd::Fixadd(ExtMachInst machInst)  : IntCIOp("fixadd", machInst, IntAluOp) {
     _destRegIdx[0] = MISCREG_CI + Ctrl_Base_DepTag;
     _srcRegIdx[0] = RS;
     _srcRegIdx[1] = RT;
     _destRegIdx[1] = RD;
     _numSrcRegs = 2;
     _numDestRegs = 2;
     _numFPDestRegs = 0;
     _numIntDestRegs = 1;
     flags[Is2cycle] = true;
     flags[IsInteger] = true;;
     }
    ```
    The **IntCIOp** class is derived from **MpuStaticInst**, which is derived from the basic class **StaticInst** which 
    is defined in Gem5 for all ISA (*src/cpu/static_inst.hh*).
   * **inorder_cpu_exec.cc** contains the executing code:
   
   ```c++
    Fault Fixadd::execute(InOrderDynInst *xc, Trace::InstRecord *traceData) const {
       Fault fault = NoFault;
       uint64_t CI = 0;
       uint32_t Rs = 0;
       uint32_t Rt = 0;
       uint32_t Rd = 0;

       Rs = xc->readIntRegOperand(this, 0);
       Rt = xc->readIntRegOperand(this, 1);

       if(fault == NoFault) {
          if(SCU_U){
            ...
          } else {
            uint64_t i, j, k;
            i = Rs;
            j = Rt;
            k = i + j;
            CI = (k >> 32) ;
            int64_t a, b, c;
            a = (int32_t)Rs;
            b = (int32_t)Rt;
            c = a + b;
            if(SCU_T) Rd = c;                             // Rs = Rm + Rn(T)
            else      Rd = c > MAX_INT32 ? MAX_INT32 :    // Rs = Rm + Rn
                           c < MIN_INT32 ? MIN_INT32 : c;
          }
       }
       ...
    }
   ```
[\<------ \[Table of Content\]]({{site.url}}/index) <span style="float:right">  [\[Assembler\] ------>](Assembler)  </span>
