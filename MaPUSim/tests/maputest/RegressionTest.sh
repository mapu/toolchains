#!/bin/sh
# run the Gem5 regression test
###########################################################
#############################################################
echo "==========================================================================="
echo "Running regression test for MaPU_Gem5 ..."
if [  -d ./m5out ];\
then rm -rf m5out
fi

mkdir m5out

echo "Running single instruction test ..."
echo "1.Running asm test ..."
if [ ! -d ./m5out/asmTest ];\
then mkdir m5out/asmTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=asmTest/asm.out --stats-file=asmTest/stat.txt --dump-config=asmTest/config.ini --json-config=asmTest/config.json -r --stdout-file=asmTest/stdout -e --stderr-file=asmTest/stderr  ../../configs/example/se.py -c asm/app.s.out
if cmp -s m5out/asmTest/asm.out ./asm/asm_result.out
then echo -e "\e[32m\e[1m asm test passed~\e[0m" 
rm -rf m5out/asmTest
else echo -e "\e[31m\e[1mError in asm test!\e[0m"
fi

echo "2.Running Branch test ..."
if [ ! -d ./m5out/BranchTest ];\
then mkdir m5out/BranchTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=BranchTest/Branch.out --stats-file=BranchTest/stat.txt --dump-config=BranchTest/config.ini --json-config=BranchTest/config.json -r --stdout-file=BranchTest/stdout -e --stderr-file=BranchTest/stderr  ../../configs/example/se.py -c BranchTest/app.s.out
if cmp -s m5out/BranchTest/Branch.out ./BranchTest/Branch_result.out
then echo -e "\e[32m\e[1m Branch test passed~\e[0m" 
rm -rf m5out/BranchTest
else echo -e "\e[31m\e[1mError in Branch test!\e[0m"
fi

echo "3.Running Call test ..."
if [ ! -d ./m5out/CallTest ];\
then mkdir m5out/CallTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=CallTest/Call.out --stats-file=CallTest/stat.txt --dump-config=CallTest/config.ini --json-config=CallTest/config.json -r --stdout-file=CallTest/stdout -e --stderr-file=CallTest/stderr  ../../configs/example/se.py -c CallTest/app.s.out
if cmp -s m5out/CallTest/Call.out ./CallTest/Call_result.out
then echo -e "\e[32m\e[1m Call test passed~\e[0m" 
rm -rf m5out/CallTest
else echo -e "\e[31m\e[1mError in Call test!\e[0m"
fi

echo "4.Running Vliw test ..."
if [ ! -d ./m5out/VliwTest ];\
then mkdir m5out/VliwTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=VliwTest/Vliw.out --stats-file=VliwTest/stat.txt --dump-config=VliwTest/config.ini --json-config=VliwTest/config.json -r --stdout-file=VliwTest/stdout -e --stderr-file=VliwTest/stderr  ../../configs/example/se.py -c VliwTest/app.s.out
if cmp -s m5out/VliwTest/Vliw.out ./VliwTest/Vliw_result.out
then echo -e "\e[32m\e[1m Vliw test passed~\e[0m" 
rm -rf m5out/VliwTest
else echo -e "\e[31m\e[1mError in Vliw test!\e[0m"
fi

echo "5.Running SYN test ..."
if [ ! -d ./m5out/SYNTest ];\
then mkdir m5out/SYNTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=SYNTest/SYN.out --stats-file=SYNTest/stat.txt --dump-config=SYNTest/config.ini --json-config=SYNTest/config.json -r --stdout-file=SYNTest/stdout -e --stderr-file=SYNTest/stderr  ../../configs/example/se.py -c SYNTest/app.s.out
if cmp -s m5out/SYNTest/SYN.out ./SYNTest/SYN_result.out
then echo -e "\e[32m\e[1m SYN test passed~\e[0m" 
rm -rf m5out/SYNTest
else echo -e "\e[31m\e[1mError in SYN test!\e[0m"
fi


echo "6.Running CSU test ..."
if [ ! -d ./m5out/CSUTest ];\
then mkdir m5out/CSUTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=CSUTest/CSU.out --stats-file=CSUTest/stat.txt --dump-config=CSUTest/config.ini --json-config=CSUTest/config.json -r --stdout-file=CSUTest/stdout -e --stderr-file=CSUTest/stderr  ../../configs/example/se.py -c CSUTest/app.s.out
if cmp -s m5out/CSUTest/CSU.out ./CSUTest/CSU_result.out
then echo -e "\e[32m\e[1m CSU test passed~\e[0m" 
rm -rf m5out/CSUTest
else echo -e "\e[31m\e[1mError in CSU test!\e[0m"
fi

