#include <iostream>
#include <cstdlib> // Necessário para rand() e srand()
#include <ctime>   // Necessário para time()
#include <fstream>

#define ENABLE_SPATIAL_APPROX               1

#define MAX_NUM_OF_REGIONS                  510

namespace vvenc {

class SpatialApproxScheme {
    private:
        static int origApproxLevel, neighApproxLevel;
        static int rows, cols;
        static int spatialApproxConfig[MAX_NUM_OF_REGIONS];
        static int frameWidth, frameHeight;

    public:
        static void init(const char fileName[], int frameWidth, int frameHeight);
        static void reportSpatialApproxConfigs();

        static void defineApproxLevel();

        static int getOrigApproxLevel();
        static int getNeighApproxLevel();

};

}