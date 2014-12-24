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
# Copyright (c) 2006-2008 The Regents of The University of Michigan
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
# Authors: Steve Reinhardt

# Simple test script
#
# "m5 test.py"

import optparse
import sys

import m5
from m5.defines import buildEnv
from m5.objects import *
from m5.util import addToPath, fatal

addToPath('../common')

import Options
import Simulation
import CacheConfig
from Caches import *
from cpu2000 import *

parser = optparse.OptionParser()
Options.addCommonOptions(parser)
Options.addSEOptions(parser)
Options.addMSOptions(parser)

#if '--ruby' in sys.argv:
#    Ruby.define_options(parser)

(options, args) = parser.parse_args()

if args:
    print "Error: script doesn't take any positional arguments"
    sys.exit(1)

process = []
inputs = []
outputs = []
errouts = []

if options.input != "":
    inputs = options.input.split(';')
if options.output != "":
    outputs = options.output.split(';')
if options.errout != "":
     errouts = options.errout.split(';')

processes = []
np = options.num_cpus

for i in xrange(np):
    process = []
    smt_process = LiveProcess()
    smt_process.cmd = options.cmd;
    if inputs and inputs[0]:
        smt_process.input = inputs[0]
    if outputs and outputs[0]:
        smt_process.output = outputs[0]
    if errouts and errouts[0]:
        smt_process.errout = errouts[0]
    process += [smt_process, ]
    smt_process = LiveProcess()
    smt_process.cmd = " "
    if inputs and inputs[1]:
         smt_process.input = inputs[1]
    if outputs and outputs[1]:
         smt_process.output = outputs[1]
    if errouts and errouts[1]:
         smt_process.errout = errouts[1]
    process += [smt_process, ]
    processes += [process, ]

numThreads = 2

(CPUClass, test_mem_mode, FutureClass) = Simulation.setCPUClass(options)
CPUClass.clock = '0.5GHz'
CPUClass.numThreads = numThreads;

print "Building MaPU full system simulator..."

if options.turbo_type == "fix":
    class TurboClass(FixPointTurboDecoder): pass
else:
    class TurboClass(TurboDecoder): pass

system = MapuSystem(cpu = [CPUClass(cpu_id=i, dtb = MapuTLB(cpuid = i), itb = MapuTLB(cpuid = i), turbodec = TurboClass(cop_addr = 0x80000000, cop_size = 0x1000, cop_latency = '2ns')) for i in xrange(np)],
                physmem = SimpleMemory(range=AddrRange("128MB")),
                membus = NoncoherentBus(), 
                sbus = NoncoherentBus(), 
                cbus = NoncoherentBus(), 
                ddr0bus = NoncoherentBus(), 
                ddr1bus = NoncoherentBus(),
                ddr2bus = NoncoherentBus(),
                ddr3bus = NoncoherentBus(),
                s2sys_bridge = Bridge(),
                c2sys_bridge = Bridge(),
                ddr02sys_bridge = Bridge(),
                ddr12sys_bridge = Bridge(),
                ddr22sys_bridge = Bridge(),
                ddr32sys_bridge = Bridge(),
                mem_mode = test_mem_mode,
                ddrmem0 = SimpleMemory(range=AddrRange(0x60000000, 0x6FFFFFFF), shared=True, shmkey=options.shm_key),
                ddrmem1 = SimpleMemory(range=AddrRange(0x70000000, 0x7FFFFFFF), shared=True, shmkey=options.shm_key),
                ddrmem2 = SimpleMemory(range=AddrRange(0x80000000, 0xBFFFFFFF)),
                ddrmem3 = SimpleMemory(range=AddrRange(0xC0000000, 0xFFFFFFFF)),
                shmem = SimpleMemory(range=AddrRange(0x40400000, 0x407FFFFF), shared=True, shmkey=options.shm_key))

# Sanity check
if options.fastmem and (options.caches or options.l2cache):
    fatal("You cannot use fastmem in combination with caches!")

for i in xrange(np):
    system.cpu[i].workload = processes[i]

    if options.fastmem:
        system.cpu[i].fastmem = True

    if options.checker:
        system.cpu[i].addCheckerCpu()

#if options.ruby:
#    if not (options.cpu_type == "detailed" or options.cpu_type == "timing"):
#        print >> sys.stderr, "Ruby requires TimingSimpleCPU or O3CPU!!"
#        sys.exit(1)

#    options.use_map = True
#    Ruby.create_system(options, system)
#    assert(options.num_cpus == len(system.ruby._cpu_ruby_ports))

#    for i in xrange(np):
#        ruby_port = system.ruby._cpu_ruby_ports[i]

        # Create the interrupt controller and connect its ports to Ruby
#        system.cpu[i].createInterruptController()
#        system.cpu[i].interrupts.pio = ruby_port.master
#        system.cpu[i].interrupts.int_master = ruby_port.slave
#        system.cpu[i].interrupts.int_slave = ruby_port.master

        # Connect the cpu's cache ports to Ruby
#        system.cpu[i].icache_port = ruby_port.slave
#        system.cpu[i].dcache_port = ruby_port.slave
#else:
system.system_port = system.membus.slave
system.c2sys_bridge.slave = system.cbus.master
system.c2sys_bridge.master = system.membus.slave
system.s2sys_bridge.slave = system.sbus.master
system.s2sys_bridge.master = system.membus.slave
system.ddr02sys_bridge.slave = system.ddr0bus.master
system.ddr02sys_bridge.master = system.membus.slave
system.ddr12sys_bridge.slave = system.ddr1bus.master
system.ddr12sys_bridge.master = system.membus.slave
system.ddr22sys_bridge.slave = system.ddr2bus.master
system.ddr22sys_bridge.master = system.membus.slave
system.ddr32sys_bridge.slave = system.ddr3bus.master
system.ddr32sys_bridge.master = system.membus.slave
system.physmem.port = system.membus.master
system.ddrmem0.port = system.membus.master
system.ddrmem1.port = system.membus.master
system.ddrmem2.port = system.membus.master
system.ddrmem3.port = system.membus.master
system.shmem.port = system.membus.master
CacheConfig.config_cache(options, system)

root = Root(full_system = False, mapu_system = True, system = system)
Simulation.run(options, root, system, FutureClass)
