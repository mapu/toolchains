
class SnItem:
    def __init__(self):
	self.time="-1"
	self.cpu="-1"
	self.spumpu="'nop'"
	self.sn="-1"
	self.sln="-1"
	self.pc="'nop'"
	self.dis="'nop'"
	self.dest="'nop'"
	self.num="-1"
	self.stageList=["-1"]*20

class RegItem:
    def __init__(self):
	self.time="-1"
	self.cpu="-1"
	self.spumpu="'nop'"
	self.sn="-1"
	self.op="'nop'" #R/W
	self.type="'nop'" #mpu reg
	self.reg="-1"
	self.dis="'nop'"

class MemItem:
    def __init__(self):
	self.time="-1"
	self.type="'nop'" #mpu reg
	self.addr="'nop'"
	self.dis="'nop'"





