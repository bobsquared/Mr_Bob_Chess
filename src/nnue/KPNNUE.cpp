

#include "KPNNUE.h"



KPNNUE::KPNNUE() {  
    layers = nullptr;
}

KPNNUE::KPNNUE(const unsigned char* defaultNetwork, unsigned int bsize) {  
    std::istringstream myStream(std::string(reinterpret_cast<const char*>(defaultNetwork), bsize), std::ios::binary);
    readFromBinary(myStream);
}


KPNNUE::KPNNUE(int networkSize, int *sizes) {
    
    init_epoch = 0;
    size = networkSize;

    layers = new Layer*[size];
    for (int i = 0; i < size; i++) {
        int nOut = sizes[i + 1];
        if (i < 2) {
            nOut = sizes[2] / 2;
        }
        layers[i] = new Layer(sizes[i], nOut, (i == 0 || i == 1));
    }

}



KPNNUE::~KPNNUE() {

    for (int i = 0; i < size; i++) {
        delete layers[i];
    }
    delete [] layers;

}



void KPNNUE::setNetwork(std::string fileName) {  

    if (layers != nullptr) {
        for (int i = 0; i < size; i++) {
            delete layers[i];
        }
        delete [] layers;
        layers = nullptr;
    }

    std::fstream myFile;
    myFile.open(fileName, std::ios::in | std::ios::binary);
    if (myFile.good()) {
        readFromBinary(myFile);
        std::cout << "NNUE loaded: " << fileName << std::endl;
    }
    else {
        std::cout << "Failed to load NNUE: " << fileName << std::endl;
    }

    myFile.close();
    
}



#ifdef __AVX2__
float* KPNNUE::updateAccumulator(Bitboard &b) {
    float **whiteWeights = layers[0]->getWeights();
    float *whiteBiases = layers[0]->getBiases();
    float **blackWeights = layers[1]->getWeights();
    float *blackBiases = layers[1]->getBiases();

    float *whiteFeatures = b.getFeatures(false);
    float *blackFeatures = b.getFeatures(true);
    std::vector<int> *addWhiteAccumulate = b.getAddFeatures(false);
    std::vector<int> *addBlackAccumulate = b.getAddFeatures(true);
    std::vector<int> *removeWhiteAccumulate = b.getRemoveFeatures(false);
    std::vector<int> *removeBlackAccumulate = b.getRemoveFeatures(true);

    int numOutputs = layers[0]->getNumOutputs();
    int num_chunks = numOutputs / 8 + (numOutputs % 8 != 0);
    __m256 r[64];

    // White
    if (b.getResetFlag()) {
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm256_loadu_ps(&whiteBiases[j * 8]);
        }
    }
    else {
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm256_loadu_ps(&whiteFeatures[j * 8]);
        }
    }

    while (!addWhiteAccumulate->empty()) {
        int i = addWhiteAccumulate->back();
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm256_add_ps(r[j], _mm256_loadu_ps(&whiteWeights[i][j * 8]));
        }
        addWhiteAccumulate->pop_back();
        b.resetAccFreqIndex(i, true);
    }

    while (!removeWhiteAccumulate->empty()) {
        int i = removeWhiteAccumulate->back();
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm256_sub_ps(r[j], _mm256_loadu_ps(&whiteWeights[i][j * 8]));
        }
        removeWhiteAccumulate->pop_back();
        b.resetAccFreqIndex(i, false);
    }

    for (int j = 0; j < num_chunks; j++) {
        _mm256_storeu_ps(&whiteFeatures[j * 8], r[j]);
    }

    // Black
    numOutputs = layers[1]->getNumOutputs();
    num_chunks = numOutputs / 8 + (numOutputs % 8 != 0);

    if (b.getResetFlag()) {
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm256_loadu_ps(&blackBiases[j * 8]);
        }
        b.setResetFlag(false);
    }
    else {
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm256_loadu_ps(&blackFeatures[j * 8]);
        }
    }

    while (!addBlackAccumulate->empty()) {
        int i = addBlackAccumulate->back();
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm256_add_ps(r[j], _mm256_loadu_ps(&blackWeights[i][j * 8]));
        }
        addBlackAccumulate->pop_back();
    }

    while (!removeBlackAccumulate->empty()) {
        int i = removeBlackAccumulate->back();
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm256_sub_ps(r[j], _mm256_loadu_ps(&blackWeights[i][j * 8]));
        }
        removeBlackAccumulate->pop_back();
    }

    for (int j = 0; j < num_chunks; j++) {
        _mm256_storeu_ps(&blackFeatures[j * 8], r[j]);
    }

    return nullptr;
}

