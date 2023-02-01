#include "trainer.h"

Zobrist *zobrist = new Zobrist();
Magics *magics;

Trainer::Trainer() {
    InitColumnsMask();
    InitRowsMask();
    InitLateMoveArray();

    magics = new Magics();

    canPrintInfo = false;
    exit_thread_flag = false;
    dataSize = 0;
}


void Trainer::extractFile(std::string inputFile, std::string outputFile) {

    std::string fen;
    int count = 0;

    std::ifstream Myfile(inputFile);
    while (std::getline(Myfile, fen)) {
        count++;
    }

    Position *positions = new Position[count];

    Myfile.clear();
    Myfile.seekg(0, std::ios::beg);

    count = 0;
    while (std::getline(Myfile, fen)) {
        
        if (count % 1000 == 0) {
            std::cout <<  count << std::endl;
        }

        fen = fen.substr(0, fen.size() - 6);
        board.setPosFen(fen);

        exit_thread_flag = false;
        int eva = beginSearch(board, 6, INT_MAX, INT_MAX, 0, 0, 0, true);

        positions[count] = Position(fen, static_cast<float>(eva));
        count++;
    }

    Myfile.close();
    std::ofstream OutFile(outputFile);
    for (int i = 0; i < count; i++) {
        OutFile << positions[i] << std::endl;
    }
    OutFile.close();
}



void Trainer::getFile(std::string fileName) {

    std::string tempFen;
    std::string fen;
    std::string stringDelimiter = " =";

    float score;
    int count = 0;

    std::ifstream Myfile(fileName);
    while (std::getline(Myfile, tempFen)) {
        count++;
    }

    Myfile.clear();
    Myfile.seekg(0, std::ios::beg);

    fens = new std::string[count];
    cps = new int16_t[count];
    dataSize = count;

    count = 0;
    while (std::getline(Myfile, tempFen)) {

        if (count % (dataSize / 10) == 0) {
            std::cout << count << std::endl;
        }
        
        size_t delimiter = tempFen.find(stringDelimiter);
        fen = tempFen.substr(0, delimiter);
        score = std::stoi(tempFen.substr(delimiter + stringDelimiter.size(), tempFen.size() - delimiter - stringDelimiter.size()));
        score = static_cast<int16_t>(score);
        fens[count] = fen;
        cps[count] = score;


        count++;
    }

}




void Trainer::getFileSigmoid(std::string fileName) {

    std::string tempFen;
    std::string fen;
    std::string stringDelimiter = " [";

    float score;
    int count = 0;

    std::ifstream Myfile(fileName);
    while (std::getline(Myfile, tempFen)) {
        count++;
    }

    Myfile.clear();
    Myfile.seekg(0, std::ios::beg);

    fens = new std::string[count];
    cps = new int16_t[count];
    dataSize = count;

    count = 0;
    while (std::getline(Myfile, tempFen)) {

        if (count % (dataSize / 10) == 0) {
            std::cout << count << std::endl;
        }
        
        size_t delimiter = tempFen.find(stringDelimiter);
        fen = tempFen.substr(0, delimiter);
        score = std::stoi(tempFen.substr(delimiter + stringDelimiter.size(), 3));
        score = static_cast<int16_t>(score);
        fens[count] = fen;

        board.setPosFen(fen);
        cps[count] = score;


        count++;
    }

}



void Trainer::train(KPNNUE &model, std::string fileName, int epochs, int batchSize, double lr) {
    model.trainNetwork(dataSize, board, fens, cps, fileName, epochs, batchSize, lr);
}



void Trainer::createLossCSV(int nModels, std::string directoryName) {
    std::ofstream myfile;
    myfile.open ("loss.csv");
    myfile << "epoch,loss\n";

    for (int i = 1; i < nModels; i++) {
        std::cout << "Testing Model Number: " << i << std::endl;
        KPNNUE model = KPNNUE(directoryName + std::to_string(i) +  ".bin");
        double loss = model.bulkLoss(dataSize, board, fens, cps);
        
        myfile << i << "," << loss << "\n";
    }

    myfile.close();
    
}


    