echo "7.Running FALU test ..."
if [ ! -d ./m5out/FALUTest ];\
then mkdir m5out/FALUTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=FALUTest/FALU.out --stats-file=FALUTest/stat.txt --dump-config=FALUTest/config.ini --json-config=FALUTest/config.json -r --stdout-file=FALUTest/stdout -e --stderr-file=FALUTest/stderr  ../../configs/example/se.py -c FALUTest/app.s.out,FALUTest/app.m.out
if cmp -s m5out/FALUTest/FALU.out ./FALUTest/FALU_result.out
then echo -e "\e[32m\e[1m FALU test passed~\e[0m" 
rm -rf m5out/FALUTest
else echo -e "\e[31m\e[1mError in FALU test!\e[0m"
fi

echo "8.Running FMAC test ..."
if [ ! -d ./m5out/FMACTest ];\
then mkdir m5out/FMACTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=FMACTest/FMAC.out --stats-file=FMACTest/stat.txt --dump-config=FMACTest/config.ini --json-config=FMACTest/config.json -r --stdout-file=FMACTest/stdout -e --stderr-file=FMACTest/stderr  ../../configs/example/se.py -c FMACTest/app.s.out,FMACTest/app.m.out
if cmp -s m5out/FMACTest/FMAC.out ./FMACTest/FMAC_result.out
then echo -e "\e[32m\e[1m FMAC test passed~\e[0m" 
rm -rf m5out/FMACTest
else echo -e "\e[31m\e[1mError in FMAC test!\e[0m"
fi

echo "9.Running IALU test ..."
if [ ! -d ./m5out/IALUTest ];\
then mkdir m5out/IALUTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=IALUTest/IALU.out --stats-file=IALUTest/stat.txt --dump-config=IALUTest/config.ini --json-config=IALUTest/config.json -r --stdout-file=IALUTest/stdout -e --stderr-file=IALUTest/stderr  ../../configs/example/se.py -c IALUTest/app.s.out,IALUTest/app.m.out
if cmp -s m5out/IALUTest/IALU.out ./IALUTest/IALU_result.out
then echo -e "\e[32m\e[1m IALU test passed~\e[0m" 
rm -rf m5out/IALUTest
else echo -e "\e[31m\e[1mError in IALU test!\e[0m"
fi

echo "10.Running IMACComp test ..."
if [ ! -d ./m5out/IMACCompTest ];\
then mkdir m5out/IMACCompTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=IMACCompTest/IMACComp.out --stats-file=IMACCompTest/stat.txt --dump-config=IMACCompTest/config.ini --json-config=IMACCompTest/config.json -r --stdout-file=IMACCompTest/stdout -e --stderr-file=IMACCompTest/stderr  ../../configs/example/se.py -c IMACCompTest/app.s.out,IMACCompTest/app.m.out
if cmp -s m5out/IMACCompTest/IMACComp.out ./IMACCompTest/IMACComp_result.out
then echo -e "\e[32m\e[1m IMACComp test passed~\e[0m" 
rm -rf m5out/IMACCompTest
else echo -e "\e[31m\e[1mError in IMACComp test!\e[0m"
fi

echo "11.Running IMACMR test ..."
if [ ! -d ./m5out/IMACMRTest ];\
then mkdir m5out/IMACMRTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=IMACMRTest/IMACMR.out --stats-file=IMACMRTest/stat.txt --dump-config=IMACMRTest/config.ini --json-config=IMACMRTest/config.json -r --stdout-file=IMACMRTest/stdout -e --stderr-file=IMACMRTest/stderr  ../../configs/example/se.py -c IMACMRTest/app.s.out,IMACMRTest/app.m.out
if cmp -s m5out/IMACMRTest/IMACMR.out ./IMACMRTest/IMACMR_result.out
then echo -e "\e[32m\e[1m IMACMR test passed~\e[0m" 
rm -rf m5out/IMACMRTest
else echo -e "\e[31m\e[1mError in IMACMR test!\e[0m"
fi

