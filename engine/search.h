#include <string>
#include <iostream>
#include "search.cpp"




int minimaxR(bool useMax, Bitboard &bitboard, int depth);
std::string minimaxRoot(bool useMax, Bitboard &bitboard, int depth);

int alphabetaR(bool useMax, Bitboard &bitboard, int depth, int alpha, int beta);
std::string alphabetaRoot(bool useMax, Bitboard &bitboard, int depth, int alpha, int beta);
