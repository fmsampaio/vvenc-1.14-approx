#include "MLApproxModel.h"

namespace vvenc {

int MLApproxModel::origApproxLevel, MLApproxModel::neighApproxLevel;

/* Níveis de aproximação: 
SRAM_LOSSLESS --> 1
SRAM_ECC_APPROX_LOW_CONFIG --> 2
SRAM_ECC_APPROX_MEDIUM_CONFIG --> 3
SRAM_ECC_APPROX_HIGH_CONFIG --> 4
*/

void MLApproxModel::defineApproxLevel() {  //Felipe: definição aleatório dos níveis de aproximação para exemplo...
    std::srand(std::time(0));

    // Gera um número entre 1 e 4
    int randLevel = (std::rand() % 4) + 1;

    // std::cout << "[DBG] Sorted approx level: " << randLevel << std::endl;

    origApproxLevel = randLevel;
    neighApproxLevel = randLevel;
}

int MLApproxModel::getNeighApproxLevel() {
    return neighApproxLevel;
}

int MLApproxModel::getOrigApproxLevel() {
    return origApproxLevel;
}

}