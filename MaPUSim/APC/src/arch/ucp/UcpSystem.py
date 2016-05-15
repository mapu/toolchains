from m5.defines import buildEnv
from m5.params import *
from m5.proxy import *

from System import System

class UcpSystem(System):
    type = 'UcpSystem'
    bare_iron = Param.Bool(False, "Using Bare Iron Mode?")
    system_type = Param.UInt64(34, "Type of system we are emulating")
    system_rev = Param.UInt64(1 << 10, "Revision of system we are emulating")
    load_addr_mask = 0xffffffffff

class LinuxUcpSystem(UcpSystem):
    type = 'LinuxUcpSystem'
    system_type = 34
    system_rev = 1 << 10

    boot_cpu_frequency = Param.Frequency(Self.cpu[0].clock.frequency,
                                         "boot processor frequency")

