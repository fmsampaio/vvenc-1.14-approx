#include <cstdlib> // Necessário para rand() e srand()
#include <ctime>   // Necessário para time()

namespace vvenc {

class MLApproxModel {
    private:
        static int origApproxLevel, neighApproxLevel;

    public:

        static void defineApproxLevel();

        static int getOrigApproxLevel();
        static int getNeighApproxLevel();

};

}