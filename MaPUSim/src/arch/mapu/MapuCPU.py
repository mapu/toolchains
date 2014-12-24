from m5.params import *
from m5.proxy import *
from m5.objects import *

from Bus import NoncoherentBus
from Bus import PriorityBus
from Bridge import Bridge
from Bridge import DMBridge
from Bridge import MIMBridge
from InOrderCPU import InOrderCPU
from MemObject import MemObject

class CoProcessor(MemObject):
    type = "CoProcessor"
    abstract = True
    cop = SlavePort("Co-processor I/O port")
    cop_addr = Param.Addr("Device Address")
    cop_size = Param.Addr("Device Map Size")
    cop_latency = Param.Latency('1ns', "Programmed IO latency in simticks")
    cpu = Param.MapuCPU(Parent.any, "CPU this co-processor is part of")

class TurboDecoder(CoProcessor):
    type = "TurboDecoder"

class FixPointTurboDecoder(CoProcessor):
    type = "FixPointTurboDecoder"

class MapuCPU(InOrderCPU):
    type = "MapuCPU"

    turbodec = CoProcessor()

    sfetch_port = MasterPort("Instruction Port")
    agu_port = MasterPort("AGU Data Port")
    csuext_port = MasterPort("CSU DMA external Port")
    csuint_port = MasterPort("CSU DMA internal Port")
    biu_port = MasterPort("BIU Port")
    mfetch_port = MasterPort("MPU Instruction Port")
    turbo_port = MasterPort("Turbo decoder Port")
    ibus = PriorityBus()
    mibus = PriorityBus()
    dbus = PriorityBus()

    # Port buses
    agu_bus = NoncoherentBus()
    csuext_bus = NoncoherentBus()
    csuint_bus = NoncoherentBus()
    #hiobus = NoncoherentBus()
    im2sys_bridge = DMBridge(req_size = 1, queue_delay = '2000ps', req_delay = '100ps')
    mim2sys_bridge = MIMBridge(req_size = 2, queue_delay = '1001ps', req_delay = '0ps')
    dm02sys_bridge = DMBridge(req_size = 1, queue_delay = '2000ps', req_delay = '1001ps')
    dm12sys_bridge = DMBridge(req_size = 1, queue_delay = '2000ps', req_delay = '1001ps')
    dm22sys_bridge = DMBridge(req_size = 1, queue_delay = '2000ps', req_delay = '1001ps')
    dm32sys_bridge = DMBridge(req_size = 1, queue_delay = '2000ps', req_delay = '1001ps')
    dm42sys_bridge = DMBridge(req_size = 1, queue_delay = '2000ps', req_delay = '1001ps')
    dm52sys_bridge = DMBridge(req_size = 1, queue_delay = '2000ps', req_delay = '1001ps')
    a2i_bridge = Bridge()
    a2d_bridge = Bridge()
    a2s_bridge = Bridge(ranges = [AddrRange(0x40400000, 0x407FFFFF)])
    c2i_bridge = Bridge()
    c2mi_bridge = Bridge()
    c2d_bridge = Bridge()
    c2c_bridge = Bridge(ranges = [AddrRange(0x0, 0xFFFFFFF)])
    c2ddr0_bridge = Bridge(ranges = [AddrRange(0x60000000, 0x6FFFFFFF)])
    c2ddr1_bridge = Bridge(ranges = [AddrRange(0x70000000, 0x7FFFFFFF)])
    c2ddr2_bridge = Bridge(ranges = [AddrRange(0x80000000, 0x9FFFFFFF)])
    c2ddr3_bridge = Bridge(ranges = [AddrRange(0xA0000000, 0xBFFFFFFF)])
    def connectAllPorts(self, system_bus, cbus, sbus, ddr0bus, ddr1bus, ddr2bus, ddr3bus):
        # declare all internal buses
        # The order of the ports connetions SHALL NOT be changed, because it determines
        # the priority of each master port on the PriorityBuses

        # spu fetching port
        self.sfetch_port = self.ibus.slave
        # mpu fetching port
        self.mfetch_port = self.mibus.slave
        # biu port
        self.biu_port = self.dbus.slave
        # turbo decoder port
        self.turbo_port = self.dbus.slave
        # agu port
        self.a2i_bridge.ranges = [AddrRange(0x0 + self.cpu_id.value * 0x1000000, 0x1FFFFF + self.cpu_id.value * 0x1000000)]
        self.a2d_bridge.ranges = [AddrRange(0x400000 + self.cpu_id.value * 0x1000000, 0xFFFFFF + self.cpu_id.value * 0x1000000)]
        self.agu_port = self.agu_bus.slave
        self.agu_bus.master = self.a2i_bridge.slave
        self.a2i_bridge.master = self.ibus.slave
        self.agu_bus.master = self.a2d_bridge.slave
        self.a2d_bridge.master = self.dbus.slave
        self.agu_bus.master = self.a2s_bridge.slave
        self.a2s_bridge.master = sbus.slave
        self.agu_bus.master = self.turbodec.cop
        #agu_bus.slave = sbus.master
        #self.agu_bus.slave = system_bus.master
        # csu import port
        self.c2i_bridge.ranges = [AddrRange(0x0 + self.cpu_id.value * 0x1000000, 0x1FFFFF + self.cpu_id.value * 0x1000000)]
        self.c2mi_bridge.ranges = [AddrRange(0x200000 + self.cpu_id.value * 0x1000000, 0x3FFFFF + self.cpu_id.value * 0x1000000)]
        self.c2d_bridge.ranges = [AddrRange(0x400000 + self.cpu_id.value * 0x1000000, 0xFFFFFF + self.cpu_id.value * 0x1000000)]
        self.csuint_port = self.csuint_bus.slave
        self.csuint_bus.master = self.c2i_bridge.slave
        self.c2i_bridge.master = self.ibus.slave
        self.csuint_bus.master = self.c2mi_bridge.slave
        self.c2mi_bridge.master = self.mibus.slave
        self.csuint_bus.master = self.c2d_bridge.slave
        self.c2d_bridge.master = self.dbus.slave
        # csu export port
        self.csuext_port = self.csuext_bus.slave
        self.csuext_bus.master = self.c2c_bridge.slave
        self.c2c_bridge.master = cbus.slave
        self.csuext_bus.master = self.c2ddr0_bridge.slave
        self.c2ddr0_bridge.master = ddr0bus.slave
        self.csuext_bus.master = self.c2ddr1_bridge.slave
        self.c2ddr1_bridge.master = ddr1bus.slave
        self.csuext_bus.master = self.c2ddr2_bridge.slave
        self.c2ddr2_bridge.master = ddr2bus.slave
        self.csuext_bus.master = self.c2ddr3_bridge.slave
        self.c2ddr3_bridge.master = ddr3bus.slave
        #csu_bus.slave = hiobus.master
        # im
        self.ibus.master = self.im2sys_bridge.slave
        self.im2sys_bridge.master = system_bus.slave
        # mim
        self.mibus.master = self.mim2sys_bridge.slave
        self.mim2sys_bridge.master = system_bus.slave
        # dm
        self.dm02sys_bridge.ranges = [AddrRange(0x400000 + self.cpu_id.value * 0x1000000, 0x5FFFFF + self.cpu_id.value * 0x1000000)]
        self.dm12sys_bridge.ranges = [AddrRange(0x600000 + self.cpu_id.value * 0x1000000, 0x7FFFFF + self.cpu_id.value * 0x1000000)]
        self.dm22sys_bridge.ranges = [AddrRange(0x800000 + self.cpu_id.value * 0x1000000, 0x9FFFFF + self.cpu_id.value * 0x1000000)]
        self.dm32sys_bridge.ranges = [AddrRange(0xA00000 + self.cpu_id.value * 0x1000000, 0xBFFFFF + self.cpu_id.value * 0x1000000)]
        self.dm42sys_bridge.ranges = [AddrRange(0xC00000 + self.cpu_id.value * 0x1000000, 0xDFFFFF + self.cpu_id.value * 0x1000000)]
        self.dm52sys_bridge.ranges = [AddrRange(0xE00000 + self.cpu_id.value * 0x1000000, 0xFFFFFF + self.cpu_id.value * 0x1000000)]
        self.dbus.master = self.dm02sys_bridge.slave
        self.dbus.master = self.dm12sys_bridge.slave
        self.dbus.master = self.dm22sys_bridge.slave
        self.dbus.master = self.dm32sys_bridge.slave
        self.dbus.master = self.dm42sys_bridge.slave
        self.dbus.master = self.dm52sys_bridge.slave
        self.dm02sys_bridge.master = system_bus.slave
        self.dm12sys_bridge.master = system_bus.slave
        self.dm22sys_bridge.master = system_bus.slave
        self.dm32sys_bridge.master = system_bus.slave
        self.dm42sys_bridge.master = system_bus.slave
        self.dm52sys_bridge.master = system_bus.slave
        # outside
        #cbus.slave = system_bus.master
