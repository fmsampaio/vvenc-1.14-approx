#include "MLApproxModel.h"
#include "CodingStructure.h"
#include "Picture.h"
#include "Unit.h"
#include "MLFeaturesManager.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

namespace vvenc {

int MLApproxModel::origApproxLevel = 1;
int MLApproxModel::neighApproxLevel = 1;

namespace
{
constexpr int kSramLossless              = 1;
constexpr int kSramApproxLowConfig       = 2;
constexpr int kSramApproxMediumConfig    = 3;
constexpr int kSramApproxHighConfig      = 4;

constexpr size_t NUM_FEATURES = 82;

constexpr std::array<double, NUM_FEATURES> kFeatureMeans = { 0.0 }; 
constexpr std::array<double, NUM_FEATURES> kFeatureStds  = { 1.0 }; 
constexpr std::array<double, NUM_FEATURES> kWeights      = { 0.0 }; 
constexpr double kIntercept = 0.0; 

std::vector<double> buildFeatureVector( const MLFeatureData& data )
{
    std::vector<double> f;
    f.reserve(NUM_FEATURES);

    f.push_back((double)data.xPos);
    f.push_back((double)data.yPos);
    f.push_back((double)data.blockWidth);
    f.push_back((double)data.blockHeight);
    f.push_back((double)data.blockArea);
    f.push_back((double)data.blockAreaGroup);
    f.push_back((double)data.frameLevel);
    f.push_back((double)data.borderContactMask);    
    f.push_back((double)data.cuQp);
    f.push_back((double)data.depth);
    f.push_back((double)data.qtDepth);
    f.push_back((double)data.btDepth);
    f.push_back((double)data.splitSeries);
    f.push_back((double)data.orientationGroup);
    f.push_back((double)data.aspectRatioGroup);    
    f.push_back((double)data.interCost);
    f.push_back((double)data.csInterHad);
    f.push_back((double)data.interHadPerPixel);    
    f.push_back((double)data.availLeft);
    f.push_back((double)data.availAbove);
    f.push_back((double)data.leftIsIntra);
    f.push_back((double)data.aboveIsIntra);
    f.push_back((double)data.leftIntraDir);
    f.push_back((double)data.aboveIntraDir);    
    f.push_back((double)data.refLineVariance);
    f.push_back((double)data.refColVariance);
    f.push_back((double)data.refLineMean);
    f.push_back((double)data.refLineRange);    
    f.push_back((double)data.ctuPosInCtuX);
    f.push_back((double)data.ctuPosInCtuY);
    f.push_back((double)data.isFirstLineOfCTU);
    f.push_back((double)data.sliceType);    
    f.push_back((double)data.cuMtDepth);
    f.push_back((double)data.cuBtDepth);
    f.push_back((double)data.canUseMIP);
    f.push_back((double)data.canUseISP);    
    f.push_back((double)data.mpm0);
    f.push_back((double)data.mpmAngularVar);
    f.push_back((double)data.numIntraCiipNeighbors);
    f.push_back((double)data.leftDepth);
    f.push_back((double)data.aboveDepth);
    f.push_back((double)data.leftQtDepth);
    f.push_back((double)data.aboveQtDepth);
    f.push_back((double)data.leftMtDepth);
    f.push_back((double)data.aboveMtDepth);
    f.push_back((double)data.numIntraNeighbors4);
    f.push_back((double)data.leftIsMIP);
    f.push_back((double)data.aboveIsMIP);
    f.push_back((double)data.leftISPMode);
    f.push_back((double)data.aboveISPMode);
    f.push_back((double)data.neighborMeanDepth);
    f.push_back((double)data.refIsIntra);
    f.push_back((double)data.refIntraAreaRatio);
    f.push_back((double)data.refBiPredAreaRatio);
    f.push_back((double)data.refMvAvgMagnitude);
    f.push_back((double)data.refMvMagnitudeVar);
    f.push_back((double)data.relativeBlockArea);
    f.push_back((double)data.deltaQP);
    f.push_back((double)data.contrastRatio);
    f.push_back((double)data.directionalDominance);
    f.push_back((double)data.variancePerArea);
    f.push_back((double)data.meanMismatch);
    f.push_back((double)data.varMismatch);
    f.push_back((double)data.coefVariation);
    f.push_back((double)data.refDominance);
    f.push_back((double)data.mpmDelta);
    f.push_back((double)data.distCenterX);
    f.push_back((double)data.distCenterY);
    f.push_back((double)data.boundaryComplexityRatio);
    f.push_back((double)data.splittingDensity);
    f.push_back((double)data.centerFocusWeight);
    f.push_back((double)data.blkPixelMean);
    f.push_back((double)data.blkPixelVariance);
    f.push_back((double)data.blkPixelStdDev);
    f.push_back((double)data.blkPixelSum);
    f.push_back((double)data.blkVarH);
    f.push_back((double)data.blkVarV);
    f.push_back((double)data.blkStdH);
    f.push_back((double)data.blkStdV);
    f.push_back((double)data.blkMin);
    f.push_back((double)data.blkMax);
    f.push_back((double)data.blkRange);

    return f;
}

double runLogisticRegression( const std::vector<double>& featureVector )
{
    if (featureVector.size() != NUM_FEATURES) return 0.0;

    double score = kIntercept;
    for( size_t i = 0; i < NUM_FEATURES; ++i )
    {
        double normalizedFeature = ( featureVector[i] - kFeatureMeans[i] ) / (kFeatureStds[i] + 1e-9);
        score += normalizedFeature * kWeights[i];
    }
    return 1.0 / ( 1.0 + std::exp( -score ) );
}

int probabilityToApproxLevel( const double probability )
{
    if( probability < 0.25 ) return kSramLossless;
    if( probability < 0.50 ) return kSramApproxLowConfig;
    if( probability < 0.75 ) return kSramApproxMediumConfig;
    return kSramApproxHighConfig;
}

} // namespace

void MLApproxModel::defineApproxLevel( const CodingStructure& cs, const CodingUnit& cu, double interCost )
{
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
    
    const auto features = buildFeatureVector( featData );
    const double probability = runLogisticRegression( features );
    
    int approxLevel = probabilityToApproxLevel( probability );

    origApproxLevel = approxLevel;
    neighApproxLevel = approxLevel;
}

int MLApproxModel::getOrigApproxLevel() { return origApproxLevel; }
int MLApproxModel::getNeighApproxLevel() { return neighApproxLevel; }

} // namespace vvenc