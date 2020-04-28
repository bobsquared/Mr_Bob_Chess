
#pragma once
#include <unordered_map>


// Zobrist hashing
class Zobrist {

public:

  Zobrist();

  // Determine hashing key
  uint64_t hashBoard(uint64_t *pieces, uint64_t& occupied, uint64_t& blacks, bool turn);
  uint64_t hashBoardM(uint64_t board, int pieceFrom, int pieceTo, int moveType, int captureType, bool turn, bool isEnpassant);

private:

  // table and flags

  uint64_t table[64][12];
  uint64_t blackTurn;
  uint64_t whiteKingCastle;
  uint64_t blackKingCastle;
  uint64_t whiteQueenCastle;
  uint64_t blackQueenCastle;

};
