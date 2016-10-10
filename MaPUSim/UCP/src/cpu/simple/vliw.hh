#ifndef __CPU_SIMPLE_VLIW_HH__
#define __CPU_SIMPLE_VLIW_HH__

#include "cpu/simple/base.hh"
#include "params/VliwSimpleCPU.hh"

class VliwSimpleCPU : public BaseSimpleCPU
{
  public:

    VliwSimpleCPU(VliwSimpleCPUParams *params);
    virtual ~VliwSimpleCPU();

    virtual void init();

  private:

    struct TickEvent : public Event
    {
        VliwSimpleCPU *cpu;

        TickEvent(VliwSimpleCPU *c);
        void process();
        const char *description() const;
    };

    TickEvent tickEvent;

    const int width;
    bool locked;
    const bool simulate_data_stalls;
    const bool simulate_inst_stalls;

    // main simulation loop (one cycle)
    void tick();

    /**
     * An VliwCPUPort overrides the default behaviour of the
     * recvVliw and ignores the packet instead of panicking.
     */
    class VliwCPUPort : public CpuPort
    {

      public:

        VliwCPUPort(const std::string &_name, BaseCPU* _cpu)
            : CpuPort(_name, _cpu)
        { }

      protected:

        virtual Tick recvVliwSnoop(PacketPtr pkt)
        {
            // Snooping a coherence request, just return
            return 0;
        }

    };

    VliwCPUPort icachePort;
    VliwCPUPort dcachePort;

    bool fastmem;
    Request ifetch_req;
    Request data_read_req;
    Request data_write_req;

    bool dcache_access;
    Tick dcache_latency;

  protected:

    /** Return a reference to the data port. */
    virtual CpuPort &getDataPort() { return dcachePort; }

    /** Return a reference to the instruction port. */
    virtual CpuPort &getInstPort() { return icachePort; }

  public:

    virtual void serialize(std::ostream &os);
    virtual void unserialize(Checkpoint *cp, const std::string &section);
    virtual void resume();

    void switchOut();
    void takeOverFrom(BaseCPU *oldCPU);

    virtual void activateContext(ThreadID thread_num, int delay);
    virtual void suspendContext(ThreadID thread_num);

    Fault readMem(Addr addr, uint8_t *data, unsigned size, unsigned flags);

    Fault writeMem(uint8_t *data, unsigned size,
                   Addr addr, unsigned flags, uint64_t *res);

    /**
     * Print state of address in memory system via PrintReq (for
     * debugging).
     */
    void printAddr(Addr a);
};

#endif // __CPU_SIMPLE_VLIW_HH__
