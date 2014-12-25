#ifndef __MAPU_FAULTS_HH__
#define __MAPU_FAULTS_HH__

#include "cpu/thread_context.hh"
#include "debug/MapuPRA.hh"
#include "sim/faults.hh"
#include "sim/full_system.hh"

namespace MapuISA
{

typedef const Addr FaultVect;

enum ExcCode {
    // A dummy value to use when the code isn't defined or doesn't matter.
    ExcCodeDummy = 0,

    ExcCodeInt = 0,
    ExcCodeMod = 1,
    ExcCodeTlbL = 2,
    ExcCodeTlbS = 3,
    ExcCodeAdEL = 4,
    ExcCodeAdES = 5,
    ExcCodeIBE = 6,
    ExcCodeDBE = 7,
    ExcCodeSys = 8,
    ExcCodeBp = 9,
    ExcCodeRI = 10,
    ExcCodeCpU = 11,
    ExcCodeOv = 12,
    ExcCodeTr = 13,
    ExcCodeC2E = 18,
    ExcCodeMDMX = 22,
    ExcCodeWatch = 23,
    ExcCodeMCheck = 24,
    ExcCodeThread = 25,
    ExcCodeCacheErr = 30
};

class MapuFaultBase : public FaultBase
{
  public:
    struct FaultVals
    {
        const FaultName name;
        const FaultVect offset;
        const ExcCode code;
    };

    void setExceptionState(ThreadContext *, uint8_t);

    virtual FaultVect offset(ThreadContext *tc) const = 0;
    virtual ExcCode code() const = 0;
    virtual FaultVect base(ThreadContext *tc) const
    {
//        StatusReg status = tc->readMiscReg(MISCREG_STATUS);
//        if (!status.bev)
//            return tc->readMiscReg(MISCREG_EBASE);
//        else
            return 0xbfc00200;
    }

    FaultVect
    vect(ThreadContext *tc) const
    {
        return base(tc) + offset(tc);
    }

    void invoke(ThreadContext * tc,
            StaticInstPtr inst = StaticInst::nullStaticInstPtr);
};

template <typename T>
class MapuFault : public MapuFaultBase
{
  protected:
    static FaultVals vals;
  public:
    FaultName name() const { return vals.name; }
    FaultVect offset(ThreadContext *tc) const { return vals.offset; }
    ExcCode code() const { return vals.code; }
};

class SystemCallFault : public MapuFault<SystemCallFault> {};
class ReservedInstructionFault : public MapuFault<ReservedInstructionFault> {};
class ThreadFault : public MapuFault<ThreadFault> {};
class IntegerOverflowFault : public MapuFault<IntegerOverflowFault> {};
class TrapFault : public MapuFault<TrapFault> {};
class BreakpointFault : public MapuFault<BreakpointFault> {};
class DspStateDisabledFault : public MapuFault<DspStateDisabledFault> {};

class MachineCheckFault : public MapuFault<MachineCheckFault>
{
  public:
    bool isMachineCheckFault() { return true; }
};

class ResetFault : public MapuFault<ResetFault>
{
  public:
    void invoke(ThreadContext * tc,
            StaticInstPtr inst = StaticInst::nullStaticInstPtr);

};

class SoftResetFault : public MapuFault<SoftResetFault>
{
  public:
    void invoke(ThreadContext * tc,
            StaticInstPtr inst = StaticInst::nullStaticInstPtr);
};

class NonMaskableInterrupt : public MapuFault<NonMaskableInterrupt>
{
  public:
    void invoke(ThreadContext * tc,
            StaticInstPtr inst = StaticInst::nullStaticInstPtr);
};

class CoprocessorUnusableFault : public MapuFault<CoprocessorUnusableFault>
{
  protected:
    int coProcID;
  public:
    CoprocessorUnusableFault(int _procid) : coProcID(_procid)
    {}

