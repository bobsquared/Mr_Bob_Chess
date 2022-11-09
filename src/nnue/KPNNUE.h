#include "layer.h"
#include "../bitboard.h"
#include <omp.h>

#define PAWNPHASE   0
#define KNIGHTPHASE 1
#define BISHOPPHASE 1
#define ROOKPHASE   2
#define QUEENPHASE  4
#define TOTALPHASE (PAWNPHASE * 16 + KNIGHTPHASE * 4 + BISHOPPHASE * 4 + ROOKPHASE * 4 + QUEENPHASE * 2)



class KPNNUE {
public:
    KPNNUE(std::string fileName);
    KPNNUE(int networkSize, int *sizes);
    ~KPNNUE();
    double nnue_evaluate(Bitboard &board);
    void backpropagate(int phase, double *X, int16_t Y, double ***grad, double **bias, double *loss);
    void testBackPropagate();
    void updateWeights(double ***grad, double **bias, double lr, double beta1, double beta2, int batch);
    double forwardpropagate(int phase, double *input);
    void trainNetwork(int dataSize, Bitboard &board, std::string *fens, int16_t *expected, std::string fileName);
    void writeToBinary(std::string fileName);
    void readFromBinary(std::string fileName);
    int evaluate(std::string fen, Bitboard &board);
    int evaluate(Bitboard &board);
    void setupBoardDouble(Bitboard &board, double *output);
    double*** createGradientWeights();
    double** createGradientBias();
    void deleteGradientWeights(double*** grad);
    void deleteGradientBias(double** bias);

    void setupBoardFen(Bitboard &board, std::string fens, double *input);

    int batchSize;


private:
    int init_epoch;
    
    int size;
    Layer **layers;


    
    int getPhase(Bitboard &board);
    
};