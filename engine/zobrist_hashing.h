
#pragma once
#include <unordered_map>


// Zobrist hashing
class Zobrist {

public:

  Zobrist();

  // Determine hashing key
  uint64_t hashBoard(std::unordered_map<uint8_t, uint64_t>& pieces, uint64_t& occupied, uint64_t& blacks, bool turn);
  uint64_t hashBoardM(uint64_t board, int8_t pieceFrom, int8_t pieceTo, int8_t moveType, int8_t captureType, bool turn, bool isEnpassant);

private:

  // table and flags
  std::unordered_map<uint8_t, std::unordered_map<uint8_t, uint64_t>> table = {};
  uint64_t blackTurn;
  uint64_t whiteKingCastle;
  uint64_t blackKingCastle;
  uint64_t whiteQueenCastle;
  uint64_t blackQueenCastle;

};
