
#pragma once
#include <unordered_map>
#include <vector>
#include <iostream>
#include <cassert>



class Magics{

public:

  ~Magics();
  Magics(uint64_t *rookMoves, uint64_t *bishopMoves);
  void Generate_Magic_Rooks();
  void Generate_Magic_Bishops();
  uint64_t rookAttacksMask(uint64_t occupations, int index);
  uint64_t bishopAttacksMask(uint64_t occupations, int index);

private:

  struct MagicPro {
    uint64_t bitboard;
    uint32_t shift;
    uint64_t magic;
    uint64_t mask;
  };

  uint64_t *rookMoves;
  uint64_t *bishopMoves;

  uint64_t bitCombinations(uint64_t index, uint64_t bitboard);
  bool InitBlocksRook(uint64_t bitboard, uint64_t index, uint64_t magic);
  bool InitBlocksBishop(uint64_t bitboard, uint8_t index, uint64_t magic);
  uint8_t count_population(uint64_t bitboard);

  void optimalMagicRook();
  void optimalMagicBishop();

  std::unordered_map<uint8_t, uint64_t> magicR = {};
  std::unordered_map<uint8_t, uint64_t> magicB = {};

  MagicPro attacksR[64];
  MagicPro attacksB[64];
  // std::unordered_map<uint8_t, MagicPro> attacksR = {};
  // std::unordered_map<uint8_t, MagicPro> attacksB = {};

  uint64_t *rookComb;
  uint64_t *bishopComb;
  // std::unordered_map<uint8_t, std::unordered_map<uint64_t, uint64_t>> rookComb = {};
  // std::unordered_map<uint8_t, std::unordered_map<uint64_t, uint64_t>> bishopComb = {};


};
