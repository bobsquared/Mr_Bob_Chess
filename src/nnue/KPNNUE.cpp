#include "KPNNUE.h"


KPNNUE::KPNNUE(std::string fileName) {
    readFromBinary(fileName);
}


KPNNUE::KPNNUE(int networkSize, int *sizes) {
    init_epoch = 0;
    size = networkSize - 1;

    layers = new Layer*[networkSize - 1];
    for (int i = 0; i < networkSize - 1; i++) {
        layers[i] = new Layer(sizes[i], sizes[i + 1]);
    }
}



KPNNUE::~KPNNUE() {

    for (int i = 0; i < size; i++) {
        delete layers[i];
    }
    delete [] layers;

}



double KPNNUE::nnue_evaluate(Bitboard &board) {
    

    // |***************Want to replace with own way to parse ***********|
    // for (int i = 0; i < M; i++) {
    //     input[i] = accumulator[stm][i];
    //     input[M + i] = accumulator[!stm][i];
    // }
    double input[256] = {0.0};

    for (int i = 0; i < 64; i++) {
        input[i] = ((1 << i) & board.pieces[0]) != 0;
        input[64 + i] = ((1 << i) & board.pieces[1]) != 0;
        input[128 + i] = ((1 << i) & board.pieces[10]) != 0;
        input[192 + i] = ((1 << i) & board.pieces[11]) != 0;
    }



    double buffer[1024];
    double *curr_output = buffer;
    double *curr_input = input;
    double *next_output;

    for (int i = 0; i < size - 1; i++) {
        // Evaluate one layer and move both input and output forward.
        // Last output becomes the next input.
        next_output = layers[i]->linear(curr_output, input); // in 256 out 16
        curr_input = curr_output;
        curr_output = next_output;

        next_output = layers[i]->Relu(curr_output, curr_input); // in 16 out 16
        curr_input = curr_output;
        curr_output = next_output;
    }

    next_output = layers[size - 1]->linear(curr_output, curr_input); // in 8 out 1

    // We're done. The last layer should have put 1 value out under *curr_output.
    return *curr_output;
}



double KPNNUE::forwardpropagate(int phase, double *input) {
    double buffer[1024];
    double *curr_output = buffer;
    double *curr_input = input;
    double *next_output;
    
    for (int k = 0; k < size - 1; k++) {
        // Evaluate one layer and move both input and output forward.
        // Last output becomes the next input.
        next_output = layers[k]->linear(curr_output, curr_input); // in 256 out 8
        curr_input = curr_output;
        curr_output = next_output;

        next_output = layers[k]->Relu(curr_output, curr_input); // in 8 out 8
        curr_input = curr_output;
        curr_output = next_output;
        
    }

    next_output = layers[size - 1]->linear(curr_output, curr_input); // in 8 out 1
    int phaseTotal = (phase * 256 + (TOTALPHASE / 2)) / TOTALPHASE;

    return ((curr_output[0] * (256 - phaseTotal)) + (curr_output[1] * phaseTotal)) / 256;
}



void KPNNUE::backpropagate(int phase, double *X, int16_t Y, double ***grad, double **bias, double *loss = nullptr) {
    
    double **y = new double*[size];
    for (int i = 0; i < size; i++) {
        y[i] = new double[layers[i]->getNumOutputs()]();
    }

    for (int i = size - 1; i >= 0; i--) {
        
        Layer *layer = layers[i];
        const int nOutputs = layer->getNumOutputs();
        
        double *dAdZ = new double[nOutputs];
        
        if (loss != nullptr && i == size - 1) {
            y[i][0] = loss[0];
            y[i][1] = loss[1];
            dAdZ[0] = 1.0;
            dAdZ[1] = 1.0;
        }
        else if (i == size - 1) {
            y[i][0] = layer->DMeanSquaredError(0, Y);
            dAdZ[0] = layer->DSigmoid(0);
        }
        else {
            layer->DRelu(&dAdZ);
        }
        
        if (i != 0) {
            double *dZdW = layers[i - 1]->getActivations();
            double **dZdA = layer->getWeights();

            for (int j = 0; j < layer->getNumInputs(); j++) {
                for (int k = 0; k < nOutputs; k++) {
                    grad[i][j][k] += dZdW[j] * dAdZ[k] * y[i][k];
                    bias[i][k] += dAdZ[k] * y[i][k];
                    y[i - 1][j] += dZdA[j][k] * dAdZ[k] * y[i][k];
                }
            }  
        }
        else {
            for (int j = 0; j < layer->getNumInputs(); j++) {
                for (int k = 0; k < nOutputs; k++) {
                    grad[i][j][k] += X[j] * dAdZ[k] * y[i][k];
                    bias[i][k] += dAdZ[k] * y[i][k];
                }
               
            }
        }
        
        delete [] y[i];
        delete [] dAdZ;

    }
    

    delete [] y;
}



