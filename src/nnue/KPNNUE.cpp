

#include "KPNNUE.h"


KPNNUE::KPNNUE(std::string fileName) {
    readFromBinary(fileName);
}


KPNNUE::KPNNUE(int networkSize, int *sizes) {
    
    init_epoch = 0;
    size = networkSize - 1;

    layers = new Layer*[networkSize - 1];
    for (int i = 0; i < networkSize - 1; i++) {
        layers[i] = new Layer(sizes[i], sizes[i + 1], i == 0);
    }

}



KPNNUE::~KPNNUE() {

    for (int i = 0; i < size; i++) {
        delete layers[i];
    }
    delete [] layers;

}


#ifdef __AVX2__
float* KPNNUE::updateAccumulator(Bitboard &b) {
    float **weights = layers[0]->getWeights();
    float *biases = layers[0]->getBiases();
    float *features = b.getFeatures();
    std::vector<Accumulator::Features> *addAccumulate = b.getAddFeatures();
    std::vector<Accumulator::Features> *removeAccumulate = b.getRemoveFeatures();
    int numOutputs = layers[0]->getNumOutputs();
    int num_chunks = numOutputs / 8 + (numOutputs % 8 != 0);
    __m256 r[64];

    if (b.getResetFlag()) {
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm256_loadu_ps(&biases[j * 8]);
        }
        b.setResetFlag(false);
    }
    else {
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm256_loadu_ps(&features[j * 8]);
        }
    }

    while (!addAccumulate->empty()) {
        Accumulator::Features i = addAccumulate->back();
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm256_add_ps(r[j], _mm256_loadu_ps(&weights[i.pieceType + i.location][j * 8]));
        }
        addAccumulate->pop_back();
    }

    while (!removeAccumulate->empty()) {
        Accumulator::Features i = removeAccumulate->back();
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm256_sub_ps(r[j], _mm256_loadu_ps(&weights[i.pieceType + i.location][j * 8]));
        }
        removeAccumulate->pop_back();
    }

    for (int j = 0; j < num_chunks; j++) {
        _mm256_storeu_ps(&features[j * 8], r[j]);
    }

    return nullptr;
}

#elif defined(__SSE2__)
float* KPNNUE::updateAccumulator(Bitboard &b) {
    float **weights = layers[0]->getWeights();
    float *biases = layers[0]->getBiases();
    float *features = b.getFeatures();
    std::vector<Accumulator::Features> *addAccumulate = b.getAddFeatures();
    std::vector<Accumulator::Features> *removeAccumulate = b.getRemoveFeatures();
    int numOutputs = layers[0]->getNumOutputs();
    int num_chunks = numOutputs / 4 + (numOutputs % 4 != 0);
    __m128 r[64];

    if (b.getResetFlag()) {
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm_loadu_ps(&biases[j * 4]);
        }
        b.setResetFlag(false);
    }
    else {
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm_loadu_ps(&features[j * 4]);
        }
    }

    while (!addAccumulate->empty()) {
        Accumulator::Features i = addAccumulate->back();
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm_add_ps(r[j], _mm_loadu_ps(&weights[i.pieceType + i.location][j * 4]));
        }
        addAccumulate->pop_back();
    }

    while (!removeAccumulate->empty()) {
        Accumulator::Features i = removeAccumulate->back();
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm_sub_ps(r[j], _mm_loadu_ps(&weights[i.pieceType + i.location][j * 4]));
        }
        removeAccumulate->pop_back();
    }

    for (int j = 0; j < num_chunks; j++) {
        _mm_storeu_ps(&features[j * 4], r[j]);
    }

    return nullptr;
}

#else

float* KPNNUE::updateAccumulator(Bitboard &b) {
    float **weights = layers[0]->getWeights();
    float *biases = layers[0]->getBiases();
    float *features = b.getFeatures();
    std::vector<Accumulator::Features> *addAccumulate = b.getAddFeatures();
    std::vector<Accumulator::Features> *removeAccumulate = b.getRemoveFeatures();

    if (b.getResetFlag()) {
        for (int j = 0; j < layers[0]->getNumOutputs(); j++) {
            features[j] = biases[j];
        }
        b.setResetFlag(false);
    }

    while (!addAccumulate->empty()) {
        Accumulator::Features i = addAccumulate->back();
        for (int j = 0; j < layers[0]->getNumOutputs(); j++) {
            features[j] += weights[i.pieceType + i.location][j];
        }
        addAccumulate->pop_back();
    }

    while (!removeAccumulate->empty()) {
        Accumulator::Features i = removeAccumulate->back();
        for (int j = 0; j < layers[0]->getNumOutputs(); j++) {
            features[j] -= weights[i.pieceType + i.location][j];
        }
        removeAccumulate->pop_back();
    }

    return nullptr;
}
#endif


