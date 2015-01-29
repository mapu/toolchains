//===-- MMPUCompiler main.cpp - MMPUCompiler's driver --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===--------------------------------------------------------------------===//
//
//
//===--------------------------------------------------------------------===//

#include "MMPUCompiler.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include <fstream>
#include <iostream>
using namespace llvm;
using namespace MMPU;

//Command line options

static cl::opt<std::string>
InputFilename(cl::Positional, cl::desc("MMPU serial program filename"));

static cl::opt<std::string>
OutputFilename("o", cl::desc("Output filename"), cl::value_desc("filename"));

static cl::opt<std::string>
AsmOutputFilename("s", cl::desc("Asm Output filename"), cl::value_desc("filename"));

static cl::opt<unsigned>
OptimizationLevel("O", cl::desc("Optimization level"), cl::value_desc("level"));
#if 0
static cl::opt<bool>
ArrayBoundsChecking("abc", cl::desc("Enable array bounds checking"));

static cl::opt<bool>
JIT("jit", cl::desc("Run program Just-In-Time"));
#endif

//Add main function so can be fully compiled
void addMainFunction(Module *mod) {
  //define i32 @main(i32 %argc, i8 **%argv)
  Function *main_func = cast<Function>(mod->
    getOrInsertFunction("main", IntegerType::getInt32Ty(mod->getContext()),
                        IntegerType::getInt32Ty(mod->getContext()),
                        PointerType::getUnqual(PointerType::getUnqual(
                          IntegerType::getInt8Ty(mod->getContext()))), NULL));
  {
    Function::arg_iterator args = main_func->arg_begin();
    Value *arg_0 = args++;
    arg_0->setName("argc");
    Value *arg_1 = args++;
    arg_1->setName("argv");
  }

  //main.0:
  BasicBlock *bb = BasicBlock::Create(mod->getContext(), "main.0", main_func);

  //call void @brainf()
  {
    CallInst *brainf_call = CallInst::Create(mod->getFunction("brainf"),
                                             "", bb);
    brainf_call->setTailCall(false);
  }

  //ret i32 0
  ReturnInst::Create(mod->getContext(),
                     ConstantInt::get(mod->getContext(), APInt(32, 0)), bb);
}

int main(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv, " MMPU compiler\n");
  
  InitializeNativeTarget();
  LLVMContext &Context = getGlobalContext();

  /*if (InputFilename == "") {
    errs() << "Error: You must specify the filename of the program to "
    "be compiled.  Use --help to see the options.\n";
    abort();
  }*/

  //Get the output stream
  raw_ostream *out = &outs();
  raw_ostream *asmout = NULL;
  if (OutputFilename == "") {
    std::string base = InputFilename;
  if (InputFilename == "-") { base = "a"; }

    // Use default filename.
    OutputFilename = base + ".m";
  }
  if (OutputFilename != "-") {
    std::error_code ErrInfo;
    out = new raw_fd_ostream(OutputFilename.c_str(), ErrInfo, sys::fs::F_Text);
  }
  if (AsmOutputFilename != "-") {
    std::error_code ErrInfo;
    asmout = new raw_fd_ostream(AsmOutputFilename.c_str(), ErrInfo, sys::fs::F_Text);
  }
  
  //Get the input stream
  std::istream *in = &std::cin;
  if (InputFilename != "-")
    in = new std::ifstream(InputFilename.c_str());
  
  MMPUCompiler *mc = new MMPUCompiler(in, out, asmout, Context);
  
  mc->setOptLevel(OptimizationLevel);
  
  // Make the module, which holds all the code.
  mc->parse();
  
  mc->GenAPDL();
  
  out->flush();
  asmout->flush();
  
  llvm_shutdown();
  return 0;
}
