#ifndef __ARCH_MAPU_BARE_IRON_SYSTEM_HH__
#define __ARCH_MAPU_BARE_IRON_SYSTEM_HH__

#include "arch/mapu/system.hh"
#include "params/BareIronMapuSystem.hh"

/**
 * This class contains linux specific system code (Loading, Events).
 * It points to objects that are the system binaries to load and patches them
 * appropriately to work in simulator.
 */
class BareIronMapuSystem : public MapuSystem
{
  public:
    static const int CommandLineSize = 256;

    BareIronMapuSystem(Params *p);
    ~BareIronMapuSystem();
};

#endif // __ARCH_MAPU_BARE_IRON_SYSTEM_HH__
