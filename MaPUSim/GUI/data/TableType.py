
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

class TimeItem:
    def __init__(self):
	self.time="-1"
	#DM BIU0
	self.DMBIU0="-1"
	self.DMBIU0Value="'nop'"
	self.BIU0DM="-1"
	self.BIU0DMValue="'nop'"
	#DM BIU1
	self.DMBIU1="-1"
	self.DMBIU1Value="'nop'"
	self.BIU1DM="-1"
	self.BIU1DMValue="'nop'"
	#DM BIU2
	self.DMBIU2="-1"
	self.DMBIU2Value="'nop'"
	self.BIU2DM="-1"
	self.BIU2DMValue="'nop'"
	#BIU0 SHU0
	self.BIU0SHU0="-1"
	self.BIU0SHU0Value="'nop'"
	self.SHU0BIU0="-1"
	self.SHU0BIU0Value="'nop'"
	#BIU1 SHU0
	self.BIU1SHU0="-1"
	self.BIU1SHU0Value="'nop'"
	self.SHU0BIU1="-1"
	self.SHU0BIU1Value="'nop'"
	#BIU2 SHU0
	self.BIU2SHU0="-1"
	self.BIU2SHU0Value="'nop'"
	self.SHU0BIU2="-1"
	self.SHU0BIU2Value="'nop'"
	#BIU0 SHU1
	self.BIU0SHU1="-1"
	self.BIU0SHU1Value="'nop'"
	self.SHU1BIU0="-1"
	self.SHU1BIU0Value="'nop'"
	#BIU1 SHU1
	self.BIU1SHU1="-1"
	self.BIU1SHU1Value="'nop'"
	self.SHU1BIU1="-1"
	self.SHU1BIU1Value="'nop'"
	#BIU2 SHU1
	self.BIU2SHU1="-1"
	self.BIU2SHU1Value="'nop'"
	self.SHU1BIU2="-1"
	self.SHU1BIU2Value="'nop'"
	#BIU0 MRF
	self.BIU0MRF="-1"
	self.BIU0MRFValue="'nop'"
	self.MRFBIU0="-1"
	self.MRFBIU0Value="'nop'"
	#BIU1 MRF
	self.BIU1MRF="-1"
	self.BIU1MRFValue="'nop'"
	self.MRFBIU1="-1"
	self.MRFBIU1Value="'nop'"
	#BIU2 MRF
	self.BIU2MRF="-1"
	self.BIU2MRFValue="'nop'"
	self.MRFBIU2="-1"
	self.MRFBIU2Value="'nop'"
	#SHU0 IALU
	self.SHU0IALU="-1"
	self.SHU0IALUValue="'nop'"
	self.IALUSHU0="-1"
	self.IALUSHU0Value="'nop'"
	#SHU0 IMAC
	self.SHU0IMAC="-1"
	self.SHU0IMACValue="'nop'"
	self.IMACSHU0="-1"
	self.IMACSHU0Value="'nop'"
	#SHU0 FALU
	self.SHU0FALU="-1"
	self.SHU0FALUValue="'nop'"
	self.FALUSHU0="-1"
	self.FALUSHU0Value="'nop'"
	#SHU0 FMAC
	self.SHU0FMAC="-1"
	self.SHU0FMACValue="'nop'"
	self.FMACSHU0="-1"
	self.FMACSHU0Value="'nop'"
	#MRF IALU
	self.MRFIALU="-1"
	self.MRFIALUValue="'nop'"
	self.IALUMRF="-1"
	self.IALUMRFValue="'nop'"
	#MRF IMAC
	self.MRFIMAC="-1"
	self.MRFIMACValue="'nop'"
	self.IMACMRF="-1"
	self.IMACMRFValue="'nop'"
	#MRF FALU
	self.MRFFALU="-1"
	self.MRFFALUValue="'nop'"
	self.FALUMRF="-1"
	self.FALUMRFValue="'nop'"
	#MRF FMAC
	self.MRFFMAC="-1"
	self.MRFFMACValue="'nop'"
	self.FMACMRF="-1"
	self.FMACMRFValue="'nop'"
	#SHU1 IALU
	self.SHU1IALU="-1"
	self.SHU1IALUValue="'nop'"
	self.IALUSHU1="-1"
	self.IALUSHU1Value="'nop'"
	#SHU1 IMAC
	self.SHU1IMAC="-1"
	self.SHU1IMACValue="'nop'"
	self.IMACSHU1="-1"
	self.IMACSHU1Value="'nop'"
	#SHU1 FALU
	self.SHU1FALU="-1"
	self.SHU1FALUValue="'nop'"
	self.FALUSHU1="-1"
	self.FALUSHU1Value="'nop'"
	#SHU1 FMAC
	self.SHU1FMAC="-1"
	self.SHU1FMACValue="'nop'"
	self.FMACSHU1="-1"
	self.FMACSHU1Value="'nop'"
	#IALU FALU
	self.IALUFALU="-1"
	self.IALUFALUValue="'nop'"
	self.FALUIALU="-1"
	self.FALUIALUValue="'nop'"
	#SHU0 MRF
	self.SHU0MRF="-1"
	self.SHU0MRFValue="'nop'"
	self.MRFSHU0="-1"
	self.MRFSHU0Value="'nop'"
	#MRF SHU1
	self.MRFSHU1="-1"
	self.MRFSHU1Value="'nop'"
	self.SHU1MRF="-1"
	self.SHU1MRFValue="'nop'"
	#IALU IMAC
	self.IALUIMAC="-1"
	self.IALUIMACValue="'nop'"
	self.IMACIALU="-1"
	self.IMACIALUValue="'nop'"
	#IMAC FALU
	self.IMACFALU="-1"
	self.IMACFALUValue="'nop'"
	self.FALUIMAC="-1"
	self.FALUIMACValue="'nop'"
	#FALU FMAC
	self.FALUFMAC="-1"
	self.FALUFMACValue="'nop'"
	self.FMACFALU="-1"
	self.FMACFALUValue="'nop'"