echo "12.Running IMACMul test ..."
if [ ! -d ./m5out/IMACMulTest ];\
then mkdir m5out/IMACMulTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=IMACMulTest/IMACMul.out --stats-file=IMACMulTest/stat.txt --dump-config=IMACMulTest/config.ini --json-config=IMACMulTest/config.json -r --stdout-file=IMACMulTest/stdout -e --stderr-file=IMACMulTest/stderr  ../../configs/example/se.py -c IMACMulTest/app.s.out,IMACMulTest/app.m.out
if cmp -s m5out/IMACMulTest/IMACMul.out ./IMACMulTest/IMACMul_result.out
then echo -e "\e[32m\e[1m IMACMul test passed~\e[0m" 
rm -rf m5out/IMACMulTest
else echo -e "\e[31m\e[1mError in IMACMul test!\e[0m"
fi

echo "13.Running Cond test ..."
if [ ! -d ./m5out/CondTest ];\
then mkdir m5out/CondTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=CondTest/Cond.out --stats-file=CondTest/stat.txt --dump-config=CondTest/config.ini --json-config=CondTest/config.json -r --stdout-file=CondTest/stdout -e --stderr-file=CondTest/stderr  ../../configs/example/se.py -c CondTest/app.s.out,CondTest/app.m.out
if cmp -s m5out/CondTest/Cond.out ./CondTest/Cond_result.out
then echo -e "\e[32m\e[1m Cond test passed~\e[0m" 
rm -rf m5out/CondTest
else echo -e "\e[31m\e[1mError in Cond test!\e[0m"
fi

echo "14.Running c test ..."
if [ ! -d ./m5out/cTest ];\
then mkdir m5out/cTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=cTest/c.out --stats-file=cTest/stat.txt --dump-config=cTest/config.ini --json-config=cTest/config.json -r --stdout-file=cTest/stdout -e --stderr-file=cTest/stderr  ../../configs/example/se.py -c c/app.s.out
if cmp -s m5out/cTest/c.out ./c/c_result.out
then echo -e "\e[32m\e[1m c test passed~\e[0m" 
rm -rf m5out/cTest
else echo -e "\e[31m\e[1mError in c test!\e[0m"
fi

echo "15.Running CI test ..."
if [ ! -d ./m5out/CITest ];\
then mkdir m5out/CITest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=CITest/CI.out --stats-file=CITest/stat.txt --dump-config=CITest/config.ini --json-config=CITest/config.json -r --stdout-file=CITest/stdout -e --stderr-file=CITest/stderr  ../../configs/example/se.py -c CITest/app.s.out
if cmp -s m5out/CITest/CI.out ./CITest/CI_result.out
then echo -e "\e[32m\e[1m CI test passed~\e[0m" 
rm -rf m5out/CITest
else echo -e "\e[31m\e[1mError in CI test!\e[0m"
fi

echo "16.Running SM test ..."
if [ ! -d ./m5out/SMTest ];\
then mkdir m5out/SMTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=SMTest/SM.out --stats-file=SMTest/stat.txt --dump-config=SMTest/config.ini --json-config=SMTest/config.json -r --stdout-file=SMTest/stdout -e --stderr-file=SMTest/stderr  ../../configs/example/se.py -c SMTest/app.s.out
if cmp -s m5out/SMTest/SM.out ./SMTest/SM_result.out
then echo -e "\e[32m\e[1m SM test passed~\e[0m" 
rm -rf m5out/SMTest
else echo -e "\e[31m\e[1mError in SM test!\e[0m"
fi

echo "17.Running SM2APE test ..."
if [ ! -d ./m5out/SM2APETest ];\
then mkdir m5out/SM2APETest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=SM2APETest/SM2APE.out --stats-file=SM2APETest/stat.txt --dump-config=SM2APETest/config.ini --json-config=SM2APETest/config.json -r --stdout-file=SM2APETest/stdout -e --stderr-file=SM2APETest/stderr  ../../configs/example/se.py -c "SM2APETest/app0.s.out;SM2APETest/app1.s.out" -n 2
if cmp -s m5out/SM2APETest/SM2APE.out ./SM2APETest/SM2APE_result.out
then echo -e "\e[32m\e[1m SM2APE test passed~\e[0m" 
rm -rf m5out/SM2APETest
else echo -e "\e[31m\e[1mError in SM2APE test!\e[0m"
fi

echo "18.Running IMACMul1 test ..."
if [ ! -d ./m5out/IMACMul1Test ];\
then mkdir m5out/IMACMul1Test
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=IMACMul1Test/IMACMul1.out --stats-file=IMACMul1Test/stat.txt --dump-config=IMACMul1Test/config.ini --json-config=IMACMul1Test/config.json -r --stdout-file=IMACMul1Test/stdout -e --stderr-file=IMACMul1Test/stderr  ../../configs/example/se.py -c IMACMul1Test/app.s.out,IMACMul1Test/app.m.out
if cmp -s m5out/IMACMul1Test/IMACMul1.out ./IMACMul1Test/IMACMul1_result.out
then echo -e "\e[32m\e[1m IMACMul1 test passed~\e[0m" 
rm -rf m5out/IMACMul1Test
else echo -e "\e[31m\e[1mError in IMACMul1 test!\e[0m"
fi