#elif defined(__SSE2__)
float* KPNNUE::updateAccumulator(Bitboard &b) {
    float **whiteWeights = layers[0]->getWeights();
    float *whiteBiases = layers[0]->getBiases();
    float **blackWeights = layers[1]->getWeights();
    float *blackBiases = layers[1]->getBiases();

    float *whiteFeatures = b.getFeatures(false);
    float *blackFeatures = b.getFeatures(true);
    std::vector<int> *addWhiteAccumulate = b.getAddFeatures(false);
    std::vector<int> *addBlackAccumulate = b.getAddFeatures(true);
    std::vector<int> *removeWhiteAccumulate = b.getRemoveFeatures(false);
    std::vector<int> *removeBlackAccumulate = b.getRemoveFeatures(true);

    int numOutputs = layers[0]->getNumOutputs();
    int num_chunks = numOutputs / 4 + (numOutputs % 4 != 0);
    __m128 r[64];

    // White
    if (b.getResetFlag()) {
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm_loadu_ps(&whiteBiases[j * 4]);
        }
    }
    else {
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm_loadu_ps(&whiteFeatures[j * 4]);
        }
    }

    while (!addWhiteAccumulate->empty()) {
        int i = addWhiteAccumulate->back();
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm_add_ps(r[j], _mm_loadu_ps(&whiteWeights[i][j * 4]));
        }
        addWhiteAccumulate->pop_back();
    }

    while (!removeWhiteAccumulate->empty()) {
        int i = removeWhiteAccumulate->back();
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm_sub_ps(r[j], _mm_loadu_ps(&whiteWeights[i][j * 4]));
        }
        removeWhiteAccumulate->pop_back();
    }

    for (int j = 0; j < num_chunks; j++) {
        _mm_storeu_ps(&whiteFeatures[j * 4], r[j]);
    }

    numOutputs = layers[1]->getNumOutputs();
    num_chunks = numOutputs / 4 + (numOutputs % 4 != 0);

    // Black
    if (b.getResetFlag()) {
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm_loadu_ps(&blackBiases[j * 4]);
        }
        b.setResetFlag(false);
    }
    else {
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm_loadu_ps(&blackFeatures[j * 4]);
        }
    }

    while (!addBlackAccumulate->empty()) {
        int i = addBlackAccumulate->back();
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm_add_ps(r[j], _mm_loadu_ps(&blackWeights[i][j * 4]));
        }
        addBlackAccumulate->pop_back();
    }

    while (!removeBlackAccumulate->empty()) {
        int i = removeBlackAccumulate->back();
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm_sub_ps(r[j], _mm_loadu_ps(&blackWeights[i][j * 4]));
        }
        removeBlackAccumulate->pop_back();
    }

    for (int j = 0; j < num_chunks; j++) {
        _mm_storeu_ps(&blackFeatures[j * 4], r[j]);
    }

    return nullptr;
}

#else

float* KPNNUE::updateAccumulator(Bitboard &b) {

    float **whiteWeights = layers[0]->getWeights();
    float *whiteBiases = layers[0]->getBiases();
    float **blackWeights = layers[1]->getWeights();
    float *blackBiases = layers[1]->getBiases();

    float *whiteFeatures = b.getFeatures(false);
    float *blackFeatures = b.getFeatures(true);
    std::vector<int> *addWhiteAccumulate = b.getAddFeatures(false);
    std::vector<int> *addBlackAccumulate = b.getAddFeatures(true);
    std::vector<int> *removeWhiteAccumulate = b.getRemoveFeatures(false);
    std::vector<int> *removeBlackAccumulate = b.getRemoveFeatures(true);

    if (b.getResetFlag()) {
        // Reset White
        for (int j = 0; j < layers[0]->getNumOutputs(); j++) {
            whiteFeatures[j] = whiteBiases[j];
        }

        // Reset Black
        for (int j = 0; j < layers[1]->getNumOutputs(); j++) {
            blackFeatures[j] = blackBiases[j];
        }

        b.setResetFlag(false);
    }

    // For White
    while (!addWhiteAccumulate->empty()) {
        int i = addWhiteAccumulate->back();
        for (int j = 0; j < layers[0]->getNumOutputs(); j++) {
            whiteFeatures[j] += whiteWeights[i][j];
        }
        addWhiteAccumulate->pop_back();
    }

    while (!removeWhiteAccumulate->empty()) {
        int i = removeWhiteAccumulate->back();
        for (int j = 0; j < layers[0]->getNumOutputs(); j++) {
            whiteFeatures[j] -= whiteWeights[i][j];
        }
        removeWhiteAccumulate->pop_back();
    }

    // For Black
    while (!addBlackAccumulate->empty()) {
        int i = addBlackAccumulate->back();
        for (int j = 0; j < layers[1]->getNumOutputs(); j++) {
            whiteFeatures[j] += whiteWeights[i][j];
        }
        addBlackAccumulate->pop_back();
    }

    while (!removeBlackAccumulate->empty()) {
        int i = removeBlackAccumulate->back();
        for (int j = 0; j < layers[1]->getNumOutputs(); j++) {
            blackFeatures[j] -= blackWeights[i][j];
        }
        removeBlackAccumulate->pop_back();
    }

    return nullptr;
}
#endif


