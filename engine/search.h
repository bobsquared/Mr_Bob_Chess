#include <string>
#include <iostream>
#include "search.cpp"



int minimaxR(bool useMax, Bitboard &bitboard, int depth);
std::string minimaxRoot(bool useMax, Bitboard &bitboard, int depth);

int alphabetaR(bool useMax, Bitboard &bitboard, int depth, int alpha, int beta);
ReturnInfo alphabetaRoot(bool useMax, Bitboard &bitboard, int depth, int alpha, int beta);
