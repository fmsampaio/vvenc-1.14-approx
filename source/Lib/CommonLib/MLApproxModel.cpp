#include "MLApproxModel.h"

#include "CodingStructure.h"
#include "Picture.h"
#include "Unit.h"
#include "UnitPartitioner.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>
// Use OpenCV for feature computations to match collection pipeline
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

namespace vvenc {

int MLApproxModel::origApproxLevel = 1;
int MLApproxModel::neighApproxLevel = 1;

namespace
{
constexpr int kSramLossless              = 1;
constexpr int kSramApproxLowConfig       = 2;
constexpr int kSramApproxMediumConfig    = 3;
constexpr int kSramApproxHighConfig      = 4;

constexpr std::array<double, 18> kFeatureMeans = {
    29.498982, 2968.920100, 1620.036158, 20.526696, 701.823577, 3.732432,
    3.000142, 0.338898, 34.885955, 3.436997, 2.744968, 435.321747,
    16.699484, 73166.326438, 282.532960, 266.210012, 12.100976, 235.000000
};

constexpr std::array<double, 18> kFeatureScales = {
    5.590259, 1147.282980, 598.161229, 19.989478, 1837.701655, 1.996419,
    1.414257, 1.502107, 7.238839, 1.051184, 1.002910, 628.794566,
    12.507956, 229344.445100, 460.429885, 453.887745, 10.248764, 54.099641
};

template<typename T>
T clipValue( const T value, const T low, const T high )
{
    return std::max( low, std::min( value, high ) );
}

double normalizeFeature( const double value, const size_t idx )
{
    return ( value - kFeatureMeans[idx] ) / kFeatureScales[idx];
}

double varianceFromSamples( const std::vector<double>& samples )
{
    if( samples.empty() )
    {
        return 0.0;
    }

    double sum = 0.0;
    double sumSq = 0.0;
    for( const double sample : samples )
    {
        sum += sample;
        sumSq += sample * sample;
    }

    const double count = static_cast<double>( samples.size() );
    const double mean   = sum / count;
    return std::max( 0.0, ( sumSq / count ) - ( mean * mean ) );
}

std::array<double, 18> buildFeatureVector( const CodingStructure& cs, const CodingUnit& cu, const Pel* orgBuf, const int orgStride, const CompArea& block )
{
    std::array<double, 18> features{};

    const int frameWidth  = cs.picture->lwidth();
    const int frameHeight = cs.picture->lheight();
    const int blockWidth   = block.width;
    const int blockHeight  = block.height;
    const int blockArea    = blockWidth * blockHeight;
    const int blockSide    = std::max( blockWidth, blockHeight );

    const int compFrameWidth  = frameWidth  >> getComponentScaleX( block.compID, block.chromaFormat );
    const int compFrameHeight = frameHeight >> getComponentScaleY( block.compID, block.chromaFormat );

    int borderContactMask = 0;
    if( block.x == 0 ) borderContactMask |= 1;
    if( block.y == 0 ) borderContactMask |= 2;
    if( block.x + blockWidth >= compFrameWidth ) borderContactMask |= 4;
    if( block.y + blockHeight >= compFrameHeight ) borderContactMask |= 8;

    const int blockAreaGroup = clipValue( floorLog2( std::max( 1, blockSide ) ) - 1, 1, 6 );
    const int frameLevel = cs.slice->isIntra() ? 0 : static_cast<int>( cs.slice->TLayer ) + 1;

    // 1. Usar CV_32F (float) exatamente como estava no ImageFeatures.cpp
    cv::Mat blk_f(blockHeight, blockWidth, CV_32F);
    for( int y = 0; y < blockHeight; ++y )
    {
        for( int x = 0; x < blockWidth; ++x )
        {
            blk_f.at<float>( y, x ) = static_cast<float>( orgBuf[y * orgStride + x] );
        }
    }

    // 2. Estatísticas globais do bloco
    const double sum = cv::sum(blk_f)[0];
    cv::Scalar meanScalar, stddevScalar;
    cv::meanStdDev(blk_f, meanScalar, stddevScalar);
    const double pixelStdDev = stddevScalar[0];
    const double pixelVar    = pixelStdDev * pixelStdDev;

    double minVal = 0.0, maxVal = 0.0;
    cv::minMaxLoc(blk_f, &minVal, &maxVal);

    // 3. varH, stdH (Cálculo 1:1 tirado do calculate_stats_cv)
    cv::Mat row_means; 
    cv::reduce(blk_f, row_means, 1, cv::REDUCE_AVG);
    cv::Mat row_means_exp; 
    cv::repeat(row_means, 1, blk_f.cols, row_means_exp);
    cv::Mat diff_row = blk_f - row_means_exp;
    cv::Mat row_vars; 
    cv::reduce(diff_row.mul(diff_row), row_vars, 1, cv::REDUCE_AVG);
    cv::Mat row_stds; 
    cv::sqrt(row_vars, row_stds);
    const double varH = cv::mean(row_vars)[0];
    const double stdH = cv::mean(row_stds)[0];

    // 4. varV (Cálculo 1:1 tirado do calculate_stats_cv)
    cv::Mat col_means; 
    cv::reduce(blk_f, col_means, 0, cv::REDUCE_AVG);
    cv::Mat col_means_exp; 
    cv::repeat(col_means, blk_f.rows, 1, col_means_exp);
    cv::Mat diff_col = blk_f - col_means_exp;
    cv::Mat col_vars; 
    cv::reduce(diff_col.mul(diff_col), col_vars, 0, cv::REDUCE_AVG);
    const double varV = cv::mean(col_vars)[0];

    features[0]  = normalizeFeature( static_cast<double>( cs.baseQP ), 0 );
    features[1]  = normalizeFeature( static_cast<double>( frameWidth ), 1 );
    features[2]  = normalizeFeature( static_cast<double>( frameHeight ), 2 );
    features[3]  = normalizeFeature( static_cast<double>( blockHeight ), 3 );
    features[4]  = normalizeFeature( static_cast<double>( blockArea ), 4 );
    features[5]  = normalizeFeature( static_cast<double>( blockAreaGroup ), 5 );
    features[6]  = normalizeFeature( static_cast<double>( frameLevel ), 6 );
    features[7]  = normalizeFeature( static_cast<double>( borderContactMask ), 7 );
    features[8]  = normalizeFeature( static_cast<double>( cu.qp ), 8 );
    features[9]  = normalizeFeature( static_cast<double>( cu.depth ), 9 );
    features[10] = normalizeFeature( static_cast<double>( cu.qtDepth ), 10 );
    features[11] = normalizeFeature( pixelVar, 11 );
    features[12] = normalizeFeature( pixelStdDev, 12 );
    features[13] = normalizeFeature( sum, 13 );
    features[14] = normalizeFeature( varH, 14 );
    features[15] = normalizeFeature( varV, 15 );
    features[16] = normalizeFeature( stdH, 16 );
    features[17] = normalizeFeature( maxVal, 17 );

    return features;
}

double runLogisticRegression( const std::array<double, 18>& featureVector )
{
    double score = -0.1409745871952766;

    score += featureVector[0]  * -1.9221417989732441;
    score += featureVector[1]  * -4.6668393537063473;
    score += featureVector[2]  *  5.3095329777676943;
    score += featureVector[3]  *  0.4063274801515502;
    score += featureVector[4]  * -0.6032519761116906;
    score += featureVector[5]  * -0.1379538547424307;
    score += featureVector[6]  * -0.7005603969895647;
    score += featureVector[7]  * -0.2056834290194104;
    score += featureVector[8]  *  2.2614241046698460;
    score += featureVector[9]  *  0.7465005898979555;
    score += featureVector[10] * -0.3022759304609476;
    score += featureVector[11] *  0.3073835800502374;
    score += featureVector[12] *  0.2606695343374303;
    score += featureVector[13] * -0.3760401036353281;
    score += featureVector[14] * -0.7566037728568388;
    score += featureVector[15] * -0.4214301781114111;
    score += featureVector[16] *  0.5270410167138336;
    score += featureVector[17] *  0.1956714806883464;

    return 1.0 / ( 1.0 + std::exp( -score ) );
}

int probabilityToApproxLevel( const double probability )
{
    if( probability < 0.25 ) return kSramLossless;
    if( probability < 0.50 ) return kSramApproxLowConfig;
    if( probability < 0.75 ) return kSramApproxMediumConfig;
    return kSramApproxHighConfig;
}

/*
int probabilityToApproxLevel( const double probability )
{
    if( probability < 0.60 ) return kSramLossless;
    if( probability < 0.85 ) return kSramApproxLowConfig;
    if( probability < 0.95 ) return kSramApproxMediumConfig;
    return kSramApproxHighConfig;
}
*/

} // namespace

void MLApproxModel::defineApproxLevel( const CodingStructure& cs, const CodingUnit& cu, const Pel* orgBuf, const int orgStride, const CompArea& block )
{
    if( cs.slice->isIntra() )
    {
        origApproxLevel = kSramLossless;
        neighApproxLevel = kSramLossless;
        return;
    }

    const auto features = buildFeatureVector( cs, cu, orgBuf, orgStride, block );
    const double probability = runLogisticRegression( features );
    const int approxLevel = probabilityToApproxLevel( probability );

    origApproxLevel = approxLevel;
    neighApproxLevel = approxLevel;
}

int MLApproxModel::getNeighApproxLevel()
{
    return neighApproxLevel;
}

int MLApproxModel::getOrigApproxLevel()
{
    return origApproxLevel;
}

} // namespace vvenc