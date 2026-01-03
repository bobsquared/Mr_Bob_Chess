#ifndef KPNNUE_H
#define KPNNUE_H

#include "layer.h"
#include "../board/bitboard.h"
#include "../board/fen.h"
#include <random>
#include <algorithm>
#include <immintrin.h>
#include <sstream>

#define PAWNPHASE   0
#define KNIGHTPHASE 1
#define BISHOPPHASE 1
#define ROOKPHASE   2
#define QUEENPHASE  4
#define TOTALPHASE (PAWNPHASE * 16 + KNIGHTPHASE * 4 + BISHOPPHASE * 4 + ROOKPHASE * 4 + QUEENPHASE * 2)



class KPNNUE {
public:
    KPNNUE();
    KPNNUE(const unsigned char* defaultNetwork, unsigned int bsize);
    KPNNUE(int networkSize, int *sizes);
    ~KPNNUE();
    void setNetwork(std::string fileName);
    
    void trainNetwork
    (
        int dataSize, 
        Board &board, 
        std::string *fens, 
        int16_t *expected, 
        std::string fileName, 
        int epochs, 
        int batchSize, 
        double lr
    );

    double bulkLoss(int dataSize, Board &board, std::string *fens, int16_t *expected);
    int evaluate(std::string fen, Board &board);
    int evaluate(Board &board);

private:
    int init_epoch;
    int batchSize;
    int size;
    Layer **layers;
    float *features;
    
    int getPhase(Board &board);
    void backpropagate(Board &board, int16_t Y, float ***grad, float **bias);
    void updateWeights(float ***grad, float **bias, float lr, float beta1, float beta2, int batch);
    int forwardpropagate(float *whiteInput, float *blackInput, bool toMove);
    void writeToBinary(std::string fileName);
    void readFromBinary(std::istream &fileName);
    void readDefault();

    float*** createGradientWeights();
    float** createGradientBias();
    void resetWeightsAndBias(float*** grad, float **bias);
    void deleteGradientWeights(float*** grad);
    void deleteGradientBias(float** bias);

    void setupBoardFen(Board &board, std::string fens, float *input);
    void setupBoardFloat(Board &board, float *output);

    float* updateAccumulatorTrainer(Board &b);
    float *updateAccumulator(Board &b);
    
};

#endif