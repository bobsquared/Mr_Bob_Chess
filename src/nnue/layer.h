#ifndef LAYER_H
#define LAYER_H

#include <algorithm>
#include <cmath>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <random>
#include <cassert>
#include <immintrin.h>


class Layer {

public:
    Layer(std::fstream &file);
    Layer(int nInputs, int nOutputs, bool isL0);
    ~Layer();

    int getNumInputs();
    int getNumOutputs();

    float* linear(float *output, float *input);
    float* ClippedRelu(float *output, float *input);
    void updateWeights(float **dCdW, float *dCdB, float lr, float beta1, float beta2, int batchSize, int batch);

    void DRelu(float *output[]);
    float DMeanSquaredError(int16_t expected);
    float* getActivations();
    float* getForwards();
    float** getWeights();
    float* getBiases();
    float MeanSquaredError(int16_t expected);
    void writeToBinary(std::fstream &file);
    void readFromBinary(std::fstream &file);
    float sigmoidW(float x);
    float *Sigmoid(float *output, float *input);
    float DSigmoid();

    friend std::ostream& operator<< (std::ostream &out, const Layer &data);
    

private:
    bool l0;
    int t;
    int numInputs;
    int numOutputs;

    float **weights; // [col][row] [in][out]
    float *biases;

    float **momentW;
    float *momentB;

    float **vW;
    float *vB;

    float *activations;
    float *forwards;
    

    
    void Dlinear(float *output, float *input);
    

    
    
};

#endif