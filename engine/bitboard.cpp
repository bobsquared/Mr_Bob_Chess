#include <bitset>
#include <iostream>
#include "bitboard.h"
#include "dumb7flooding.h"
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string>
#include <algorithm>
#include "piecesquaretable.h"
#include "config.h"
#include <cmath>


bool operator==(const Bitboard::Move& lhs, const Bitboard::Move& rhs) {
  return (lhs.fromLoc == rhs.fromLoc) && (lhs.toLoc == rhs.toLoc);
}

bool operator!=(const Bitboard::Move& lhs, const Bitboard::Move& rhs) {
  return (lhs.fromLoc != rhs.fromLoc) && (lhs.toLoc != rhs.toLoc);
}

std::ostream& operator<<(std::ostream& stream, const Bitboard::Move& rhs) {
  return stream << rhs.fromLoc << rhs.toLoc;
}

bool compareSortVal(Bitboard::SortMove &s1, Bitboard::SortMove &s2) {
  return s1.eval > s2.eval;
}


Bitboard::Bitboard() {

  // lookup.rehash(357913941);
  materialScore = 0;
  enpasssantFlag = 0;
  whiteToMove = true;

  kingMovedWhite = false;
  kingMovedBlack = false;
  rookMovedWhiteA = false;
  rookMovedWhiteH = false;
  rookMovedBlackA = false;
  rookMovedBlackH = false;

  whiteCastled = false;
  blackCastled = false;

  InitPieceBoards();
  InitDistanceArray();
  InitWhitePawnMoves();
  InitBlackPawnMoves();
  InitKnightMoves();
  InitBishopMoves();
  InitRookMoves();
  InitQueenMoves();
  InitKingMoves();

  InitRayAttacks();
  InitMvvLva();

  magics = Magics(rookMoves, bishopMoves);

  whites = whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKings;
  blacks = blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKings;
  occupied = whites | blacks;

  pieces[0] = (whitePawns | blackPawns);
  pieces[1] = (whiteKnights | blackKnights);
  pieces[2] = (whiteBishops | blackBishops);
  pieces[3] = (whiteRooks | blackRooks);
  pieces[4] = (whiteQueens | blackQueens);
  pieces[5] = (whiteKings | blackKings);


  zobrist = Zobrist();


  for (uint16_t i = 0; i < 1024; i++) {
    for (uint8_t j = 0; j < 2; j++) {
      for (uint8_t k = 0; k < 2; k++) {
        killerMoves[k][i][j] = Move();
      }
    }
  }

  moveStack.reserve(1024);
  prevPositions.reserve(1024);
  numHashes = hashSize * 0xFFFFF / sizeof(ZobristVal);
  lookup2 = new ZobristVal [numHashes];
  // std::cout << hashSize * 0xFFFFF / sizeof(ZobristVal) <<std::endl;
  lookup.reserve(1);
  prevPositions.emplace_back(hashBoard(whiteToMove));

  for (uint8_t i = 0; i < 64; i++) {
    for (uint8_t j = 0; j < 64; j++) {
      for (uint8_t k = 0; j < 2; j++) {
        history[k][i][j] = 0;
      }
    }
  }

  countPawnsW = count_population(whitePawns);
  countPawnsB = count_population(blackPawns);

  countKnightsW = count_population(whiteKnights);
  countKnightsB = count_population(blackKnights);

  countBishopsW = count_population(whiteBishops);
  countBishopsB = count_population(blackBishops);

  countRooksW = count_population(whiteRooks);
  countRooksB = count_population(blackRooks);

  countQueensW = count_population(whiteQueens);
  countQueensB = count_population(blackQueens);

  halfMove = 0;

}

Bitboard::~Bitboard() {
  delete lookup2;
}

void Bitboard::InitDistanceArray() {

  for (uint8_t i = 0; i < 64; i++) {
    for (uint8_t j = 0; j < 64; j++) {
      uint8_t colI = i % 8;
      uint8_t rowI = std::floor(i / 8);
      uint8_t colJ = j % 8;
      uint8_t rowJ = std::floor(j / 8);
      manhattanArray[i][j] = abs(colI - colJ) + abs(rowI - rowJ);
      chebyshevArray[i][j] = std::max(abs(colI - colJ), abs(rowI - rowJ));
    }
  }

}

void Bitboard::InitPieceBoards() {

  for (uint8_t i = 0; i < 8; i++) {
    for (uint8_t j = 0; j < 8; j++) {
      whitePawnTable[i * 8 + j] = WHITE_PAWN_TABLE[(7 - i) * 8 + j];
      blackPawnTable[i * 8 + j] = BLACK_PAWN_TABLE[(7 - i) * 8 + j];
      whitePawnTableEG[i * 8 + j] = WHITE_PAWN_TABLE_ENDGAME[(7 - i) * 8 + j];
      blackPawnTableEG[i * 8 + j] = BLACK_PAWN_TABLE_ENDGAME[(7 - i) * 8 + j];
      whiteKnightTable[i * 8 + j] = WHITE_KNIGHT_TABLE[(7 - i) * 8 + j];
      blackKnightTable[i * 8 + j] = BLACK_KNIGHT_TABLE[(7 - i) * 8 + j];
      whiteBishopTable[i * 8 + j] = WHITE_BISHOP_TABLE[(7 - i) * 8 + j];
      blackBishopTable[i * 8 + j] = BLACK_BISHOP_TABLE[(7 - i) * 8 + j];
      whiteRookTable[i * 8 + j] = WHITE_ROOK_TABLE[(7 - i) * 8 + j];
      blackRookTable[i * 8 + j] = BLACK_ROOK_TABLE[(7 - i) * 8 + j];
      whiteQueenTable[i * 8 + j] = WHITE_QUEEN_TABLE[(7 - i) * 8 + j];
      blackQueenTable[i * 8 + j] = BLACK_QUEEN_TABLE[(7 - i) * 8 + j];
      whiteKingTable[i * 8 + j] = WHITE_KING_TABLE[(7 - i) * 8 + j];
      blackKingTable[i * 8 + j] = BLACK_KING_TABLE[(7 - i) * 8 + j];
    }
  }

}

void Bitboard::InitRayAttacks() {

  for (uint8_t i = 0; i < 64; i++) {

    rayAttacks[0][i] = dumb7FloodingN(0, 1ULL << i);
    rayAttacks[1][i] = dumb7FloodingNE(0, 1ULL << i);
    rayAttacks[2][i] = dumb7FloodingE(0, 1ULL << i);
    rayAttacks[3][i] = dumb7FloodingSE(0, 1ULL << i);
    rayAttacks[4][i] = dumb7FloodingS(0, 1ULL << i);
    rayAttacks[5][i] = dumb7FloodingSW(0, 1ULL << i);
    rayAttacks[6][i] = dumb7FloodingW(0, 1ULL << i);
    rayAttacks[7][i] = dumb7FloodingNW(0, 1ULL << i);

  }

}


void Bitboard::InitMvvLva() {

  for (uint16_t i = 0; i < 6; i++) {
    for (int16_t j = 5; j >= 0; j--) {
      mvvlva[i][j] = 100 + j * 100 + (6 - i) * 10;
      if (j >= i) {
        mvvlva[i][j] *= 10;
      }
    }
  }

}

void Bitboard::resetBoard() {

  materialScore = 0;
  enpasssantFlag = 0;
  whiteToMove = true;

  kingMovedWhite = false;
  kingMovedBlack = false;
  rookMovedWhiteA = false;
  rookMovedWhiteH = false;
  rookMovedBlackA = false;
  rookMovedBlackH = false;

  whiteCastled = false;
  blackCastled = false;

  whites = whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKings;
  blacks = blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKings;
  occupied = whites | blacks;

  pieces[0] = (whitePawns | blackPawns);
  pieces[1] = (whiteKnights | blackKnights);
  pieces[2] = (whiteBishops | blackBishops);
  pieces[3] = (whiteRooks | blackRooks);
  pieces[4] = (whiteQueens | blackQueens);
  pieces[5] = (whiteKings | blackKings);


  moveStack = {};
  prevPositions = {};

  for (uint8_t i = 0; i < 64; i++) {
    for (uint8_t j = 0; j < 64; j++) {
      for (uint8_t k = 0; j < 2; j++) {
        history[k][i][j] = 0;
      }
    }
  }


  for (uint16_t i = 0; i < 1024; i++) {
    for (uint8_t j = 0; j < 2; j++) {
      for (uint8_t k = 0; k < 2; k++) {
        killerMoves[k][i][j] = Move();
      }
    }
  }

  prevPositions.emplace_back(hashBoard(whiteToMove));

  countPawnsW = count_population(whitePawns);
  countPawnsB = count_population(blackPawns);

  countKnightsW = count_population(whiteKnights);
  countKnightsB = count_population(blackKnights);

  countBishopsW = count_population(whiteBishops);
  countBishopsB = count_population(blackBishops);

  countRooksW = count_population(whiteRooks);
  countRooksB = count_population(blackRooks);

  countQueensW = count_population(whiteQueens);
  countQueensB = count_population(blackQueens);

  halfMove = 0;

}

