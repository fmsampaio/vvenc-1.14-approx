#include "CommonDef.h"
#include "Unit.h"
#include "approx.h"

#include <vector>

namespace vvenc {

#define SRAM_LOSSLESS                           1
#define SRAM_ECC_APPROX_LOW_CONFIG              2            // SRAM 0.7V  ECC
#define SRAM_ECC_APPROX_MEDIUM_CONFIG           3            // SRAM 0.6V  ECC
#define SRAM_ECC_APPROX_HIGH_CONFIG             4            // SRAM 0.55V ECC

#define ENABLE_ORIG_SB_APPROX                   1         
#define ENABLE_NEIGH_SB_APPROX                  1
          
#define ORIG_SB_BUFFER_Y                        1
#define ORIG_SB_BUFFER_CB                       2
#define ORIG_SB_BUFFER_CR                       3

#define NEIGH_SB_BUFFER_Y                       4
#define NEIGH_SB_BUFFER_Y_FILT                  5
#define NEIGH_SB_BUFFER_CB                      6
#define NEIGH_SB_BUFFER_CR                      7


class ApproxHandler {
    public:
        static Pel* approxIntraOrigBufferY;
        static Pel* approxIntraOrigBufferCb;
        static Pel* approxIntraOrigBufferCr;
        static const Pel* bkpIntraOrigBufferY;
        static const Pel* bkpIntraOrigBufferCb;
        static const Pel* bkpIntraOrigBufferCr;

        // BASELINE APPROXIMATION FUNCTIONS      
        static void allocIntraOrigSB();
        static Pel* initIntraOrigSB(CPelBuf origBuffer, ComponentID comp);
        static const Pel* restoreIntraOrigSB(ComponentID comp);
        
        static void addApproxIntraOrigSB(ComponentID comp, int approxLevel);
        static void removeApproxIntraOrigSB(ComponentID comp);
        
        static void addApproxIntraNeighSB(Pel* refBuffer, ComponentID comp, int filt, int approxLevel);
        static void removeApproxIntraNeighSB(Pel* refBuffer);
        
        static void startGlobalLevel();
        static void endGlobalLevel();

};

}