#include "layer.h"




Layer::Layer(std::istream &file) {
    readFromBinary(file);
}


Layer::Layer(int nInputs, int nOutputs, bool isL0) {
    std::default_random_engine generator;
    std::normal_distribution<float> distribution(0.0, sqrt( 2.0 / nInputs));

    l0 = isL0;
    numInputs = nInputs;
    numOutputs = nOutputs;
    int numInputsPadded = nInputs + (8 - (nInputs % 8));
    int numOutputsPadded = nOutputs + (8 - (nOutputs % 8));
    t = 1;

    biases = new float[numOutputsPadded]; 
    for (int i = 0; i < numOutputs; i++) {
        biases[i] = distribution(generator);
    }

    momentB = new float[numOutputs](); 
    vB = new float[numOutputs](); 

    activations = new float[numOutputsPadded](); 
    forwards = new float[numOutputsPadded](); 

    if (l0) {
        weights = new float*[numInputsPadded];
        for (int i = 0; i < numInputsPadded; i++) {
            weights[i] = new float[numOutputsPadded] ();
        }

        for (int i = 0; i < numInputs; i++) {
            for (int j = 0; j < numOutputs; j++) {
                weights[i][j] = distribution(generator);
            }
        }

    }
    else {
        weights = new float*[numOutputsPadded];
        for (int i = 0; i < numOutputsPadded; i++) {
            weights[i] = new float[numInputsPadded] ();
        }

        for (int i = 0; i < numOutputs; i++) {
            for (int j = 0; j < numInputs; j++) {
                weights[i][j] = distribution(generator);
            }
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

    delete [] momentB;
    momentB = nullptr;

    delete [] vB;
    vB = nullptr;

    delete [] activations;
    activations = nullptr;

    delete [] forwards;
    forwards = nullptr;

    if (l0) {
        for (int i = 0; i < numInputs; i++) {
            delete [] weights[i];
            weights[i] = nullptr;
        }
    }
    else {
        for (int i = 0; i < numOutputs; i++) {
            delete [] weights[i];
            weights[i] = nullptr;
        }
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

        for (int j = 0; j < numInputs; j++) {
            momentW[j][i] = beta1 * momentW[j][i] + (1 - beta1) * grad[j][i] / batchSize;
            vW[j][i] = beta2 * vW[j][i] + (1 - beta2) * std::pow(grad[j][i] / batchSize, 2);

            float mhat = momentW[j][i] / (1 - std::pow(beta1, batch + 1));
            float vhat = vW[j][i] / (1 - std::pow(beta2, batch + 1));

            if (l0) {
                weights[j][i] = weights[j][i] - lr * mhat / (sqrt(vhat) + 1e-8);
            }
            else {
                weights[i][j] = weights[i][j] - lr * mhat / (sqrt(vhat) + 1e-8);
            }
        }

        momentB[i] = beta1 * momentB[i] + (1 - beta1) * bias[i] / batchSize;
        vB[i] = beta2 * vB[i] + (1 - beta2) * std::pow(bias[i] / batchSize, 2);

        float mhat = momentB[i] / (1 - std::pow(beta1, t + 1));
        float vhat = vB[i] / (1 - std::pow(beta2, t + 1));
            
        biases[i] = biases[i] - lr * mhat / (sqrt(vhat) + 1e-8);

    }
    t++;
}




#ifdef __AVX2__
float* Layer::linear(float *output, float *input) {

    assert(!l0);
    int input_chunk_size = (numInputs % 8 != 0) + numInputs / 8;
    int output_chunk_size = (numOutputs % 4 != 0) + numOutputs / 4;

    for (int i = 0; i < output_chunk_size; i++) {

        __m256 s1= _mm256_setzero_ps();
        __m256 s2= _mm256_setzero_ps();
        __m256 s3= _mm256_setzero_ps();
        __m256 s4= _mm256_setzero_ps();

        int outIndex1 = i * 4;
        int outIndex2 = i * 4 + 1;
        int outIndex3 = i * 4 + 2;
        int outIndex4 = i * 4 + 3;

        for (int j = 0; j < input_chunk_size; j++) {
            int inIndex = j * 8;
            __m256 x = _mm256_loadu_ps(&input[inIndex]);

            __m256 w1 = _mm256_loadu_ps(&weights[outIndex1][inIndex]);
            __m256 w2 = _mm256_loadu_ps(&weights[outIndex2][inIndex]);
            __m256 w3 = _mm256_loadu_ps(&weights[outIndex3][inIndex]);
            __m256 w4 = _mm256_loadu_ps(&weights[outIndex4][inIndex]);

            s1 = _mm256_add_ps(s1, _mm256_mul_ps(x, w1));
            s2 = _mm256_add_ps(s2, _mm256_mul_ps(x, w2));
            s3 = _mm256_add_ps(s3, _mm256_mul_ps(x, w3));
            s4 = _mm256_add_ps(s4, _mm256_mul_ps(x, w4));
        }

        s1 = _mm256_hadd_ps(s1, s2);
        s3 = _mm256_hadd_ps(s3, s4);
        s1 = _mm256_hadd_ps(s1, s3);

        __m128 hi = _mm256_extractf128_ps(s1, 1);
        __m128 lo = _mm256_extractf128_ps(s1, 0);
        __m128 bias = _mm_loadu_ps(&biases[i * 4]);

        hi = _mm_add_ps(bias, _mm_add_ps(hi, lo));
        _mm_storeu_ps(&output[i * 4], hi);

        #ifdef NNUE_TRAINER
        _mm_storeu_ps(&forwards[i * 4], hi);
        _mm_storeu_ps(&activations[i * 4], hi);
        #endif
    }

    return output + numOutputs;
}

#elif defined(__SSE2__)
float* Layer::linear(float *output, float *input) {

    assert(!l0);
    int input_chunk_size = (numInputs % 4 != 0) + numInputs / 4;
    int output_chunk_size = (numOutputs % 4 != 0) + numOutputs / 4;

    for (int i = 0; i < output_chunk_size; i++) {

        __m128 s1= _mm_setzero_ps();
        __m128 s2= _mm_setzero_ps();
        __m128 s3= _mm_setzero_ps();
        __m128 s4= _mm_setzero_ps();

        int outIndex1 = i * 4;
        int outIndex2 = i * 4 + 1;
        int outIndex3 = i * 4 + 2;
        int outIndex4 = i * 4 + 3;

        for (int j = 0; j < input_chunk_size; j++) {
            int inIndex = j * 4;
            __m128 x = _mm_loadu_ps(&input[inIndex]);

            __m128 w1 = _mm_loadu_ps(&weights[outIndex1][inIndex]);
            __m128 w2 = _mm_loadu_ps(&weights[outIndex2][inIndex]);
            __m128 w3 = _mm_loadu_ps(&weights[outIndex3][inIndex]);
            __m128 w4 = _mm_loadu_ps(&weights[outIndex4][inIndex]);

            s1 = _mm_add_ps(s1, _mm_mul_ps(x, w1));
            s2 = _mm_add_ps(s2, _mm_mul_ps(x, w2));
            s3 = _mm_add_ps(s3, _mm_mul_ps(x, w3));
            s4 = _mm_add_ps(s4, _mm_mul_ps(x, w4));
        }

        __m128 r1 = _mm_movehl_ps(s1, s2);
        __m128 r2 = _mm_movelh_ps(s2, s1);
        s1 = _mm_add_ps(r1, r2);

        r1 = _mm_movehl_ps(s3, s4);
        r2 = _mm_movelh_ps(s4, s3);
        s2 = _mm_add_ps(r1, r2);

        s1 = _mm_shuffle_ps(s1, s1, _MM_SHUFFLE(1, 3, 0, 2));
        s2 = _mm_shuffle_ps(s2, s2, _MM_SHUFFLE(1, 3, 0, 2));
        r1 = _mm_movehl_ps(s2, s1);
        r2 = _mm_movelh_ps(s1, s2);
        s1 = _mm_add_ps(r1, r2);

        __m128 bias = _mm_loadu_ps(&biases[i * 4]);

        s1 = _mm_add_ps(bias, s1);
        _mm_store_ps(&output[i * 4], s1);

        #ifdef NNUE_TRAINER
        _mm_storeu_ps(&forwards[i * 4], s1);
        _mm_storeu_ps(&activations[i * 4], s1);
        #endif
    }

    return output + numOutputs;
}

#else 

float* Layer::linear(float *output, float *input) {
    for (int i = 0; i < numOutputs; i++) {
        output[i] = biases[i];
    }

    for (int i = 0; i < numInputs; i++) {
        for (int j = 0; j < numOutputs; j++) {
            output[j] += input[i] * weights[j][i];
        }
    }

    for (int i = 0; i < numOutputs; i++) {
        forwards[i] = output[i];
        activations[i] = forwards[i];
    }

    return output + numOutputs;
}

#endif


#ifdef __AVX2__
float* Layer::relu(float *output, float *input) {
    int num_chunks = numOutputs / 8 + (numOutputs % 8 != 0);

    __m256 regzeroes = _mm256_setzero_ps();

    for (int i = 0; i < num_chunks; i++) {
        __m256 reg = _mm256_loadu_ps(&input[i * 8]);
        reg = _mm256_max_ps(reg, regzeroes);

        _mm256_storeu_ps(&output[i * 8], reg);

        #ifdef NNUE_TRAINER
        _mm256_storeu_ps(&activations[i * 8], reg);
        #endif
    }
    
    return output + numOutputs;
}

#elif defined(__SSE2__)

float* Layer::relu(float *output, float *input) {
    int num_chunks = numOutputs / 4 + (numOutputs % 4 != 0);

    __m128 regzeroes = _mm_setzero_ps();

    for (int i = 0; i < num_chunks; i++) {
        __m128 reg = _mm_loadu_ps(&input[i * 4]);
        reg = _mm_max_ps(reg, regzeroes);

        _mm_storeu_ps(&output[i * 4], reg);

        #ifdef NNUE_TRAINER
        _mm_storeu_ps(&activations[i * 4], reg);
        #endif
    }
    
    return output + numOutputs;
}

#else

float* Layer::relu(float *output, float *input) {
    for (int i = 0; i < numOutputs; i++) {
        output[i] = std::max(input[i], 0.0f);
        activations[i] = output[i];
    }
    return output + numOutputs;
}

#endif



void Layer::DRelu(float *output[]) {
    for (int i = 0; i < numOutputs; i++) {
        (*output)[i] = forwards[i] > 0.0;
    }
}


float Layer::sigmoidW(float x) {
    return 1.0 / (1.0 + exp(-x / 225));
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
    file.write(reinterpret_cast<const char *>(&l0), sizeof(l0));
    file.write(reinterpret_cast<const char *>(&numInputs), sizeof(numInputs));
    file.write(reinterpret_cast<const char *>(&numOutputs), sizeof(numOutputs));
    int numInputsPadded = numInputs + (8 - (numInputs % 8));
    int numOutputsPadded = numOutputs + (8 - (numOutputs % 8));

    // write weights and biases
    if (l0) {
        for (int i = 0; i < numInputsPadded; i++) {
            for (int j = 0; j < numOutputsPadded; j++) {
                file.write(reinterpret_cast<const char *>(&weights[i][j]), sizeof(weights[i][j]));
            }
        }
    }
    else {
        for (int i = 0; i < numOutputsPadded; i++) {
            for (int j = 0; j < numInputsPadded; j++) {
                file.write(reinterpret_cast<const char *>(&weights[i][j]), sizeof(weights[i][j]));
            }
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



void Layer::readFromBinary(std::istream &file) {

    // write number of inputs outputs
    file.read(reinterpret_cast<char *>(&l0), sizeof(l0));
    file.read(reinterpret_cast<char *>(&numInputs), sizeof(numInputs));
    file.read(reinterpret_cast<char *>(&numOutputs), sizeof(numOutputs));
    int numInputsPadded = numInputs + (8 - (numInputs % 8));
    int numOutputsPadded = numOutputs + (8 - (numOutputs % 8));

    // write weights and biases
    if (l0) {
        weights = new float*[numInputsPadded];
        for (int i = 0; i < numInputsPadded; i++) {
            weights[i] = new float[numOutputsPadded];
            for (int j = 0; j <  numOutputsPadded; j++) {
                file.read(reinterpret_cast<char *>(&weights[i][j]), sizeof(weights[i][j]));
            }
        }
    }
    else {
        weights = new float*[numOutputsPadded];
        for (int i = 0; i < numOutputsPadded; i++) {
            weights[i] = new float[numInputsPadded];
            for (int j = 0; j <  numInputsPadded; j++) {
                file.read(reinterpret_cast<char *>(&weights[i][j]), sizeof(weights[i][j]));
            }
        }
    }

    biases = new float[numOutputsPadded];
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
    activations = new float[numOutputsPadded];
    forwards = new float[numOutputsPadded];

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


