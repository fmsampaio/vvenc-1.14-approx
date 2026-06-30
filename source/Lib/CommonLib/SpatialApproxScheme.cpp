#include "SpatialApproxScheme.h"

namespace vvenc {

int SpatialApproxScheme::frameWidth, SpatialApproxScheme::frameHeight;

int SpatialApproxScheme::origApproxLevel, SpatialApproxScheme::neighApproxLevel;
int SpatialApproxScheme::rows, SpatialApproxScheme::cols;
int SpatialApproxScheme::spatialApproxConfig[MAX_NUM_OF_REGIONS];


/* Níveis de aproximação: 
SRAM_LOSSLESS --> 1
SRAM_ECC_APPROX_LOW_CONFIG --> 2
SRAM_ECC_APPROX_MEDIUM_CONFIG --> 3
SRAM_ECC_APPROX_HIGH_CONFIG --> 4
*/

void SpatialApproxScheme::init(const char fileName[], int frameWidth, int frameHeight) {
    // FILE* approxCfgFile = fopen(fileName, "r");
    std::ifstream approxCfgFile(fileName);

    SpatialApproxScheme::frameWidth = frameWidth;
    SpatialApproxScheme::frameHeight = frameHeight;

    // res = fscanf(approxCfgFile, "%d", &cols);
    // res = fscanf(approxCfgFile, "%d", &rows);
    approxCfgFile >> SpatialApproxScheme::cols >> SpatialApproxScheme::rows;
    
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            // res = fscanf(approxCfgFile, "%d", &approxLevel);
            int pos = c + r * cols;
            approxCfgFile >> spatialApproxConfig[pos];
        }        
    }        

    approxCfgFile.close();

    reportSpatialApproxConfigs();
}

void SpatialApproxScheme::reportSpatialApproxConfigs() {
    std::cout << "[INFO] Spatial Approx. Config:" << std::endl;
    std::cout << "Columns: " << cols << std::endl;
    std::cout << "Rows: " << rows << std::endl;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int regionId = c + r * cols;

            std::cout << "Region " << regionId << ":" << std::endl;

            int xLeft = (frameWidth / cols) * c;
            int xRight = (frameWidth / cols) * (c + 1);
            int yTop = (frameHeight / rows) * r;
            int yBottom = (frameHeight / rows) * (r + 1);

            std::cout << "Left/Top: (" << xLeft << "," << yTop << ") ";
            std::cout << "Right/Bottom: (" << xRight << "," << yBottom << ") ";

            std::cout << "Approx. Level: " << spatialApproxConfig[regionId] << std::endl;
        }        
    }

}

void SpatialApproxScheme::defineApproxLevel(ComponentID comp, int xCU, int yCU, int wCU, int hCU) { 
    int regionId = getApproxRegionId(comp, xCU, yCU, wCU, hCU);
        
    origApproxLevel = spatialApproxConfig[regionId];
    neighApproxLevel = spatialApproxConfig[regionId];
}

int SpatialApproxScheme::getApproxRegionId(ComponentID comp, int xCU, int yCU, int wCU, int hCU) {
    int colWidth = getColWidth(comp);
    int rowHeight = getRowHeight(comp);

    int xMiddleCU = (xCU + wCU) / 2;
    int yMiddleCU = (yCU + hCU) / 2;

    int targetCol = xMiddleCU / colWidth;
    int targetRow = yMiddleCU / rowHeight;

    targetRow = (targetRow >= rows) ? rows - 1 : targetRow;
    targetCol = (targetCol >= cols) ? cols - 1 : targetCol;
    
    return targetCol + targetRow * cols;
}

int SpatialApproxScheme::getNeighApproxLevel() {
    return neighApproxLevel;
}

int SpatialApproxScheme::getOrigApproxLevel() {
    return origApproxLevel;
}

}