echo "19.Running Div test ..."
if [ ! -d ./m5out/DivTest ];\
then mkdir m5out/DivTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=DivTest/Div.out --stats-file=DivTest/stat.txt --dump-config=DivTest/config.ini --json-config=DivTest/config.json -r --stdout-file=DivTest/stdout -e --stderr-file=DivTest/stderr  ../../configs/example/se.py -c DivTest/app.s.out,DivTest/app.m.out
if cmp -s m5out/DivTest/Div.out ./DivTest/Div_result.out
then echo -e "\e[32m\e[1m Div test passed~\e[0m" 
rm -rf m5out/DivTest
else echo -e "\e[31m\e[1mError in Div test!\e[0m"
fi

echo "20.Running RECIPEXPD test ..."
if [ ! -d ./m5out/RECIPEXPDTest ];\
then mkdir m5out/RECIPEXPDTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=RECIPEXPDTest/RECIPEXPD.out --stats-file=RECIPEXPDTest/stat.txt --dump-config=RECIPEXPDTest/config.ini --json-config=RECIPEXPDTest/config.json -r --stdout-file=RECIPEXPDTest/stdout -e --stderr-file=RECIPEXPDTest/stderr  ../../configs/example/se.py -c RECIPEXPDTest/app.s.out,RECIPEXPDTest/app.m.out
if cmp -s m5out/RECIPEXPDTest/RECIPEXPD.out ./RECIPEXPDTest/RECIPEXPD_result.out
then echo -e "\e[32m\e[1m RECIPEXPD test passed~\e[0m" 
rm -rf m5out/RECIPEXPDTest
else echo -e "\e[31m\e[1mError in RECIPEXPD test!\e[0m"
fi

echo "21.Running BIU0_LD_LD test ..."
if [ ! -d ./m5out/BIUKGTest ];\
then mkdir m5out/BIUKGTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=BIUKGTest/BIU0_LD_LD.out --stats-file=BIUKGTest/stat.txt --dump-config=BIUKGTest/config.ini --json-config=BIUKGTest/config.json -r --stdout-file=BIUKGTest/stdout -e --stderr-file=BIUKGTest/stderr  ../../configs/example/se.py -c BIUKGTest/BIU0_LD_LD/app.s.out,BIUKGTest/BIU0_LD_LD/app.m.out
if cmp -s m5out/BIUKGTest/BIU0_LD_LD.out ./BIUKGTest/BIU0_LD_LD/BIU0_LD_LD_result.out
then echo -e "\e[32m\e[1m BIU0_LD_LD test passed~\e[0m" 
#rm -rf m5out/IMACMulTest
else echo -e "\e[31m\e[1mError in BIU0_LD_LD test!\e[0m"
fi

echo "22.Running BIU0_LD_ST test ..."
if [ ! -d ./m5out/BIUKGTest ];\
then mkdir m5out/BIUKGTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=BIUKGTest/BIU0_LD_ST.out --stats-file=BIUKGTest/stat.txt --dump-config=BIUKGTest/config.ini --json-config=BIUKGTest/config.json -r --stdout-file=BIUKGTest/stdout -e --stderr-file=BIUKGTest/stderr  ../../configs/example/se.py -c BIUKGTest/BIU0_LD_ST/app.s.out,BIUKGTest/BIU0_LD_ST/app.m.out
if cmp -s m5out/BIUKGTest/BIU0_LD_ST.out ./BIUKGTest/BIU0_LD_ST/BIU0_LD_ST_result.out
then echo -e "\e[32m\e[1m BIU0_LD_ST test passed~\e[0m" 
#rm -rf m5out/IMACMulTest
else echo -e "\e[31m\e[1mError in BIU0_LD_ST test!\e[0m"
fi

