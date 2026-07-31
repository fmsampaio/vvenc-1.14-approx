#pragma once

#include "CommonDef.h"

namespace vvenc {

class CodingStructure;
class CodingUnit;

class MLApproxModel {
    private:
        static int origApproxLevel, neighApproxLevel;

        static long long countTotalEval;
        static long long countIsSplit;
        static long long countNotIntraKept;
        static long long countLossless;

    public:
        static void defineApproxLevel( const CodingStructure& cs, const CodingUnit& cu, double interCost );

        static int getOrigApproxLevel();
        static int getNeighApproxLevel();

        static void printSummary();
};

} // namespace vvenc