#File Name    :  Makefile
#Creating Date:  2012-10-17
#Author       :  leizu.yin@ia.ac.cn
#Description  :
#Last Commit  :  $Id: Makefile 1405 2013-01-05 07:37:27Z zhangx $

ToolPath=../../../../../Software/SPU
include ../../../Makefile.Sim

LOG	    = Run.log
TestTop     = ../../../Source/TestTop.sv 
TestIF      = ../../../Source/AXIBusIF.sv
TestCase    = APCTest.sv
TestConfig  = TestConfig.sv
FILE_LIST   = -f filelist.f ${TestConfig} ${TestTop} ${TestCase} ${TestIF} 
DPIFILE     = ../../../Source/fifo-server.cpp
VCS_DEFINE  = +define+PLATFORM_${Platform}  +define+DVE_DBG +define+UNIT_DELAY +define+no_warning +define+OVL_SVA+OVL_ASSERT_ON+OVL_COVER_ON+OVL_XCHECK_OFF
VCS_RGRS_DEFINE  = +define+PLATFORM_${Platform} +define+UNIT_DELAY +define+no_warning +define+OVL_SVA+OVL_ASSERT_ON+OVL_COVER_ON+OVL_XCHECK_OFF
VCS_INCLUDE = +incdir+directory+../../../../../RTL/Include +incdir+directory+../../../Source/      +incdir+directory+../../../../Syn/Source/MaPU_HPD/ACU/include +incdir+directory+../../../../Syn/Source/MaPU_HPD/SCU/include  +incdir+directory+${QHOME}/share/assertion_lib/OVL/verilog +incdir+directory+../../../../Syn/Source/MaPU_HPD/SystemVerilogForTb/include
VCS_OPTION  = -full64 +plusarg_save +sv_lib ../../../../Syn/Source/MaPU_HPD/xiaorz/SourceCode/Float.so -sverilog +libext+.v+.sv -y ${QHOME}/share/assertion_lib/OVL/verilog ${VCS_INCLUDE} ${VCS_DEFINE} +nospecify +notimingcheck -R -debug -q -l ${LOG} +memcbk -y ${QHOME}/share/assertion_lib/OVL/verilog 
VCS_RGRS_OPTION  = -full64 -sverilog +libext+.v+.sv -y ${QHOME}/share/assertion_lib/OVL/verilog ${VCS_INCLUDE} ${VCS_RGRS_DEFINE} +nospecify +notimingcheck -R -debug -q -l ${LOG} +memcbk -y ${QHOME}/share/assertion_lib/OVL/verilog +plusarg_save +sv_lib ../../../../Syn/Source/MaPU_HPD/xiaorz/SourceCode/Float.so
VCS_DEBUG_OPTION = -full64 +plusarg_save +sv_lib ../../../../Syn/Source/MaPU_HPD/xiaorz/SourceCode/Float.so -sverilog +libext+.v+.sv -y ${QHOME}/share/assertion_lib/OVL/verilog ${VCS_INCLUDE} ${VCS_DEFINE} +nospecify +notimingcheck  -debug -q +memcbk
COV_OPTION  = -cm line+cond -cm_line contassign -cm_cond allops
COV_OPTION_RUN  = -cm line+cond -cm_line contassign allops

all:clean APP
	#Generate the filelist 
	$(call GenFilelist)
	vcs ${VCS_OPTION} ${FILE_LIST} ${COV_OPTION}
	urg -dir simv.vdb -format text
	@rm -rf simv simv.daidir work  *.lib++ *~ *.rc csrc *.key filelist.f DVEFiles, *.vpd
	@rm -rf *.lof  *.out  *.map  *.mmap  *.sct  *.tab  *.s  *.dis  *~  *.def *.xml *.data

dve_dbg:APP
	#Generate the filelist
	$(call GenFilelist)
	vcs ${VCS_DEBUG_OPTION} ${FILE_LIST} ${COV_OPTION}
	./simv +vcs+stop+2000 ${COV_OPTION_RUN}
	urg -dir simv.vdb -format both

OCD_Debug:APP
	$(call GenFilelist)
	vcs ${VCS_DEBUG_OPTION} ${FILE_LIST} ${DPIFILE} -debug_pp +define+OCD_DBG +define+DVE_DBG +vcsd

rgrs_debug:APP_NOCKOUT
	$(call GenFilelist)
	vcs ${VCS_RGRS_OPTION} ${FILE_LIST}
	@rm -rf *.lof  *.out  *.map  *.mmap  *.sct  *.tab  *.s  *.dis  *~  *.def *.xml *.data
	@rm -rf simv*  work  *.lib++ *~ *.rc csrc *.key filelist.f
	@rm -rf *.vpd DVEfiles vc_hdrs.h urgReport
	@rm -rf *.o *.sym *.txt

APP_NOCKOUT:
	$(call AsIns)
	$(call LnkIns)
	$(call DisAsIns)
	$(call GemInsData)
	$(call GenMaPUData)

APP:
	$(call ChkOutMaPU_HPD)
	$(call AsIns)
	$(call LnkIns)
	$(call DisAsIns)
	$(call GemInsData)
#	${ToolPath}/PostProcessDis.sh
	$(call GenMaPUData)

clean:
	@rm -rf *.lof  *.out  *.map  *.mmap  *.sct  *.tab  *.s  *.dis  *~  *.def *.xml *.data
	@rm -rf simv*  work  *.lib++ *Log *.log *~ *.rc csrc *.key filelist.f
	@rm -rf *.vpd DVEfiles vc_hdrs.h urgReport
	@rm -rf *.o *.sym *.txt
