# Copyright (c) 2012 ARM Limited
# All rights reserved.
#
# The license below extends only to copyright in the software and shall
# not be construed as granting a license to any other intellectual
# property including but not limited to intellectual property relating
# to a hardware implementation of the functionality of the software
# licensed hereunder.  You may use the software subject to the license
# terms below provided that you ensure that this notice is replicated
# unmodified and in its entirety in all distributions of the software,
# modified or unmodified, in source code or in binary form.
#
# Copyright (c) 2005-2008 The Regents of The University of Michigan
# Copyright (c) 2011 Regents of the University of California
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met: redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer;
# redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution;
# neither the name of the copyright holders nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Authors: Nathan Binkert
#          Rick Strong
#          Andreas Hansson

import sys

from m5.defines import buildEnv
from m5.params import *
from m5.proxy import *

from Bus import CoherentBus
from InstTracer import InstTracer
from ExeTracer import ExeTracer
from MemObject import MemObject

default_tracer = ExeTracer()

if buildEnv['TARGET_ISA'] == 'alpha':
    from AlphaTLB import AlphaDTB, AlphaITB
    from AlphaInterrupts import AlphaInterrupts
elif buildEnv['TARGET_ISA'] == 'sparc':
    from SparcTLB import SparcTLB
    from SparcInterrupts import SparcInterrupts
elif buildEnv['TARGET_ISA'] == 'x86':
    from X86TLB import X86TLB
    from X86LocalApic import X86LocalApic
elif buildEnv['TARGET_ISA'] == 'mips':
    from MipsTLB import MipsTLB
    from MipsInterrupts import MipsInterrupts
elif buildEnv['TARGET_ISA'] == 'arm':
    from ArmTLB import ArmTLB
    from ArmInterrupts import ArmInterrupts
elif buildEnv['TARGET_ISA'] == 'power':
    from PowerTLB import PowerTLB
    from PowerInterrupts import PowerInterrupts
elif buildEnv['TARGET_ISA'] == 'mapu':
    from MapuTLB import MapuTLB
    from MapuInterrupts import MapuInterrupts
elif buildEnv['TARGET_ISA'] == 'ucp':
    from UcpTLB import UcpTLB