    void
    invoke(ThreadContext * tc,
            StaticInstPtr inst = StaticInst::nullStaticInstPtr)
    {
        MapuFault<CoprocessorUnusableFault>::invoke(tc, inst);
        if (FullSystem) {
//            CauseReg cause = tc->readMiscReg(MISCREG_CAUSE);
//            cause.ce = coProcID;
//            tc->setMiscRegNoEffect(MISCREG_CAUSE, cause);
        }
    }
};

class InterruptFault : public MapuFault<InterruptFault>
{
  public:
    FaultVect
    offset(ThreadContext *tc) const
    {
        //CauseReg cause = tc->readMiscRegNoEffect(MISCREG_CAUSE);
        // offset 0x200 for release 2, 0x180 for release 1.
        //return cause.iv ? 0x200 : 0x180;
    	return 0x180;
    }
};

template <typename T>
class AddressFault : public MapuFault<T>
{
  protected:
    Addr vaddr;
    bool store;

    AddressFault(Addr _vaddr, bool _store) : vaddr(_vaddr), store(_store)
    {}

    void
    invoke(ThreadContext * tc,
            StaticInstPtr inst = StaticInst::nullStaticInstPtr)
    {
        MapuFault<T>::invoke(tc, inst);
        //if (FullSystem)
            //tc->setMiscRegNoEffect(MISCREG_BADVADDR, vaddr);
    }
};

class AddressErrorFault : public AddressFault<AddressErrorFault>
{
  public:
    AddressErrorFault(Addr _vaddr, bool _store) :
        AddressFault<AddressErrorFault>(_vaddr, _store)
    {}

    ExcCode
    code() const
    {
        return store ? ExcCodeAdES : ExcCodeAdEL;
    }

};

template <typename T>
class TlbFault : public AddressFault<T>
{
  protected:
    Addr asid;
    Addr vpn;

    TlbFault(Addr _asid, Addr _vaddr, Addr _vpn, bool _store) :
        AddressFault<T>(_vaddr, _store), asid(_asid), vpn(_vpn)
    {}

    void
    setTlbExceptionState(ThreadContext *tc, uint8_t excCode)
    {
        this->setExceptionState(tc, excCode);

        //tc->setMiscRegNoEffect(MISCREG_BADVADDR, this->vaddr);
        //EntryHiReg entryHi = tc->readMiscReg(MISCREG_ENTRYHI);
//        entryHi.asid = this->asid;
//        entryHi.vpn2 = this->vpn >> 2;
//        entryHi.vpn2x = this->vpn & 0x3;
//        tc->setMiscRegNoEffect(MISCREG_ENTRYHI, entryHi);
//
//        ContextReg context = tc->readMiscReg(MISCREG_CONTEXT);
//        context.badVPN2 = this->vpn >> 2;
//        tc->setMiscRegNoEffect(MISCREG_CONTEXT, context);
    }

    void
    invoke(ThreadContext * tc,
            StaticInstPtr inst = StaticInst::nullStaticInstPtr)
    {
        if (FullSystem) {
            DPRINTF(MapuPRA, "Fault %s encountered.\n", this->name());
            Addr vect = this->vect(tc);
            setTlbExceptionState(tc, this->code());
            tc->pcState(vect);
        } else {
            AddressFault<T>::invoke(tc, inst);
        }
    }

    ExcCode
    code() const
    {
        return this->store ? ExcCodeTlbS : ExcCodeTlbL;
    }
};

class TlbRefillFault : public TlbFault<TlbRefillFault>
{
  public:
    TlbRefillFault(Addr asid, Addr vaddr, Addr vpn, bool store) :
        TlbFault<TlbRefillFault>(asid, vaddr, vpn, store)
    {}

    FaultVect
    offset(ThreadContext *tc) const
    {
        //StatusReg status = tc->readMiscReg(MISCREG_STATUS);
        //return status.exl ? 0x180 : 0x000;
    	return 0x180;
    }
};

class TlbInvalidFault : public TlbFault<TlbInvalidFault>
{
  public:
    TlbInvalidFault(Addr asid, Addr vaddr, Addr vpn, bool store) :
        TlbFault<TlbInvalidFault>(asid, vaddr, vpn, store)
    {}
};

class TlbModifiedFault : public TlbFault<TlbModifiedFault>
{
  public:
    TlbModifiedFault(Addr asid, Addr vaddr, Addr vpn) :
        TlbFault<TlbModifiedFault>(asid, vaddr, vpn, false)
    {}

    ExcCode code() const { return MapuFault<TlbModifiedFault>::code(); }
};

} // namespace MapuISA

#endif // __MAPU_FAULTS_HH__

