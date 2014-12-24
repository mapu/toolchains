from m5.defines import buildEnv
from m5.params import *
from m5.proxy import *

from System import System

class MapuSystem(System):
    type = 'MapuSystem'
    bare_iron = Param.Bool(False, "Using Bare Iron Mode?")
    system_type = Param.UInt64(34, "Type of system we are emulating")
    system_rev = Param.UInt64(1 << 10, "Revision of system we are emulating")
    load_addr_mask = 0xffffffffff

class LinuxMapuSystem(MapuSystem):
    type = 'LinuxMapuSystem'
    system_type = 34
    system_rev = 1 << 10

    boot_cpu_frequency = Param.Frequency(Self.cpu[0].clock.frequency,
                                         "boot processor frequency")

class BareIronMapuSystem(MapuSystem):
    type = 'BareIronMapuSystem'
    bare_iron = True
    system_type = 34
    system_rev = 1 << 10
    hex_file_name = Param.String('test.hex',"hex file that contains [address,data] pairs")