void Bitboard::InitBlackPawnMoves() {
  for (uint8_t i = 0; i < 64; i++) {
    uint64_t tempBitBoard = 1ULL << i;
    tempBitBoard |= tempBitBoard >> 8;

    if (i < 56 && i > 47) {
      tempBitBoard |= tempBitBoard >> 8;
    }
    tempBitBoard ^= 1ULL << i;
    blackPawnMoves[i] = tempBitBoard;
  }

  for (uint8_t i = 0; i < 64; i++) {
    uint64_t tempBitBoard = 1ULL << i;
    uint64_t tempBitBoard1 = tempBitBoard >> 9;
    uint64_t tempBitBoard2 = tempBitBoard >> 7;

    tempBitBoard1 &= RIGHT_MASK;
    tempBitBoard2 &= LEFT_MASK;
    blackPawnAttacks[i] = tempBitBoard1 | tempBitBoard2;
  }

  blackPawns = (1ULL << 48) | (1ULL << 49) | (1ULL << 50) | (1ULL << 51) | (1ULL << 52) | (1ULL << 53) | (1ULL << 54) | (1ULL << 55);

}

void Bitboard::InitWhitePawnMoves() {
  for (uint8_t i = 0; i < 64; i++) {
    uint64_t tempBitBoard = 1ULL << i;
    tempBitBoard |= tempBitBoard << 8;

    if (i < 16 && i > 7) {
      tempBitBoard |= tempBitBoard << 8;
    }
    tempBitBoard ^= 1ULL << i;
    whitePawnMoves[i] = tempBitBoard;
  }

  for (uint8_t i = 0; i < 64; i++) {
    uint64_t tempBitBoard = 1ULL << i;
    uint64_t tempBitBoard1 = tempBitBoard << 9;
    uint64_t tempBitBoard2 = tempBitBoard << 7;

    tempBitBoard1 &= LEFT_MASK;
    tempBitBoard2 &= RIGHT_MASK;
    whitePawnAttacks[i] = tempBitBoard1 | tempBitBoard2;
  }

  whitePawns = (1ULL << 8) | (1ULL << 9) | (1ULL << 10) | (1ULL << 11) | (1ULL << 12) | (1ULL << 13) | (1ULL << 14) | (1ULL << 15);

}


void Bitboard::InitKnightMoves() {

  for (uint8_t i = 0; i < 64; i++) {
    uint64_t tempBitBoard = 0;
    uint64_t tempBitBoard1 = 0;
    uint64_t tempBitBoard2 = 0;
    uint64_t tempBitBoard3 = 0;
    uint64_t tempBitBoard4 = 0;

    tempBitBoard |= 1ULL << i;
    tempBitBoard1 |= ((tempBitBoard << 15) & ALL_ONES);
    tempBitBoard1 |= tempBitBoard >> 17;
    tempBitBoard1 &= 9187201950435737471U;

    tempBitBoard2 |= ((tempBitBoard << 6) & ALL_ONES);
    tempBitBoard2 |= tempBitBoard >> 10;
    tempBitBoard2 &= 13816973012072644543U;
    tempBitBoard2 &= 9187201950435737471U;

    tempBitBoard3 |= ((tempBitBoard << 10) & ALL_ONES);
    tempBitBoard3 |= tempBitBoard >> 6;
    tempBitBoard3 &= 18374403900871474942U;
    tempBitBoard3 &= 18302063728033398269U;

    tempBitBoard4 |= ((tempBitBoard << 17) & ALL_ONES);
    tempBitBoard4 |= tempBitBoard >> 15;
    tempBitBoard4 &= 18374403900871474942U;

    knightMoves[i] = tempBitBoard1 | tempBitBoard2 | tempBitBoard3 | tempBitBoard4;

  }

  whiteKnights = (1ULL << 1) | (1ULL << 6);
  blackKnights = (1ULL << 62) | (1ULL << 57);

}

void Bitboard::InitBishopMoves() {

  for (uint8_t i = 0; i < 64; i++) {
    uint64_t tempBitBoard = 0;
    uint64_t tempBitBoard1 = 0;
    uint64_t tempBitBoard2 = 0;
    uint64_t tempBitBoard3 = 0;
    uint64_t tempBitBoard4 = 0;

    tempBitBoard |= 1ULL << i;
    uint8_t col = i % 8;

    for (uint8_t k = col; k < 8; k++) {
      if (k == 7) {
        break;
      }
      else {
          tempBitBoard1 |= tempBitBoard << (9 + 9 * (k - col));
      }
    }

    for (uint8_t k = col; k < 8; k++) {
      if (k == 7) {
        break;
      }
      else {
        tempBitBoard2 |= tempBitBoard >> (7 + 7 * (k - col));
      }
    }

    for (int8_t k = col; k >= 0; k--) {
      if (k == 0) {
        break;
      }
      else {
        tempBitBoard3 |= tempBitBoard << (7 + 7 * (col - k));
      }
    }

    for (int8_t k = col; k >= 0; k--) {
      if (k == 0) {
        break;
      }
      else {
        tempBitBoard4 |= tempBitBoard >> (9 + 9 * (col - k));
      }
    }

    bishopMoves[i] = ((tempBitBoard1 & ALL_ONES) | tempBitBoard2 | tempBitBoard4 | (tempBitBoard3 & ALL_ONES));
  }

  whiteBishops = (1ULL << 2) | (1ULL << 5);
  blackBishops = (1ULL << 61) | (1ULL << 58);

}


void Bitboard::InitRookMoves() {

  for (uint8_t i = 0; i < 64; i++) {
    uint64_t tempBitBoard = 0;
    uint64_t tempBitBoard1 = 0;
    uint64_t tempBitBoard2 = 0;
    uint64_t tempBitBoard3 = 0;
    uint64_t tempBitBoard4 = 0;

    tempBitBoard |= 1ULL << i;
    uint8_t col = i % 8;

    for (uint8_t k = 0; k < 8; k++) {
      tempBitBoard1 |= tempBitBoard >> (8 + k * 8);
      tempBitBoard2 |= ((tempBitBoard << (8 + k * 8)) & ALL_ONES);
    }

    for (uint8_t k = col; k < 8; k++) {
      if (k == 7) {
        break;
      }
      else {
        tempBitBoard3 |= ((tempBitBoard << (1 + 1 * (k - col))) & ALL_ONES);
      }
    }

    for (int8_t k = col; k >= 0; k--) {
      if (k == 0) {
        break;
      }
      else {
        tempBitBoard4 |= tempBitBoard >> (1 + 1 * (col - k));
      }
    }

    rookMoves[i] = (tempBitBoard1 | tempBitBoard2 | tempBitBoard4 | tempBitBoard3) & (tempBitBoard ^ ALL_ONES);
  }

  whiteRooks = 1 | (1ULL << 7);
  blackRooks = (1ULL << 56) | (1ULL << 63);
}



void Bitboard::InitQueenMoves() {

  for (uint8_t i = 0; i < 64; i++) {
    queenMoves[i] = rookMoves[i] | bishopMoves[i];
  }

  whiteQueens = 1ULL << 3;
  blackQueens = 1ULL << 59;

}

void Bitboard::InitKingMoves() {

  for (uint8_t i = 0; i < 64; i++) {
    uint64_t tempBitBoard = 0;
    uint64_t tempBitBoard1 = 0;
    uint64_t tempBitBoard2 = 0;

    tempBitBoard = 1ULL << i;

    tempBitBoard |= tempBitBoard >> 8;
    tempBitBoard |= tempBitBoard << 8;

    tempBitBoard1 |= tempBitBoard >> 1;
    tempBitBoard1 &= RIGHT_MASK;

    tempBitBoard2 |= tempBitBoard << 1;
    tempBitBoard2 &= LEFT_MASK;

    tempBitBoard |= tempBitBoard1 | tempBitBoard2;
    tempBitBoard &= (1ULL << i) ^ ALL_ONES;

    kingMoves[i] = tempBitBoard;
  }

  whiteKings = 1ULL << 4;
  blackKings = 1ULL << 60;

}




uint64_t Bitboard::pawnAttacksWhite(uint64_t bitboard) {

  uint64_t tempBitBoard1 = (bitboard << 9);
  uint64_t tempBitBoard2 = (bitboard << 7);

  tempBitBoard1 &= LEFT_MASK;
  tempBitBoard2 &= RIGHT_MASK;
  return tempBitBoard1 | tempBitBoard2;

}

uint64_t Bitboard::pawnAttacksBlack(uint64_t bitboard) {

  uint64_t tempBitBoard1 = (bitboard >> 9);
  uint64_t tempBitBoard2 = (bitboard >> 7);

  tempBitBoard1 &= RIGHT_MASK;
  tempBitBoard2 &= LEFT_MASK;
  return tempBitBoard1 | tempBitBoard2;

}


// Adapted from https://www.chessprogramming.org/Knight_Pattern#MultipleKnightAttacks
uint64_t Bitboard::knightAttacks(uint64_t knights) {
  uint64_t l1 = (knights >> 1) & 0x7f7f7f7f7f7f7f7f;
  uint64_t l2 = (knights >> 2) & 0x3f3f3f3f3f3f3f3f;
  uint64_t r1 = (knights << 1) & 0xfefefefefefefefe;
  uint64_t r2 = (knights << 2) & 0xfcfcfcfcfcfcfcfc;
  uint64_t h1 = l1 | r1;
  uint64_t h2 = l2 | r2;
  return (h1 << 16) | (h1 >> 16) | (h2 << 8) | (h2 >> 8);
}





//Adapted from https://www.chessprogramming.org/BitScan
uint8_t Bitboard::bitScanF(uint64_t bitboard) {
  unsigned int folded;
  bitboard ^= bitboard - 1;
  folded = (int)(bitboard ^ (bitboard >> 32));
  return LSB_TABLE[(folded * 0x78291ACF) >> 26];
}

