#include "arch/mapu/bare_iron/system.hh"
#include "params/BareIronMapuSystem.hh"

BareIronMapuSystem::BareIronMapuSystem(Params *p)
    : MapuSystem(p)
{ }

BareIronMapuSystem::~BareIronMapuSystem()
{ }

BareIronMapuSystem *
BareIronMapuSystemParams::create()
{
    return new BareIronMapuSystem(this);
}

