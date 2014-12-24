from m5.defines import buildEnv

from System import *

class BareIronMapuSystem(MapuSystem):
    type = 'BareIronMapuSystem'
    system_type = 34
    system_rev = 1 << 10
    hex_file_name = Param.String('test.hex',"hex file that contains [address,data] pairs")

