#include <iostream>
#include <cstdlib> // Necessário para rand() e srand()
#include <ctime>   // Necessário para time()
#include <fstream>

#include "CommonDef.h"

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

        static void defineApproxLevel(ComponentID comp, int xCU, int yCU, int wCU, int hCU);

        static int getColWidth(ComponentID comp) { return comp == COMP_Y ? (frameWidth / cols) : ((frameWidth / 2) / cols); }
        static int getRowHeight(ComponentID comp)  { return comp == COMP_Y ? (frameHeight / rows) : ((frameHeight / 2) / rows); }

        static int getApproxRegionId(ComponentID comp, int xCU, int yCU, int wCU, int hCU);

        static int getOrigApproxLevel();
        static int getNeighApproxLevel();

};

}