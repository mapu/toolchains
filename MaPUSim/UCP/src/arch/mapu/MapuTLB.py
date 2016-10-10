from m5.SimObject import SimObject
from m5.params import *

from BaseTLB import BaseTLB

class MapuTLB(BaseTLB):
    type = 'MapuTLB'
    cxx_class = 'MapuISA::TLB'
    size = Param.Int(64, "TLB size")
    cpuid = Param.Int(-1, "APE identifier")
