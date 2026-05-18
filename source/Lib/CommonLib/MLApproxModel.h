#pragma once

#include "CommonDef.h"

namespace vvenc {

class CodingStructure;
class CodingUnit;
class Partitioner;
struct CompArea;

class MLApproxModel {
    private:
        static int origApproxLevel, neighApproxLevel;

    public:
        static void defineApproxLevel( const CodingStructure& cs, const CodingUnit& cu, const Pel* orgBuf, int orgStride, const CompArea& block );

        static int getOrigApproxLevel();
        static int getNeighApproxLevel();
};

} // namespace vvenc