//Adapted from https://www.chessprogramming.org/BitScan
uint8_t Bitboard::bitScanR(uint64_t bitboard) {
  return MSB_TABLE[((bitboard ^ (bitboard - 1)) * 0x03f79d71b4cb0a89) >> 58];
}


uint8_t Bitboard::count_population(uint64_t bitboard) {

  uint8_t count = 0;
  while (bitboard != 0) {
    count++;
    bitboard &= (bitboard - 1);
  }

  return count;
}


void Bitboard::printBoard(uint64_t board) {

  std::bitset<64> x(board);
  for (int16_t i = 7; i >= 0; i--) {
    for (int16_t j = 7; j >= 0; j--) {
      std::cout << x[(7 - j) + 8 * i] << " ";
    }
    std::cout << std::endl;
  }

}


void Bitboard::printPretty() {

  std::bitset<64> oc(occupied);
  std::bitset<64> wh(whites);
  std::bitset<64> bl(blacks);
  std::bitset<64> pa(pieces[0]);
  std::bitset<64> kn(pieces[1]);
  std::bitset<64> bi(pieces[2]);
  std::bitset<64> ro(pieces[3]);
  std::bitset<64> qu(pieces[4]);
  std::bitset<64> ki(pieces[5]);
  std::cout << "-----------------------------------------" << std::endl;

  for (int16_t i = 7; i >= 0; i--) {
    for (int16_t j = 7; j >= 0; j--) {
      std::string s = "";
      if (oc[(7 - j) + 8 * i] != 0) {
        if (wh[(7 - j) + 8 * i] != 0) {
          s += "w";
        }
        if (bl[(7 - j) + 8 * i] != 0) {
          s += "b";
        }
        if (pa[(7 - j) + 8 * i] != 0) {
          s += "P";
        }
        if (kn[(7 - j) + 8 * i] != 0) {
          s += "N";
        }
        if (bi[(7 - j) + 8 * i] != 0) {
          s += "B";
        }
        if (ro[(7 - j) + 8 * i] != 0) {
          s += "R";
        }
        if (qu[(7 - j) + 8 * i] != 0) {
          s += "Q";
        }
        if (ki[(7 - j) + 8 * i] != 0) {
          s += "K";
        }


      }

      if (s == "") {
        s += "  ";
      }
      std::cout << s << " ";
    }
    std::cout << std::endl;
  }

  std::cout << "-----------------------------------------" << std::endl;

}




// ------------------- Above are Initializiation functions, rest will be called during play -------------------/

bool Bitboard::filterCheck(bool color) {

  uint64_t ret = 0;

  if (color == 0) {
    if (pieces[5] & whites) {

      uint64_t indexP = pieces[5] & whites;
      uint8_t index = bitScanR(indexP);
      assert ((1ULL << index) == indexP);

      uint64_t bishopAttacksMaskI = magics.bishopAttacksMask(occupied, index);
      uint64_t rookAttacksMaskI = magics.rookAttacksMask(occupied, index);
      ret = blacks & pieces[0] & whitePawnAttacks[index];
      ret |= blacks & pieces[1] & knightMoves[index];
      ret |= blacks & pieces[2] & bishopAttacksMaskI;
      ret |= blacks & pieces[3] & rookAttacksMaskI;
      ret |= blacks & pieces[4] & (bishopAttacksMaskI | rookAttacksMaskI);
      ret |= blacks & pieces[5] & kingMoves[index];
    }
    else {
      return false;
    }

  }
  else {

    if (pieces[5] & blacks) {

      uint64_t indexP = pieces[5] & blacks;
      uint8_t index = bitScanR(indexP);
      assert ((1ULL << index) == indexP);

      uint64_t bishopAttacksMaskI = magics.bishopAttacksMask(occupied, index);
      uint64_t rookAttacksMaskI = magics.rookAttacksMask(occupied, index);
      ret = whites & pieces[0] & blackPawnAttacks[index];
      ret |= whites & pieces[1] & knightMoves[index];
      ret |= whites & pieces[2] & bishopAttacksMaskI;
      ret |= whites & pieces[3] & rookAttacksMaskI;
      ret |= whites & pieces[4] & (bishopAttacksMaskI | rookAttacksMaskI);
      ret |= whites & pieces[5] & kingMoves[index];


    }
    else {
      return false;
    }

  }

  return ret == 0;

}


bool Bitboard::isAttacked(uint8_t index, bool color) {

  uint64_t ret = 0;

  if (!color) {

    uint64_t bishopAttacksMaskI = magics.bishopAttacksMask(occupied, index);
    uint64_t rookAttacksMaskI = magics.rookAttacksMask(occupied, index);
    ret = blacks & pieces[0] & whitePawnAttacks[index];
    ret |= blacks & pieces[1] & knightMoves[index];
    ret |= blacks & pieces[2] & bishopAttacksMaskI;
    ret |= blacks & pieces[3] & rookAttacksMaskI;
    ret |= blacks & pieces[4] & (bishopAttacksMaskI | rookAttacksMaskI);
    ret |= blacks & pieces[5] & kingMoves[index];

  }
  else {

    uint64_t bishopAttacksMaskI = magics.bishopAttacksMask(occupied, index);
    uint64_t rookAttacksMaskI = magics.rookAttacksMask(occupied, index);
    ret = whites & pieces[0] & blackPawnAttacks[index];
    ret |= whites & pieces[1] & knightMoves[index];
    ret |= whites & pieces[2] & bishopAttacksMaskI;
    ret |= whites & pieces[3] & rookAttacksMaskI;
    ret |= whites & pieces[4] & (bishopAttacksMaskI | rookAttacksMaskI);
    ret |= whites & pieces[5] & kingMoves[index];

  }

  return ret == 0;

}



std::vector<Bitboard::Move> Bitboard::allValidMoves(bool color) {

  std::vector<Move> ret = {};
  ret.reserve(128);
  bool quiet = true;
  int8_t pieceFC = -1;
  int8_t pieceTC = -1;
  int score = 0;
  uint8_t promotion = 0;
  bool isEnpassant;

  if (color == 0){

    std::vector<uint8_t> loc = whitePiecesLoc();
    for (uint8_t i : loc) {

      if (((1ULL << i) & pieces[0]) != 0) {
        pieceFC = 0;
      }
      else if (((1ULL << i) & pieces[1]) != 0) {
        pieceFC = 1;
      }
      else if (((1ULL << i) & pieces[2]) != 0) {
        pieceFC = 2;
      }
      else if (((1ULL << i) & pieces[3]) != 0) {
        pieceFC = 3;
      }
      else if (((1ULL << i) & pieces[4]) != 0) {
        pieceFC = 4;
      }
      else if (((1ULL << i) & pieces[5]) != 0) {
        pieceFC = 5;
      }

      std::vector<uint8_t> move = validMovesWhite(i);

      for (uint8_t j : move) {
        if (((1ULL << j) & occupied) != 0) {
          quiet = false;
          if (((1ULL << j) & pieces[0]) != 0) {
            pieceTC = 0;
          }
          else if (((1ULL << j) & pieces[1]) != 0) {
            pieceTC = 1;
          }
          else if (((1ULL << j) & pieces[2]) != 0) {
            pieceTC = 2;
          }
          else if (((1ULL << j) & pieces[3]) != 0) {
            pieceTC = 3;
          }
          else if (((1ULL << j) & pieces[4]) != 0) {
            pieceTC = 4;
          }
          else if (((1ULL << j) & pieces[5]) != 0) {
            pieceTC = 5;
          }

          if (pieceFC == 0 && j > 55) {
            promotion = 4;
          }

          isEnpassant = false;
          score = pieceTC - pieceFC;

        }
        else if (pieceFC == 0 && j > 55) {
          quiet = false;
          pieceTC = -1;
          score = 6;
          promotion = 4;
          isEnpassant = false;
        }
        else if (pieceFC == 0 && (j == i + 7 || j == i + 9) && ((1ULL << j) & occupied) == 0) {
          quiet = false;
          pieceTC = -1;
          score = 0;
          promotion = 0;
          isEnpassant = true;
        }
        else {
          quiet = true;
          pieceTC = -1;
          promotion = 0;
          score = 0;
          isEnpassant = false;
        }

        ret.emplace_back(i, j, quiet, pieceFC, pieceTC, score, promotion, isEnpassant);
      }

    }

  }
  else {

    std::vector<uint8_t> loc = blackPiecesLoc();
    for (uint8_t i : loc) {

      if (((1ULL << i) & pieces[0]) != 0) {
        pieceFC = 0;
      }
      else if (((1ULL << i) & pieces[1]) != 0) {
        pieceFC = 1;
      }
      else if (((1ULL << i) & pieces[2]) != 0) {
        pieceFC = 2;
      }
      else if (((1ULL << i) & pieces[3]) != 0) {
        pieceFC = 3;
      }
      else if (((1ULL << i) & pieces[4]) != 0) {
        pieceFC = 4;
      }
      else if (((1ULL << i) & pieces[5]) != 0) {
        pieceFC = 5;
      }

      std::vector<uint8_t> move = validMovesBlack(i);

      for (uint8_t j : move) {
        if (((1ULL << j) & occupied) != 0) {
          quiet = false;
          if (((1ULL << j) & pieces[0]) != 0) {
            pieceTC = 0;
          }
          else if (((1ULL << j) & pieces[1]) != 0) {
            pieceTC = 1;
          }
          else if (((1ULL << j) & pieces[2]) != 0) {
            pieceTC = 2;
          }
          else if (((1ULL << j) & pieces[3]) != 0) {
            pieceTC = 3;
          }
          else if (((1ULL << j) & pieces[4]) != 0) {
            pieceTC = 4;
          }
          else if (((1ULL << j) & pieces[5]) != 0) {
            pieceTC = 5;
          }

          if (pieceFC == 0 && j < 8) {
            promotion = 4;
          }


          score = pieceTC - pieceFC;
          isEnpassant = false;
        }
        else if (pieceFC == 0 && j < 8) {
          quiet = false;
          pieceTC = -1;
          score = 0;
          promotion = 4;
          isEnpassant = false;
        }
        else if (pieceFC == 0 && (j == i - 7 || j == i - 9) && ((1ULL << j) & occupied) == 0) {
          quiet = false;
          pieceTC = -1;
          score = 6;
          promotion = 0;
          isEnpassant = true;
        }
        else {
          quiet = true;
          pieceTC = -1;
          promotion = 0;
          score = 0;
          isEnpassant = false;
        }

        ret.emplace_back(i, j, quiet, pieceFC, pieceTC, score, promotion, isEnpassant);
      }

    }

  }

  return ret;

}


