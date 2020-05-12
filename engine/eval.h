#pragma once
#include "piecesquaretable.h"



class Eval {

public:

  // Evaluate position
  int evaluate(int materialScore, bool whiteToMove, int *pieces, int countPawnsW, int countPawnsB, int countKnightsW, int countKnightsB, int countBishopsW, int countBishopsB, int countRooksW, int countRooksB);



private:

  const int pieceValues[6] = {100, 321, 333, 500, 900, 20000};
  const int attackWeight[8] = {0, 25, 50, 75, 88, 94, 97, 99};
  const int knightWeight[9] = {-20, -16, -12, -8, -4, 0, 4, 8, 12};
  const int rookWeight[9] = {15, 12, 9, 6, 3, 0, -3, -6, -9};

  // Piece square tables
  int whitePawnTable[64];
  int blackPawnTable[64];
  int whitePawnTableEG[64];
  int blackPawnTableEG[64];
  int whiteKnightTable[64];
  int blackKnightTable[64];
  int whiteBishopTable[64];
  int blackBishopTable[64];
  int whiteRookTable[64];
  int blackRookTable[64];
  int whiteQueenTable[64];
  int blackQueenTable[64];
  int whiteKingTable[64];
  int blackKingTable[64];
  int whiteKingTableEG[64];
  int blackKingTableEG[64];

  // Initializing masks
  void InitPieceBoards();
  void InitDistanceArray();
  void InitPassedPawnsMask();
  void InitIsolatedPawnsMask();
  void InitColumnsMask();
  void InitKingZoneMask();

  uint64_t whitePassedPawnMask[64];
  uint64_t blackPassedPawnMask[64];
  uint64_t isolatedPawnMask[64];
  uint64_t columnMask[8];
  uint64_t kingZoneMaskWhite[64];
  uint64_t kingZoneMaskBlack[64];

  // Evaluation functions
  int evaluateMobility(uint64_t whitePawns, uint64_t blackPawns, uint64_t whiteKnights, uint64_t blackKnights,
    uint64_t whiteBishops, uint64_t blackBishops, uint64_t whiteRooks, uint64_t blackRooks, uint64_t whiteQueens, uint64_t blackQueens, bool endgame);
  int evaluateKingSafety(uint8_t whiteKingIndex, uint8_t blackKingIndex, uint64_t whiteKnights, uint64_t blackKnights,
    uint64_t whiteBishops, uint64_t blackBishops, uint64_t whiteRooks, uint64_t blackRooks, uint64_t whiteQueens, uint64_t blackQueens);
  int evaluatePawns(uint64_t whitePawns, uint64_t blackPawns);

  uint8_t manhattanArray[64][64]; // Distance arrays
  uint8_t chebyshevArray[64][64]; // Distance arrays

};
