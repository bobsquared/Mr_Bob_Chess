#include <algorithm>
#include <cmath>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <random>


class Layer {

public:
    Layer(std::fstream &file);
    Layer(int nInputs, int nOutputs);
    ~Layer();

    int getNumInputs();
    int getNumOutputs();

    double* linear(double *output, double *input);
    double* Relu(double *output, double *input);
    void updateWeights(double **dCdW, double *dCdB, double lr, double beta1, double beta2, int batchSize, int batch);

    void DRelu(double *output[]);
    double DMeanSquaredError(int phaseTotal, int16_t expected);
    double* getActivations();
    double* getForwards();
    double** getWeights();
    double MeanSquaredError(int phaseTotal, int16_t expected);
    void writeToBinary(std::fstream &file);
    void readFromBinary(std::fstream &file);
    double sigmoidW(double x);
    double *Sigmoid(double *output, double *input);
    double DSigmoid(int phaseTotal);

    friend std::ostream& operator<< (std::ostream &out, const Layer &data);
    

private:
    int t;
    int numInputs;
    int numOutputs;

    double **weights; // [col][row] [in][out]
    double *biases;

    double *alpha;

    double **momentW;
    double *momentB;

    double **vW;
    double *vB;

    double *activations;
    double *forwards;
    

    
    void Dlinear(double *output, double *input);
    

    
    
};