echo "23.Running BIU0_ST_LD test ..."
if [ ! -d ./m5out/BIUKGTest ];\
then mkdir m5out/BIUKGTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=BIUKGTest/BIU0_ST_LD.out --stats-file=BIUKGTest/stat.txt --dump-config=BIUKGTest/config.ini --json-config=BIUKGTest/config.json -r --stdout-file=BIUKGTest/stdout -e --stderr-file=BIUKGTest/stderr  ../../configs/example/se.py -c BIUKGTest/BIU0_ST_LD/app.s.out,BIUKGTest/BIU0_ST_LD/app.m.out
if cmp -s m5out/BIUKGTest/BIU0_ST_LD.out ./BIUKGTest/BIU0_ST_LD/BIU0_ST_LD_result.out
then echo -e "\e[32m\e[1m BIU0_ST_LD test passed~\e[0m" 
#rm -rf m5out/IMACMulTest
else echo -e "\e[31m\e[1mError in BIU0_ST_LD test!\e[0m"
fi

echo "24.Running BIU1_LD_LD test ..."
if [ ! -d ./m5out/BIUKGTest ];\
then mkdir m5out/BIUKGTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=BIUKGTest/BIU1_LD_LD.out --stats-file=BIUKGTest/stat.txt --dump-config=BIUKGTest/config.ini --json-config=BIUKGTest/config.json -r --stdout-file=BIUKGTest/stdout -e --stderr-file=BIUKGTest/stderr  ../../configs/example/se.py -c BIUKGTest/BIU1_LD_LD/app.s.out,BIUKGTest/BIU1_LD_LD/app.m.out
if cmp -s m5out/BIUKGTest/BIU1_LD_LD.out ./BIUKGTest/BIU1_LD_LD/BIU1_LD_LD_result.out
then echo -e "\e[32m\e[1m BIU1_LD_LD test passed~\e[0m" 
#rm -rf m5out/IMACMulTest
else echo -e "\e[31m\e[1mError in BIU1_LD_LD test!\e[0m"
fi

echo "25.Running BIU1_LD_ST test ..."
if [ ! -d ./m5out/BIUKGTest ];\
then mkdir m5out/BIUKGTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=BIUKGTest/BIU1_LD_ST.out --stats-file=BIUKGTest/stat.txt --dump-config=BIUKGTest/config.ini --json-config=BIUKGTest/config.json -r --stdout-file=BIUKGTest/stdout -e --stderr-file=BIUKGTest/stderr  ../../configs/example/se.py -c BIUKGTest/BIU1_LD_ST/app.s.out,BIUKGTest/BIU1_LD_ST/app.m.out
if cmp -s m5out/BIUKGTest/BIU1_LD_ST.out ./BIUKGTest/BIU1_LD_ST/BIU1_LD_ST_result.out
then echo -e "\e[32m\e[1m BIU1_LD_ST test passed~\e[0m" 
#rm -rf m5out/IMACMulTest
else echo -e "\e[31m\e[1mError in BIU1_LD_ST test!\e[0m"
fi

echo "26.Running BIU1_ST_LD test ..."
if [ ! -d ./m5out/BIUKGTest ];\
then mkdir m5out/BIUKGTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=BIUKGTest/BIU1_ST_LD.out --stats-file=BIUKGTest/stat.txt --dump-config=BIUKGTest/config.ini --json-config=BIUKGTest/config.json -r --stdout-file=BIUKGTest/stdout -e --stderr-file=BIUKGTest/stderr  ../../configs/example/se.py -c BIUKGTest/BIU1_ST_LD/app.s.out,BIUKGTest/BIU1_ST_LD/app.m.out
if cmp -s m5out/BIUKGTest/BIU1_ST_LD.out ./BIUKGTest/BIU1_ST_LD/BIU1_ST_LD_result.out
then echo -e "\e[32m\e[1m BIU1_ST_LD test passed~\e[0m" 
#rm -rf m5out/IMACMulTest
else echo -e "\e[31m\e[1mError in BIU1_ST_LD test!\e[0m"
fi

echo "27.Running BIU2_LD_LD test ..."
if [ ! -d ./m5out/BIUKGTest ];\
then mkdir m5out/BIUKGTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=BIUKGTest/BIU2_LD_LD.out --stats-file=BIUKGTest/stat.txt --dump-config=BIUKGTest/config.ini --json-config=BIUKGTest/config.json -r --stdout-file=BIUKGTest/stdout -e --stderr-file=BIUKGTest/stderr  ../../configs/example/se.py -c BIUKGTest/BIU2_LD_LD/app.s.out,BIUKGTest/BIU2_LD_LD/app.m.out
if cmp -s m5out/BIUKGTest/BIU2_LD_LD.out ./BIUKGTest/BIU2_LD_LD/BIU2_LD_LD_result.out
then echo -e "\e[32m\e[1m BIU2_LD_LD test passed~\e[0m" 
#rm -rf m5out/IMACMulTest
else echo -e "\e[31m\e[1mError in BIU2_LD_LD test!\e[0m"
fi

