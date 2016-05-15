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
#addToPath('../ruby')

import Options
#import Ruby
import Simulation
import CacheConfig
from Caches import *
from cpu2000 import *

parser = optparse.OptionParser()
Options.addCommonOptions(parser)
Options.addSEOptions(parser)

#if '--ruby' in sys.argv:
#    Ruby.define_options(parser)

(options, args) = parser.parse_args()

if args:
    print "Error: script doesn't take any positional arguments"
    sys.exit(1)

multiprocesses = []
apps = []

if options.bench:
    apps = options.bench.split("-")
    if len(apps) != options.num_cpus:
        print "number of benchmarks not equal to set num_cpus!"
        sys.exit(1)

    for app in apps:
        try:
            if buildEnv['TARGET_ISA'] == 'alpha':
                exec("workload = %s('alpha', 'tru64', 'ref')" % app)
            else:
                exec("workload = %s(buildEnv['TARGET_ISA'], 'linux', 'ref')" % app)
            multiprocesses.append(workload.makeLiveProcess())
        except:
            print >>sys.stderr, "Unable to find workload for %s: %s" % (buildEnv['TARGET_ISA'], app)
            sys.exit(1)
elif options.cmd:
    process = LiveProcess()
    process.executable = options.cmd
    process.cmd = [options.cmd] + options.options.split()
    multiprocesses.append(process)
else:
    print >> sys.stderr, "No workload specified. Exiting!\n"
    sys.exit(1)


if options.input != "":
    process.input = options.input
if options.output != "":
    process.output = options.output
if options.errout != "":
    process.errout = options.errout


# By default, set workload to path of user-specified binary
workloads = options.cmd
numThreads = []
process_pairs = []

if options.cpu_type == "detailed" or options.cpu_type == "inorder":
    #check for Heterogenious Double Threading workload
    workloads = options.cmd.split(';')
    if len(workloads) >= 1:
        process_pair = []
        smt_idx = 0
        inputs = []
        outputs = []
        errouts = []
        opts = []

        if options.input != "":
            inputs = options.input.split(';')
        if options.output != "":
            outputs = options.output.split(';')
        if options.errout != "":
            errouts = options.errout.split(';')
        if options.options != "":
            opts = options.options.split(';')

        for wrkld in workloads:
            threads = wrkld.split(',')
            process_pair = []
            for thrdld in threads:
                thread = LiveProcess()
                thread.executable = thrdld
                if opts and opts[smt_idx]:
                    thread.cmd = thrdld + " " + opts[smt_idx]
                else:
                    thread.cmd = thrdld
                if inputs and inputs[smt_idx]:
                    thread.input = inputs[smt_idx]
                if outputs and outputs[smt_idx]:
                    thread.output = outputs[smt_idx]
                if errouts and errouts[smt_idx]:
                    thread.errout = errouts[smt_idx]
                process_pair += [thread, ]
            process_pairs += [process_pair, ]
            smt_idx += 1
            numThreads += [len(process_pair),]
            print "numThreads %d" % len(process_pair)

(CPUClass, test_mem_mode, FutureClass) = Simulation.setCPUClass(options)
CPUClass.clock = '0.5GHz'

np = options.num_cpus

if buildEnv['TARGET_ISA'] == 'mapu':
    if options.turbo_type == "fix":
        class TurboClass(FixPointTurboDecoder): pass
    elif options.turbo_type == "float"::
        class TurboClass(TurboDecoder): pass
    class TheTLB(MapuTLB): pass
elif buildEnv['TARGET_ISA'] == 'ucp':
    class TheTLB(UcpTLB): pass

dumplocal = False
dumpddr = False
dumpshare = False
memdump = options.mem_dump.split(',');
if len(memdump) >= 1:
    for memtype in memdump:
        if memtype == 'local':
            dumplocal = True
        elif memtype == 'ddr':
            dumpddr = True
        elif memtype == 'share':
            dumpshare = True
        else:
            print "Unknown memory type %s!" % memtype


if buildEnv['TARGET_ISA'] == 'mapu':
    system = System(cpu = [CPUClass(cpu_id=i, numThreads = numThreads[i],
                                dtb = TheTLB(cpuid = i), itb = TheTLB(cpuid = i), turbodec = TurboClass(cop_addr = 0x80000000, cop_size = 0x1000, cop_latency = '2ns')) for i in xrange(np)],
                physmem = SimpleMemory(range=AddrRange("128MB"), needdump = dumplocal),
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
                ddrmem0 = SimpleMemory(range=AddrRange(0x60000000, 0x6FFFFFFF), needdump = dumpddr),
                ddrmem1 = SimpleMemory(range=AddrRange(0x70000000, 0x7FFFFFFF), needdump = dumpddr),
                ddrmem2 = SimpleMemory(range=AddrRange(0x80000000, 0x9FFFFFFF), needdump = dumpddr),
                ddrmem3 = SimpleMemory(range=AddrRange(0xA0000000, 0xBFFFFFFF), needdump = dumpddr),
                shmem = SimpleMemory(range=AddrRange(0x40400000, 0x407FFFFF), needdump = dumpshare))
elif buildEnv['TARGET_ISA'] == 'ucp':
    system = System(cpu = [CPUClass(cpu_id=i, numThreads = numThreads[i],
                                dtb = TheTLB(cpuid = i), itb = TheTLB(cpuid = i)) for i in xrange(np)],
                physmem = SimpleMemory(range=AddrRange("128MB"), needdump = dumplocal),
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
                ddrmem0 = SimpleMemory(range=AddrRange(0x60000000, 0x6FFFFFFF), needdump = dumpddr),
                ddrmem1 = SimpleMemory(range=AddrRange(0x70000000, 0x7FFFFFFF), needdump = dumpddr),
                ddrmem2 = SimpleMemory(range=AddrRange(0x80000000, 0x9FFFFFFF), needdump = dumpddr),
                ddrmem3 = SimpleMemory(range=AddrRange(0xA0000000, 0xBFFFFFFF), needdump = dumpddr),
                shmem = SimpleMemory(range=AddrRange(0x40400000, 0x407FFFFF), needdump = dumpshare))

print "Waiting for GDB connection to thread %s" % m5.options.debug_tid
system.gdb_wait = m5.options.debug_tid;

# Sanity check
if options.fastmem and (options.caches or options.l2cache):
    fatal("You cannot use fastmem in combination with caches!")

print "Begin to deploy workloads to multiple APE cores in the simulating APC:"
for i in xrange(np):
    if len(multiprocesses) == 1:
        print "Task %s is assigned on APE%d->SPU." % (process_pairs[i][0].cmd, i)
        if (len(process_pairs[i]) > 1):
            print "Task %s is assigned on APE%d->MPU." % (process_pairs[i][1].cmd, i)
        system.cpu[i].workload = process_pairs[i]
    else:
        system.cpu[i].workload = multiprocesses[i]

    if options.fastmem:
        system.cpu[i].fastmem = True

    if options.checker:
        system.cpu[i].addCheckerCpu()
print "Task assignment is completed. Please review the configuration."

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
system.ddrmem1.port = system.membus.master
system.ddrmem2.port = system.membus.master
system.ddrmem3.port = system.membus.master
system.shmem.port = system.membus.master
CacheConfig.config_cache(options, system)

root = Root(full_system = False, mapu_system = False, system = system)
Simulation.run(options, root, system, FutureClass)
