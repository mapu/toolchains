//  Date: 2013-03-28
//  Name: app.m.asm
//  Author: tao.wang@ia.ac.cn
//  The file to test the function of IMAC

.text
    // Write to M itself
    .global IMACTest
    

_start:
    NOP;;
    NOP;;

// X Cluster Test
IMACTest:
   NOP;;
   NOP;;
   NOP;;

   IMAC.T1 (I0)= SHU0.T1 IND T3 ;;  //IMAC.T1 = SHU0.T1
   IMAC.T2 (I0)= SHU0.T2 IND T3 ;;  //IMAC.T2 = SHU0.T2
   IMAC.T3 (I0)= SHU0.T0 IND T3 ;;  //IMAC.T3 = SHU0.T3   
   NOP;;

//MR test, CR
   IALU.T0 = MR += T1 * T2 (CR)(H);;
   IALU.T1 = MR += T1 * T2 (CR)(H) ;;
   MR += T1 * T2 (CR)(H) ;;
   MR += T1 * T2 (CR)(H) ;;
   MR += T1 * T2 (CR)(H) ;;
   MR += T1 * T2 (CR)(H) ;;
   IALU.T2 = MR += T1 * T2 (H) ;;
   IALU.T3 = MR += T1 * T2 (H) ;;
   IALU.T3 = MR += T1 * T2 (H);;
   NOP;;
   
//MAC    
   FALU.T0 = MR += T1 * T2 (CR)(H);;
   FALU.T1 = MR += T1 * T2     (H);;
   FALU.T2 = MR += T1 * T2     (H);;
   FALU.T0:1 = MR += T1 * T2(L)(H);;
   NOP;;
   NOP;;
   NOP;;
   NOP;;
   NOP;;
   NOP;;
   NOP;;
   NOP;;
   NOP;;

//ACC   
   IALU.T0 = MR += T1    (H);;
   NOP;;
   NOP;;
   NOP;;
   NOP;;
   NOP;;
   IALU.T1 = MR += T1    (H);;
   NOP;;
   NOP;;
   NOP;;
   NOP;;
   NOP;;
   IALU.T2 = MR += T1    (H);;
   //IALU.T0:1 = MR += T0:1 (L)(H);;

   NOP;;
   NOP;;
   Repeat at(10);;
   

   NOP;;
   NOP;;
   NOP;;

   
   
   MPU.Stop;;