std::vector<Bitboard::Move> Bitboard::allValidCaptures(bool color) {

  std::vector<Move> ret = {};
  ret.reserve(128);
  bool quiet = true;
  int8_t pieceFC = -1;
  int8_t pieceTC = -1;
  int score = 0;
  uint8_t promotion = 0;
  bool isEnpassant;

  if (color == 0){

    std::vector<uint8_t> loc = whitePiecesLoc();
    for (uint8_t i : loc) {

      if (((1ULL << i) & pieces[0]) != 0) {
        pieceFC = 0;
      }
      else if (((1ULL << i) & pieces[1]) != 0) {
        pieceFC = 1;
      }
      else if (((1ULL << i) & pieces[2]) != 0) {
        pieceFC = 2;
      }
      else if (((1ULL << i) & pieces[3]) != 0) {
        pieceFC = 3;
      }
      else if (((1ULL << i) & pieces[4]) != 0) {
        pieceFC = 4;
      }
      else if (((1ULL << i) & pieces[5]) != 0) {
        pieceFC = 5;
      }

      std::vector<uint8_t> move = validMovesWhite(i);

      for (uint8_t j : move) {

        if (((1ULL << j) & occupied) != 0) {
          quiet = false;
          if (((1ULL << j) & pieces[0]) != 0) {
            pieceTC = 0;
          }
          else if (((1ULL << j) & pieces[1]) != 0) {
            pieceTC = 1;
          }
          else if (((1ULL << j) & pieces[2]) != 0) {
            pieceTC = 2;
          }
          else if (((1ULL << j) & pieces[3]) != 0) {
            pieceTC = 3;
          }
          else if (((1ULL << j) & pieces[4]) != 0) {
            pieceTC = 4;
          }
          else if (((1ULL << j) & pieces[5]) != 0) {
            pieceTC = 5;
          }

          if (pieceFC == 0 && j > 55) {
            promotion = 4;
          }
          else {
            promotion = 0;
          }

          score = (pieceTC - pieceFC) * 10000 + mvvlva[pieceFC][pieceTC];
          isEnpassant = false;

          Move mv = {i, j, quiet, pieceFC, pieceTC, score, promotion, isEnpassant};
          ret.emplace_back(mv);

        }
        else if (pieceFC == 0 && j > 55) {
          quiet = false;
          pieceTC = -1;
          score = 70000;
          promotion = 4;
          isEnpassant = false;

          Move mv = {i, j, quiet, pieceFC, pieceTC, score, promotion, isEnpassant};
          ret.emplace_back(mv);
        }
        else if (pieceFC == 0 && (j == i + 7 || j == i + 9) && ((1ULL << j) & occupied) == 0) {
          quiet = false;
          pieceTC = -1;
          score = 60;
          promotion = 0;
          isEnpassant = true;

          Move mv = {i, j, quiet, pieceFC, pieceTC, score, promotion, isEnpassant};
          ret.emplace_back(mv);
        }



      }

    }

  }
  else {

    std::vector<uint8_t> loc = blackPiecesLoc();
    for (uint8_t i : loc) {

      if (((1ULL << i) & pieces[0]) != 0) {
        pieceFC = 0;
      }
      else if (((1ULL << i) & pieces[1]) != 0) {
        pieceFC = 1;
      }
      else if (((1ULL << i) & pieces[2]) != 0) {
        pieceFC = 2;
      }
      else if (((1ULL << i) & pieces[3]) != 0) {
        pieceFC = 3;
      }
      else if (((1ULL << i) & pieces[4]) != 0) {
        pieceFC = 4;
      }
      else if (((1ULL << i) & pieces[5]) != 0) {
        pieceFC = 5;
      }

      std::vector<uint8_t> move = validMovesBlack(i);

      for (uint8_t j : move) {

        if (((1ULL << j) & occupied) != 0) {
          quiet = false;
          if (((1ULL << j) & pieces[0]) != 0) {
            pieceTC = 0;
          }
          else if (((1ULL << j) & pieces[1]) != 0) {
            pieceTC = 1;
          }
          else if (((1ULL << j) & pieces[2]) != 0) {
            pieceTC = 2;
          }
          else if (((1ULL << j) & pieces[3]) != 0) {
            pieceTC = 3;
          }
          else if (((1ULL << j) & pieces[4]) != 0) {
            pieceTC = 4;
          }
          else if (((1ULL << j) & pieces[5]) != 0) {
            pieceTC = 5;
          }

          if (pieceFC == 0 && j < 8) {
            promotion = 4;
          }
          else {
            promotion = 0;
          }

          score = (pieceTC - pieceFC) * 10000 + mvvlva[pieceFC][pieceTC];
          isEnpassant = false;

          Move mv = {i, j, quiet, pieceFC, pieceTC, score, promotion, isEnpassant};
          ret.emplace_back(mv);
        }
        else if (pieceFC == 0 && j < 8) {
          quiet = false;
          pieceTC = -1;
          score = 70000;
          promotion = 4;
          isEnpassant = false;

          Move mv = {i, j, quiet, pieceFC, pieceTC, score, promotion, isEnpassant};
          ret.emplace_back(mv);
        }
        else if (pieceFC == 0 && (j == i - 7 || j == i - 9) && ((1ULL << j) & occupied) == 0) {
          quiet = false;
          pieceTC = -1;
          score = 60;
          promotion = 0;
          isEnpassant = true;

          Move mv = {i, j, quiet, pieceFC, pieceTC, score, promotion, isEnpassant};
          ret.emplace_back(mv);
        }



      }

    }

  }

  return ret;

}


std::vector<uint8_t> Bitboard::whitePiecesLoc() {
  std::vector<uint8_t> ret = {};
  ret.reserve(32);
  uint64_t bitboard = whites;
  while (bitboard != 0){
    uint8_t toApp = bitScanR(bitboard);
    bitboard &= bitboard - 1;
    ret.emplace_back(toApp);
  }
  return ret;
}


std::vector<uint8_t> Bitboard::blackPiecesLoc() {
  std::vector<uint8_t> ret = {};
  ret.reserve(32);
  uint64_t bitboard = blacks;
  while (bitboard != 0){
    uint8_t toApp = bitScanR(bitboard);
    bitboard &= bitboard - 1;
    ret.emplace_back(toApp);
  }
  return ret;
}


