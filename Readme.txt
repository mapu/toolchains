# MakeFile Example
# Name: Readme.txt
# leizu.yin@ia.ac.cn
# 2012-4-13
# $ID$

Simulation Flow:
 //---------------------------------------------------
 //-------------------Generate Code-------------------
 1 Write your TestCode
 2 Genertate Binary Code--By lasp,lasm,llnk
 3 Dispose it With exe2txt--exe2txt
   For Example: /MaPU/VPE/Software/exe2txt -i /MaPU/VPE/Software/IMConfig.txt -e /MaPU/VPE/Platform/Sim/TestCase/FetchTest/app.out
 
 //---------------------------------------------------
 //-------------Simulate with dve or verdi------------
 4 cd MaPU/VPE/Platform/Sim/Run/
 5 make -f ../Scripts/Makefile verdi_dbg/dve_dbg
 6 ./simv +vcs+stop+50 +IMFile="./***Your FileFolder of contents_IM.mmap**/contents_IM.mmap"
 7 Debug with Verdi or DVE
