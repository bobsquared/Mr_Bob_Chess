
#pragma once
#include <unordered_map>


class Zobrist {

public:
  Zobrist();
  uint64_t hashBoard(std::unordered_map<uint8_t, uint64_t>& pieces, uint64_t& occupied, uint64_t& blacks, bool turn);

private:


  std::unordered_map<uint8_t, std::unordered_map<uint8_t, uint64_t>> table = {};

};
