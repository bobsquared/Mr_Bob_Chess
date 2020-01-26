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
#include <cmath>


bool operator==(const Bitboard::Move& lhs, const Bitboard::Move& rhs) {
  return (lhs.fromLoc == rhs.fromLoc) && (lhs.toLoc == rhs.toLoc);
}

bool operator!=(const Bitboard::Move& lhs, const Bitboard::Move& rhs) {
  return (lhs.fromLoc != rhs.fromLoc) && (lhs.toLoc != rhs.toLoc);
}

bool operator==(const Bitboard::KillerMove& lhs, const Bitboard::KillerMove& rhs) {
  return (lhs.move == rhs.move) && (lhs.depth == rhs.depth);
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
      killerMoves[i][j] = KillerMove();
    }
  }

  moveStack.reserve(1024);
  prevPositions.reserve(1024);
  lookup.reserve(16777215);
  prevPositions.emplace_back(hashBoard(whiteToMove));

  for (uint8_t i = 0; i < 64; i++) {
    for (uint8_t j = 0; j < 64; j++) {
      history[i][j] = 0;
    }
  }

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
      whiteKnightTable[i * 8 + j] = WHITE_KNIGHT_TABLE[(7 - i) * 8 + j];
      blackKnightTable[i * 8 + j] = BLACK_KNIGHT_TABLE[(7 - i) * 8 + j];
      whiteBishopTable[i * 8 + j] = WHITE_BISHOP_TABLE[(7 - i) * 8 + j];
      blackBishopTable[i * 8 + j] = BLACK_BISHOP_TABLE[(7 - i) * 8 + j];
      whiteRookTable[i * 8 + j] = WHITE_ROOK_TABLE[(7 - i) * 8 + j];
      blackRookTable[i * 8 + j] = BLACK_ROOK_TABLE[(7 - i) * 8 + j];
      whiteQueenTable[i * 8 + j] = WHITE_QUEEN_TABLE[(7 - i) * 8 + j];
      blackQueenTable[i * 8 + j] = BLACK_QUEEN_TABLE[(7 - i) * 8 + j];
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
      history[i][j] = 0;
    }
  }


  for (uint16_t i = 0; i < 1024; i++) {
    for (uint8_t j = 0; j < 2; j++) {
      killerMoves[i][j] = KillerMove();
    }
  }

  prevPositions.emplace_back(hashBoard(whiteToMove));

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
      if (!((1ULL << index) == indexP)) {
        printBoard(indexP);
        std::cout << index << " " << indexP << std::endl;
      }
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
  int pieceFC = -1;
  int pieceTC = -1;
  int score = 0;

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

        }
        else {
          quiet = true;
          pieceTC = -1;
          score = 0;
        }
        Move mv = {i, j, quiet, pieceFC, pieceTC, score};
        ret.emplace_back(mv);
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

        }
        else {
          quiet = true;
          pieceTC = -1;
          score = 0;
        }
        Move mv = {i, j, quiet, pieceFC, pieceTC, score};
        ret.emplace_back(mv);
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
    uint8_t toApp = MSB_TABLE[((bitboard ^ (bitboard - 1)) * 0x03f79d71b4cb0a89U) >> 58];
    bitboard ^= 1ULL << toApp;
    ret.emplace_back(toApp);
  }
  return ret;
}


