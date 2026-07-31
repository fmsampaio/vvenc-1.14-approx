#include "MLApproxModel.h"
#include "CodingStructure.h"
#include "Picture.h"
#include "Unit.h"
#include "MLFeaturesManager.h"

#include "decision_tree_single_mdecision_tree_All_Blocks-issplit.h"
#include "decision_tree_single_mdecision_tree_All_Blocks-intrakept.h"

#include <algorithm>
#include <vector>
#include <iostream>
#include <iomanip>

namespace vvenc {

int MLApproxModel::origApproxLevel = 1;
int MLApproxModel::neighApproxLevel = 1;

long long MLApproxModel::countTotalEval = 0;
long long MLApproxModel::countIsSplit = 0;
long long MLApproxModel::countNotIntraKept = 0;
long long MLApproxModel::countLossless = 0;

namespace
{
constexpr int kSramLossless              = 1;
constexpr int kSramApproxLowConfig       = 2;
constexpr int kSramApproxMediumConfig    = 3;
constexpr int kSramApproxHighConfig      = 4;

std::vector<double> buildIsSplitVector( const MLFeatureData& data )
{
    std::vector<double> f;
    f.reserve(10);
    f.push_back((double)data.interCost);
    f.push_back((double)data.csInterHad);
    f.push_back((double)data.refLineVariance);
    f.push_back((double)data.refLineRange);
    f.push_back((double)data.numIntraCiipNeighbors);
    f.push_back((double)data.varMismatch);
    f.push_back((double)data.distCenterY);
    f.push_back((double)data.splittingDensity);
    f.push_back((double)data.blkMax);
    f.push_back((double)data.blkRange);
    return f;
}

std::vector<double> buildIntraKeptVector( const MLFeatureData& data )
{
    std::vector<double> f;
    f.reserve(13);
    f.push_back((double)data.frameLevel);
    f.push_back((double)data.splitSeries);
    f.push_back((double)data.interHadPerPixel);
    f.push_back((double)data.refLineRange);
    f.push_back((double)data.numIntraCiipNeighbors);
    f.push_back((double)data.leftDepth);
    f.push_back((double)data.relativeBlockArea);
    f.push_back((double)data.deltaQP);
    f.push_back((double)data.contrastRatio);
    f.push_back((double)data.directionalDominance);
    f.push_back((double)data.varMismatch);
    f.push_back((double)data.blkStdV);
    f.push_back((double)data.blkRange);
    return f;
}

} // namespace

void MLApproxModel::defineApproxLevel( const CodingStructure& cs, const CodingUnit& cu, double interCost )
{
    countTotalEval++;
    if( cs.slice->isIntra() )
    {
        origApproxLevel = kSramLossless;
        neighApproxLevel = kSramLossless;
        return;
    }

    if( cu.chType == vvenc::CH_C )
    {
        return;
    }

    MLFeatureData featData = MLFeaturesManager::extractFeatures(cs, cu, interCost);
    MLFeaturesManager::saveFeatures( featData );
    
    int approxLevel = kSramLossless;     

    const auto featuresSplit = buildIsSplitVector( featData );
    int isSplit = decision_tree_single_mdecision_tree_All_Blocks_issplit( featuresSplit );
    
    if (isSplit == 1) 
    {
        countIsSplit++;
        approxLevel = kSramApproxHighConfig; 
    }
    else 
    {
        const auto featuresIntra = buildIntraKeptVector( featData );
        int isIntraKept = decision_tree_single_mdecision_tree_All_Blocks_intrakept( featuresIntra );
        
        if (isIntraKept == 0)
        {
            countNotIntraKept++;
            approxLevel = kSramApproxHighConfig;
        }
        else
        {
            countLossless++;
            approxLevel = kSramLossless;
        }
    }

    origApproxLevel = approxLevel;
    neighApproxLevel = approxLevel;
}

int MLApproxModel::getOrigApproxLevel() { return origApproxLevel; }
int MLApproxModel::getNeighApproxLevel() { return neighApproxLevel; }

void MLApproxModel::printSummary()
{
    std::cout << "\n=======================================================\n";
    std::cout << "[MLApproxModel] Decision Report\n";
    std::cout << "=======================================================\n";

    if (countTotalEval == 0) {
        std::cout << "No blocks were evaluated by the ML models.\n";
        std::cout << "=======================================================\n";
        return;
    }

    double pctSplit = (countIsSplit * 100.0) / countTotalEval;
    double pctNotIntra = (countNotIntraKept * 100.0) / countTotalEval;
    double pctLossless = (countLossless * 100.0) / countTotalEval;
    double pctTotalSkipped = pctSplit + pctNotIntra;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Total Blocks Evaluated    : " << countTotalEval << " (100.00%)\n";
    std::cout << " -> Approximated (IsSplit): " << countIsSplit << " (" << pctSplit << "%)\n";
    std::cout << " -> Approximated (!Intra) : " << countNotIntraKept << " (" << pctNotIntra << "%)\n";
    std::cout << " -> Kept (Lossless)       : " << countLossless << " (" << pctLossless << "%)\n";
    std::cout << "-------------------------------------------------------\n";
    std::cout << "Summary: " << pctTotalSkipped << "% of the blocks were approximated.\n";
    std::cout << "=======================================================\n";
}

} // namespace vvenc