#ifdef __AVX2__
// Handle update differently if training
float* KPNNUE::updateAccumulatorTrainer(Bitboard &b) {
    float **whiteWeights = layers[0]->getWeights();
    float *whiteBiases = layers[0]->getBiases();
    float **blackWeights = layers[1]->getWeights();
    float *blackBiases = layers[1]->getBiases();

    float *whiteFeatures = b.getFeatures(false);
    float *blackFeatures = b.getFeatures(true);
    std::vector<int> *addWhiteAccumulate = b.getAddFeatures(false);
    std::vector<int> *addBlackAccumulate = b.getAddFeatures(true);

    // White Update
    int numOutputs = layers[0]->getNumOutputs();
    int num_chunks = numOutputs / 8 + (numOutputs % 8 != 0);
    __m256 r[64];
    for (int j = 0; j < num_chunks; j++) {
        r[j] = _mm256_loadu_ps(&whiteBiases[j * 8]);
    }
    for (int i : *addWhiteAccumulate) {
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm256_add_ps(r[j], _mm256_loadu_ps(&whiteWeights[i][j * 8]));
        }
    }
    for (int j = 0; j < num_chunks; j++) {
        _mm256_storeu_ps(&whiteFeatures[j * 8], r[j]);
    }


    // Black Update
    numOutputs = layers[1]->getNumOutputs();
    num_chunks = numOutputs / 8 + (numOutputs % 8 != 0);

    for (int j = 0; j < num_chunks; j++) {
        r[j] = _mm256_loadu_ps(&blackBiases[j * 8]);
    }
    for (int i : *addBlackAccumulate) {
        for (int j = 0; j < num_chunks; j++) {
            r[j] = _mm256_add_ps(r[j], _mm256_loadu_ps(&blackWeights[i][j * 8]));
        }
    }

    for (int j = 0; j < num_chunks; j++) {
        _mm256_storeu_ps(&blackFeatures[j * 8], r[j]);
    }
    return nullptr;
}

#else

// Handle update differently if training
float* KPNNUE::updateAccumulatorTrainer(Bitboard &b) {
    float **whiteWeights = layers[0]->getWeights();
    float *whiteBiases = layers[0]->getBiases();
    float **blackWeights = layers[1]->getWeights();
    float *blackBiases = layers[1]->getBiases();

    float *whiteFeatures = b.getFeatures(false);
    float *blackFeatures = b.getFeatures(true);
    std::vector<int> *addWhiteAccumulate = b.getAddFeatures(false);
    std::vector<int> *addBlackAccumulate = b.getAddFeatures(true);

    // White features update
    for (int j = 0; j < layers[0]->getNumOutputs(); j++) {
        whiteFeatures[j] = whiteBiases[j];
    }

    for (int i : *addWhiteAccumulate) {
        for (int j = 0; j < layers[0]->getNumOutputs(); j++) {
            whiteFeatures[j] += whiteWeights[i][j];
        }
    }

    // Black features update
    for (int j = 0; j < layers[1]->getNumOutputs(); j++) {
        blackFeatures[j] = blackBiases[j];
    }

    for (int i : *addBlackAccumulate) {
        for (int j = 0; j < layers[1]->getNumOutputs(); j++) {
            blackFeatures[j] += blackWeights[i][j];
        }
    }

    return nullptr;
}

