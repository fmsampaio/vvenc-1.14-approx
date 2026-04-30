#include "MLApproxModel.h"

namespace vvenc {

int MLApproxModel::origApproxLevel, MLApproxModel::neighApproxLevel;

void MLApproxModel::defineApproxLevel() {  //Felipe: definição aleatório dos níveis de aproximação para exemplo...
    std::srand(std::time(0));

    // Gera um número entre 0 e 3
    int randLevel = std::rand() % 4;

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