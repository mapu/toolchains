//  Date: 2013-03-25
//  Name: app.m.asm
//  Author: tao.wang@ia.ac.cn
//  The file to test the function of IALU 

.text
    // Write to M itself
    .global _IALUTest
    

_start:
    NOP;;
    NOP;;

// X Cluster Test
_IALUTest:
   NOP;;
   NOP;;

   IALU.T0 (I0)= SHU0.T3 IND T3 ;;
   IALU.T1 (I0)= SHU0.T1 IND T3 ;;  //IALU.T1 = SHU0.T1
   IALU.T2 (I0)= SHU0.T2 IND T3 ;;  //IALU.T2 = SHU0.T2
   IALU.T3 (I0)= SHU0.T0 IND T3 ;;  //IALU.T3 = SHU0.T0
   NOP;;

//Basic IALU
   IMAC.T0 = T1 + T2 ;;         //512'h11111021_11111020_1111101f_1111101e_1111101d_1111101c_1111101b_1111101a_11111019_11111018_11111017_11111016_11111015_11111014_11111013_11111012
   IMAC.T1 = T1 - T2 ;;         //512'heeeeedff_eeeeedfe_eeeeedfd_eeeeedfc_eeeeedfb_eeeeedfa_eeeeedf9_eeeeedf8_eeeeedf7_eeeeedf6_eeeeedf5_eeeeedf4_eeeeedf3_eeeeedf2_eeeeedf1_eeeeedf0
   IMAC.T2 = MAX(T1,T2)(U);;    //512'hffffff10_ffffff0f_ffffff0e_ffffff0d_ffffff0c_ffffff0b_ffffff0a_ffffff09_ffffff08_ffffff07_ffffff06_ffffff05_ffffff04_ffffff03_ffffff02_ffffff01
   IMAC.T3 = MIN(T1,T2)(U)(H);; //512'h11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111
   IMAC.T0 = ABS(T1-T2);;
   IMAC.T1 = T0 ? T1 : T2 ;;
   IMAC.T2 = T1 ? T1 : T2 ;;
   

//logic
   FALU.T0 = T1 & T2 ;;
   FALU.T1 = T1 | T2 ;;
   FALU.T2 = T1 ^ T2 ;;
   FALU.T3 = ~T2     ;;

//shift
   IMAC.T0 = T2 >> T3;;
   IMAC.T1 = T2 << T3;;
   IMAC.T2 = T2 >> 2 ;;
   IMAC.T3 = T2 << 2 ;;

//compare
   FALU.T0 = T1 > T2 ;;
   FALU.T1 = T1 > T2 (B);;
   FALU.T2 = T1 > T2 (U);;
   FALU.T3 = T1 > T2 (U)(B);;

//divide

   NOP;;
   NOP;;
   NOP;;

   
   MPU.Stop;;