class BaseCPU(MemObject):
    type = 'BaseCPU'
    abstract = True

    system = Param.System(Parent.any, "system object")
    cpu_id = Param.Int(-1, "CPU identifier")
    numThreads = Param.Unsigned(2, "number of HW thread contexts")

    function_trace = Param.Bool(False, "Enable function trace")
    function_trace_start = Param.Tick(0, "Cycle to start function trace")

    checker = Param.BaseCPU(NULL, "checker CPU")

    do_checkpoint_insts = Param.Bool(True,
        "enable checkpoint pseudo instructions")
    do_statistics_insts = Param.Bool(True,
        "enable statistics pseudo instructions")

    profile = Param.Latency('0ns', "trace the kernel stack")
    do_quiesce = Param.Bool(True, "enable quiesce instructions")

    workload = VectorParam.Process([], "processes to run")

    if buildEnv['TARGET_ISA'] == 'sparc':
        dtb = Param.SparcTLB(SparcTLB(), "Data TLB")
        itb = Param.SparcTLB(SparcTLB(), "Instruction TLB")
        interrupts = Param.SparcInterrupts(
                NULL, "Interrupt Controller")
    elif buildEnv['TARGET_ISA'] == 'alpha':
        dtb = Param.AlphaTLB(AlphaDTB(), "Data TLB")
        itb = Param.AlphaTLB(AlphaITB(), "Instruction TLB")
        interrupts = Param.AlphaInterrupts(
                NULL, "Interrupt Controller")
    elif buildEnv['TARGET_ISA'] == 'x86':
        dtb = Param.X86TLB(X86TLB(), "Data TLB")
        itb = Param.X86TLB(X86TLB(), "Instruction TLB")
        interrupts = Param.X86LocalApic(NULL, "Interrupt Controller")
    elif buildEnv['TARGET_ISA'] == 'mips':
        dtb = Param.MipsTLB(MipsTLB(), "Data TLB")
        itb = Param.MipsTLB(MipsTLB(), "Instruction TLB")
        interrupts = Param.MipsInterrupts(
                NULL, "Interrupt Controller")
    elif buildEnv['TARGET_ISA'] == 'arm':
        dtb = Param.ArmTLB(ArmTLB(), "Data TLB")
        itb = Param.ArmTLB(ArmTLB(), "Instruction TLB")
        interrupts = Param.ArmInterrupts(
                NULL, "Interrupt Controller")
    elif buildEnv['TARGET_ISA'] == 'power':
        UnifiedTLB = Param.Bool(True, "Is this a Unified TLB?")
        dtb = Param.PowerTLB(PowerTLB(), "Data TLB")
        itb = Param.PowerTLB(PowerTLB(), "Instruction TLB")
        interrupts = Param.PowerInterrupts(
                NULL, "Interrupt Controller")
    elif buildEnv['TARGET_ISA'] == 'mapu':
        UnifiedTLB = Param.Bool(True, "Is this a Unified TLB?")
        dtb = Param.MapuTLB(MapuTLB(), "Data TLB")
        itb = Param.MapuTLB(MapuTLB(), "Instruction TLB")
        interrupts = Param.MapuInterrupts(
                NULL, "Interrupt Controller")
    elif buildEnv['TARGET_ISA'] == 'ucp':
        UnifiedTLB = Param.Bool(True, "Is this a Unified TLB?")
        dtb = Param.UcpTLB(UcpTLB(), "Data TLB")
        itb = Param.UcpTLB(UcpTLB(), "Instruction TLB")
        interrupts = Param.UcpInterrupts(
                NULL, "Interrupt Controller")
    else:
        print "Don't know what TLB to use for ISA %s" % \
            buildEnv['TARGET_ISA']
        sys.exit(1)

    max_insts_all_threads = Param.Counter(0,
        "terminate when all threads have reached this inst count")
    max_insts_any_thread = Param.Counter(0,
        "terminate when any thread reaches this inst count")
    max_loads_all_threads = Param.Counter(0,
        "terminate when all threads have reached this load count")
    max_loads_any_thread = Param.Counter(0,
        "terminate when any thread reaches this load count")
    progress_interval = Param.Tick(0,
        "interval to print out the progress message")

    defer_registration = Param.Bool(False,
        "defer registration with system (for sampling)")

    clock = Param.Clock('1t', "clock speed")
    phase = Param.Latency('0ns', "clock phase")

    tracer = Param.InstTracer(default_tracer, "Instruction tracer")

    icache_port = MasterPort("Instruction Port")
    dcache_port = MasterPort("Data Port")
    micache_port = MasterPort("MPU Instruction Port")
    _cached_ports = ['icache_port', 'dcache_port', 'micache_port']

    if buildEnv['TARGET_ISA'] in ['x86', 'arm']:
        _cached_ports += ["itb.walker.port", "dtb.walker.port"]

    _uncached_slave_ports = []
    _uncached_master_ports = []
    if buildEnv['TARGET_ISA'] == 'x86':
        _uncached_slave_ports += ["interrupts.pio", "interrupts.int_slave"]
        _uncached_master_ports += ["interrupts.int_master"]

    def createInterruptController(self):
        if buildEnv['TARGET_ISA'] == 'sparc':
            self.interrupts = SparcInterrupts()
        elif buildEnv['TARGET_ISA'] == 'alpha':
            self.interrupts = AlphaInterrupts()
        elif buildEnv['TARGET_ISA'] == 'x86':
            _localApic = X86LocalApic(pio_addr=0x2000000000000000)
            self.interrupts = _localApic
        elif buildEnv['TARGET_ISA'] == 'mips':
            self.interrupts = MipsInterrupts()
        elif buildEnv['TARGET_ISA'] == 'arm':
            self.interrupts = ArmInterrupts()
        elif buildEnv['TARGET_ISA'] == 'power':
            self.interrupts = PowerInterrupts()
        elif buildEnv['TARGET_ISA'] == 'mapu':
            self.interrupts = MapuInterrupts()
        else:
            print "Don't know what Interrupt Controller to use for ISA %s" % \
                buildEnv['TARGET_ISA']
            sys.exit(1)

    def connectCachedPorts(self, bus):
        for p in self._cached_ports:
            exec('self.%s = bus.slave' % p)

    def connectUncachedPorts(self, bus):
        for p in self._uncached_slave_ports:
            exec('self.%s = bus.master' % p)
        for p in self._uncached_master_ports:
            exec('self.%s = bus.slave' % p)

    def connectAllPorts(self, cached_bus, uncached_bus = None):
        self.connectCachedPorts(cached_bus)
        if not uncached_bus:
            uncached_bus = cached_bus
        self.connectUncachedPorts(uncached_bus)

    def addPrivateSplitL1Caches(self, ic, dc, mic = None, iwc = None, dwc = None):
        self.icache = ic
        self.micache = mic
        self.dcache = dc
        self.icache_port = ic.cpu_side
        self.micache_port = mic.cpu_side
        self.dcache_port = dc.cpu_side
        self._cached_ports = ['icache.mem_side', 'dcache.mem_side', 'micache.mem_side']
        if buildEnv['TARGET_ISA'] in ['x86', 'arm']:
            if iwc and dwc:
                self.itb_walker_cache = iwc
                self.dtb_walker_cache = dwc
                self.itb.walker.port = iwc.cpu_side
                self.dtb.walker.port = dwc.cpu_side
                self._cached_ports += ["itb_walker_cache.mem_side", \
                                       "dtb_walker_cache.mem_side"]
            else:
                self._cached_ports += ["itb.walker.port", "dtb.walker.port"]

            # Checker doesn't need its own tlb caches because it does
            # functional accesses only
            if self.checker != NULL:
                self._cached_ports += ["checker.itb.walker.port", \
                                       "checker.dtb.walker.port"]

    def addTwoLevelCacheHierarchy(self, ic, dc, l2c, iwc = None, dwc = None):
        self.addPrivateSplitL1Caches(ic, dc, iwc, dwc)
        self.toL2Bus = CoherentBus()
        self.connectCachedPorts(self.toL2Bus)
        self.l2cache = l2c
        self.toL2Bus.master = self.l2cache.cpu_side
        self._cached_ports = ['l2cache.mem_side']

    def addCheckerCpu(self):
        pass