void KPNNUE::updateWeights(double ***grad, double **bias, double lr, double beta1, double beta2, int batch) {
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




void KPNNUE::setupBoardFen(Bitboard &board, std::string fen, double *output) {
    board.setPosFen(fen);

    for (int i = 0; i < 12; i++) {
        uint64_t piece = board.pieces[i];
        while (piece) {
            output[64 * i + bitScan(piece)] = 1.0;
            piece &= piece - 1;
        }
    }
    
}



void KPNNUE::setupBoardDouble(Bitboard &board, double *output) {

    for (int i = 0; i < 12; i++) {
        uint64_t piece = board.pieces[i];
        while (piece) {
            output[64 * i + bitScan(piece)] = 1.0;
            piece &= piece - 1;
        }
    }

}


int KPNNUE::evaluate(std::string fen, Bitboard &board) {
    double input[768] = {0.0};
    setupBoardFen(board, fen, input);
    
    int ret = forwardpropagate(getPhase(board), input);
    double *temp = layers[size - 1]->getForwards();
    return temp[0];
}


int KPNNUE::evaluate(Bitboard &board) {
    double input[768] = {0.0};
    setupBoardDouble(board, input);
    
    int ret = forwardpropagate(getPhase(board), input);
    double *temp = layers[size - 1]->getForwards();
    return temp[0];
}



double*** KPNNUE::createGradientWeights() {
    double ***grad = new double**[size];

    for (int i = 0; i < size; i++) {
        int inputs = layers[i]->getNumInputs();
        int outputs = layers[i]->getNumOutputs();

        grad[i] = new double*[inputs];
        for (int j = 0; j < inputs; j++) {
            grad[i][j] = new double[outputs]{0};
        }

    }
    return grad;
}



double** KPNNUE::createGradientBias() {
    double **bias = new double*[size];

    for (int i = 0; i < size; i++) {
        bias[i] = new double[layers[i]->getNumOutputs()]{0};
    }

    return bias;
}



void KPNNUE::deleteGradientWeights(double*** grad) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < layers[i]->getNumInputs(); j++) {
            delete [] grad[i][j];
        }
        delete [] grad[i];
    }

    delete [] grad;
}



void KPNNUE::deleteGradientBias(double** bias) {
    for (int i = 0; i < size; i++) {
        delete [] bias[i];
    }

    delete [] bias;
}



void KPNNUE::trainNetwork(int dataSize, Bitboard &board, std::string *fens, int16_t *expected, std::string fileName) {
    int validateSize = 0;
    int trainSize = dataSize - validateSize;
    batchSize = 8192;

    double err_train = 0.0;
    double err_validate = 0.0;

    for (int i = validateSize; i < dataSize; i++) {
        double input[768] = {0.0};
        setupBoardFen(board, fens[i], input);

        int phase = getPhase(board);
        forwardpropagate(phase, input);

        int phaseTotal = (phase * 256 + (TOTALPHASE / 2)) / TOTALPHASE;
        err_train += layers[size - 1]->MeanSquaredError(phaseTotal, expected[i]);
    } 

    for (int i = 0; i < validateSize; i++) {
        double input[768] = {0.0};
        setupBoardFen(board, fens[i], input);

        int phase = getPhase(board);
        forwardpropagate(phase, input);

        int phaseTotal = (phase * 256 + (TOTALPHASE / 2)) / TOTALPHASE;
        err_validate += layers[size - 1]->MeanSquaredError(phaseTotal, expected[i]);
    } 

    std::cout << "Loss_train: " << err_train / trainSize << std::endl;
    std::cout << "Loss_validate: " << err_validate / validateSize << std::endl << std::endl;

    for (int epoch = init_epoch + 1; epoch < 100000; epoch++) {

        err_train = 0.0;
        err_validate = 0.0;

        std::cout << "Epoch: " << epoch << std::endl;
        for (int batch = 0; batch < (trainSize / batchSize) + 1; batch++) {
            double ***grad = createGradientWeights();
            double **bias = createGradientBias();

            if (batch % (((trainSize / batchSize) / 20) + 1) == 0) {
                std::cout << "Batch [" << batch << " / " << (trainSize / batchSize) + 1 << "]  -  " << batch * 100 / ((trainSize / batchSize) + 1)<< "%" << std::endl;
            }
            
            int start = validateSize + batch * batchSize;
            int end = std::min(validateSize + (batch + 1) * batchSize, dataSize);

            for (int i = start; i < end; i++) {
                double input[768] = {0.0};
                setupBoardFen(board, fens[i], input);

                int phase = getPhase(board);

                forwardpropagate(phase, input);
                backpropagate(phase, input, expected[i], grad, bias);

                int phaseTotal = (phase * 256 + (TOTALPHASE / 2)) / TOTALPHASE;
                err_train += layers[size - 1]->MeanSquaredError(phaseTotal, expected[i]);
            } 

            updateWeights(grad, bias, 0.001, 0.9, 0.999, batch + epoch * batchSize);

            deleteGradientWeights(grad);
            deleteGradientBias(bias);

        }

        for (int i = 0; i < validateSize; i++) {
            double input[768] = {0.0};
            setupBoardFen(board, fens[i], input);

            int phase = getPhase(board);
            forwardpropagate(phase, input);

            int phaseTotal = (phase * 256 + (TOTALPHASE / 2)) / TOTALPHASE;
            err_validate += layers[size - 1]->MeanSquaredError(phaseTotal, expected[i]);
        } 

        std::cout << "test: ";
        for (int i = 0; i < 10; i++) {
            double input[768] = {0.0};
            setupBoardFen(board, fens[i], input);
            double tester = forwardpropagate(getPhase(board), input);

            std::cout << tester << " ";
        }
        std::cout << std::endl;
        std::cout << "Loss_train: " << err_train / trainSize << std::endl;
        std::cout << "Loss_validate: " << err_validate / validateSize << std::endl << std::endl;

        init_epoch = epoch;
        writeToBinary(fileName + "_" + std::to_string(epoch) + ".bin");
           
    }
    
 
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
