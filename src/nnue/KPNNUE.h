#ifndef KPNNUE_H
#define KPNNUE_H

#include "layer.h"
#include "../bitboard.h"
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
        Bitboard &board, 
        std::string *fens, 
        int16_t *expected, 
        std::string fileName, 
        int epochs, 
        int batchSize, 
        double lr
    );

    double bulkLoss(int dataSize, Bitboard &board, std::string *fens, int16_t *expected);
    int evaluate(std::string fen, Bitboard &board);
    int evaluate(Bitboard &board);

private:
    int init_epoch;
    int batchSize;
    int size;
    Layer **layers;
    float *features;
    
    int getPhase(Bitboard &board);
    void backpropagate(Bitboard &board, int16_t Y, float ***grad, float **bias);
    void updateWeights(float ***grad, float **bias, float lr, float beta1, float beta2, int batch);
    int forwardpropagate(float *input);
    void writeToBinary(std::string fileName);
    void readFromBinary(std::istream &fileName);
    void readDefault();

    float*** createGradientWeights();
    float** createGradientBias();
    void resetWeightsAndBias(float*** grad, float **bias);
    void deleteGradientWeights(float*** grad);
    void deleteGradientBias(float** bias);

    void setupBoardFen(Bitboard &board, std::string fens, float *input);
    void setupBoardFloat(Bitboard &board, float *output);

    float* updateAccumulatorTrainer(Bitboard &b);
    float *updateAccumulator(Bitboard &b);
    
};

#endif