#endif



int KPNNUE::forwardpropagate(float *whiteInput, float *blackInput, bool toMove) {
    float *curr_output = buffer;
    float *curr_input;
    float *next_output;


    #ifdef NNUE_TRAINER
    float *whiteForwards = layers[0]->getForwards();
    int nWhiteOuts = layers[0]->getNumOutputs();

    float *blackForwards = layers[1]->getForwards();
    int nBlackOuts = layers[1]->getNumOutputs();

    #ifdef __AVX2__
    int num_batches = nWhiteOuts / 8 + (nWhiteOuts % 8 != 0);
    for (int i = 0; i < num_batches; i++) {
        __m256 reg = _mm256_loadu_ps(&whiteInput[i * 8]);
        _mm256_storeu_ps(&whiteForwards[i * 8], reg);
    }

    num_batches = nBlackOuts / 8 + (nBlackOuts % 8 != 0);
    for (int i = 0; i < num_batches; i++) {
        __m256 reg = _mm256_loadu_ps(&blackInput[i * 8]);
        _mm256_storeu_ps(&blackForwards[i * 8], reg);
    }
    #else
    for (int i = 0; i < nWhiteOuts; i++) {
        whiteForwards[i] = whiteInput[i];
    }

    for (int i = 0; i < nBlackOuts; i++) {
        blackForwards[i] = blackInput[i];
    }
    #endif
    #endif

    if (toMove) {
        curr_input = blackInput;
        next_output = layers[1]->relu(curr_output, curr_input); // in 384 out 256
        curr_output = next_output;

        curr_input = whiteInput;
        next_output = layers[0]->relu(curr_output, curr_input); // in 384 out 256
    }
    else {
        curr_input = whiteInput;
        next_output = layers[0]->relu(curr_output, curr_input); // in 384 out 256
        curr_output = next_output;

        curr_input = blackInput;
        next_output = layers[1]->relu(curr_output, curr_input); // in 384 out 256
    }

    curr_input = buffer;
    curr_output = buffer;
    
    for (int k = 2; k < size - 1; k++) {
        next_output = layers[k]->linear(curr_output, curr_input); // in 256 out 8
        curr_input = curr_output;
        curr_output = next_output;

        next_output = layers[k]->relu(curr_output, curr_input); // in 8 out 8
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
        
        if (i > 2) {
            float *dZdW = layers[i - 1]->getActivations();
            float **dZdA = layer->getWeights();

            for (int j = 0; j < nInputs; j++) {
                for (int k = 0; k < nOutputs; k++) {
                    float dAdZy = dAdZ[k] * y[i][k];
                    grad[i][j][k] += dZdW[j] * dAdZy;
                    y[i - 1][j] += dZdA[k][j] * dAdZy;
                }
            }
        }
        else if (i == 2) {
            float *dZdWWhite = layers[i - 2]->getActivations();
            float *dZdWBlack = layers[i - 1]->getActivations();
            float **dZdA = layer->getWeights();

            int nOutputsPrev = layers[i - 1]->getNumOutputs();

            if (board.toMove) {
                for (int j = 0; j < nOutputsPrev; j++) {
                    for (int k = 0; k < nOutputs; k++) {
                        float dAdZy = dAdZ[k] * y[i][k];
                        grad[i][j][k] += dZdWBlack[j] * dAdZy;
                        y[i - 1][j] += dZdA[k][j] * dAdZy;

                        grad[i][j + nOutputsPrev][k] += dZdWWhite[j] * dAdZy;
                        y[i - 2][j] += dZdA[k][j + nOutputsPrev] * dAdZy;
                    }
                }
            }
            else {
                for (int j = 0; j < nOutputsPrev; j++) {
                    for (int k = 0; k < nOutputs; k++) {
                        float dAdZy = dAdZ[k] * y[i][k];
                        grad[i][j][k] += dZdWWhite[j] * dAdZy;
                        y[i - 2][j] += dZdA[k][j] * dAdZy;

                        grad[i][j + nOutputsPrev][k] += dZdWBlack[j] * dAdZy;
                        y[i - 1][j] += dZdA[k][j + nOutputsPrev] * dAdZy;
                    }
                }
            }
            
        }
        else {
            std::vector<int> *addFeatures = board.getAddFeatures(i);
            while (!addFeatures->empty()) {
                int index = addFeatures->back();  

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
        
        delete [] dAdZ;

    }
    
    for (int i = size - 1; i >= 0; i--) {
        delete [] y[i];
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
    return forwardpropagate(board.getFeatures(false), board.getFeatures(true), board.toMove);
}


int KPNNUE::evaluate(Bitboard &board) {
    updateAccumulator(board);
    return forwardpropagate(board.getFeatures(false), board.getFeatures(true), board.toMove);
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


void KPNNUE::resetWeightsAndBias(float ***grad, float **bias) {
    for (int i = 0; i < size; i++) {
        int inputs = layers[i]->getNumInputs();
        int outputs = layers[i]->getNumOutputs();

        for (int j = 0; j < inputs; j++) {
            std::memset(grad[i][j], 0, outputs * sizeof(float));
        }

        std::memset(bias[i], 0, outputs * sizeof(float));
    }
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

    float ***grad = createGradientWeights();
    float **bias = createGradientBias();

    for (int i = validateSize; i < dataSize; i++) {
        board.setPosFen(fens[i]);
        if (board.toMove) {
            expected[i] = -expected[i];
        }
        updateAccumulator(board);
        forwardpropagate(board.getFeatures(false), board.getFeatures(true), board.toMove);
        err_train += layers[size - 1]->MeanSquaredError(expected[i]);
    } 

    std::cout << "Loss_train: " << err_train / trainSize << std::endl;
    std::cout << "Loss_validate: " << err_validate / validateSize << std::endl << std::endl;

    for (int epoch = init_epoch + 1; epoch < epochs + 1; epoch++) {

        int et = 0;
        std::shuffle(&indexarr[0], &indexarr[dataSize], g);
        err_train = 0.0;
        err_validate = 0.0;

        std::cout << "Epoch: " << epoch << ", lr: " << lr << std::endl;
        for (int batch = 0; batch < (trainSize / batchSize) + 1; batch++) {
            resetWeightsAndBias(grad, bias);

            if (batch % (((trainSize / batchSize) / 20) + 1) == 0) {
                std::cout << "Batch [" << batch << " / " << (trainSize / batchSize) + 1 << "]  -  " << batch * 100 / ((trainSize / batchSize) + 1) << "%" << " - loss: " << err_train / et  << std::endl;
            }
            
            int start = validateSize + batch * batchSize;
            int end = std::min(validateSize + (batch + 1) * batchSize, dataSize);

            for (int i = start; i < end; i++) {
                int index = indexarr[i];
                board.setPosFen(fens[index]);
                updateAccumulatorTrainer(board);
                forwardpropagate(board.getFeatures(false), board.getFeatures(true), board.toMove);
                backpropagate(board, expected[index], grad, bias);
                err_train += layers[size - 1]->MeanSquaredError(expected[index]);
                et++;
            } 

            updateWeights(grad, bias, lr, 0.95, 0.999, batch + epoch * batchSize);

            

        }

        std::cout << "test: ";
        for (int i = 0; i < 10; i++) {
            board.setPosFen(fens[i]);
            int tester = evaluate(board);
            std::cout << tester << " (" << expected[i] << "), ";
        }
        std::cout << std::endl;
        std::cout << "Loss_train: " << err_train / trainSize << std::endl;
        std::cout << "Loss_validate: " << err_validate / validateSize << std::endl << std::endl;

        init_epoch = epoch;
        writeToBinary(fileName + "_" + std::to_string(epoch) + ".bin");
           
    }

    deleteGradientWeights(grad);
    deleteGradientBias(bias);

    delete [] indexarr;
    
}



double KPNNUE::bulkLoss(int dataSize, Bitboard &board, std::string *fens, int16_t *expected) {
    double err = 0.0;

    for (int i = 0; i < dataSize; i++) {
        board.setPosFen(fens[i]);
        updateAccumulator(board);
        forwardpropagate(board.getFeatures(false), board.getFeatures(true), board.toMove);
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



void KPNNUE::readFromBinary(std::istream &data) {
    data.read(reinterpret_cast<char *>(&init_epoch), sizeof(init_epoch));
    data.read(reinterpret_cast<char *>(&batchSize), sizeof(batchSize));
    data.read(reinterpret_cast<char *>(&size), sizeof(size));
    layers = new Layer*[size];
    for (int i = 0; i < size; i++) {
        layers[i] = new Layer(data);
    }

}