std::vector<uint8_t> Bitboard::validMovesWhite(uint8_t index) {

  uint64_t indexP = 1ULL << index;
  uint64_t whitesIndex = indexP & whites;
  std::vector<uint8_t> ret = {};
  ret.reserve(128);


  if ((pieces[0] & whitesIndex) != 0) {
    uint64_t base = (whitePawnAttacks[index] & blacks) | (whitePawnMoves[index] & ~occupied);
    if ((occupied & (1ULL << (index + 8))) != 0){
      base &= (1ULL << (index + 16)) ^ 18446744073709551615U;
    }

    uint8_t enpass = enpassantConditions(true, index);
    if (enpass) {
      ret.emplace_back(enpass);
    }

    while (base != 0) {
      uint8_t toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }

    return ret;
  }
  else if ((pieces[1] & whitesIndex) != 0) {
    uint64_t base = knightMoves[index] & ~whites;
    while (base != 0) {
      uint8_t toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if ((pieces[2] & whitesIndex) != 0) {
    uint64_t base = magics.bishopAttacksMask(occupied, index) & ~whites;
    while (base != 0) {
      uint8_t toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if ((pieces[3] & whitesIndex) != 0) {
    uint64_t base = magics.rookAttacksMask(occupied, index) & ~whites;
    while (base != 0) {
      uint8_t toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if ((pieces[4] & whitesIndex) != 0) {
    uint64_t base = (magics.bishopAttacksMask(occupied, index) | magics.rookAttacksMask(occupied, index)) & ~whites;

    while (base != 0) {
      uint8_t toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if ((pieces[5] & whitesIndex) != 0) {
    uint64_t base = kingMoves[index] & ~whites;

    if (canCastleK(true)) {
      ret.emplace_back(6);
    }

    if (canCastleQ(true)) {
      ret.emplace_back(2);
    }

    while (base != 0) {
      uint8_t toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }
    return ret;
  }

  return ret;
}

bool Bitboard::IsMoveWhite(Move &move) {
  uint8_t index = move.fromLoc;
  uint8_t index2 = move.toLoc;
  uint64_t indexP = 1ULL << index;
  uint64_t indexP2 = 1ULL << index2;
  uint64_t whitesIndex = indexP & whites;

  if (move.pieceTo != -1 && (indexP2 && pieces[move.pieceTo]) == 0) {
    return false;
  }

  if (move.pieceTo != -1 && (indexP2 && occupied) == 0) {
    return false;
  }

  if ((pieces[0] & whitesIndex) != 0) {
    uint64_t base = (whitePawnAttacks[index] & blacks) | (whitePawnMoves[index] & ~occupied);
    if ((occupied & (1ULL << (index + 8))) != 0){
      base &= (1ULL << (index + 16)) ^ 18446744073709551615U;
    }

    uint8_t enpass = enpassantConditions(true, index);
    if (enpass && index2 == enpass) {
      return true;
    }

    return (base & indexP2) != 0;
  }
  else if ((pieces[1] & whitesIndex) != 0) {
    uint64_t base = knightMoves[index] & ~whites;
    return (base & indexP2) != 0;
  }
  else if ((pieces[2] & whitesIndex) != 0) {
    uint64_t base = magics.bishopAttacksMask(occupied, index) & ~whites;
    return (base & indexP2) != 0;
  }
  else if ((pieces[3] & whitesIndex) != 0) {
    uint64_t base = magics.rookAttacksMask(occupied, index) & ~whites;
    return (base & indexP2) != 0;
  }
  else if ((pieces[4] & whitesIndex) != 0) {
    uint64_t base = (magics.bishopAttacksMask(occupied, index) | magics.rookAttacksMask(occupied, index)) & ~whites;
    return (base & indexP2) != 0;
  }
  else if ((pieces[5] & whitesIndex) != 0) {
    uint64_t base = kingMoves[index] & ~whites;

    if (canCastleK(true) && index2 == 6) {
        return true;
    }

    if (canCastleQ(true) && index2 == 2) {
        return true;
    }

    return (base & indexP2) != 0;
  }

  return false;
}




std::vector<uint8_t>  Bitboard::validMovesBlack(uint8_t index) {
  uint64_t indexP = 1ULL << index;
  uint64_t blacksIndex = indexP & blacks;
  std::vector<uint8_t> ret = {};
  ret.reserve(128);

  if ((pieces[0] & blacksIndex) != 0) {
    uint64_t base = (blackPawnAttacks[index] & whites) | (blackPawnMoves[index] & ~occupied);
    if ((index >= 16 && (occupied & (1ULL << (index - 8))) != 0)){
      base &= (1ULL << (index - 16)) ^ 18446744073709551615U;
    }

    uint8_t enpass = enpassantConditions(false, index);
    if (enpass) {
      ret.emplace_back(enpass);
    }

    while (base != 0) {
      uint8_t toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if ((pieces[1] & blacksIndex) != 0) {
    uint64_t base = knightMoves[index] & ~blacks;
    while (base != 0) {
      uint8_t toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if ((pieces[2] & blacksIndex) != 0) {
    uint64_t base = magics.bishopAttacksMask(occupied, index) & ~blacks;
    while (base != 0) {
      uint8_t toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if ((pieces[3] & blacksIndex) != 0) {
    uint64_t base = magics.rookAttacksMask(occupied, index) & ~blacks;
    while (base != 0) {
      uint8_t toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if ((pieces[4] & blacksIndex) != 0) {
    uint64_t base = (magics.bishopAttacksMask(occupied, index) | magics.rookAttacksMask(occupied, index)) & ~blacks;
    while (base != 0) {
      uint8_t toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if ((pieces[5] & blacksIndex) != 0) {
    uint64_t base = kingMoves[index] & ~blacks;

    if (canCastleK(false)) {
        ret.emplace_back(62);
    }

    if (canCastleQ(false)) {
        ret.emplace_back(58);
    }

    while (base != 0) {
      uint8_t toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }
    return ret;
  }

  return ret;
}

bool Bitboard::IsMoveBlack(Move &move) {
  uint8_t index = move.fromLoc;
  uint8_t index2 = move.toLoc;
  uint64_t indexP = 1ULL << index;
  uint64_t indexP2 = 1ULL << index2;
  uint64_t blacksIndex = indexP & blacks;

  if (move.pieceTo != -1 && (indexP2 && pieces[move.pieceTo]) == 0) {
    return false;
  }

  if (move.pieceTo != -1 && (indexP2 && occupied) == 0) {
    return false;
  }

  if ((pieces[0] & blacksIndex) != 0) {
    uint64_t base = (blackPawnAttacks[index] & whites) | (blackPawnMoves[index] & ~occupied);
    if ((index >= 16 && (occupied & (1ULL << (index - 8))) != 0)){
      base &= (1ULL << (index - 16)) ^ 18446744073709551615U;
    }

    uint8_t enpass = enpassantConditions(false, index);
    if (enpass && index2 == enpass) {
      return true;
    }

    return (base & indexP2) != 0;
  }
  else if ((pieces[1] & blacksIndex) != 0) {
    uint64_t base = knightMoves[index] & ~blacks;
    return (base & indexP2) != 0;
  }
  else if ((pieces[2] & blacksIndex) != 0) {
    uint64_t base = magics.bishopAttacksMask(occupied, index) & ~blacks;
    return (base & indexP2) != 0;
  }
  else if ((pieces[3] & blacksIndex) != 0) {
    uint64_t base = magics.rookAttacksMask(occupied, index) & ~blacks;
    return (base & indexP2) != 0;
  }
  else if ((pieces[4] & blacksIndex) != 0) {
    uint64_t base = (magics.bishopAttacksMask(occupied, index) | magics.rookAttacksMask(occupied, index)) & ~blacks;
    return (base & indexP2) != 0;
  }
  else if ((pieces[5] & blacksIndex) != 0) {
    uint64_t base = kingMoves[index] & ~blacks;

    if (canCastleK(false) && index2 == 62) {
        return true;
    }

    if (canCastleQ(false) && index2 == 58) {
        return true;
    }

    return (base & indexP2) != 0;
  }

  return false;
}




void Bitboard::movePiece(Move& move) {


  whiteToMove = !whiteToMove;
  // Before doing anything, check if this is a null move.
  if (move.fromLoc == 65 && move.toLoc == 65) {
    prevPositions.emplace_back(zobrist.hashBoardM(prevPositions.back(), move.fromLoc, move.toLoc, 0, 0, whiteToMove, false));
    moveStack.emplace_back(65, 65, 10, 10, 0, false, false, false, 0, 0);
    return;
  }

  uint64_t i1 = 1ULL << move.fromLoc;
  uint64_t i2 = 1ULL << move.toLoc;
  uint64_t i1i2 = i1 ^ i2;
  int8_t i = move.pieceFrom;
  int8_t k = move.pieceTo;
  bool useWhite;

  uint8_t rookTypeMoved = 0;
  uint8_t castled = 0;
  uint8_t enpassant = 0;
  bool kingMoved = false;
  bool promotion = false;

  assert(i != -1);

  if ((whites & i1) != 0) {
    useWhite = true;
  }
  else {
    useWhite = false;
  }

  // STEP 1: Check for white castling or if its a king move
  if (i == 5){
    if (useWhite) {
      // King side castle
      if (!kingMovedWhite && move.toLoc == 6) {
        whites ^= (1ULL << 7) | (1ULL << 5);
        pieces[3] ^= (1ULL << 7) | (1ULL << 5);
        occupied ^= (1ULL << 7) | (1ULL << 5);
        whiteCastled = true;
        castled = 1;
      }
      // Queen side Castle
      else if (!kingMovedWhite && move.toLoc == 2) {
        whites ^= 1ULL | (1ULL << 3);
        pieces[3] ^= 1ULL | (1ULL << 3);
        occupied ^= 1ULL | (1ULL << 3);
        whiteCastled = true;
        castled = 2;
      }
    }
    else {
      // King side castle
      if (!kingMovedBlack && move.toLoc == 62) {
        blacks ^= (1ULL << 63) | (1ULL << 61);
        pieces[3] ^= (1ULL << 63) | (1ULL << 61);
        occupied ^= (1ULL << 63) | (1ULL << 61);
        blackCastled = true;
        castled = 3;
      }
      // Queen side Castle
      else if (!kingMovedBlack && move.toLoc == 58) {
        blacks ^= (1ULL << 56) | (1ULL << 59);
        pieces[3] ^= (1ULL << 56) | (1ULL << 59);
        occupied ^= (1ULL << 56) | (1ULL << 59);
        blackCastled = true;
        castled = 4;
      }
    }

    // KING MOVE
    if (!kingMovedWhite && useWhite) {
      kingMovedWhite = true;
      kingMoved = true;
    }
    else if (!kingMovedBlack && !useWhite) {
      kingMovedBlack = true;
      kingMoved = true;
    }

  }




  // Any pawn moves. Captures may happen here
  else if (i == 0) {

    // STEP 2: Check for any promotions
    if (move.toLoc > 55) {
      pieces[i] ^= i2;
      pieces[4] ^= i2;
      promotion = true;
      materialScore += pieceValues[4] - pieceValues[0];
    }
    else if (move.toLoc < 8) {
      pieces[i] ^= i2;
      pieces[4] ^= i2;
      promotion = true;
      materialScore -= pieceValues[4] - pieceValues[0];
    }



    // STEP 3: Check for any enpassants
    if (useWhite && move.isEnpassant){

      blacks ^= 1ULL << (move.toLoc - 8);
      pieces[0] ^= 1ULL << (move.toLoc - 8);
      occupied ^= 1ULL << (move.toLoc - 8);
      materialScore += pieceValues[0];

      if (move.toLoc == move.fromLoc + 7) {
        enpassant = 1;
      }
      else if (move.toLoc == move.fromLoc + 9) {
        enpassant = 2;
      }

    }
    else if (move.isEnpassant) {

      whites ^= 1ULL << (move.toLoc + 8);
      pieces[0] ^= 1ULL << (move.toLoc + 8);
      occupied ^= 1ULL << (move.toLoc + 8);
      materialScore -= pieceValues[0];

      if (move.toLoc == move.fromLoc - 7) {
        enpassant = 3;
      }
      else if (move.toLoc == move.fromLoc - 9) {
        enpassant = 4;
      }
    }
  }





  // STEP 4: Check if any rooks moved
  else if (i == 3) {
    if (!rookMovedWhiteA && move.fromLoc == 0) {
      rookMovedWhiteA = true;
      rookTypeMoved = 1;
    }
    else if (!rookMovedWhiteH && move.fromLoc == 7) {
      rookMovedWhiteH = true;
      rookTypeMoved = 2;
    }
    else if (!rookMovedBlackA && move.fromLoc == 56) {
      rookMovedBlackA = true;
      rookTypeMoved = 3;
    }
    else if (!rookMovedBlackH && move.fromLoc == 63) {
      rookMovedBlackH = true;
      rookTypeMoved = 4;
    }
  }




  // STEP 5: Finally make the move
  if (k == -1) {

    if (useWhite) {
      whites ^= i1i2;
    }
    else {
      blacks ^= i1i2;
    }

    pieces[i] ^= i1i2;
    occupied ^= i1i2;

  }
  else {

    if (useWhite) {
      whites ^= i1i2;
      blacks ^= i2;
      materialScore += pieceValues[k];
    }
    else {
      blacks ^= i1i2;
      whites ^= i2;
      materialScore -= pieceValues[k];
    }

    pieces[k] ^= i2;
    pieces[i] ^= i1i2;
    occupied ^= i1;

  }


  moveStack.emplace_back(i1, i2, i, k, !useWhite, promotion, kingMoved, rookTypeMoved, castled, enpassant);
  prevPositions.emplace_back(zobrist.hashBoardM(prevPositions.back(), move.fromLoc, move.toLoc, i, k, useWhite, move.isEnpassant));

}







void Bitboard::undoMove() {


  assert(!prevPositions.empty());
  assert(!moveStack.empty());
  whiteToMove = !whiteToMove;
  MoveStack m = moveStack.back();
  uint64_t fromLoc = m.fromLoc;
  uint64_t toLoc = m.toLoc;

  if (fromLoc == 65 && toLoc == 65) {
    moveStack.pop_back();
    prevPositions.pop_back();
    return;
  }


  int8_t movePiece = m.movePiece;
  int8_t capturePiece = m.capturePiece;
  bool promotion = m.promote;

  bool kingMoved = m.kingMoved;
  uint8_t rookMoved = m.rookMoved;
  uint8_t castled = m.castled;
  uint8_t enpassant = m.enpassant;

  uint64_t i1i2 = fromLoc ^ toLoc;

  // STEP 1: Check if Enpassants
  if (enpassant == 1 || enpassant == 2) {
    blacks ^= toLoc >> 8;
    pieces[0] ^= toLoc >> 8;
    occupied ^= toLoc >> 8;
    materialScore -= pieceValues[0];
  }
  else if (enpassant == 3 || enpassant == 4) {
    whites ^= toLoc << 8;
    pieces[0] ^= toLoc << 8;
    occupied ^= toLoc << 8;
    materialScore += pieceValues[0];
  }

  // STEP 2: Check if castling
  if (castled == 1) {
    whites ^= (1ULL << 7) | (1ULL << 5);
    pieces[3] ^= (1ULL << 7) | (1ULL << 5);
    occupied ^= (1ULL << 7) | (1ULL << 5);
    whiteCastled = false;
  }
  else if (castled == 2) {
    whites ^= 1ULL | (1ULL << 3);
    pieces[3] ^= 1ULL | (1ULL << 3);
    occupied ^= 1ULL | (1ULL << 3);
    whiteCastled = false;
  }
  else if (castled == 3) {
    blacks ^= (1ULL << 63) | (1ULL << 61);
    pieces[3] ^= (1ULL << 63) | (1ULL << 61);
    occupied ^= (1ULL << 63) | (1ULL << 61);
    blackCastled = false;
  }
  else if (castled == 4) {
    blacks ^= (1ULL << 56) | (1ULL << 59);
    pieces[3] ^= (1ULL << 56) | (1ULL << 59);
    occupied ^= (1ULL << 56) | (1ULL << 59);
    blackCastled = false;
  }

  // STEP 3: Check if King moved
  if (kingMoved && !m.color) {
    kingMovedWhite = false;
  }
  else if (kingMoved && m.color) {
    kingMovedBlack = false;
  }

  // STEP 4: Check if rook moved
  if (rookMoved == 1) {
    rookMovedWhiteA = false;
  }
  else if (rookMoved == 2) {
    rookMovedWhiteH = false;
  }
  else if (rookMoved == 3) {
    rookMovedBlackA = false;
  }
  else if (rookMoved == 4) {
    rookMovedBlackH = false;
  }

  // STEP 5: Check for promotions
  if (!m.color && promotion) {
    pieces[4] ^= toLoc;
    pieces[0] ^= toLoc;
    materialScore -= pieceValues[4] - pieceValues[0];
  }
  else if (m.color && promotion) {
    pieces[4] ^= toLoc;
    pieces[0] ^= toLoc;
    materialScore += pieceValues[4] - pieceValues[0];
  }


  // STEP 6: Undo the move
  if (toLoc != fromLoc) {
    pieces[movePiece] ^= i1i2;
    if (capturePiece == -1) {
      if (!m.color) {
        whites ^= i1i2;
      }
      else {
        blacks ^= i1i2;
      }
      occupied ^= i1i2;
    }
    else {
      if (!m.color) {
        whites ^= i1i2;
        blacks ^= toLoc;
        materialScore -= pieceValues[capturePiece];
      }
      else {
        blacks ^= i1i2;
        whites ^= toLoc;
        materialScore += pieceValues[capturePiece];
      }
      pieces[capturePiece] ^= toLoc;
      occupied ^= fromLoc;
    }
  }

  moveStack.pop_back();
  prevPositions.pop_back();

}


int Bitboard::evaluate(int alpha, int beta) {

  // if (count_population(blacks & pieces[3]) != countRooksB) {
    // std::cout << unsigned(countRooksB) << " " << unsigned(count_population(blacks & pieces[3])) << std::endl;
    // printPretty();
  // }

  // assert(count_population(whites & pieces[0]) == countPawnsW);
  // assert(count_population(blacks & pieces[0]) == countPawnsB);
  // assert(count_population(whites & pieces[1]) == countKnightsW);
  // assert(count_population(blacks & pieces[1]) == countKnightsB);
  // assert(count_population(whites & pieces[2]) == countBishopsW);
  // assert(count_population(blacks & pieces[2]) == countBishopsB);
  // assert(count_population(whites & pieces[3]) == countRooksW);
  // assert(count_population(blacks & pieces[3]) == countRooksB);
  // assert(count_population(whites & pieces[4]) == countQueensW);
  // assert(count_population(blacks & pieces[4]) == countQueensB);

  int ret = materialScore;



  uint64_t whitePawns = whites & pieces[0];
  uint64_t blackPawns = blacks & pieces[0];

  uint64_t whiteKnights = whites & pieces[1];
  uint64_t blackKnights = blacks & pieces[1];

  uint64_t whiteBishops = whites & pieces[2];
  uint64_t blackBishops = blacks & pieces[2];



  if (whiteToMove) {
    ret += 10;
  }
  else {
    ret -= 10;
  }

  if (count_population(whiteBishops) == 2) {
    ret += 20;
  }

  if (count_population(blackBishops) == 2) {
    ret -= 20;
  }

  if (count_population(whiteKnights) == 2) {
    ret -= 10;

  }

  if (count_population(blackKnights) == 2) {
    ret += 10;
  }

  if (whitePawns == 0) {
    ret -= 30;
  }

  if (blackPawns == 0) {
    ret += 30;
  }

  if (whiteCastled) {
    ret += 80;
  }
  if (blackCastled) {
    ret -= 80;
  }


  // return ret;
  // if (ret < alpha - 300 || ret > beta + 300) {
  //   return whiteToMove? ret : -ret;
  // }


  bool endgame = false;
  uint8_t whiteKingIndex = bitScanR(whites & pieces[5]);
  uint8_t blackKingIndex = bitScanR(blacks & pieces[5]);

  uint64_t whiteRooks = whites & pieces[3];
  uint64_t blackRooks = blacks & pieces[3];

  uint64_t whiteQueens = whites & pieces[4];
  uint64_t blackQueens = blacks & pieces[4];

  uint64_t egPiecesWhite = whiteKnights | whiteBishops | whiteRooks | whiteQueens;
  uint64_t egPiecesBlack = blackKnights | blackBishops | blackRooks | blackQueens;
  uint8_t numWhitePawns = count_population(whitePawns);
  uint8_t numBlackPawns = count_population(blackPawns);

  if ((egPiecesWhite & (egPiecesWhite - 1)) == 0 && (egPiecesBlack & (egPiecesBlack - 1)) == 0) {
    endgame = true;
  }


  ret += evaluateMobility(whitePawns, blackPawns, whiteKnights, blackKnights, whiteBishops, blackBishops, whiteRooks, blackRooks, whiteQueens, blackQueens, endgame);
  ret += evaluateKingSafety(whiteKingIndex, blackKingIndex, whitePawns, blackPawns, whiteKnights, blackKnights, whiteBishops, blackBishops, whiteRooks, blackRooks, whiteQueens, blackQueens) / 1.2;


  ret += knightWeight[numWhitePawns] * count_population(whiteKnights);
  ret -= knightWeight[numBlackPawns] * count_population(blackKnights);

  ret += rookWeight[numWhitePawns] * count_population(whiteRooks);
  ret -= rookWeight[numBlackPawns] * count_population(blackRooks);







  return whiteToMove? ret : -ret;
}

int Bitboard::evaluateKingSafety(uint8_t whiteKingIndex, uint8_t whitePawns, uint8_t blackPawns, uint8_t blackKingIndex, uint64_t whiteKnights, uint64_t blackKnights,
  uint64_t whiteBishops, uint64_t blackBishops, uint64_t whiteRooks, uint64_t blackRooks, uint64_t whiteQueens, uint64_t blackQueens) {

  int ret = 0;
  while (whiteKnights) {
    ret -= (chebyshevArray[blackKingIndex][bitScanR(whiteKnights)] * pieceValues[1]) >> 10;
    whiteKnights &= whiteKnights - 1;
  }

  while (blackKnights) {
    ret += (chebyshevArray[whiteKingIndex][bitScanR(blackKnights)] * pieceValues[1]) >> 10;
    blackKnights &= blackKnights - 1;
  }

  while (whiteBishops) {
    ret -= (chebyshevArray[blackKingIndex][bitScanR(whiteBishops)] * pieceValues[2]) >> 10;
    whiteBishops &= whiteBishops - 1;
  }

  while (blackBishops) {
    ret += (chebyshevArray[whiteKingIndex][bitScanR(blackBishops)] * pieceValues[2]) >> 10;
    blackBishops &= blackBishops - 1;
  }

  while (whiteRooks) {
    ret -= (manhattanArray[blackKingIndex][bitScanR(whiteRooks)] * pieceValues[3]) >> 10;
    whiteRooks &= whiteRooks - 1;
  }

  while (blackRooks) {
    ret += (manhattanArray[whiteKingIndex][bitScanR(blackRooks)] * pieceValues[3]) >> 10;
    blackRooks &= blackRooks - 1;
  }

  while (whiteQueens) {
    ret -= (chebyshevArray[blackKingIndex][bitScanR(whiteQueens)] * pieceValues[4]) >> 10;
    whiteQueens &= whiteQueens - 1;
  }

  while (blackQueens) {
    ret += (chebyshevArray[whiteKingIndex][bitScanR(blackQueens)] * pieceValues[4]) >> 10;
    blackQueens &= blackQueens - 1;
  }

  return ret;
}

int Bitboard::evaluateMobility(uint64_t whitePawns, uint64_t blackPawns, uint64_t whiteKnights, uint64_t blackKnights,
  uint64_t whiteBishops, uint64_t blackBishops, uint64_t whiteRooks, uint64_t blackRooks, uint64_t whiteQueens, uint64_t blackQueens, bool endgame) {

  int ret = 0;
  int board = 0;



  if (endgame) {

    uint64_t whiteKing = whites & pieces[5];
    uint64_t blackKing = blacks & pieces[5];

    while (whitePawns) {
      board += whitePawnTableEG[bitScanR(whitePawns)];
      whitePawns &= whitePawns - 1;
    }


    // countPiece = count_population(pawnsB);
    while (blackPawns) {
      board -= blackPawnTableEG[bitScanR(blackPawns)];
      blackPawns &= blackPawns - 1;
    }

    board += whiteKingTable[bitScanR(whiteKing)];
    board -= blackKingTable[bitScanR(blackKing)];

  }
  else {

    while (whitePawns) {
      board += whitePawnTable[bitScanR(whitePawns)];
      whitePawns &= whitePawns - 1;
    }


    // countPiece = count_population(pawnsB);
    while (blackPawns) {
      board -= blackPawnTable[bitScanR(blackPawns)];
      blackPawns &= blackPawns - 1;
    }

  }


  ret += count_population(knightAttacks(whiteKnights) & (pawnAttacksBlack(blacks) ^ ALL_ONES));
  ret -= count_population(knightAttacks(blackKnights) & (pawnAttacksWhite(whites) ^ ALL_ONES));

  // countPiece = count_population(knightsW);
  while (whiteKnights) {
    board += whiteKnightTable[bitScanR(whiteKnights)];
    whiteKnights &= whiteKnights - 1;
  }

  // countPiece = count_population(knightsB);
  while (blackKnights) {
    board -= blackKnightTable[bitScanR(blackKnights)];
    blackKnights &= blackKnights - 1;
  }

  // countPiece = count_population(bishopsW);
  while (whiteBishops) {
    uint8_t bscan = bitScanR(whiteBishops);
    ret += count_population(magics.bishopAttacksMask(occupied, bscan));
    board += whiteBishopTable[bscan];
    whiteBishops &= whiteBishops - 1;
  }

  // countPiece = count_population(bishopsB);
  while (blackBishops) {
    uint8_t bscan = bitScanR(blackBishops);
    ret -= count_population(magics.bishopAttacksMask(occupied, bscan));
    board -= blackBishopTable[bscan];
    blackBishops &= blackBishops - 1;
  }


  // countPiece = count_population(rooksW);
  while (whiteRooks) {
    uint8_t bscan = bitScanR(whiteRooks);
    ret += count_population(magics.rookAttacksMask(occupied, bscan));
    board += whiteRookTable[bscan];
    whiteRooks &= whiteRooks - 1;
  }

  // countPiece = count_population(rooksB);
  while (blackRooks) {
    uint8_t bscan = bitScanR(blackRooks);
    ret -= count_population(magics.rookAttacksMask(occupied, bscan));
    board -= blackRookTable[bscan];
    blackRooks &= blackRooks - 1;
  }


  // countPiece = count_population(queensW);
  while (whiteQueens) {
    uint8_t bscan = bitScanR(whiteQueens);
    board += whiteQueenTable[bscan];
    ret += count_population((magics.rookAttacksMask(occupied, bscan) | magics.bishopAttacksMask(occupied, bscan))) / 3;
    whiteQueens &= whiteQueens - 1;
  }

  // countPiece = count_population(queensB);
  while (blackQueens) {
    uint8_t bscan = bitScanR(blackQueens);
    board -= blackQueenTable[bscan];
    ret -= count_population((magics.rookAttacksMask(occupied, bscan) | magics.bishopAttacksMask(occupied, bscan))) / 3;
    blackQueens &= blackQueens - 1;
  }

  return ret + board;

}



uint8_t Bitboard::sortMoves(std::vector<Move> &moveList, Move &move, int depth) {

  if (moveList.size() <= 1) {
    return 0;
  }

  if (move != Move()) {
    std::vector<Move>::iterator p;
    p = std::find(moveList.begin(), moveList.end(), move);
    if (p != moveList.end()) {
      *p = std::move(moveList.back());
      moveList.pop_back();
    }
  }


  //MVV/LVA
  int16_t val = -75;
  int16_t valMin = 0;
  for (uint8_t i = 0; i < moveList.size(); i++) {

    if (pieceValues[moveList[i].pieceTo] - pieceValues[moveList[i].pieceFrom] > val) {
      val = pieceValues[moveList[i].pieceTo] - pieceValues[moveList[i].pieceFrom];
    }

    if (pieceValues[moveList[i].pieceTo] - pieceValues[moveList[i].pieceFrom] < valMin) {
      valMin = pieceValues[moveList[i].pieceTo] - pieceValues[moveList[i].pieceFrom];
    }

    if (moveList[i].score < 3000000) {
      // if (moveList[i].promotion == 4) {
      //   moveList[i].score = 1600000;
      // }
      // else if (moveList[i].pieceTo != -1 && mvvlva[moveList[i].pieceFrom][moveList[i].pieceTo] >= 1000){
      //   moveList[i].score = 1500000 + mvvlva[moveList[i].pieceFrom][moveList[i].pieceTo];
      // }
      //
      // else if (moveList[i].pieceTo != -1){
      //   moveList[i].score = 800000 + mvvlva[moveList[i].pieceFrom][moveList[i].pieceTo];
      // }

      if (moveList[i].quiet){
        if (moveList[i] == killerMoves[whiteToMove][depth][0]) {
          moveList[i].score = 1000000;
        }
        else if (moveList[i] == killerMoves[whiteToMove][depth][1]) {
          moveList[i].score = 900000;
        }
        else {
          moveList[i].score += history[whiteToMove][moveList[i].pieceFrom][moveList[i].toLoc];
        }
      }
    }

  }
  //
  for (uint8_t i = 0; i < moveList.size(); i++) {

    if (!moveList[i].quiet && moveList[i].score < 3000000) {

      if (moveList[i].promotion == 4) {
        moveList[i].score = 1600000;
        continue;
      }
      if (pieceValues[moveList[i].pieceTo] - pieceValues[moveList[i].pieceFrom] == val) {
        moveList[i].score = 1500000 + mvvlva[moveList[i].pieceFrom][moveList[i].pieceTo];
      }
      else if (moveList[i].pieceTo - moveList[i].pieceFrom == 0) {
        moveList[i].score = 1400000 + mvvlva[moveList[i].pieceFrom][moveList[i].pieceTo];
      }

      else if (moveList[i].pieceTo - moveList[i].pieceFrom == valMin) {
        moveList[i].score = -800000 + mvvlva[moveList[i].pieceFrom][moveList[i].pieceTo];
      }
      else if (mvvlva[moveList[i].pieceFrom][moveList[i].pieceTo] >= 1000) {
        moveList[i].score = 700000 + mvvlva[moveList[i].pieceFrom][moveList[i].pieceTo];
      }
      else if (moveList[i].pieceTo - moveList[i].pieceFrom < 0 && moveList[i].pieceTo - moveList[i].pieceFrom > -500) {
        moveList[i].score = 800000 + mvvlva[moveList[i].pieceFrom][moveList[i].pieceTo];
      }

    }

  }



  std::sort(moveList.begin(), moveList.end());

  return 0;


}


void Bitboard::InsertKiller(Move move, int depth) {

  killerMoves[whiteToMove][depth][1] = killerMoves[whiteToMove][depth][0];
  killerMoves[whiteToMove][depth][0] = move;

}


void Bitboard::InsertLookup(Move move, int score, int depth, uint8_t flag, uint64_t key) {

  ZobristVal val = ZobristVal();
  uint64_t posKey = key % numHashes;

  // If there is a colision
  if (lookup2[posKey].posKey != key && lookup2[posKey].posKey != 0) {
    if (halfMove != lookup2[posKey].halfMove) {
      if (lookup2[posKey].depth > depth) {
        return;
      }
    }
  }
  val.move = move;
  val.score = score;
  val.depth = depth;
  val.flag = flag;
  val.posKey = key;
  val.halfMove = halfMove;

  lookup2[posKey] = val;

}

uint64_t Bitboard::hashBoard(bool turn) {
  return zobrist.hashBoard(pieces, occupied, blacks, turn);
}


bool Bitboard::canCastleQ(bool isWhite) {
  if (isWhite) {
    uint64_t canNotBeAttacked = 14ULL & occupied;
    if (!kingMovedWhite && !rookMovedWhiteA && canNotBeAttacked == 0 && isAttacked(4, 0) && isAttacked(3, 0) && isAttacked(2, 0)) {
      return true;
    }
  }
  else {
    uint64_t canNotBeAttacked = 1008806316530991104ULL & occupied;
    if (!kingMovedBlack && !rookMovedBlackA && canNotBeAttacked == 0 && isAttacked(60, 1) && isAttacked(59, 1) && isAttacked(58, 1)) {
      return true;
    }
  }

  return false;
}

bool Bitboard::canCastleK(bool isWhite) {
  if (isWhite) {
    uint64_t canNotBeAttacked = 96ULL & occupied;
    if (!kingMovedWhite && !rookMovedWhiteH && canNotBeAttacked == 0 && isAttacked(4, 0) && isAttacked(5, 0) && isAttacked(6, 0)) {
      return true;
    }
  }
  else {
    uint64_t canNotBeAttacked = 6917529027641081856ULL & occupied;
    if (!kingMovedBlack && !rookMovedBlackH && canNotBeAttacked == 0 && isAttacked(60, 1) && isAttacked(61, 1) && isAttacked(62, 1)) {
      return true;
    }
  }

  return false;
}


uint8_t Bitboard::enpassantConditions(bool isWhite, uint8_t pawnLocation) {
  if (moveStack.empty()) {
    return 0;
  }

  MoveStack lastMove = moveStack.back();
  uint8_t fromLoc = bitScanR(lastMove.fromLoc);
  uint8_t toLoc = bitScanR(lastMove.toLoc);
  if (isWhite) {
    if (pawnLocation >= 32 && pawnLocation < 40) {
      if (lastMove.movePiece == 0 && fromLoc >= 48 && fromLoc < 56 && toLoc >= 32 && toLoc < 40) {
        if (toLoc == pawnLocation + 1 || toLoc == pawnLocation - 1) {
          return toLoc + 8;
        }
      }
    }
  }
  else {
    if (pawnLocation >= 24 && pawnLocation < 32) {
      if (lastMove.movePiece == 0 && fromLoc >= 8 && fromLoc < 16 && toLoc >= 24 && toLoc < 32) {
        if (toLoc == pawnLocation + 1 || toLoc == pawnLocation - 1) {
          return toLoc - 8;
        }
      }
    }
  }

  return 0;
}

bool Bitboard::canNullMove() {
  if (count_population(occupied) <= 12) {
    return false;
  }

  return true;
}

std::string Bitboard::posToFEN() {

  std::string fen = "";
  std::string cStr = "";

  for (uint8_t i = 0; i < 8; i++) {
    std::string rank = "";
    uint8_t count = 0;


    for (uint8_t j = 0; j < 8; j++) {
      uint64_t calc = (1ULL << ((i * 8) + j));

      if (calc & occupied) {

        if (count > 0) {
          cStr = NUM_TO_STR[count];
          rank += cStr;
        }
        count = 0;
        if (calc & whites) {
          if (calc & pieces[0]) {
            rank += "P";
          }
          else if (calc & pieces[1]) {
            rank += "N";
          }
          else if (calc & pieces[2]) {
            rank += "B";
          }
          else if (calc & pieces[3]) {
            rank += "R";
          }
          else if (calc & pieces[4]) {
            rank += "Q";
          }
          else if (calc & pieces[5]) {
            rank += "K";
          }

        }
        else {
          if (calc & pieces[0]) {
            rank += "p";
          }
          else if (calc & pieces[1]) {
            rank += "n";
          }
          else if (calc & pieces[2]) {
            rank += "b";
          }
          else if (calc & pieces[3]) {
            rank += "r";
          }
          else if (calc & pieces[4]) {
            rank += "q";
          }
          else if (calc & pieces[5]) {
            rank += "k";
          }
        }
      }
      else {
        count++;
      }
    }

    if (count > 0) {
      cStr = NUM_TO_STR[count];
      rank += cStr;
    }

    if (i < 7) {
      rank = "/" + rank;
    }
    fen = rank + fen;

  }

  if (whiteToMove) {
    fen += " w ";
  }
  else {
    fen += " b ";
  }

  if (!kingMovedWhite) {
    if (!rookMovedWhiteA && !rookMovedWhiteH) {
      fen += "KQ";
    }
    else if (!rookMovedWhiteA) {
      fen += "Q";
    }
    else {
      fen += "K";
    }
  }

  if (!kingMovedBlack) {
    if (!rookMovedBlackA && !rookMovedBlackH) {
      fen += "kq";
    }
    else if (!rookMovedBlackA) {
      fen += "q";
    }
    else {
      fen += "k";
    }
  }

  if ((kingMovedBlack && kingMovedWhite) || (rookMovedBlackA && rookMovedBlackH && rookMovedWhiteA && rookMovedWhiteH)) {
    fen += "-";
  }

  return fen;
}


bool Bitboard::isThreeFold() {
  if (std::count(prevPositions.begin(), prevPositions.end(), prevPositions.back()) >= 2) {
    return true;
  }

  return false;
}

uint64_t Bitboard::getPosKey() {
  return prevPositions.back();
}

void Bitboard::updateHalfMove() {
  halfMove++;
}





// int8_t Bitboard::leastValuableAttacker(uint8_t loc, bool isWhite) {
//
//   uint64_t ret = 0;
//   if (isWhite) {
//
//     ret = whitePawnAttacks[loc] & blacks & pieces[0];
//     if (ret) {
//       return bitScanR(ret);
//     }
//
//     ret = knightAttacks[loc] & blacks & pieces[1];
//     if (ret) {
//       return bitScanR(ret);
//     }
//
//     ret = magics.bishopAttacksMask(occupied, loc) & blacks & pieces[2];
//     if (ret) {
//       return bitScanR(ret);
//     }
//
//     ret = magics.rookAttacksMask(occupied, loc) & blacks & pieces[3];
//     if (ret) {
//       return bitScanR(ret);
//     }
//
//     ret = (magics.rookAttacksMask(occupied, loc) | magics.bishopAttacksMask(occupied, loc)) & blacks & pieces[4];
//     if (ret) {
//       return bitScanR(ret);
//     }
//
//     ret = kingMoves[loc] & blacks & pieces[5];
//     if (ret) {
//       return bitScanR(ret);
//     }
//
//     return -1;
//   }
//
//   else {
//     ret = blackPawnAttacks[loc] & whites & pieces[0];
//     if (ret) {
//       return bitScanR(ret);
//     }
//
//     ret = knightAttacks[loc] & whites & pieces[1];
//     if (ret) {
//       return bitScanR(ret);
//     }
//
//     ret = magics.bishopAttacksMask(occupied, loc) & whites & pieces[2];
//     if (ret) {
//       return bitScanR(ret);
//     }
//
//     ret = magics.rookAttacksMask(occupied, loc) & whites & pieces[3];
//     if (ret) {
//       return bitScanR(ret);
//     }
//
//     ret = (magics.rookAttacksMask(occupied, loc) | magics.bishopAttacksMask(occupied, loc)) & whites & pieces[4];
//     if (ret) {
//       return bitScanR(ret);
//     }
//
//     ret = kingMoves[loc] & whites & pieces[5];
//     if (ret) {
//       return bitScanR(ret);
//     }
//
//     return -1;
//   }
//
// }
//
//
// int8_t Bitboard::see(uint8_t loc, bool isWhite) {
//
//   int value = 0;
//   int8_t piece = leastValuableAttacker(loc, isWhite);
//
//   if (piece != -1) {
//     movePiece(Move{piece, loc, true, })
//   }
//
// }
































//