echo "28.Running BIU2_LD_ST test ..."
if [ ! -d ./m5out/BIUKGTest ];\
then mkdir m5out/BIUKGTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=BIUKGTest/BIU2_LD_ST.out --stats-file=BIUKGTest/stat.txt --dump-config=BIUKGTest/config.ini --json-config=BIUKGTest/config.json -r --stdout-file=BIUKGTest/stdout -e --stderr-file=BIUKGTest/stderr  ../../configs/example/se.py -c BIUKGTest/BIU2_LD_ST/app.s.out,BIUKGTest/BIU2_LD_ST/app.m.out
if cmp -s m5out/BIUKGTest/BIU2_LD_ST.out ./BIUKGTest/BIU2_LD_ST/BIU2_LD_ST_result.out
then echo -e "\e[32m\e[1m BIU2_LD_ST test passed~\e[0m" 
#rm -rf m5out/IMACMulTest
else echo -e "\e[31m\e[1mError in BIU2_LD_ST test!\e[0m"
fi

echo "29.Running BIU2_ST_LD test ..."
if [ ! -d ./m5out/BIUKGTest ];\
then mkdir m5out/BIUKGTest
fi
../../build/MAPU/gem5.debug --debug-flags=Exec,-ExecSymbol --trace-file=BIUKGTest/BIU2_ST_LD.out --stats-file=BIUKGTest/stat.txt --dump-config=BIUKGTest/config.ini --json-config=BIUKGTest/config.json -r --stdout-file=BIUKGTest/stdout -e --stderr-file=BIUKGTest/stderr  ../../configs/example/se.py -c BIUKGTest/BIU2_ST_LD/app.s.out,BIUKGTest/BIU2_ST_LD/app.m.out
if cmp -s m5out/BIUKGTest/BIU2_ST_LD.out ./BIUKGTest/BIU2_ST_LD/BIU2_ST_LD_result.out
then echo -e "\e[32m\e[1m BIU2_ST_LD test passed~\e[0m" 
#rm -rf m5out/IMACMulTest
else echo -e "\e[31m\e[1mError in BIU2_ST_LD test!\e[0m"
fi

echo "Finish single instruction testing ..."

echo "========================================"

echo "Running microcode arithmetic test ..."

echo "1.Running TLB256 test ..."
if [ ! -d ./m5out/TLB256Test ];\
then mkdir m5out/TLB256Test
fi
../../build/MAPU/gem5.debug --debug-flags=MapuBIU --trace-file=TLB256Test/TLB256.out --stats-file=TLB256Test/stat.txt --dump-config=TLB256Test/config.ini --json-config=TLB256Test/config.json -r --stdout-file=TLB256Test/stdout -e --stderr-file=TLB256Test/stderr  ../../configs/example/se.py -c TLB256Test/app.s.out,TLB256Test/app.m.out
if cmp -s m5out/TLB256Test/TLB256.out ./TLB256Test/TLB256_result.out
then echo -e "\e[32m\e[1m TLB256 test passed~\e[0m" 
rm -rf m5out/TLB256Test
else echo -e "\e[31m\e[1mError in TLB256 test!\e[0m"
fi

echo "2.Running FIRFix test ..."
if [ ! -d ./m5out/FIRFixTest ];\
then mkdir m5out/FIRFixTest
fi
../../build/MAPU/gem5.debug --debug-flags=MapuBIU --trace-file=FIRFixTest/FIRFix.out --stats-file=FIRFixTest/stat.txt --dump-config=FIRFixTest/config.ini --json-config=FIRFixTest/config.json -r --stdout-file=FIRFixTest/stdout -e --stderr-file=FIRFixTest/stderr  ../../configs/example/se.py -c FIRFixTest/app.s.out,FIRFixTest/app.m.out
if cmp -s m5out/FIRFixTest/FIRFix.out ./FIRFixTest/FIRFix_result.out
then echo -e "\e[32m\e[1m FIRFix test passed~\e[0m" 
rm -rf m5out/FIRFixTest
else echo -e "\e[31m\e[1mError in FIRFix test!\e[0m"
fi

