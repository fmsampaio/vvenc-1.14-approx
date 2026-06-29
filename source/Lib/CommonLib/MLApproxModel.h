#pragma once

#include "CommonDef.h"

namespace vvenc {

class CodingStructure;
class CodingUnit;

class MLApproxModel {
    private:
        static int origApproxLevel, neighApproxLevel;

    public:
        static void defineApproxLevel( const CodingStructure& cs, const CodingUnit& cu, double interCost );

        static int getOrigApproxLevel();
        static int getNeighApproxLevel();
};

} // namespace vvenc