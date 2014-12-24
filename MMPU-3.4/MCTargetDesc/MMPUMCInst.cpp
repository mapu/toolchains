#include "MMPUMCInst.h"
#include "MCTargetDesc/MMPUMCTargetDesc.h"

using namespace llvm;

namespace llvm {
  namespace MMPU {
    void PrintInstOptions(int64_t options, std::string& reports)
    {
      reports.clear();
      if(options & c_KPP)   reports.append("(K++)");
      if(options & c_APP)   reports.append("(A++)");
      if(options & c_BR)    reports.append("(BR)");
      if(options & c_M)     reports.append("(M)");
      if(options & c_T)     reports.append("(T)");
      if(options & c_U)     reports.append("(U)");
      if(options & c_B)     reports.append("(B)");
      if(options & c_H)     reports.append("(H)");
      if(options & c_S)     reports.append("(S)");
      if(options & c_D)     reports.append("(D)");
      if(options & c_IND)   reports.append("(IND)");
      if(options & c_C)     reports.append("(C)");
      if(options & c_NC)    reports.append("(!C)");
      if(options & c_L)     reports.append("(L)");
      if(options & c_CR)    reports.append("(CR)");
      if(options & c_I)     reports.append("(I)");
      if(options & c_TC)    reports.append("(TC)");
      if(options & c_WF1)   reports.append("(WF1)");
      if(options & c_WF2)   reports.append("(WF2)");
      if(options & c_WF3)   reports.append("(WF3)");
      if(options & c_WX1)   reports.append("(WX1)");
      if(options & c_WX2)   reports.append("(WX2)");
      if(options & c_WX3)   reports.append("(WX3)");
      if(options & c_WX4)   reports.append("(WX4)");
    }