echo "3.Running FixVect test ..."
if [ ! -d ./m5out/FixVectTest ];\
then mkdir m5out/FixVectTest
fi
../../build/MAPU/gem5.debug --debug-flags=MapuBIU --trace-file=FixVectTest/FixVect.out --stats-file=FixVectTest/stat.txt --dump-config=FixVectTest/config.ini --json-config=FixVectTest/config.json -r --stdout-file=FixVectTest/stdout -e --stderr-file=FixVectTest/stderr  ../../configs/example/se.py -c FixVectTest/app.s.out,FixVectTest/app.m.out
if cmp -s m5out/FixVectTest/FixVect.out ./FixVectTest/FixVect_result.out
then echo -e "\e[32m\e[1m FixVect test passed~\e[0m" 
rm -rf m5out/FixVectTest
else echo -e "\e[31m\e[1mError in FixVect test!\e[0m"
fi

echo "Finish microcode arithmetic testing ..."


echo "========================================"
echo "Running Mapu full system test ..."
echo "1.Running ms_test..."
if [ ! -d ./m5out/ms_test ];\
then mkdir m5out/ms_test
fi
../../build/MAPU/gem5.debug --stats-file=ms_test/stat.txt --dump-config=ms_test/config.ini --json-config=ms_test/config.json -r --stdout-file=ms_test/stdout -e --stderr-file=ms_test/stderr ../../configs/example/ms.py -c /dev/ape -n 2&
cd ms_test/
make
./driver_test > ../m5out/ms_test/driver.out
if [ $? -ne 0 ]
then
echo -e "\e[31m\e[1mError in ms test!\e[0m"
cd ..
else
cd ..
if cmp -s m5out/ms_test/ms.out ./ms_test/ms_result.out
then echo -e "\e[32m\e[1m ms test passed~\e[0m" 
rm -rf m5out/ms_test
else echo -e "\e[31m\e[1mError in ms test!\e[0m"
fi
fi

echo "2.Running MatSF64MulTest..."
if [ ! -d ./m5out/MatSF64MulTest ];\
then mkdir m5out/MatSF64MulTest
fi
../../build/MAPU/gem5.debug --stats-file=MatSF64MulTest/stat.txt --dump-config=MatSF64MulTest/config.ini --json-config=MatSF64MulTest/config.json -r --stdout-file=MatSF64MulTest/stdout -e --stderr-file=MatSF64MulTest/stderr ../../configs/example/ms.py -c /dev/ape&
cd MatSF64MulTest
make
./matmul > ../m5out/MatSF64MulTest/driver.out
if [ $? -ne 0 ]
then
echo -e "\e[31m\e[1mError in MatSF64Mul test!\e[0m"
cd ..
else
cd ..
if [ -s ./m5out/MatSF64MulTest/MatSF64Mul.out ]
then echo -e "\e[31m\e[1mError in MatSF64Mul test!\e[0m"
else echo -e "\e[32m\e[1m MatSF64Mul test passed~\e[0m" 
rm -rf m5out/MatSF64MulTest
fi
fi

echo "3.Running MatSF64MulKETest..."
if [ ! -d ./m5out/MatSF64MulKETest ];\
then mkdir m5out/MatSF64MulKETest
fi
../../build/MAPU/gem5.debug --stats-file=MatSF64MulKETest/stat.txt --dump-config=MatSF64MulKETest/config.ini --json-config=MatSF64MulKETest/config.json -r --stdout-file=MatSF64MulKETest/stdout -e --stderr-file=MatSF64MulKETest/stderr ../../configs/example/ms.py -c /dev/ape&
cd MatSF64MulKETest
make
./matmul > ../m5out/MatSF64MulKETest/driver.out
if [ $? -ne 0 ]
then
echo -e "\e[31m\e[1mError in MatSF64MulKET test!\e[0m"
cd ..
else
cd ..
if [ -s ./m5out/MatSF64MulKETest/MatSF64MulKE.out ]
then echo -e "\e[31m\e[1mError in MatSF64MulKE test!\e[0m"
else echo -e "\e[32m\e[1m MatSF64MulKE test passed~\e[0m" 
rm -rf m5out/MatSF64MulKETest
fi
fi

