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
            int yBottom = (frameHeight / rows) * (c + 1);

            std::cout << "Left/Top: (" << xLeft << "," << yTop << ")" << std::endl;
            std::cout << "Right/Bottom: (" << xRight << "," << yBottom << ")" << std::endl;

            std::cout << "Approx. Level: " << spatialApproxConfig[regionId] << std::endl;
        }        
    }

}

void SpatialApproxScheme::defineApproxLevel() {  //Felipe: definição aleatório dos níveis de aproximação para exemplo...
    std::srand(std::time(0));

    // Gera um número entre 1 e 4
    int randLevel = (std::rand() % 4) + 1;

    // std::cout << "[DBG] Sorted approx level: " << randLevel << std::endl;

    origApproxLevel = randLevel;
    neighApproxLevel = randLevel;
}

int SpatialApproxScheme::getNeighApproxLevel() {
    return neighApproxLevel;
}

int SpatialApproxScheme::getOrigApproxLevel() {
    return origApproxLevel;
}

}