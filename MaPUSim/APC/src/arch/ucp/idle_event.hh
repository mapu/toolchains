#ifndef __KERN_UCP_IDLE_EVENT_HH__
#define __KERN_UCP_IDLE_EVENT_HH__

#include "cpu/pc_event.hh"

class IdleStartEvent : public PCEvent
{
  public:
    IdleStartEvent(PCEventQueue *q, const std::string &desc, Addr addr)
        : PCEvent(q, desc, addr)
    {}
    virtual void process(ThreadContext *tc);
};

#endif // __KERN_UCP_IDLE_EVENT_HH__