echo "4.Running FFTFloat64Ep0Test..."
if [ ! -d ./m5out/FFTFloat64Ep0Test ];\
then mkdir m5out/FFTFloat64Ep0Test
fi
../../build/MAPU/gem5.debug --stats-file=FFTFloat64Ep0Test/stat.txt --dump-config=FFTFloat64Ep0Test/config.ini --json-config=FFTFloat64Ep0Test/config.json -r --stdout-file=FFTFloat64Ep0Test/stdout -e --stderr-file=FFTFloat64Ep0Test/stderr ../../configs/example/ms.py -c /dev/ape&
cd FFTFloat64Ep0Test
make
./matmul > ../m5out/FFTFloat64Ep0Test/driver.out
if [ $? -ne 0 ]
then
echo -e "\e[31m\e[1mError in FFTFloat64Ep0 test!\e[0m"
cd ..
else
cd ..
if [ -s ./m5out/FFTFloat64Ep0Test/FFTFloat64Ep0.out ]
then echo -e "\e[31m\e[1mError in FFTFloat64Ep0 test!\e[0m"
else echo -e "\e[32m\e[1m FFTFloat64EP0 test passed~\e[0m" 
rm -rf m5out/FFTFloat64Ep0Test
fi
fi

echo "5.Running FFTFloat64Test..."
if [ ! -d ./m5out/FFTFloat64Test ];\
then mkdir m5out/FFTFloat64Test
fi
../../build/MAPU/gem5.debug --stats-file=FFTFloat64Test/stat.txt --dump-config=FFTFloat64Test/config.ini --json-config=FFTFloat64Test/config.json -r --stdout-file=FFTFloat64Test/stdout -e --stderr-file=FFTFloat64Test/stderr ../../configs/example/ms.py -c /dev/ape&
cd FFTFloat64Test
make
./matmul > ../m5out/FFTFloat64Test/driver.out
if [ $? -ne 0 ]
then
echo -e "\e[31m\e[1mError in FFTFloat64 test!\e[0m"
cd ..
else
cd ..
if [ -s ./m5out/FFTFloat64Test/FFTFloat64.out ]
then echo -e "\e[31m\e[1mError in FFTFloat64 test!\e[0m"
else echo -e "\e[32m\e[1m FFTFloat64 test passed~\e[0m" 
rm -rf m5out/FFTFloat64Test
fi
fi

echo "6.Running FFTFloat128MTest..."
if [ ! -d ./m5out/FFTFloat128MTest ];\
then mkdir m5out/FFTFloat128MTest
fi
../../build/MAPU/gem5.debug --stats-file=FFTFloat128MTest/stat.txt --dump-config=FFTFloat128MTest/config.ini --json-config=FFTFloat128MTest/config.json -r --stdout-file=FFTFloat128MTest/stdout -e --stderr-file=FFTFloat128MTest/stderr ../../configs/example/ms.py -c /dev/ape&
cd FFTFloat128MTest
make
./matmul > ../m5out/FFTFloat128MTest/driver.out
if [ $? -ne 0 ]
then
echo -e "\e[31m\e[1mError in FFTFloat128M test!\e[0m"
cd ..
else
cd ..
if [ -s ./m5out/FFTFloat128MTest/FFTFloat128M.out ]
then echo -e "\e[31m\e[1mError in FFTFloat128M test!\e[0m"
else echo -e "\e[32m\e[1m FFTFloat128M test passed~\e[0m" 
rm -rf m5out/FFTFloat128MTest
fi
fi

echo "7.Running FFTFloat4096MTest..."
if [ ! -d ./m5out/FFTFloat4096MTest ];\
then mkdir m5out/FFTFloat4096MTest
fi
../../build/MAPU/gem5.debug --stats-file=FFTFloat4096MTest/stat.txt --dump-config=FFTFloat4096MTest/config.ini --json-config=FFTFloat4096MTest/config.json -r --stdout-file=FFTFloat4096MTest/stdout -e --stderr-file=FFTFloat4096MTest/stderr ../../configs/example/ms.py -c /dev/ape&
cd FFTFloat4096MTest
make
./matmul > ../m5out/FFTFloat4096MTest/driver.out
if [ $? -ne 0 ]
then
echo -e "\e[31m\e[1mError in FFTFloat4096M test!\e[0m"
cd ..
else
cd ..
if [ -s ./m5out/FFTFloat4096MTest/FFTFloat4096M.out ]
then echo -e "\e[31m\e[1mError in FFTFloat4096M test!\e[0m"
else echo -e "\e[32m\e[1m FFTFloat4096M test passed~\e[0m" 
rm -rf m5out/FFTFloat4096MTest
fi
fi

echo "Finish Mapu full system testing ..."
echo "========================================"

#if [ -z $(ls m5out) ];\
#if m5out is empty, remove it
if [ "'ls -A $m5out'" = "" ];\
then rm -rf m5out
fi

echo "All Tests are done!"
echo "==========================================================================="


#############################################################
