#include <iostream>
#include <cstdlib> // Necessário para rand() e srand()
#include <ctime>   // Necessário para time()

namespace vvenc {

class MLApproxModel {
    private:
        static int origApproxLevel, neighApproxLevel;

    public:
        // Anotações Felipe:
        /************************************************
         * Aqui podem ser definidas as funções para a extração das features e processamento das lógicas dos modelos de MLApproxModel (função defineApproxLevel está com uma lógica de definição aleatória para exemplificação)
         * 
         * Os atributos origApproxLevel e neighApproxLevel definem os níveis de aproximação que serão aplicados em cada uma das memórias
         * 
         * As funções getOrigApproxLevel e getNeighApproxLevel já estão sendo chamadas pelas rotinas que aplicam o nível de aproximação (ver arquivo IntraSearch.cpp)
         * 
         * Caso precisemos de um arquivo para definir os níveis de aproximação (igual ao que fazemos no frame-level e cu-level), na branch frame-level-dev tem o exemplo de código que faz leitura do caminho do arquivo pela linha de comando do vvencFFapp
        *************************************************/
        static void defineApproxLevel();

        static int getOrigApproxLevel();
        static int getNeighApproxLevel();

};

}