std::vector<uint8_t> Bitboard::blackPiecesLoc() {
  std::vector<uint8_t> ret = {};
  ret.reserve(32);
  uint64_t bitboard = blacks;
  while (bitboard != 0){
    uint8_t toApp = MSB_TABLE[((bitboard ^ (bitboard - 1)) * 0x03f79d71b4cb0a89U) >> 58];
    bitboard ^= 1ULL << toApp;
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
      uint8_t toApp = MSB_TABLE[((base ^ (base - 1)) * 0x03f79d71b4cb0a89U) >> 58];
      base ^= 1ULL << toApp;
      ret.emplace_back(toApp);
    }

    return ret;
  }
  else if ((pieces[1] & whitesIndex) != 0) {
    uint64_t base = knightMoves[index] & ~whites;
    while (base != 0) {
      uint8_t toApp = MSB_TABLE[((base ^ (base - 1)) * 0x03f79d71b4cb0a89U) >> 58];
      base ^= 1ULL << toApp;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if ((pieces[2] & whitesIndex) != 0) {
    uint64_t base = magics.bishopAttacksMask(occupied, index) & ~whites;
    while (base != 0) {
      uint8_t toApp = MSB_TABLE[((base ^ (base - 1)) * 0x03f79d71b4cb0a89U) >> 58];
      base ^= 1ULL << toApp;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if ((pieces[3] & whitesIndex) != 0) {
    uint64_t base = magics.rookAttacksMask(occupied, index) & ~whites;
    while (base != 0) {
      uint8_t toApp = MSB_TABLE[((base ^ (base - 1)) * 0x03f79d71b4cb0a89U) >> 58];
      base ^= 1ULL << toApp;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if ((pieces[4] & whitesIndex) != 0) {
    uint64_t base = (magics.bishopAttacksMask(occupied, index) | magics.rookAttacksMask(occupied, index)) & ~whites;

    while (base != 0) {
      uint8_t toApp = MSB_TABLE[((base ^ (base - 1)) * 0x03f79d71b4cb0a89U) >> 58];
      base ^= 1ULL << toApp;
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
      uint8_t toApp = MSB_TABLE[((base ^ (base - 1)) * 0x03f79d71b4cb0a89U) >> 58];
      base ^= 1ULL << toApp;
      ret.emplace_back(toApp);
    }
    return ret;
  }

  return ret;
}

bool Bitboard::IsMoveWhite(uint8_t index, uint8_t index2) {
  uint64_t indexP = 1ULL << index;
  uint64_t indexP2 = 1ULL << index2;
  uint64_t whitesIndex = indexP & whites;

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
      uint8_t toApp = MSB_TABLE[((base ^ (base - 1)) * 0x03f79d71b4cb0a89U) >> 58];
      base ^= 1ULL << toApp;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if ((pieces[1] & blacksIndex) != 0) {
    uint64_t base = knightMoves[index] & ~blacks;
    while (base != 0) {
      uint8_t toApp = MSB_TABLE[((base ^ (base - 1)) * 0x03f79d71b4cb0a89U) >> 58];
      base ^= 1ULL << toApp;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if ((pieces[2] & blacksIndex) != 0) {
    uint64_t base = magics.bishopAttacksMask(occupied, index) & ~blacks;
    while (base != 0) {
      uint8_t toApp = MSB_TABLE[((base ^ (base - 1)) * 0x03f79d71b4cb0a89U) >> 58];
      base ^= 1ULL << toApp;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if ((pieces[3] & blacksIndex) != 0) {
    uint64_t base = magics.rookAttacksMask(occupied, index) & ~blacks;
    while (base != 0) {
      uint8_t toApp = MSB_TABLE[((base ^ (base - 1)) * 0x03f79d71b4cb0a89U) >> 58];
      base ^= 1ULL << toApp;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if ((pieces[4] & blacksIndex) != 0) {
    uint64_t base = (magics.bishopAttacksMask(occupied, index) | magics.rookAttacksMask(occupied, index)) & ~blacks;
    while (base != 0) {
      uint8_t toApp = MSB_TABLE[((base ^ (base - 1)) * 0x03f79d71b4cb0a89U) >> 58];
      base ^= 1ULL << toApp;
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
      uint8_t toApp = MSB_TABLE[((base ^ (base - 1)) * 0x03f79d71b4cb0a89U) >> 58];
      base ^= 1ULL << toApp;
      ret.emplace_back(toApp);
    }
    return ret;
  }

  return ret;
}

bool Bitboard::IsMoveBlack(uint8_t index, uint8_t index2) {
  uint64_t indexP = 1ULL << index;
  uint64_t indexP2 = 1ULL << index2;
  uint64_t blacksIndex = indexP & blacks;

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




// void Bitboard::movePiece(Move& move) {
//
//   whiteToMove = !whiteToMove;
//   uint64_t i1 = 1ULL << move.fromLoc;
//   uint64_t i2 = 1ULL << move.toLoc;
//   uint64_t i1i2 = i1 ^ i2;
//   int8_t i = move.pieceFrom;
//   int8_t k = move.pieceTo;
//   bool useWhite;
//
//   assert(i != -1);
//
//   if ((whites & i1) != 0) {
//     useWhite = true;
//   }
//   else {
//     useWhite = false;
//   }
//
//   if (move.fromLoc == 65 && move.toLoc == 65) {
//     prevPositions.emplace_back(hashBoard(whiteToMove));
//     moveStack.emplace_back((MoveStack){65, 65, 10, 10, 0, false, false, false, 0, 0});
//     return;
//   }
//   else if (i == 5 && !kingMovedWhite && move.toLoc == 6) {
//     whites ^= (1ULL << 7) | (1ULL << 5);
//     pieces[3] ^= (1ULL << 7) | (1ULL << 5);
//     occupied ^= (1ULL << 7) | (1ULL << 5);
//     whiteCastled = true;
//     castled = 1;
//
//     whites ^= i1i2;
//     pieces[i] ^= i1i2;
//     occupied ^= i1i2;
//     moveStack.emplace_back((MoveStack){i1, i2, i, -1, !useWhite, false, false, false, 0, 0});
//   }
//   else if (i == 5 && !kingMovedWhite && move.toLoc == 2) {
//     whites ^= 1ULL | (1ULL << 3);
//     pieces[3] ^= 1ULL | (1ULL << 3);
//     occupied ^= 1ULL | (1ULL << 3);
//     whiteCastled = true;
//     castled = 2;
//
//     whites ^= i1i2;
//     pieces[i] ^= i1i2;
//     occupied ^= i1i2;
//     moveStack.emplace_back((MoveStack){i1, i2, i, -1, !useWhite, false, false, false, 0, 0});
//   }
//   else if (move.quiet) {
//     assert(k == -1);
//
//     if (useWhite) {
//       whites ^= i1i2;
//     }
//     else {
//       blacks ^= i1i2;
//     }
//
//     pieces[i] ^= i1i2;
//     occupied ^= i1i2;
//     moveStack.emplace_back((MoveStack){i1, i2, i, -1, !useWhite, false, false, false, 0, 0});
//
//   }
//   else if (!move.quiet) {
//     assert(k != -1);
//
//     if (useWhite) {
//       whites ^= i1i2;
//       blacks ^= i2;
//       materialScore += pieceValues[k];
//     }
//     else {
//       blacks ^= i1i2;
//       whites ^= i2;
//       materialScore -= pieceValues[k];
//     }
//
//     pieces[k] ^= i2;
//     occupied ^= i1;
//     pieces[i] ^= i1i2;
//
//     moveStack.emplace_back((MoveStack){i1, i2, i, k, !useWhite, false, false, false, 0 ,0});
//   }
//
//   if (i == 5 && !kingMovedWhite) {
//     kingMovedWhite = true;
//     kingMoved = true;
//   }
//
//   if (i == 3 && !rookMovedWhiteA && index1 == 0) {
//     rookMovedWhiteA = true;
//     rookTypeMoved = 1;
//   }
//
//   if (i == 3 && !rookMovedWhiteH && index1 == 7) {
//     rookMovedWhiteH = true;
//     rookTypeMoved = 2;
//   }
//
//   if (i == 0 && index2 == index1 + 7) {
//     blacks ^= 1ULL << (index2 - 8);
//     pieces[0] ^= 1ULL << (index2 - 8);
//     occupied ^= 1ULL << (index2 - 8);
//     materialScore += pieceValues[0];
//     enpassant = 1;
//   }
//
//   if (i == 0 && index2 == index1 + 9) {
//     blacks ^= 1ULL << (index2 - 8);
//     pieces[0] ^= 1ULL << (index2 - 8);
//     occupied ^= 1ULL << (index2 - 8);
//     materialScore += pieceValues[0];
//     enpassant = 2;
//   }
//
//   prevPositions.emplace_back(hashBoard(whiteToMove));
//
// }



void Bitboard::movePiece(uint8_t index1, uint8_t index2) {

  whiteToMove = !whiteToMove;
  if (index1 == 65 && index2 == 65) {
    prevPositions.emplace_back(zobrist.hashBoardM(prevPositions.back(), index1, index2, 0, 0, whiteToMove));
    moveStack.emplace_back(65, 65, 10, 10, 0, false, false, false, 0, 0);
    return;
  }

  uint64_t i1 = 1ULL << index1;
  uint64_t i2 = 1ULL << index2;
  uint64_t i1i2 = i1 ^ i2;

  int8_t k = -1;
  int8_t i = -1;
  if ((pieces[0] & i1) != 0) {
    i = 0;
  }
  else if ((pieces[1] & i1) != 0) {
    i = 1;
  }
  else if ((pieces[2] & i1) != 0) {
    i = 2;
  }
  else if ((pieces[3] & i1) != 0) {
    i = 3;
  }
  else if ((pieces[4] & i1) != 0) {
    i = 4;
  }
  else if ((pieces[5] & i1) != 0) {
    i = 5;
  }


  // std::cout << unsigned(index1) << std::endl;
  // printPretty();
  assert(i != -1);

  if (!(i2 & occupied)) {
    pieces[i] ^= i1i2;

    if (whites & i1) {
      whites ^= i1i2;
      uint8_t rookTypeMoved = 0;
      uint8_t castled = 0;
      uint8_t enpassant = 0;
      bool kingMoved = false;

      if (i == 5 && !kingMovedWhite && index2 == 6) {
        whites ^= (1ULL << 7) | (1ULL << 5);
        pieces[3] ^= (1ULL << 7) | (1ULL << 5);
        occupied ^= (1ULL << 7) | (1ULL << 5);
        whiteCastled = true;
        castled = 1;
      }

      if (i == 5 && !kingMovedWhite && index2 == 2) {
        whites ^= 1ULL | (1ULL << 3);
        pieces[3] ^= 1ULL | (1ULL << 3);
        occupied ^= 1ULL | (1ULL << 3);
        whiteCastled = true;
        castled = 2;
      }

      if (i == 5 && !kingMovedWhite) {
        kingMovedWhite = true;
        kingMoved = true;
      }

      if (i == 3 && !rookMovedWhiteA && index1 == 0) {
        rookMovedWhiteA = true;
        rookTypeMoved = 1;
      }

      if (i == 3 && !rookMovedWhiteH && index1 == 7) {
        rookMovedWhiteH = true;
        rookTypeMoved = 2;
      }

      if (i == 0 && index2 == index1 + 7) {
        blacks ^= 1ULL << (index2 - 8);
        pieces[0] ^= 1ULL << (index2 - 8);
        occupied ^= 1ULL << (index2 - 8);
        materialScore += pieceValues[0];
        enpassant = 1;
      }

      if (i == 0 && index2 == index1 + 9) {
        blacks ^= 1ULL << (index2 - 8);
        pieces[0] ^= 1ULL << (index2 - 8);
        occupied ^= 1ULL << (index2 - 8);
        materialScore += pieceValues[0];
        enpassant = 2;
      }


      if (i == 0 && index2 > 55) {
        pieces[i] ^= i2;
        pieces[4] ^= i2;
        moveStack.emplace_back(i1, i2, i, -1, 0, true, kingMoved, rookTypeMoved, castled, enpassant);
        materialScore += pieceValues[4] - pieceValues[0];
      }
      else {
        moveStack.emplace_back(i1, i2, i, -1, 0, false, kingMoved, rookTypeMoved, castled, enpassant);
      }

    }
    else if (blacks & i1) {
      blacks ^= i1i2;
      uint8_t rookTypeMoved = 0;
      uint8_t castled = 0;
      uint8_t enpassant = 0;
      bool kingMoved = false;

      if (i == 5 && !kingMovedBlack && index2 == 62) {
        blacks ^= (1ULL << 63) | (1ULL << 61);
        pieces[3] ^= (1ULL << 63) | (1ULL << 61);
        occupied ^= (1ULL << 63) | (1ULL << 61);
        blackCastled = true;
        castled = 3;
      }

      if (i == 5 && !kingMovedBlack && index2 == 58) {
        blacks ^= (1ULL << 56) | (1ULL << 59);
        pieces[3] ^= (1ULL << 56) | (1ULL << 59);
        occupied ^= (1ULL << 56) | (1ULL << 59);
        blackCastled = true;
        castled = 4;
      }

      if (i == 5 && !kingMovedBlack) {
        kingMovedBlack = true;
        kingMoved = true;
      }

      if (i == 3 && !rookMovedBlackA && index1 == 56) {
        rookMovedBlackA = true;
        rookTypeMoved = 3;
      }

      if (i == 3 && !rookMovedBlackH && index1 == 63) {
        rookMovedBlackH = true;
        rookTypeMoved = 4;
      }

      if (i == 0 && index2 == index1 - 7) {
        whites ^= 1ULL << (index2 + 8);
        pieces[0] ^= 1ULL << (index2 + 8);
        occupied ^= 1ULL << (index2 + 8);
        materialScore -= pieceValues[0];
        enpassant = 3;
      }

      if (i == 0 && index2 == index1 - 9) {
        whites ^= 1ULL << (index2 + 8);
        pieces[0] ^= 1ULL << (index2 + 8);
        occupied ^= 1ULL << (index2 + 8);
        materialScore -= pieceValues[0];
        enpassant = 4;
      }

      if (i == 0 && index2 < 8) {
        pieces[i] ^= i2;
        pieces[4] ^= i2;
        moveStack.emplace_back(i1, i2, i, -1, 1, true, kingMoved, rookTypeMoved, castled, enpassant);
        materialScore -= pieceValues[4] - pieceValues[0];
      }
      else {
        moveStack.emplace_back(i1, i2, i, -1, 1, false, kingMoved, rookTypeMoved, castled, enpassant);
      }


    }

    occupied ^= i1i2;
  }
  else {

    if (whites & i1) {
      whites ^= i1i2;
      uint8_t rookTypeMoved = 0;
      bool kingMoved = false;

      assert((blacks & i2) != 0);
      if (blacks & i2) {
        if ((pieces[0] & i2) != 0) {
          k = 0;
        }
        else if ((pieces[1] & i2) != 0) {
          k = 1;
        }
        else if ((pieces[2] & i2) != 0) {
          k = 2;
        }
        else if ((pieces[3] & i2) != 0) {
          k = 3;
        }
        else if ((pieces[4] & i2) != 0) {
          k = 4;
        }
        else if ((pieces[5] & i2) != 0) {
          k = 5;
        }

        assert(k != -1);

        pieces[k] ^= i2;
        blacks ^= i2;

        if (i == 5 && !kingMovedWhite) {
          kingMovedWhite = true;
          kingMoved = true;
        }

        if (i == 3 && !rookMovedWhiteA && index1 == 0) {
          rookMovedWhiteA = true;
          rookTypeMoved = 1;
        }

        if (i == 3 && !rookMovedWhiteH && index1 == 7) {
          rookMovedWhiteH = true;
          rookTypeMoved = 2;
        }

        if (i == 0 && index2 > 55) {
          pieces[i] ^= i2;
          pieces[4] ^= i2;
          moveStack.emplace_back(i1, i2, i, k, 0, true, kingMoved, rookTypeMoved,0 ,0);
          materialScore += pieceValues[4] - pieceValues[0];
        }
        else {
          moveStack.emplace_back(i1, i2, i, k, 0, false, kingMoved, rookTypeMoved,0 ,0);
        }

        materialScore += pieceValues[k];

      }

    }
    else if (blacks & i1) {
      blacks ^= i1i2;
      uint8_t rookTypeMoved = 0;
      bool kingMoved = false;

      assert((whites & i2) != 0);
      if (whites & i2) {

        if ((pieces[0] & i2) != 0) {
          k = 0;
        }
        else if ((pieces[1] & i2) != 0) {
          k = 1;
        }
        else if ((pieces[2] & i2) != 0) {
          k = 2;
        }
        else if ((pieces[3] & i2) != 0) {
          k = 3;
        }
        else if ((pieces[4] & i2) != 0) {
          k = 4;
        }
        else if ((pieces[5] & i2) != 0) {
          k = 5;
        }

        assert(k != -1);

        pieces[k] ^= i2;
        whites ^= i2;

        if (i == 5 && !kingMovedBlack) {
          kingMovedBlack = true;
          kingMoved = true;
        }

        if (i == 3 && !rookMovedBlackA && index1 == 56) {
          rookMovedBlackA = true;
          rookTypeMoved = 3;
        }

        if (i == 3 && !rookMovedBlackH && index1 == 63) {
          rookMovedBlackH = true;
          rookTypeMoved = 4;
        }


        if (i == 0 && index2 < 8) {
          pieces[i] ^= i2;
          pieces[4] ^= i2;
          moveStack.emplace_back(i1, i2, i, k, 1, true, kingMoved, rookTypeMoved, 0, 0);
          materialScore -= pieceValues[4] - pieceValues[0];
        }
        else {
          moveStack.emplace_back(i1, i2, i, k, 1, false, kingMoved, rookTypeMoved, 0, 0);
        }

        materialScore -= pieceValues[k];

      }

    }

    occupied ^= i1;
    pieces[i] ^= i1i2;

  }
  // prevPositions.emplace_back(zobrist.hashBoard(pieces, occupied, blacks, whiteToMove));
  prevPositions.emplace_back(zobrist.hashBoardM(prevPositions.back(), index1, index2, i, k, whiteToMove));

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

  if (enpassant == 1 || enpassant == 2) {
    blacks ^= toLoc >> 8;
    pieces[0] ^= toLoc >> 8;
    occupied ^= toLoc >> 8;
    materialScore -= pieceValues[0];
  }

  if (enpassant == 3 || enpassant == 4) {
    whites ^= toLoc << 8;
    pieces[0] ^= toLoc << 8;
    occupied ^= toLoc << 8;
    materialScore += pieceValues[0];
  }

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

  if (kingMoved && !m.color) {
    kingMovedWhite = false;
  }
  else if (kingMoved && m.color) {
    kingMovedBlack = false;
  }

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


  if (toLoc != fromLoc) {
    pieces[movePiece] ^= i1i2;
    if (!m.color) {
      whites ^= i1i2;
      if (capturePiece != -1) {
        pieces[capturePiece] ^= toLoc;
        blacks ^= toLoc;
        occupied ^= fromLoc;
        materialScore -= pieceValues[capturePiece];
      }
      else {
        occupied ^= i1i2;
      }

      if (promotion) {
        pieces[4] ^= toLoc;
        pieces[0] ^= toLoc;
        materialScore -= pieceValues[4] - pieceValues[0];
      }
    }
    else {
      blacks ^= i1i2;
      if (capturePiece != -1) {
        pieces[capturePiece] ^= toLoc;
        whites ^= toLoc;
        occupied ^= fromLoc;
        materialScore += pieceValues[capturePiece];
      }
      else {
        occupied ^= i1i2;
      }

      if (promotion) {
        pieces[4] ^= toLoc;
        pieces[0] ^= toLoc;
        materialScore += pieceValues[4] - pieceValues[0];
      }

    }
  }

  moveStack.pop_back();
  prevPositions.pop_back();

}


int Bitboard::evaluate() {
  int ret = materialScore;

  ret += evaluateMobility();
  ret += evaluateKingSafety();
  if (whiteCastled) {
    ret += 80;
  }
  if (blackCastled) {
    ret -= 80;
  }

  return ret;
}

int Bitboard::evaluateKingSafety() {
  int ret = 0;
  uint8_t whiteKingIndex = bitScanR(whites & pieces[5]);
  uint8_t blackKingIndex = bitScanR(blacks & pieces[5]);

  uint64_t whiteKnights = whites & pieces[1];
  uint64_t blackKnights = blacks & pieces[1];
  while (whiteKnights) {
    ret -= (chebyshevArray[blackKingIndex][bitScanR(whiteKnights)] * pieceValues[1]) >> 10;
    whiteKnights &= whiteKnights - 1;
  }

  while (blackKnights) {
    ret += (chebyshevArray[whiteKingIndex][bitScanR(blackKnights)] * pieceValues[1]) >> 10;
    blackKnights &= blackKnights - 1;
  }

  uint64_t whiteBishops = whites & pieces[2];
  uint64_t blackBishops = blacks & pieces[2];
  while (whiteBishops) {
    ret -= (chebyshevArray[blackKingIndex][bitScanR(whiteBishops)] * pieceValues[2]) >> 10;
    whiteBishops &= whiteBishops - 1;
  }

  while (blackBishops) {
    ret += (chebyshevArray[whiteKingIndex][bitScanR(blackBishops)] * pieceValues[2]) >> 10;
    blackBishops &= blackBishops - 1;
  }

  uint64_t whiteRooks = whites & pieces[3];
  uint64_t blackRooks = blacks & pieces[3];
  while (whiteRooks) {
    ret -= (manhattanArray[blackKingIndex][bitScanR(whiteRooks)] * pieceValues[3]) >> 10;
    whiteRooks &= whiteRooks - 1;
  }

  while (blackRooks) {
    ret += (manhattanArray[whiteKingIndex][bitScanR(blackRooks)] * pieceValues[3]) >> 10;
    blackRooks &= blackRooks - 1;
  }

  uint64_t whiteQueens = whites & pieces[4];
  uint64_t blackQueens = blacks & pieces[4];
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

int Bitboard::evaluateMobility() {

  int ret = 0;
  int board = 0;

  uint64_t pawnsW = whites & pieces[0];
  uint64_t pawnsB = blacks & pieces[0];

  uint8_t countPiece = count_population(pawnsW);
  for (uint8_t i = 0; i < countPiece; i++) {
    board += whitePawnTable[bitScanR(pawnsW)];
    pawnsW &= pawnsW - 1;
  }


  countPiece = count_population(pawnsB);
  for (uint8_t i = 0; i < countPiece; i++) {
    board -= blackPawnTable[bitScanR(pawnsB)];
    pawnsB &= pawnsB - 1;
  }

  uint64_t knightsW = whites & pieces[1];
  uint64_t knightsB = blacks & pieces[1];
  ret += count_population(knightAttacks(knightsW) & (pawnAttacksBlack(blacks) ^ ALL_ONES));
  ret -= count_population(knightAttacks(knightsB) & (pawnAttacksWhite(whites) ^ ALL_ONES));

  countPiece = count_population(knightsW);
  for (uint8_t i = 0; i < countPiece; i++) {
    board += whiteKnightTable[bitScanR(knightsW)];
    knightsW &= knightsW - 1;
  }

  countPiece = count_population(knightsB);
  for (uint8_t i = 0; i < countPiece; i++) {
    board -= blackKnightTable[bitScanR(knightsB)];
    knightsB &= knightsB - 1;
  }


  uint64_t bishopsW = whites & pieces[2];
  uint64_t bishopsB = blacks & pieces[2];

  countPiece = count_population(bishopsW);
  for (uint8_t i = 0; i < countPiece; i++) {
    uint8_t bscan = bitScanR(bishopsW);
    ret += count_population(magics.bishopAttacksMask(occupied, bscan));
    board += whiteBishopTable[bscan];
    bishopsW &= bishopsW - 1;
  }

  countPiece = count_population(bishopsB);
  for (uint8_t i = 0; i < countPiece; i++) {
    uint8_t bscan = bitScanR(bishopsB);
    ret -= count_population(magics.bishopAttacksMask(occupied, bscan));
    board -= blackBishopTable[bscan];
    bishopsB &= bishopsB - 1;
  }

  uint64_t rooksW = whites & pieces[3];
  uint64_t rooksB = blacks & pieces[3];
  countPiece = count_population(rooksW);
  for (uint8_t i = 0; i < countPiece; i++) {
    uint8_t bscan = bitScanR(rooksW);
    ret += count_population(magics.rookAttacksMask(occupied, bscan));
    board += whiteRookTable[bscan];
    rooksW &= rooksW - 1;
  }

  countPiece = count_population(rooksB);
  for (uint8_t i = 0; i < countPiece; i++) {
    uint8_t bscan = bitScanR(rooksB);
    ret -= count_population(magics.rookAttacksMask(occupied, bscan));
    board -= blackRookTable[bscan];
    rooksB &= rooksB - 1;
  }

  uint64_t queensW = whites & pieces[4];
  uint64_t queensB = blacks & pieces[4];
  countPiece = count_population(queensW);
  for (uint8_t i = 0; i < countPiece; i++) {
    uint8_t bscan = bitScanR(queensW);
    board += whiteQueenTable[bscan];
    ret += count_population(magics.rookAttacksMask(occupied, bscan) | magics.bishopAttacksMask(occupied, bscan)) / 3;
  }

  countPiece = count_population(queensB);
  for (uint8_t i = 0; i < countPiece; i++) {
    uint8_t bscan = bitScanR(queensB);
    board -= blackQueenTable[bscan];
    ret -= count_population(magics.rookAttacksMask(occupied, bscan) | magics.bishopAttacksMask(occupied, bscan)) / 3;
  }

  return ret + board;

}



uint8_t Bitboard::sortMoves(std::vector<Move> &moveList, Move move, int depth) {

  if (moveList.size() <= 1) {
    return 0;
  }


  uint8_t insertIndex = 0;
  std::vector<Move>::iterator p;

  p = std::find(moveList.begin(), moveList.end(), move);
  if (p != moveList.end()) {
    *p = std::move(moveList.back());
    moveList.pop_back();
  }



  //MVV/LVA
  int8_t val = 0;
  for (uint8_t i = insertIndex; i < moveList.size() && insertIndex < moveList.size(); i++) {

    if (!moveList[i].quiet){
      if (moveList[i].pieceTo - moveList[i].pieceFrom > val) {
        val = moveList[i].pieceTo - moveList[i].pieceFrom;
      }
    }

    if (moveList[i].quiet) {
      if (moveList[i] == killerMoves[depth][0].move) {
        moveList[i].score = 1000000;
      }
      else if (moveList[i] == killerMoves[depth][1].move) {
        moveList[i].score = 900000;
      }
      else {
        moveList[i].score = history[moveList[i].fromLoc][moveList[i].toLoc];
      }
    }

  }
  //
  for (uint8_t i = insertIndex; i < moveList.size() && insertIndex < moveList.size(); i++) {

    if (!moveList[i].quiet){
      if (moveList[i].pieceTo - moveList[i].pieceFrom == val) {
        moveList[i].score = 1500000;
      }
    }
  }

  // if (move != Move()) {
  //   p = std::find(moveList.begin(), moveList.end(), move);
  //   if (p != moveList.end()) {
  //     p->score = 2000000;
  //   }
  // }


  // Null Move
  Move nm = {65, 65, 10, 0, false, 1700000};
  moveList.push_back(nm);
  insertIndex++;


  std::sort(moveList.begin(), moveList.end());

  return insertIndex;


}


void Bitboard::InsertKiller(Move move, int depth) {
  KillerMove val = KillerMove();

  killerMoves[depth][1] = killerMoves[depth][0];
  val.move = move;
  val.depth = depth;
  killerMoves[depth][0] = val;


}


void Bitboard::InsertLookup(Move move, int score, int alpha, int beta, int depth, uint8_t flag, uint64_t key) {
  ZobristVal val = ZobristVal();
  move.score = 2000000;
  val.move = move;
  val.score = score;
  val.alpha = alpha;
  val.beta = beta;
  val.depth = depth;
  val.flag = flag;

  lookup[key] = val;

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
  if (std::count(prevPositions.begin(), prevPositions.end(), prevPositions.back()) >= 3) {
    return true;
  }


  return false;
}

uint64_t Bitboard::getPosKey() {
  return prevPositions.back();
}































//