    const static InstDesc ISADesc[] =
    {
     // opcode,               operand,     mask,                            mandatory,  exclusive,                inclusive
     { MMPUInst::BIU0LdToCalc,  3, ~(c_M|c_BR|c_APP|c_KPP|c_C|c_NC),            0,    {c_C|c_NC, 0},                  {0}, 8, {BIU0Port, 8, 1, -1}          },
     { MMPUInst::BIU0LdToMw0,   1, ~(c_M|c_BR|c_APP|c_KPP|c_C|c_NC),            0,    {c_C|c_NC, 0},                  {0}, 8, {BIU0Port, 8, 1, -1}          },
     { MMPUInst::BIU0LdToShu,   2, ~(c_M|c_BR|c_APP|c_KPP|c_C|c_NC),            0,    {c_C|c_NC, 0},                  {0}, 8, {BIU0Port, 8, 1, -1}          },
     { MMPUInst::BIU0MovToMw0,  1, ~(c_C|c_NC),                                 0,    {c_C|c_NC, 0},                  {0}, 3, {BIU0Port, 3, 1, -1}          },
     { MMPUInst::BIU0StToDM,    0, ~(c_M|c_BR|c_APP|c_KPP|c_IND|c_C|c_NC),      0,    {c_C|c_NC, 0},                  {0},-1, {-1}          },

     { MMPUInst::BIU1LdToCalc,  3, ~(c_M|c_BR|c_APP|c_KPP|c_C|c_NC),            0,    {c_C|c_NC, 0},                  {0}, 8, {BIU1Port, 8, 1, -1}          },
     { MMPUInst::BIU1LdToMw1,   1, ~(c_M|c_BR|c_APP|c_KPP|c_C|c_NC),            0,    {c_C|c_NC, 0},                  {0}, 8, {BIU1Port, 8, 1, -1}          },
     { MMPUInst::BIU1LdToShu,   2, ~(c_M|c_BR|c_APP|c_KPP|c_C|c_NC),            0,    {c_C|c_NC, 0},                  {0}, 8, {BIU1Port, 8, 1, -1}          },
     { MMPUInst::BIU1MovToMw1,  1, ~(c_C|c_NC),                                 0,    {c_C|c_NC, 0},                  {0}, 3, {BIU0Port, 3, 1, -1}          },
     { MMPUInst::BIU1StToDM,    0, ~(c_M|c_BR|c_APP|c_KPP|c_IND|c_C|c_NC),      0,    {c_C|c_NC, 0},                  {0},-1, {-1}          },

     { MMPUInst::BIU2LdToCalc,  3, ~(c_M|c_BR|c_APP|c_KPP|c_C|c_NC),            0,    {c_C|c_NC, 0},                  {0}, 8, {BIU2Port, 8, 1, -1}          },
     { MMPUInst::BIU2LdToMw2,   1, ~(c_M|c_BR|c_APP|c_KPP|c_C|c_NC),            0,    {c_C|c_NC, 0},                  {0}, 8, {BIU2Port, 8, 1, -1}           },
     { MMPUInst::BIU2LdToShu,   2, ~(c_M|c_BR|c_APP|c_KPP|c_C|c_NC),            0,    {c_C|c_NC, 0},                  {0}, 8, {BIU2Port, 8, 1, -1}           },
     { MMPUInst::BIU2MovToMw2,  1, ~(c_C|c_NC),                                 0,    {c_C|c_NC, 0},                  {0}, 3, {BIU2Port, 3, 1, -1}           },
     { MMPUInst::BIU2StToDM,    0, ~(c_M|c_BR|c_APP|c_KPP|c_IND|c_C|c_NC),      0,    {c_C|c_NC, 0},                  {0},-1, {-1}           },

     { MMPUInst::FALUAbsToBIU,  2,     ~(c_C|c_NC|c_S|c_D),                  0,          {c_C|c_NC , c_S|c_D , 0},     {c_S|c_D , 0},2, {FALUPort,2,1,-1}  },
     { MMPUInst::FALUAbsToMw3,  2,     ~(c_C|c_NC|c_S|c_D),                  0,          {c_C|c_NC , c_S|c_D , 0},     {c_S|c_D , 0},2, {FALUPort,2,1,-1}  },
     { MMPUInst::FALUAbsToUnit, 3,     ~(c_C|c_NC|c_S|c_D),                  0,          {c_C|c_NC , c_S|c_D , 0},     {c_S|c_D , 0},2, {FALUPort,2,1,-1}  },

     { MMPUInst::FALUAddSubToBIU,     3,     ~(c_C|c_NC|c_S),                      c_S,        {c_C|c_NC , 0},               {0},5, {FALUPort,5,1,-1}  },
     { MMPUInst::FALUAddSubToMw3,     3,     ~(c_C|c_NC|c_S),                      c_S,        {c_C|c_NC , 0},               {0},5, {FALUPort,5,1,-1}  },
     { MMPUInst::FALUAddSubToUnit,    4,     ~(c_C|c_NC|c_S),                      c_S,        {c_C|c_NC , 0},               {0},5, {FALUPort,5,1,-1}  },

     { MMPUInst::FALUAddToBIU,       3,     ~(c_C|c_NC|c_T|c_S|c_D),              0,          {c_C|c_NC , c_S|c_D , 0},     {c_S|c_D , 0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUAddToMw3,       3,     ~(c_C|c_NC|c_T|c_S|c_D),              0,          {c_C|c_NC , c_S|c_D , 0},     {c_S|c_D , 0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUAddToUnit,      4,     ~(c_C|c_NC|c_T|c_S|c_D),              0,          {c_C|c_NC , c_S|c_D , 0},     {c_S|c_D , 0},5, {FALUPort,5,1,-1} },

     { MMPUInst::FALUDblToBIU,       2,     ~(c_C|c_NC|c_U|c_S),                 0,        {c_C|c_NC , c_U|c_S , 0},     {0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUDblToMw3,       2,     ~(c_C|c_NC|c_U|c_S),                 0,        {c_C|c_NC , c_U|c_S , 0},     {0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUDblToUnit,      3,     ~(c_C|c_NC|c_U|c_S),                 0,        {c_C|c_NC , c_U|c_S , 0},     {0},5, {FALUPort,5,1,-1} },

     { MMPUInst::FALUEQToBIU,       3,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUEQToMw3,       3,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUEQToUnit,      4,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },

     { MMPUInst::FALUGEToBIU,       3,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUGEToMw3,       3,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUGEToUnit,      4,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },

     { MMPUInst::FALUGTToBIU,       3,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUGTToMw3,       3,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUGTToUnit,      4,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },

     { MMPUInst::FALULEToBIU,       3,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALULEToMw3,       3,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALULEToUnit,      4,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },

     { MMPUInst::FALULTToBIU,       3,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALULTToMw3,       3,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0} },
     { MMPUInst::FALULTToUnit,      4,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },

     { MMPUInst::FALUNEToBIU,       3,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUNEToMw3,       3,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUNEToUnit,      4,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },

     { MMPUInst::FALUMaxToBIU,      3,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUMaxToMw3,      3,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUMaxToUnit,     4,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },

     { MMPUInst::FALUMinToBIU,      3,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUMinToMw3,      3,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUMinToUnit,     4,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },

     { MMPUInst::FALURSqRtToBIU,    2,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALURSqRtToMw3,    2,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALURSqRtToUnit,   3,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },

     { MMPUInst::FALURecipToBIU,    2,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALURecipToMw3,    2,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALURecipToUnit,   3,     ~(c_C|c_NC|c_D|c_S),                 0,        {c_C|c_NC , c_D|c_S , 0},     {c_D|c_S ,0},5, {FALUPort,5,1,-1} },

     { MMPUInst::FALUSubToBIU,      3,     ~(c_C|c_NC|c_T|c_S|c_D),              0,          {c_C|c_NC , c_S|c_D , 0},     {c_S|c_D , 0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUSubToMw3,      3,     ~(c_C|c_NC|c_T|c_S|c_D),              0,          {c_C|c_NC , c_S|c_D , 0},     {c_S|c_D , 0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUSubToUnit,     4,     ~(c_C|c_NC|c_T|c_S|c_D),              0,          {c_C|c_NC , c_S|c_D , 0},     {c_S|c_D , 0},5, {FALUPort,5,1,-1} },

     { MMPUInst::FALUSglIntToBIU,   2,     ~(c_C|c_NC|c_U|c_T),             0,            {c_C|c_NC , 0},     {0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUSglDblToBIU,   2,     ~(c_C|c_NC|c_D|c_T),             c_D,            {c_C|c_NC , 0},     {0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUSglIntToMw3,   2,     ~(c_C|c_NC|c_U|c_T),             0,            {c_C|c_NC , 0},     {0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUSglDblToMw3,   2,     ~(c_C|c_NC|c_D|c_T),             c_D,            {c_C|c_NC , 0},     {0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUSglIntToUnit,  3,     ~(c_C|c_NC|c_U|c_T),             0,            {c_C|c_NC , 0},     {0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUSglDblToUnit,  3,     ~(c_C|c_NC|c_D|c_T),             c_D,            {c_C|c_NC , 0},     {0},5, {FALUPort,5,1,-1} },

     { MMPUInst::FALUIntSglToBIU,   2,     ~(c_C|c_NC|c_S|c_T),             c_S,            {c_C|c_NC , 0},     {0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUIntDblToBIU,   2,     ~(c_C|c_NC|c_D|c_T),             c_D,            {c_C|c_NC , 0},     {0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUIntSglToMw3,   2,     ~(c_C|c_NC|c_S|c_T),             c_S,            {c_C|c_NC , 0},     {0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUIntDblToMw3,   2,     ~(c_C|c_NC|c_D|c_T),             c_D,            {c_C|c_NC , 0},     {0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUIntSglToUnit,  3,     ~(c_C|c_NC|c_S|c_T),             c_S,            {c_C|c_NC , 0},     {0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUIntDblToUnit,  3,     ~(c_C|c_NC|c_D|c_T),             c_D,            {c_C|c_NC , 0},     {0},5, {FALUPort,5,1,-1} },

     { MMPUInst::FALUUIntSglToBIU,  2,     ~(c_C|c_NC|c_S|c_T),             c_S,            {c_C|c_NC , 0},     {0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUUIntDblToBIU,  2,     ~(c_C|c_NC|c_D|c_T),             c_D,            {c_C|c_NC , 0},     {0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUUIntSglToMw3,  2,     ~(c_C|c_NC|c_S|c_T),             c_S,            {c_C|c_NC , 0},     {0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUUIntDblToMw3,  2,     ~(c_C|c_NC|c_D|c_T),             c_D,            {c_C|c_NC , 0},     {0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUUIntSglToUnit, 3,     ~(c_C|c_NC|c_S|c_T),             c_S,            {c_C|c_NC , 0},     {0},5, {FALUPort,5,1,-1} },
     { MMPUInst::FALUUIntDblToUnit, 3,     ~(c_C|c_NC|c_D|c_T),             c_D,            {c_C|c_NC , 0},     {0},5, {FALUPort,5,1,-1} },

     { MMPUInst::FMACMulAddToMR,    3,     ~(c_C|c_NC|c_S|c_D),                  0,          {c_C|c_NC , c_S|c_D , 0},     {c_S|c_D , 0},-1,{-1}  },

     { MMPUInst::FMACMulToBIU,    3,     ~(c_C|c_NC|c_T|c_S|c_D),              0,          {c_C|c_NC , c_S|c_D , 0},     {c_S|c_D , 0}, 6, {FMACPort,6,1,-1}  },
     { MMPUInst::FMACMulToMw3,    3,     ~(c_C|c_NC|c_T|c_S|c_D),              0,          {c_C|c_NC , c_S|c_D , 0},     {c_S|c_D , 0}, 6, {FMACPort,6,1,-1}  },
     { MMPUInst::FMACMulToUnit,   4,     ~(c_C|c_NC|c_T|c_S|c_D),              0,          {c_C|c_NC , c_S|c_D , 0},     {c_S|c_D , 0}, 6, {FMACPort,6,1,-1}  },

     { MMPUInst::FMACMulAccOnMR,  2,     ~(c_C|c_NC|c_CR|c_S|c_D),             0,          {c_C|c_NC , c_S|c_D , 0},     {c_S|c_D , 0}, -1, {-1}  },

     { MMPUInst::FMACMulAccOnMRXferToBIU,    3,     ~(c_C|c_NC|c_CR|c_S|c_D),             0,          {c_C|c_NC , c_S|c_D , 0},     {c_S|c_D , 0}, 9, {FMACPort,9,1,-1}  },
     { MMPUInst::FMACMulAccOnMRXferToMw3,    3,     ~(c_C|c_NC|c_CR|c_S|c_D),             0,          {c_C|c_NC , c_S|c_D , 0},     {c_S|c_D , 0}, 9, {FMACPort,9,1,-1}  },
     { MMPUInst::FMACMulAccOnMRXferToUnit,   4,     ~(c_C|c_NC|c_CR|c_S|c_D),             0,          {c_C|c_NC , c_S|c_D , 0},     {c_S|c_D , 0}, 9, {FMACPort,9,1,-1}  },

     { MMPUInst::FMACComplexMulToBIU,    3,     ~(c_C|c_NC|c_S),                      c_S,        {c_C|c_NC , 0},               {0}, 10, {FMACPort,10,1,-1}  },
     { MMPUInst::FMACComplexMulToMw3,    3,     ~(c_C|c_NC|c_S),                      c_S,        {c_C|c_NC , 0},               {0}, 10, {FMACPort,10,1,-1}  },
     { MMPUInst::FMACComplexMulToUnit,   4,     ~(c_C|c_NC|c_S),                      c_S,        {c_C|c_NC , 0},               {0}, 10, {FMACPort,10,1,-1}  },

     { MMPUInst::IALUAddToBIU,    3,     ~(c_C|c_NC|c_U|c_T|c_B|c_H),                  0,        {c_C|c_NC , c_B|c_H ,0},               {0}, 3, {IALUPort,3,1,-1}  },
     { MMPUInst::IALUAddToMw3,    3,     ~(c_C|c_NC|c_U|c_T|c_B|c_H),                  0,        {c_C|c_NC , c_B|c_H ,0},               {0}, 3, {IALUPort,3,1,-1}  },
     { MMPUInst::IALUAddToUnit,   4,     ~(c_C|c_NC|c_U|c_T|c_B|c_H),                  0,        {c_C|c_NC , c_B|c_H ,0},               {0}, 3, {IALUPort,3,1,-1}  },

     { MMPUInst::IALUAndToBIU,  3, ~(c_C|c_NC),                 0,   {c_C|c_NC, 0},           {0}, 3, {IALUPort,3,1,-1}          },
     { MMPUInst::IALUAndToMw3,  3, ~(c_C|c_NC),                 0,   {c_C|c_NC, 0},           {0}, 3, {IALUPort,3,1,-1}          },
     { MMPUInst::IALUAndToUnit, 4, ~(c_C|c_NC),                 0,   {c_C|c_NC, 0},           {0}, 3, {IALUPort,3,1,-1}          },

     { MMPUInst::IALUBitNotToBIU,  2, ~(c_C|c_NC),                 0,   {c_C|c_NC, 0},           {0}, 3, {IALUPort,3,1,-1}          },
     { MMPUInst::IALUBitNotToMw3,  2, ~(c_C|c_NC),                 0,   {c_C|c_NC, 0},           {0}, 3, {IALUPort,3,1,-1}          },
     { MMPUInst::IALUBitNotToUnit, 3, ~(c_C|c_NC),                 0,   {c_C|c_NC, 0},           {0}, 3, {IALUPort,3,1,-1}          },

     { MMPUInst::IALUCondEQ, 2, ~(c_C|c_NC|c_B|c_H),         0,   {c_C|c_NC, c_B|c_H, 0}, {0}, 3, {IALUPort,3,1,-1}          },
     { MMPUInst::IALUCondGE, 2, ~(c_C|c_NC|c_B|c_H|c_U),     0,   {c_C|c_NC, c_B|c_H, 0}, {0}, 3, {IALUPort,3,1,-1}          },
     { MMPUInst::IALUCondGT, 2, ~(c_C|c_NC|c_B|c_H|c_U),     0,   {c_C|c_NC, c_B|c_H, 0}, {0}, 3, {IALUPort,3,1,-1}          },
     { MMPUInst::IALUCondLE, 2, ~(c_C|c_NC|c_B|c_H|c_U),     0,   {c_C|c_NC, c_B|c_H, 0}, {0}, 3, {IALUPort,3,1,-1}          },
     { MMPUInst::IALUCondLT, 2, ~(c_C|c_NC|c_B|c_H|c_U),     0,   {c_C|c_NC, c_B|c_H, 0}, {0}, 3, {IALUPort,3,1,-1}          },
     { MMPUInst::IALUCondNE, 2, ~(c_C|c_NC|c_B|c_H),         0,   {c_C|c_NC, c_B|c_H, 0}, {0}, 3, {IALUPort,3,1,-1}          },

     { MMPUInst::IALUCprsToBIU, 3, ~(c_C|c_NC|c_T|c_H|c_U),     0,   {c_C|c_NC, 0}, {0}, 3, {IALUPort,3,1,-1}          },
     { MMPUInst::IALUCprsToMw3, 3, ~(c_C|c_NC|c_T|c_H|c_U),     0,   {c_C|c_NC, 0}, {0}, 3, {IALUPort,3,1,-1}          },
     { MMPUInst::IALUCprsToUnit, 4, ~(c_C|c_NC|c_T|c_H|c_U),     0,   {c_C|c_NC, 0}, {0}, 3, {IALUPort,3,1,-1}          },

     { MMPUInst::IALUExpdToBIU,  2, ~(c_C|c_NC|c_B|c_H|c_U),     0,   {c_C|c_NC, c_B|c_H, 0}, {c_B|c_H, 0}, 3, {IALUPort,3,2,-1}          },
     { MMPUInst::IALUExpdToMw3,  2, ~(c_C|c_NC|c_B|c_H|c_U),     0,   {c_C|c_NC, c_B|c_H, 0}, {c_B|c_H, 0}, 3, {IALUPort,3,2,-1}           },
     { MMPUInst::IALUExpdToUnit, 3, ~(c_C|c_NC|c_B|c_H|c_U),     0,   {c_C|c_NC, c_B|c_H, 0}, {c_B|c_H, 0}, 3, {IALUPort,3,2,-1}           },

     { MMPUInst::IALUDivQ,  0, ~(c_C|c_NC),                 0,   {c_C|c_NC, 0},           {0}, -1, {-1}         },
     { MMPUInst::IALUDivS,  2, ~(c_U|c_C|c_NC),             0,   {c_C|c_NC, 0},           {0}, -1, {-1}         },
     { MMPUInst::IALUMDivQ, 0, ~(c_C|c_NC),                 0,   {c_C|c_NC, 0},           {0}, -1, {-1}         },
     { MMPUInst::IALUMDivR, 0, ~(c_C|c_NC),                 0,   {c_C|c_NC, 0},           {0}, -1, {-1}         },

     { MMPUInst::IALUDivRToBIU,  1, ~(c_C|c_NC),                 0,   {c_C|c_NC, 0},           {0}, 2, {IALUPort,2,2,-1}         },
     { MMPUInst::IALUDivRToMw3,  1, ~(c_C|c_NC),                 0,   {c_C|c_NC, 0},           {0}, 2, {IALUPort,2,2,-1}           },
     { MMPUInst::IALUDivRToUnit, 2, ~(c_C|c_NC),                 0,   {c_C|c_NC, 0},           {0}, 2, {IALUPort,2,2,-1}           },

     { MMPUInst::IALUEQToBIU,  3, ~(c_C|c_NC|c_B|c_H),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALUEQToMw3,  3, ~(c_C|c_NC|c_B|c_H),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALUEQToUnit, 4, ~(c_C|c_NC|c_B|c_H),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },

     { MMPUInst::IALUGEToBIU,  3, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALUGEToMw3,  3, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALUGEToUnit, 4, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },

     { MMPUInst::IALUGTToBIU,  3, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALUGTToMw3,  3, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALUGTToUnit, 4, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },

     { MMPUInst::IALULEToBIU,  3, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALULEToMw3,  3, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALULEToUnit, 4, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },

     { MMPUInst::IALULTToBIU,  3, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALULTToMw3,  3, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALULTToUnit, 4, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },

     { MMPUInst::IALUNEToBIU,  3, ~(c_C|c_NC|c_B|c_H),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALUNEToMw3,  3, ~(c_C|c_NC|c_B|c_H),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}        },
     { MMPUInst::IALUNEToUnit, 4, ~(c_C|c_NC|c_B|c_H),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },

     { MMPUInst::IALUMaxToBIU,  3, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALUMaxToMw3,  3, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALUMaxToUnit, 4, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },

     { MMPUInst::IALUMinToBIU,  3, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALUMinToMw3,  3, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALUMinToUnit, 4, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },

     { MMPUInst::IALUOrToBIU,  3, ~(c_C|c_NC),                 0,   {c_C|c_NC, 0},           {0}, 3, {IALUPort,3,1}          },
     { MMPUInst::IALUOrToMw3,  3, ~(c_C|c_NC),                 0,   {c_C|c_NC, 0},           {0}, 3, {IALUPort,3,1}          },
     { MMPUInst::IALUOrToUnit, 4, ~(c_C|c_NC),                 0,   {c_C|c_NC, 0},           {0}, 3, {IALUPort,3,1}          },

     { MMPUInst::IALURMaxToBIU,  2, ~(c_C|c_NC|c_B|c_U),                 c_B,   {c_C|c_NC, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALURMaxToMw3,  2, ~(c_C|c_NC|c_B|c_U),                 c_B,   {c_C|c_NC, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALURMaxToUnit, 3, ~(c_C|c_NC|c_B|c_U),                 c_B,   {c_C|c_NC, 0},           {0}, 3, {IALUPort,3,1,-1}         },

     { MMPUInst::IALURMinToBIU,  2, ~(c_C|c_NC|c_B|c_U),                 c_B,   {c_C|c_NC, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALURMinToMw3,  2, ~(c_C|c_NC|c_B|c_U),                 c_B,   {c_C|c_NC, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALURMinToUnit, 3, ~(c_C|c_NC|c_B|c_U),                 c_B,   {c_C|c_NC, 0},           {0}, 3, {IALUPort,3,1,-1}         },

     { MMPUInst::IALUSLImmToBIU,  3, ~(c_C|c_NC|c_B|c_H),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALUSLImmToMw3,  3, ~(c_C|c_NC|c_B|c_H),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALUSLImmToUnit, 4, ~(c_C|c_NC|c_B|c_H),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },

     { MMPUInst::IALUSLToBIU,  3, ~(c_C|c_NC|c_B|c_H),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALUSLToMw3,  3, ~(c_C|c_NC|c_B|c_H),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALUSLToUnit, 4, ~(c_C|c_NC|c_B|c_H),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },

     { MMPUInst::IALUSRImmToBIU, 3, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALUSRImmToMw3, 3, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALUSRImmToUnit, 4, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},          {0}, 3, {IALUPort,3,1,-1}         },

     { MMPUInst::IALUSRToBIU, 3, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALUSRToMw3, 3, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALUSRToUnit, 4, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},          {0}, 3, {IALUPort,3,1,-1}         },

     { MMPUInst::IALUSelectToBIU, 4, ~(c_C|c_NC|c_B|c_H),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALUSelectToMw3, 4, ~(c_C|c_NC|c_B|c_H),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALUSelectToUnit, 5, ~(c_C|c_NC|c_B|c_H),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },

     { MMPUInst::IALUSubAbsToBIU, 3, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALUSubAbsToMw3, 3, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },
     { MMPUInst::IALUSubAbsToUnit, 4, ~(c_C|c_NC|c_B|c_H|c_U),                 0,   {c_C|c_NC, c_B|c_H, 0},           {0}, 3, {IALUPort,3,1,-1}         },

     { MMPUInst::IALUSubToBIU,    3,     ~(c_C|c_NC|c_U|c_T|c_B|c_H),                  0,        {c_C|c_NC , c_B|c_H ,0},               {0}, 3, {IALUPort,3,1,-1}  },
     { MMPUInst::IALUSubToMw3,    3,     ~(c_C|c_NC|c_U|c_T|c_B|c_H),                  0,        {c_C|c_NC , c_B|c_H ,0},               {0}, 3, {IALUPort,3,1,-1}  },
     { MMPUInst::IALUSubToUnit,   4,     ~(c_C|c_NC|c_U|c_T|c_B|c_H),                  0,        {c_C|c_NC , c_B|c_H ,0},               {0}, 3, {IALUPort,3,1,-1}  },

     { MMPUInst::IALUXOrToBIU, 3, ~(c_C|c_NC),                 0,   {c_C|c_NC, 0},           {0}, 3, {IALUPort,3,1,-1}          },
     { MMPUInst::IALUXOrToMw3, 3, ~(c_C|c_NC),                 0,   {c_C|c_NC, 0},           {0}, 3, {IALUPort,3,1,-1}          },
     { MMPUInst::IALUXOrToUnit, 4, ~(c_C|c_NC),                 0,   {c_C|c_NC, 0},           {0}, 3, {IALUPort,3,1,-1}          },

     { MMPUInst::IMACAccOnMR,    1,     ~(c_C|c_NC|c_CR|c_U|c_B|c_H),         0,          {c_C|c_NC , c_B|c_H , 0},     {0}, 2, {IMACMR,2,1,-1} },
     { MMPUInst::IMACMulAccOnMR, 2,     ~(c_C|c_NC|c_CR|c_U|c_B|c_H),         0,          {c_C|c_NC , c_B|c_H , 0},     {0}, 6, {IMACMR,6,1,-1} },
     { MMPUInst::IMACMulAddToMR, 3,     ~(c_C|c_NC|c_U|c_B|c_H),              0,          {c_C|c_NC , c_B|c_H , 0},     {0}, 6, {IMACMR,6,1,-1} },

     { MMPUInst::IMACMulToBIU,    3,     ~(c_C|c_NC|c_T|c_U|c_I|c_B|c_H),      c_T,        {c_C|c_NC , c_B|c_H , 0},     {0}, 5, {IMACPort,5,1,-1} },
     { MMPUInst::IMACMulToMw3,    3,     ~(c_C|c_NC|c_T|c_U|c_I|c_B|c_H),      c_T,        {c_C|c_NC , c_B|c_H , 0},     {0}, 5, {IMACPort,5,1,-1} },
     { MMPUInst::IMACMulToUnit,   4,     ~(c_C|c_NC|c_T|c_U|c_I|c_B|c_H),      c_T,        {c_C|c_NC , c_B|c_H , 0},     {0}, 5, {IMACPort,5,1,-1} },

     { MMPUInst::IMACMulExtToBIU, 3,     ~(c_C|c_NC|c_U|c_I|c_B|c_H),          0,          {c_C|c_NC , c_B|c_H , 0},     {0}, 5, {IMACPort,5,2,-1} },
     { MMPUInst::IMACMulExtToMw3, 3,     ~(c_C|c_NC|c_U|c_I|c_B|c_H),          0,          {c_C|c_NC , c_B|c_H , 0},     {0}, 5, {IMACPort,5,2,-1} },
     { MMPUInst::IMACMulExtToUnit,4,     ~(c_C|c_NC|c_U|c_I|c_B|c_H),          0,          {c_C|c_NC , c_B|c_H , 0},     {0}, 5, {IMACPort,5,2,-1} },

     { MMPUInst::IMACAddSubMulToBIU,  4,     ~(c_C|c_NC|c_H),                      c_H,        {c_C|c_NC , 0},               {0}, 7, {IMACPort,7,1,-1} },
     { MMPUInst::IMACAddSubMulToMw3,  4,     ~(c_C|c_NC|c_H),                      c_H,        {c_C|c_NC , 0},               {0}, 7, {IMACPort,7,1,-1} },
     { MMPUInst::IMACAddSubMulToUnit, 5,     ~(c_C|c_NC|c_H),                      c_H,        {c_C|c_NC , 0},               {0}, 7, {IMACPort,7,1,-1} },

     { MMPUInst::IMACAccOnMRXferToBIU,  2,     ~(c_C|c_NC|c_CR|c_U|c_B|c_H),         0,          {c_C|c_NC , c_B|c_H , 0},     {0},4,{IMACMR,2,1,IMACPort,4,1,-1} },
     { MMPUInst::IMACAccOnMRXferToMw3,  2,     ~(c_C|c_NC|c_CR|c_U|c_B|c_H),         0,          {c_C|c_NC , c_B|c_H , 0},     {0},4,{IMACMR,2,1,IMACPort,4,1,-1} },
     { MMPUInst::IMACAccOnMRXferToUnit, 3,     ~(c_C|c_NC|c_CR|c_U|c_B|c_H),         0,          {c_C|c_NC , c_B|c_H , 0},     {0},4,{IMACMR,2,1,IMACPort,4,1,-1} },

     { MMPUInst::IMACMulAccOnMRXferToBIU,   3,     ~(c_C|c_NC|c_CR|c_U|c_B|c_H),         0,          {c_C|c_NC , c_B|c_H , 0},     {0},8,{IMACMR,6,1,IMACPort,8,1,-1} },
     { MMPUInst::IMACMulAccOnMRXferToMw3,   3,     ~(c_C|c_NC|c_CR|c_U|c_B|c_H),         0,          {c_C|c_NC , c_B|c_H , 0},     {0},8,{IMACMR,6,1,IMACPort,8,1,-1} },
     { MMPUInst::IMACMulAccOnMRXferToUnit,  4,     ~(c_C|c_NC|c_CR|c_U|c_B|c_H),         0,          {c_C|c_NC , c_B|c_H , 0},     {0},8,{IMACMR,6,1,IMACPort,8,1,-1} },

     { MMPUInst::IMACMulExtAccOnMRXferToBIU,   3,     ~(c_C|c_NC|c_L|c_CR|c_U|c_B|c_H),     c_L,        {c_C|c_NC , c_B|c_H , 0},     {0},8,{IMACMR,6,1,IMACPort,8,2,-1} },
     { MMPUInst::IMACMulExtAccOnMRXferToMw3,   3,     ~(c_C|c_NC|c_L|c_CR|c_U|c_B|c_H),     c_L,        {c_C|c_NC , c_B|c_H , 0},     {0},8,{IMACMR,6,1,IMACPort,8,2,-1} },
     { MMPUInst::IMACMulExtAccOnMRXferToUnit,  4,     ~(c_C|c_NC|c_L|c_CR|c_U|c_B|c_H),     c_L,        {c_C|c_NC , c_B|c_H , 0},     {0},8,{IMACMR,6,1,IMACPort,8,2,-1} },


     { MMPUInst::Mr0ToCalc,  4,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {MR0Port,2,1,-1} },
     { MMPUInst::Mr0ToMw3,   2,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {MR0Port,2,1,-1} },
     { MMPUInst::Mr0ToShu,   3,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {MR0Port,2,1,-1} },

     { MMPUInst::Mr1ToCalc,  4,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {MR1Port,2,1,-1} },
     { MMPUInst::Mr1ToShu,   3,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {MR1Port,2,1,-1} },

     /// added by wangt
     /*
     //there is two parts: with WFn/WXn and without
     */
     { MMPUInst::Mr2ToCalc,  4,     ~(c_C|c_NC|c_WF1|c_WF2|c_WF3|c_WX1|c_WX2|c_WX3|c_WX4), 0, {c_C|c_NC , 0}, {0}, 2, {-1} },

     { MMPUInst::Mr3ToBIU,   2,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {MR3Port,2,1,-1} },
     { MMPUInst::Mr3ToBIUKG, 2,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {MR3Port,2,1,-1} },

     { MMPUInst::Shu0Combine,        5,     ~(c_C|c_NC|c_TC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU0Port,2,1,-1} },
     { MMPUInst::Shu0CombineToBIU,   5,     ~(c_C|c_NC|c_TC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU0Port,2,1,-1} },
     { MMPUInst::Shu0CombineToCalc,  7,     ~(c_C|c_NC|c_TC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU0Port,2,1,-1} },
     { MMPUInst::Shu0CombineToMw3,   5,     ~(c_C|c_NC|c_TC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU0Port,2,1,-1} },

     { MMPUInst::Shu0Indx,           4,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU0Port,2,1,-1} },
     { MMPUInst::Shu0IndxToBIU,      4,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU0Port,2,1,-1} },
     { MMPUInst::Shu0IndxToCalc,     6,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU0Port,2,1,-1} },
     { MMPUInst::Shu0IndxToMw3,      4,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU0Port,2,1,-1} },

     { MMPUInst::Shu0IndxTB,         3,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU0Port,2,1,-1} },
     { MMPUInst::Shu0IndxTBToBIU,    3,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU0Port,2,1,-1} },
     { MMPUInst::Shu0IndxTBToCalc,   5,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU0Port,2,1,-1} },
     { MMPUInst::Shu0IndxTBToMw3,    3,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU0Port,2,1,-1} },

     { MMPUInst::Shu0IndxTBWithImm,       4,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU0Port,2,1,-1} },
     { MMPUInst::Shu0IndxTBWithImmToBIU,  4,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU0Port,2,1,-1} },
     { MMPUInst::Shu0IndxTBWithImmToCalc, 6,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU0Port,2,1,-1} },
     { MMPUInst::Shu0IndxTBWithImmToMw3,  4,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU0Port,2,1,-1} },

     { MMPUInst::Shu0IndxWithImm,       5,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU0Port,2,1,-1} },
     { MMPUInst::Shu0IndxWithImmToBIU,  5,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU0Port,2,1,-1} },
     { MMPUInst::Shu0IndxWithImmToCalc, 7,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU0Port,2,1,-1} },
     { MMPUInst::Shu0IndxWithImmToMw3,  5,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU0Port,2,1,-1} },

     { MMPUInst::Shu1Combine,       5,     ~(c_C|c_NC|c_TC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU1Port,2,1,-1} },
     { MMPUInst::Shu1CombineToBIU,  5,     ~(c_C|c_NC|c_TC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU1Port,2,1,-1} },
     { MMPUInst::Shu1CombineToCalc, 7,     ~(c_C|c_NC|c_TC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU1Port,2,1,-1} },
     { MMPUInst::Shu1CombineToMw3,  5,     ~(c_C|c_NC|c_TC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU1Port,2,1,-1} },

     { MMPUInst::Shu1Indx,       4,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU1Port,2,1,-1} },
     { MMPUInst::Shu1IndxToBIU,  4,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU1Port,2,1,-1} },
     { MMPUInst::Shu1IndxToCalc, 6,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU1Port,2,1,-1} },
     { MMPUInst::Shu1IndxToMw3,  4,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU1Port,2,1,-1} },

     { MMPUInst::Shu1IndxTB,       3,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU1Port,2,1,-1} },
     { MMPUInst::Shu1IndxTBToBIU,  3,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU1Port,2,1,-1} },
     { MMPUInst::Shu1IndxTBToCalc, 5,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU1Port,2,1,-1} },
     { MMPUInst::Shu1IndxTBToMw3,  3,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU1Port,2,1,-1} },

     { MMPUInst::Shu1IndxTBWithImm,       4,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU1Port,2,1,-1} },
     { MMPUInst::Shu1IndxTBWithImmToBIU,  4,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU1Port,2,1,-1} },
     { MMPUInst::Shu1IndxTBWithImmToCalc, 6,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU1Port,2,1,-1} },
     { MMPUInst::Shu1IndxTBWithImmToMw3,  4,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU1Port,2,1,-1} },

     { MMPUInst::Shu1IndxWithImm,         5,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU1Port,2,1,-1} },
     { MMPUInst::Shu1IndxWithImmToBIU,    5,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU1Port,2,1,-1} },
     { MMPUInst::Shu1IndxWithImmToCalc,   7,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU1Port,2,1,-1} },
     { MMPUInst::Shu1IndxWithImmToMw3,    5,     ~(c_C|c_NC),     0,        {c_C|c_NC , 0},     {0}, 2, {SHU1Port,2,1,-1} },

     // end constraints of instruction options.
     { MMPUInst::INSTRUCTION_LIST_END,    0,        0,            0,        {0},                {0}, -1, {-1} }
    };

    const InstDesc* GetISADesc() { return ISADesc; }
  }
}
