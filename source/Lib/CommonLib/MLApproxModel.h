#pragma once

#include "CommonDef.h"

namespace vvenc {

class CodingStructure;
class CodingUnit;

extern "C" {
    void setMLBaseApproxLevel_C(int level);
    void setMLFallbackApprox_C(int level);
}

class MLApproxModel {
    private:
        static int origApproxLevel;
        static int neighApproxLevel;
        
        static int baseApproxLevel;      // Level for discarted blocks
        static int fallbackApproxLevel;  // Level for important blocks

        static long long countTotalEval;
        static long long countIsSplit;
        static long long countNotIntraKept;
        static long long countLossless;

    public:
        static void setBaseApproxLevel(int level) { baseApproxLevel = level; }
        static void setFallbackApproxLevel(int level) { fallbackApproxLevel = level; }

        static void defineApproxLevel( const CodingStructure& cs, const CodingUnit& cu, double interCost );

        static int getOrigApproxLevel();
        static int getNeighApproxLevel();

        static void printSummary();
};

} // namespace vvenc