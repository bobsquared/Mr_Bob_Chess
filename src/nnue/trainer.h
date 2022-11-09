#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <ostream>
#include "../bitboard.h"
#include "../search.h"


class Trainer {

    
public:

    struct Position {
        std::string fen;
        float cp;

        Position() : fen(""), cp(0.0) {}
        Position(std::string fen, float cp) : fen(fen), cp(cp) {}

        friend std::ostream& operator<<(std::ostream& os, const Position& pos) {
            os << pos.fen << " =" << pos.cp;
            return os; 
        }
    };

    Trainer();
    void extractFile(std::string inputFile, std::string outputFile);
    void getFile(std::string name);
    void getFileSigmoid(std::string fileName);
    void train(KPNNUE &model, std::string fileName);

private:
    int dataSize;
    Bitboard board = Bitboard();

    std::string *fens;
    int16_t *cps;


};