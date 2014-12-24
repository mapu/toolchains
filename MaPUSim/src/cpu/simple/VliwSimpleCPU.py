from m5.params import *
from BaseSimpleCPU import BaseSimpleCPU

class VliwSimpleCPU(BaseSimpleCPU):
    type = 'VliwSimpleCPU'
    width = Param.Int(1, "CPU width")
    simulate_data_stalls = Param.Bool(False, "Simulate dcache stall cycles")
    simulate_inst_stalls = Param.Bool(False, "Simulate icache stall cycles")
    fastmem = Param.Bool(False, "Access memory directly")
