#include "arch/mapu/isa_traits.hh"
#include "arch/mapu/process.hh"
#include "base/loader/elf_object.hh"
#include "base/loader/object_file.hh"
#include "base/misc.hh"
#include "cpu/thread_context.hh"
#include "debug/Loader.hh"
#include "mem/page_table.hh"
#include "sim/process.hh"
#include "sim/process_impl.hh"
#include "sim/system.hh"

using namespace std;
using namespace MapuISA;

MapuLiveProcess::MapuLiveProcess(LiveProcessParams * params,
        ObjectFile *objFile)
    : LiveProcess(params, objFile)
{
    // Set up stack. On Mapu, stack starts at the top of kuseg
    // user address space. Mapu stack grows down from here
    stack_base = 0x20000;

    // Set pointer for next thread stack.  Reserve 8M for main stack.
    next_thread_stack_base = stack_base - (8 * 1024);

    // Set up break point (Top of Heap)
    /*brk_point = objFile->dataBase() + objFile->dataSize() + objFile->bssSize();
    brk_point = roundUp(brk_point, VMPageSize);

    // Set up region for mmaps.  Start it 1GB above the top of the heap.
    mmap_start = mmap_end = brk_point + 0x40000000L;*/
}

void
MapuLiveProcess::initState()
{
    if (MaPUSystem) {
      if (contextIds.empty())
          fatal("Process %s is not associated with any HW contexts!\n", name());
      ThreadContext *tc = system->getThreadContext(contextIds[0]);
      tc->halt(0);
      tc->pcState(0);
    } else {
      LiveProcess::initState();
      argsInit<uint32_t>(VMPageSize);
      localCsu = CsuObject::createSingleSideCSU();
    }
}

template<class IntType>
void
MapuLiveProcess::argsInit(int pageSize)
{
    int intSize = sizeof(IntType);

    // load object file into target memory
    if (objFile)
      objFile->loadSections(initVirtMem);

    typedef AuxVector<IntType> auxv_t;
    std::vector<auxv_t> auxv;

    ElfObject * elfObject = dynamic_cast<ElfObject *>(objFile);
    if (elfObject)
    {
        // Set the system page size
        auxv.push_back(auxv_t(M5_AT_PAGESZ, MapuISA::VMPageSize));
        // Set the frequency at which time() increments
        auxv.push_back(auxv_t(M5_AT_CLKTCK, 100));
        // For statically linked executables, this is the virtual
        // address of the program header tables if they appear in the
        // executable image.
        auxv.push_back(auxv_t(M5_AT_PHDR, elfObject->programHeaderTable()));
        DPRINTF(Loader, "auxv at PHDR %08p\n", elfObject->programHeaderTable());
        // This is the size of a program header entry from the elf file.
        auxv.push_back(auxv_t(M5_AT_PHENT, elfObject->programHeaderSize()));
        // This is the number of program headers from the original elf file.
        auxv.push_back(auxv_t(M5_AT_PHNUM, elfObject->programHeaderCount()));
        //The entry point to the program
        auxv.push_back(auxv_t(M5_AT_ENTRY, objFile->entryPoint()));
        //Different user and group IDs
        auxv.push_back(auxv_t(M5_AT_UID, uid()));
        auxv.push_back(auxv_t(M5_AT_EUID, euid()));
        auxv.push_back(auxv_t(M5_AT_GID, gid()));
        auxv.push_back(auxv_t(M5_AT_EGID, egid()));
    }

    // Calculate how much space we need for arg & env & auxv arrays.
    int argv_array_size = intSize * (argv.size() + 1);
    int envp_array_size = intSize * (envp.size() + 1);
    int auxv_array_size = intSize * 2 * (auxv.size() + 1);

    int arg_data_size = 0;
    for (vector<string>::size_type i = 0; i < argv.size(); ++i) {
        arg_data_size += argv[i].size() + 1;
    }
    int env_data_size = 0;
    for (vector<string>::size_type i = 0; i < envp.size(); ++i) {
        env_data_size += envp[i].size() + 1;
    }

    int space_needed =
        argv_array_size +
        envp_array_size +
        auxv_array_size +
        arg_data_size +
        env_data_size +
        3 * intSize; // for SP, R2, R3

    // set bottom of stack
    stack_min = stack_base - space_needed;
    // align it
    stack_min = roundDown(stack_min, pageSize);
    stack_size = stack_base - stack_min;
    // map memory
    allocateMem(stack_min, roundUp(stack_size, pageSize));

    // map out initial stack contents
    IntType argv_array_base = stack_min + intSize + intSize; // room for argc
    IntType envp_array_base = argv_array_base + argv_array_size;
    IntType auxv_array_base = envp_array_base + envp_array_size;
    IntType arg_data_base = auxv_array_base + auxv_array_size;
    IntType env_data_base = arg_data_base + arg_data_size;

    // write contents to stack
    IntType argc = argv.size();

    argc = htog((IntType)argc);

    initVirtMem.writeBlob(stack_min, (uint8_t*)&argc, intSize);

    IntType argv_array_base_t = htog((IntType)argv_array_base);

    initVirtMem.writeBlob(stack_min + intSize, (uint8_t*)&argv_array_base_t, intSize);

    IntType sp = htog((IntType)stack_min);

    initVirtMem.writeBlob(stack_base - intSize, (uint8_t*)&sp, intSize);

    copyStringArray(argv, argv_array_base, arg_data_base, initVirtMem);

    copyStringArray(envp, envp_array_base, env_data_base, initVirtMem);

    // Copy the aux vector
    /*for (typename vector<auxv_t>::size_type x = 0; x < auxv.size(); x++) {
        initVirtMem.writeBlob(auxv_array_base + x * 2 * intSize,
                (uint8_t*)&(auxv[x].a_type), intSize);
        initVirtMem.writeBlob(auxv_array_base + (x * 2 + 1) * intSize,
                (uint8_t*)&(auxv[x].a_val), intSize);
    }*/

    // Write out the terminating zeroed auxilliary vector
    /*for (unsigned i = 0; i < 2; i++) {
        const IntType zero = 0;
        const Addr addr = auxv_array_base + 2 * intSize * (auxv.size() + i);
        initVirtMem.writeBlob(addr, (uint8_t*)&zero, intSize);
    }*/

    ThreadContext *tc = system->getThreadContext(contextIds[0]);

    if (objFile)
      tc->pcState(objFile->entryPoint());
    else
      tc->pcState(0);
}


MapuISA::IntReg
MapuLiveProcess::getSyscallArg(ThreadContext *tc, int &i)
{
    assert(i < 6);
    return tc->readIntReg(FirstArgumentReg + i++);
}

void
MapuLiveProcess::setSyscallArg(ThreadContext *tc,
        int i, MapuISA::IntReg val)
{
    assert(i < 6);
    tc->setIntReg(FirstArgumentReg + i, val);
}

void
MapuLiveProcess::setSyscallReturn(ThreadContext *tc,
        SyscallReturn return_value)
{
    if (return_value.successful()) {
        // no error
        tc->setIntReg(SyscallSuccessReg, 0);
        tc->setIntReg(ReturnValueReg, return_value.value());
    } else {
        // got an error, return details
        tc->setIntReg(SyscallSuccessReg, (IntReg) -1);
        tc->setIntReg(ReturnValueReg, -return_value.value());
    }
}
