#include "layer.h"




Layer::Layer(std::fstream &file) {
    readFromBinary(file);
}


Layer::Layer(int nInputs, int nOutputs) {
    std::default_random_engine generator;
    std::normal_distribution<float> distribution(0.0, sqrt( 2.0 / nInputs));

    numInputs = nInputs;
    numOutputs = nOutputs;
    t = 1;

    biases = new float[numOutputs]; 
    for (int i = 0; i < numOutputs; i++) {
        biases[i] = distribution(generator);
    }

    alpha = new float[numOutputs](); 
    momentB = new float[numOutputs](); 
    vB = new float[numOutputs](); 

    activations = new float[numOutputs](); 
    forwards = new float[numOutputs](); 

    weights = new float*[numInputs];
    for (int i = 0; i < numInputs; i++) {
        weights[i] = new float[numOutputs];
        for (int j = 0; j < numOutputs; j++) {
            weights[i][j] = distribution(generator);
        }
    }

    momentW = new float*[numInputs];
    for (int i = 0; i < numInputs; i++) {
        momentW[i] = new float[numOutputs]();
    }

    vW = new float*[numInputs];
    for (int i = 0; i < numInputs; i++) {
        vW[i] = new float[numOutputs]();
    }

}

Layer::~Layer() {
    delete [] biases;
    biases = nullptr;

    delete [] alpha;
    alpha = nullptr;

    delete [] momentB;
    momentB = nullptr;

    delete [] vB;
    vB = nullptr;

    delete [] activations;
    activations = nullptr;

    delete [] forwards;
    forwards = nullptr;

    for (int i = 0; i < numInputs; i++) {
        delete [] weights[i];
        weights[i] = nullptr;
    }
    delete [] weights;
    weights = nullptr;

    for (int i = 0; i < numInputs; i++) {
        delete [] momentW[i];
        momentW[i] = nullptr;
    }
    delete [] momentW;
    momentW = nullptr;

    for (int i = 0; i < numInputs; i++) {
        delete [] vW[i];
        vW[i] = nullptr;
    }
    delete [] vW;
    vW = nullptr;
}

int Layer::getNumInputs() {
    return numInputs;
}

int Layer::getNumOutputs() {
    return numOutputs;
}

float* Layer::getActivations() {
    return activations;
}


float* Layer::getForwards() {
    return forwards;
}


float** Layer::getWeights() {
    return weights;
}


float* Layer::getBiases() {
    return biases;
}

void Layer::updateWeights(float **grad, float *bias, float lr, float beta1, float beta2, int batchSize, int batch) {

    for (int i = 0; i < numOutputs; i++) {

        // alpha[i] = lr * sqrt(1 - std::pow(beta2, batch + 1)) / (1 - std::pow(beta1, batch + 1));
        for (int j = 0; j < numInputs; j++) {
            momentW[j][i] = beta1 * momentW[j][i] + (1 - beta1) * grad[j][i] / batchSize;
            vW[j][i] = beta2 * vW[j][i] + (1 - beta2) * std::pow(grad[j][i] / batchSize, 2);

            float mhat = momentW[j][i] / (1 - std::pow(beta1, batch + 1));
            float vhat = vW[j][i] / (1 - std::pow(beta2, batch + 1));

            weights[j][i] = weights[j][i] - lr * mhat / (sqrt(vhat) + 1e-8);
        }

        momentB[i] = beta1 * momentB[i] + (1 - beta1) * bias[i] / batchSize;
        vB[i] = beta2 * vB[i] + (1 - beta2) * std::pow(bias[i] / batchSize, 2);

        float mhat = momentB[i] / (1 - std::pow(beta1, t + 1));
        float vhat = vB[i] / (1 - std::pow(beta2, t + 1));
            
        biases[i] = biases[i] - lr * mhat / (sqrt(vhat) + 1e-8);

    }
    t++;
}

float* Layer::linear(float *output, float *input) {
    for (int i = 0; i < numOutputs; i++) {
        output[i] = biases[i];
    }

    for (int i = 0; i < numInputs; i++) {
        for (int j = 0; j < numOutputs; j++) {
            output[j] += input[i] * weights[i][j];
        }
    }

    for (int i = 0; i < numOutputs; i++) {
        forwards[i] = output[i];
        activations[i] = forwards[i];
    }

    return output + numOutputs;
}


float* Layer::Relu(float *output, float *input) {
    for (int i = 0; i < numOutputs; i++) {
        output[i] = std::min(1.0f, std::max(input[i], 0.0f));
        activations[i] = output[i];
    }
    return output + numOutputs;
}



void Layer::DRelu(float *output[]) {
    for (int i = 0; i < numOutputs; i++) {
        (*output)[i] = (forwards[i] > 0.0 && forwards[i] < 1.0);
    }
}


float Layer::sigmoidW(float x) {
    return 1.0 / (1.0 + exp(-x / 400));
}


float* Layer::Sigmoid(float *output, float *input) {
    for (int i = 0; i < numOutputs; i++) {
        output[i] = sigmoidW(input[i]);
        activations[i] = output[i];
    }

    return output + numOutputs;
}