#ifdef __AVX2__
// Handle update differently if training
float* KPNNUE::updateAccumulatorTrainer(Bitboard &b) {
    float **weights = layers[0]->getWeights();
    float *biases = layers[0]->getBiases();
    float *features = b.getFeatures();
    std::vector<Accumulator::Features> *addAccumulate = b.getAddFeatures();
    int numOutputs = layers[0]->getNumOutputs();
    int num_chunks = numOutputs / 8 + (numOutputs % 8 != 0);
    __m256 r[64];

    for (int j = 0; j < num_chunks; j++) {
        r[j] = _mm256_loadu_ps(&biases[j * 8]);
    }

    for (Accumulator::Features i : *addAccumulate) {
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm256_add_ps(r[j], _mm256_loadu_ps(&weights[i.pieceType + i.location][j * 8]));
        }
    }

    for (int j = 0; j < num_chunks; j++) {
        _mm256_storeu_ps(&features[j * 8], r[j]);
    }

    return nullptr;
}

#else

// Handle update differently if training
float* KPNNUE::updateAccumulatorTrainer(Bitboard &b) {
    float **weights = layers[0]->getWeights();
    float *biases = layers[0]->getBiases();
    float *features = b.getFeatures();
    std::vector<Accumulator::Features> *addAccumulate = b.getAddFeatures();

    for (int j = 0; j < layers[0]->getNumOutputs(); j++) {
        features[j] = biases[j];
    }

    for (Accumulator::Features i : *addAccumulate) {
        for (int j = 0; j < layers[0]->getNumOutputs(); j++) {
            features[j] += weights[i.pieceType + i.location][j];
        }
    }

    return nullptr;
}

#endif



int KPNNUE::forwardpropagate(float *input) {
    float buffer[1024];
    float *curr_output = buffer;
    float *curr_input = input;
    float *next_output;
    

    #ifdef NNUE_TRAINER
    float *forwards = layers[0]->getForwards();
    int nOuts = layers[0]->getNumOutputs();

    #ifdef __AVX2__
    int num_batches = nOuts / 8 + (nOuts % 8 != 0);
    for (int i = 0; i < num_batches; i++) {
        __m256 reg = _mm256_loadu_ps(&input[i * 8]);
        _mm256_storeu_ps(&forwards[i * 8], reg);
    }
    #else
    for (int i = 0; i < nOuts; i++) {
        forwards[i] = input[i];
    }
    #endif
    #endif

    next_output = layers[0]->ClippedRelu(curr_output, curr_input); // in 8 out 8
    curr_input = curr_output;
    curr_output = next_output;
    
    for (int k = 1; k < size - 1; k++) {
        next_output = layers[k]->linear(curr_output, curr_input); // in 256 out 8
        curr_input = curr_output;
        curr_output = next_output;

        next_output = layers[k]->ClippedRelu(curr_output, curr_input); // in 8 out 8
        curr_input = curr_output;
        curr_output = next_output;
        
    }

    next_output = layers[size - 1]->linear(curr_output, curr_input); // in 8 out 1
    return *curr_output;
    
}



void KPNNUE::backpropagate(Bitboard &board, int16_t Y, float ***grad, float **bias) {
    
    float **y = new float*[size];
    for (int i = 0; i < size; i++) {
        int nOutputs = layers[i]->getNumOutputs();
        int nOutputsPadded = nOutputs + (8 - (nOutputs % 8));
        y[i] = new float[nOutputsPadded]();
    }

    for (int i = size - 1; i >= 0; i--) {
        
        Layer *layer = layers[i];
        const int nOutputs = layer->getNumOutputs();
        const int nInputs = layer->getNumInputs();
        int nOutputsPadded = nOutputs + (8 - (nOutputs % 8));
        int num_chunk_out = nOutputs / 8 + (nOutputs % 8 != 0);
        
        float *dAdZ = new float[nOutputsPadded];
        
        if (i == size - 1) {
            y[i][0] = layer->DMeanSquaredError(Y);
            dAdZ[0] = layer->DSigmoid();
        }
        else {
            layer->DRelu(&dAdZ);
        }
        
        if (i != 0) {
            float *dZdW = layers[i - 1]->getActivations();
            float **dZdA = layer->getWeights();

            for (int j = 0; j < nInputs; j++) {
                for (int k = 0; k < nOutputs; k++) {
                    grad[i][j][k] += dZdW[j] * dAdZ[k] * y[i][k];
                    y[i - 1][j] += dZdA[k][j] * dAdZ[k] * y[i][k];
                }
            }
        }
        else {
            std::vector<Accumulator::Features> *addFeatures = board.getAddFeatures();
            while (!addFeatures->empty()) {
                Accumulator::Features j = addFeatures->back();  
                int index = j.pieceType + j.location;

                #ifdef __AVX2__
                for (int k = 0; k < num_chunk_out; k++) {
                    __m256 dAdZv = _mm256_loadu_ps(&dAdZ[k * 8]);
                    __m256 yv = _mm256_loadu_ps(&y[i][k * 8]);
                    __m256 gv = _mm256_loadu_ps(&grad[i][index][k * 8]);

                    gv = _mm256_add_ps(gv, _mm256_mul_ps(dAdZv, yv));
                    _mm256_storeu_ps(&grad[i][index][k * 8], gv);
                }
                
                #else
                for (int k = 0; k < nOutputs; k++) {
                    grad[i][index][k] += dAdZ[k] * y[i][k];
                }
                #endif

                addFeatures->pop_back();
            }

        }

        #ifdef __AVX2__
        for (int k = 0; k < num_chunk_out; k++) {
            __m256 b = _mm256_loadu_ps(&bias[i][k * 8]);
            b = _mm256_add_ps(b, _mm256_mul_ps(_mm256_loadu_ps(&dAdZ[k * 8]), _mm256_loadu_ps(&y[i][k * 8])));
            _mm256_storeu_ps(&bias[i][k * 8], b);
        }
        #else
        for (int k = 0; k < nOutputs; k++) {
            bias[i][k] += dAdZ[k] * y[i][k];
        }
        #endif
        
        delete [] y[i];
        delete [] dAdZ;

    }
    

    delete [] y;
}



void KPNNUE::updateWeights(float ***grad, float **bias, float lr, float beta1, float beta2, int batch) {
    for (int i = 0; i < size; i++) {
        layers[i]->updateWeights(grad[i], bias[i], lr, beta1, beta2, batchSize, batch);
    }
}



int KPNNUE::getPhase(Bitboard &board) {
    int phase = TOTALPHASE;
    phase -= (board.pieceCount[0] + board.pieceCount[1]) * PAWNPHASE;
    phase -= (board.pieceCount[2] + board.pieceCount[3]) * KNIGHTPHASE;
    phase -= (board.pieceCount[4] + board.pieceCount[5]) * BISHOPPHASE;
    phase -= (board.pieceCount[6] + board.pieceCount[7]) * ROOKPHASE;
    phase -= (board.pieceCount[8] + board.pieceCount[9]) * QUEENPHASE;
    return phase;
}




void KPNNUE::setupBoardFen(Bitboard &board, std::string fen, float *output) {
    board.setPosFen(fen);

    for (int i = 0; i < 12; i++) {
        uint64_t piece = board.pieces[i];
        while (piece) {
            output[64 * i + bitScan(piece)] = 1.0;
            piece &= piece - 1;
        }
    }
    
}



void KPNNUE::setupBoardFloat(Bitboard &board, float *output) {

    for (int i = 0; i < 12; i++) {
        uint64_t piece = board.pieces[i];
        while (piece) {
            output[64 * i + bitScan(piece)] = 1.0;
            piece &= piece - 1;
        }
    }

}


int KPNNUE::evaluate(std::string fen, Bitboard &board) {
    board.setPosFen(fen);
    updateAccumulator(board);
    return forwardpropagate(board.getFeatures());
}


int KPNNUE::evaluate(Bitboard &board) {
    updateAccumulator(board);
    return forwardpropagate(board.getFeatures());
}



float*** KPNNUE::createGradientWeights() {
    float ***grad = new float**[size];

    for (int i = 0; i < size; i++) {
        int inputs = layers[i]->getNumInputs();
        int outputs = layers[i]->getNumOutputs();

        grad[i] = new float*[inputs];
        for (int j = 0; j < inputs; j++) {
            grad[i][j] = new float[outputs]{0};
        }

    }
    return grad;
}



float** KPNNUE::createGradientBias() {
    float **bias = new float*[size];

    for (int i = 0; i < size; i++) {
        bias[i] = new float[layers[i]->getNumOutputs()]{0};
    }

    return bias;
}



void KPNNUE::deleteGradientWeights(float*** grad) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < layers[i]->getNumInputs(); j++) {
            delete [] grad[i][j];
        }
        delete [] grad[i];
    }

    delete [] grad;
}



void KPNNUE::deleteGradientBias(float** bias) {
    for (int i = 0; i < size; i++) {
        delete [] bias[i];
    }

    delete [] bias;
}