float Layer::DSigmoid() {
    return sigmoidW(activations[0]) * (1.0 - sigmoidW(activations[0]));
}


// expected -> The expected value
// input -> The value that we got
float Layer::MeanSquaredError(int16_t expected) {
    return pow(sigmoidW(activations[0]) - sigmoidW(expected), 2);
}


float Layer::DMeanSquaredError(int16_t expected) {
    return sigmoidW(activations[0]) - sigmoidW(expected);
}

void Layer::writeToBinary(std::fstream &file) {

    // write number of inputs outputs
    file.write(reinterpret_cast<const char *>(&numInputs), sizeof(numInputs));
    file.write(reinterpret_cast<const char *>(&numOutputs), sizeof(numOutputs));

    // write weights and biases
    for (int i = 0; i < numInputs; i++) {
        for (int j = 0; j < numOutputs; j++) {
            file.write(reinterpret_cast<const char *>(&weights[i][j]), sizeof(weights[i][j]));
        }
    }

    for (int i = 0; i < numOutputs; i++) {
        file.write(reinterpret_cast<const char *>(&biases[i]), sizeof(biases[i]));
    }

    // write optimizer parameters
    for (int i = 0; i < numInputs; i++) {
        for (int j = 0; j < numOutputs; j++) {
            file.write(reinterpret_cast<const char *>(&momentW[i][j]), sizeof(momentW[i][j]));
        }
    }

    for (int i = 0; i < numOutputs; i++) {
        file.write(reinterpret_cast<const char *>(&momentB[i]), sizeof(momentB[i]));
    }

    for (int i = 0; i < numInputs; i++) {
        for (int j = 0; j < numOutputs; j++) {
            file.write(reinterpret_cast<const char *>(&vW[i][j]), sizeof(vW[i][j]));
        }
    }

    for (int i = 0; i < numOutputs; i++) {
        file.write(reinterpret_cast<const char *>(&vB[i]), sizeof(vB[i]));
    }

    // // feed forward data
    for (int i = 0; i < numOutputs; i++) {
        file.write(reinterpret_cast<const char *>(&activations[i]), sizeof(activations[i]));
    }

    for (int i = 0; i < numOutputs; i++) {
        file.write(reinterpret_cast<const char *>(&forwards[i]), sizeof(forwards[i]));
    }

}



void Layer::readFromBinary(std::fstream &file) {

    // write number of inputs outputs
    file.read(reinterpret_cast<char *>(&numInputs), sizeof(numInputs));
    file.read(reinterpret_cast<char *>(&numOutputs), sizeof(numOutputs));

    // write weights and biases
    weights = new float*[numInputs];
    for (int i = 0; i < numInputs; i++) {
        weights[i] = new float[numOutputs];
        for (int j = 0; j <  numOutputs; j++) {
            file.read(reinterpret_cast<char *>(&weights[i][j]), sizeof(weights[i][j]));
        }
    }

    biases = new float[numOutputs];
    for (int i = 0; i <  numOutputs; i++) {
        file.read(reinterpret_cast<char *>(&biases[i]), sizeof(biases[i]));
    }


    // write optimizer parameters
    momentW = new float*[numInputs];
    for (int i = 0; i < numInputs; i++) {
        momentW[i] = new float[numOutputs];
        for (int j = 0; j <  numOutputs; j++) {
            file.read(reinterpret_cast<char *>(&momentW[i][j]), sizeof(momentW[i][j]));
        }
    }

    momentB = new float[numOutputs];
    for (int i = 0; i <  numOutputs; i++) {
        file.read(reinterpret_cast<char *>(&momentB[i]), sizeof(momentB[i]));
    }

    vW = new float*[numInputs];
    for (int i = 0; i < numInputs; i++) {
        vW[i] = new float[numOutputs];
        for (int j = 0; j <  numOutputs; j++) {
            file.read(reinterpret_cast<char *>(&vW[i][j]), sizeof(vW[i][j]));
        }
    }

    vB = new float[numOutputs];
    for (int i = 0; i <  numOutputs; i++) {
        file.read(reinterpret_cast<char *>(&vB[i]), sizeof(vB[i]));
    }

    // feed forward data
    activations = new float[numOutputs];
    forwards = new float[numOutputs];

    for (int i = 0; i <  numOutputs; i++) {
        file.read(reinterpret_cast<char *>(&activations[i]), sizeof(activations[i]));
    }

    for (int i = 0; i <  numOutputs; i++) {
        file.read(reinterpret_cast<char *>(&forwards[i]), sizeof(forwards[i]));
    }

}


std::ostream& operator<< (std::ostream &out, const Layer &data) {
    out << "Weights:" << std::endl;
    
    for (int i = 0; i < data.numOutputs; i++) {
        out << "["; 
        for (int j = 0; j < data.numInputs - 1; j++) {
            out << data.weights[j][i] << ", ";
        }
        out << data.weights[data.numInputs - 1][i];
        out << "]" << std::endl;
    }

    out << std::endl << "Biases:" << std::endl;
    out << "["; 
    for (int i = 0; i < data.numOutputs - 1; i++) {
        out << data.biases[i] << ", ";
    }
    out << data.biases[data.numOutputs - 1];
    out << "]" << std::endl;

    return out;
}