int* createIndexArray(int dataSize) {
    int *ret = new int[dataSize];
    for (int i = 0; i < dataSize; i++) {
        ret[i] = i;
    }
    return ret;
}



void KPNNUE::trainNetwork
(
    int dataSize, 
    Bitboard &board, 
    std::string *fens, 
    int16_t *expected, 
    std::string fileName, 
    int epochs, 
    int bs, 
    double lr
) {
    int validateSize = 0;
    int rseed = 72828000;
    int trainSize = dataSize - validateSize;
    batchSize = bs;

    std::mt19937 g(rseed);
    int *indexarr = createIndexArray(dataSize);

    double err_train = 0.0;
    double err_validate = 0.0;

    for (int i = validateSize; i < dataSize; i++) {
        board.setPosFen(fens[i]);
        updateAccumulator(board);
        forwardpropagate(board.getFeatures());
        err_train += layers[size - 1]->MeanSquaredError(expected[i]);
    } 

    std::cout << "Loss_train: " << err_train / trainSize << std::endl;
    std::cout << "Loss_validate: " << err_validate / validateSize << std::endl << std::endl;

    for (int epoch = init_epoch + 1; epoch < epochs + 1; epoch++) {

        std::shuffle(&indexarr[0], &indexarr[dataSize], g);
        err_train = 0.0;
        err_validate = 0.0;

        std::cout << "Epoch: " << epoch << ", lr: " << lr << std::endl;
        for (int batch = 0; batch < (trainSize / batchSize) + 1; batch++) {
            float ***grad = createGradientWeights();
            float **bias = createGradientBias();

            if (batch % (((trainSize / batchSize) / 20) + 1) == 0) {
                std::cout << "Batch [" << batch << " / " << (trainSize / batchSize) + 1 << "]  -  " << batch * 100 / ((trainSize / batchSize) + 1)<< "%" << std::endl;
            }
            
            int start = validateSize + batch * batchSize;
            int end = std::min(validateSize + (batch + 1) * batchSize, dataSize);

            for (int i = start; i < end; i++) {
                int index = indexarr[i];
                board.setPosFen(fens[index]);
                updateAccumulatorTrainer(board);
                forwardpropagate(board.getFeatures());

                backpropagate(board, expected[index], grad, bias);
                err_train += layers[size - 1]->MeanSquaredError(expected[index]);
            } 

            updateWeights(grad, bias, lr, 0.9, 0.999, batch + epoch * batchSize);

            deleteGradientWeights(grad);
            deleteGradientBias(bias);

        }

        std::cout << "test: ";
        for (int i = 0; i < 10; i++) {
            board.setPosFen(fens[i]);
            int tester = evaluate(board);
            std::cout << tester << " ";
        }
        std::cout << std::endl;
        std::cout << "Loss_train: " << err_train / trainSize << std::endl;
        std::cout << "Loss_validate: " << err_validate / validateSize << std::endl << std::endl;

        init_epoch = epoch;
        writeToBinary(fileName + "_" + std::to_string(epoch) + ".bin");
           
    }

    delete [] indexarr;
    
}



double KPNNUE::bulkLoss(int dataSize, Bitboard &board, std::string *fens, int16_t *expected) {
    double err = 0.0;

    for (int i = 0; i < dataSize; i++) {
        board.setPosFen(fens[i]);
        updateAccumulator(board);
        forwardpropagate(board.getFeatures());
        err += layers[size - 1]->MeanSquaredError(expected[i]);
    } 

    return err / dataSize;
}



void KPNNUE::writeToBinary(std::string fileName) {

    std::fstream myFile(fileName, std::ios::out | std::ios::binary);
    myFile.write(reinterpret_cast<const char *>(&init_epoch), sizeof(init_epoch));
    myFile.write(reinterpret_cast<const char *>(&batchSize), sizeof(batchSize));
    myFile.write(reinterpret_cast<const char *>(&size), sizeof(size));

    for (int i = 0; i < size; i++) {
        layers[i]->writeToBinary(myFile);
    }

    myFile.close();
}



void KPNNUE::readFromBinary(std::string fileName) {
    std::fstream myFile;
    myFile.open(fileName, std::ios::in | std::ios::binary);
    myFile.read(reinterpret_cast<char *>(&init_epoch), sizeof(init_epoch));
    myFile.read(reinterpret_cast<char *>(&batchSize), sizeof(batchSize));
    myFile.read(reinterpret_cast<char *>(&size), sizeof(size));
    
    layers = new Layer*[size];
    for (int i = 0; i < size; i++) {
        layers[i] = new Layer(myFile);
    }

    myFile.close();
}
