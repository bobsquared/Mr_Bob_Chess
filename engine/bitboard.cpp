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

  InitMvvLva();
  InitPassedPawnsMask();
  InitIsolatedPawnsMask();
  InitColumnsMask();
  InitRowsMask();
  InitKingZoneMask();

  enemyTerritoryWhite = rowMask[3] | rowMask[4] | rowMask[5];
  enemyTerritoryBlack = rowMask[3] | rowMask[4] | rowMask[2];

  magics = new Magics(rookMoves, bishopMoves);

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


  for (int i = 0; i < 1024; i++) {
    for (int j = 0; j < 2; j++) {
      for (int k = 0; k < 2; k++) {
        killerMoves[k][i][j] = Move();
      }
    }
  }

  moveStack.reserve(1024);
  prevPositions.reserve(1024);
  prevPositions.emplace_back(hashBoard(whiteToMove));

  for (int i = 0; i < 64; i++) {
    for (int j = 0; j < 64; j++) {
      for (int k = 0; j < 2; j++) {
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
  delete magics;
}



void Bitboard::InitDistanceArray() {

  for (int i = 0; i < 64; i++) {
    for (int j = 0; j < 64; j++) {
      int colI = i % 8;
      int rowI = std::floor(i / 8);
      int colJ = j % 8;
      int rowJ = std::floor(j / 8);
      manhattanArray[i][j] = abs(colI - colJ) + abs(rowI - rowJ);
      chebyshevArray[i][j] = std::max(abs(colI - colJ), abs(rowI - rowJ));
    }
  }

}



void Bitboard::InitPieceBoards() {

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
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
      whiteKingTableEG[i * 8 + j] = WHITE_KING_TABLE_EG[(7 - i) * 8 + j];
      blackKingTableEG[i * 8 + j] = BLACK_KING_TABLE_EG[(7 - i) * 8 + j];
    }
  }

}





void Bitboard::InitPassedPawnsMask() {

  for (int i = 0; i < 64; i++) {

    whitePassedPawnMask[i] = 0;
    blackPassedPawnMask[i] = 0;

    if (i % 8 == 0) {

      if (i + 8 <= 63) {
        whitePassedPawnMask[i] |= 1ULL << (i + 8);
        whitePassedPawnMask[i] |= whitePassedPawnMask[i] << 1;
      }

      if (i - 8 >= 0) {
        blackPassedPawnMask[i] |= 1ULL << (i - 8);
        blackPassedPawnMask[i] |= blackPassedPawnMask[i] << 1;
      }

    }
    else if (i % 8 == 7) {

      if (i + 8 <= 63) {
        whitePassedPawnMask[i] |= 1ULL << (i + 8);
        whitePassedPawnMask[i] |= whitePassedPawnMask[i] >> 1;
      }

      if (i - 8 >= 0) {
        blackPassedPawnMask[i] |= 1ULL << (i - 8);
        blackPassedPawnMask[i] |= blackPassedPawnMask[i] >> 1;
      }
    }
    else {

      if (i + 8 <= 63) {
        whitePassedPawnMask[i] |= 1ULL << (i + 8);
        whitePassedPawnMask[i] |= whitePassedPawnMask[i] << 1;
        whitePassedPawnMask[i] |= whitePassedPawnMask[i] >> 1;
      }

      if (i - 8 >= 0) {
        blackPassedPawnMask[i] |= 1ULL << (i - 8);
        blackPassedPawnMask[i] |= blackPassedPawnMask[i] << 1;
        blackPassedPawnMask[i] |= blackPassedPawnMask[i] >> 1;
      }
    }

    whitePassedPawnMask[i] |= whitePassedPawnMask[i] << 8;
    whitePassedPawnMask[i] |= whitePassedPawnMask[i] << 16;
    whitePassedPawnMask[i] |= whitePassedPawnMask[i] << 32;

    blackPassedPawnMask[i] |= blackPassedPawnMask[i] >> 8;
    blackPassedPawnMask[i] |= blackPassedPawnMask[i] >> 16;
    blackPassedPawnMask[i] |= blackPassedPawnMask[i] >> 32;

  }

}



void Bitboard::InitIsolatedPawnsMask() {

  for (int i = 0; i < 64; i++) {

    isolatedPawnMask[i] = 0;

    if (i % 8 == 0) {
      isolatedPawnMask[i] |= 1ULL << (i + 1);
    }
    else if (i % 8 == 7) {
      isolatedPawnMask[i] |= 1ULL << (i - 1);
    }
    else {
      isolatedPawnMask[i] |= 1ULL << (i + 1);
      isolatedPawnMask[i] |= 1ULL << (i - 1);
    }

    isolatedPawnMask[i] |= isolatedPawnMask[i] << 8;
    isolatedPawnMask[i] |= isolatedPawnMask[i] << 16;
    isolatedPawnMask[i] |= isolatedPawnMask[i] << 32;

    isolatedPawnMask[i] |= isolatedPawnMask[i] >> 8;
    isolatedPawnMask[i] |= isolatedPawnMask[i] >> 16;
    isolatedPawnMask[i] |= isolatedPawnMask[i] >> 32;

  }

}



void Bitboard::InitKingZoneMask() {

  for (int i = 0; i < 64; i++) {

    kingZoneMaskWhite[i] = kingMoves[i] | (1ULL << i);
    // kingZoneMaskWhite[i] |= kingZoneMaskWhite[i] << 8;
    // kingZoneMaskWhite[i] |= kingZoneMaskWhite[i] << 8;
    // kingZoneMaskWhite[i] |= kingZoneMaskWhite[i] << 8;

    kingZoneMaskBlack[i] = kingMoves[i] | (1ULL << i);
    // kingZoneMaskBlack[i] |= kingZoneMaskBlack[i] >> 8;
    // kingZoneMaskBlack[i] |= kingZoneMaskBlack[i] >> 8;
    // kingZoneMaskBlack[i] |= kingZoneMaskBlack[i] >> 8;

  }

}



void Bitboard::InitColumnsMask() {

  for (int i = 0; i < 8; i++) {

    columnMask[i] = 1ULL << i;

    columnMask[i] |= columnMask[i] << 8;
    columnMask[i] |= columnMask[i] << 16;
    columnMask[i] |= columnMask[i] << 32;

  }

}



void Bitboard::InitRowsMask() {

  for (int i = 0; i < 8; i++) {

    rowMask[i] = 1ULL << (i * 8);

    rowMask[i] |= rowMask[i] << 1;
    rowMask[i] |= rowMask[i] << 2;
    rowMask[i] |= rowMask[i] << 4;

  }

}



void Bitboard::InitMvvLva() {

  for (int i = 0; i < 6; i++) {
    for (int j = 5; j >= 0; j--) {
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

  for (int i = 0; i < 64; i++) {
    for (int j = 0; j < 64; j++) {
      for (int k = 0; j < 2; j++) {
        history[k][i][j] = 0;
      }
    }
  }


  for (int i = 0; i < 1024; i++) {
    for (int j = 0; j < 2; j++) {
      for (int k = 0; k < 2; k++) {
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
  for (int i = 0; i < 64; i++) {
    uint64_t tempBitBoard = 1ULL << i;
    tempBitBoard |= tempBitBoard >> 8;

    if (i < 56 && i > 47) {
      tempBitBoard |= tempBitBoard >> 8;
    }
    tempBitBoard ^= 1ULL << i;
    blackPawnMoves[i] = tempBitBoard;
  }

  for (int i = 0; i < 64; i++) {
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
  for (int i = 0; i < 64; i++) {
    uint64_t tempBitBoard = 1ULL << i;
    tempBitBoard |= tempBitBoard << 8;

    if (i < 16 && i > 7) {
      tempBitBoard |= tempBitBoard << 8;
    }
    tempBitBoard ^= 1ULL << i;
    whitePawnMoves[i] = tempBitBoard;
  }

  for (int i = 0; i < 64; i++) {
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

  for (int i = 0; i < 64; i++) {
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

  for (int i = 0; i < 64; i++) {
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

  for (int i = 0; i < 64; i++) {
    uint64_t tempBitBoard = 0;
    uint64_t tempBitBoard1 = 0;
    uint64_t tempBitBoard2 = 0;
    uint64_t tempBitBoard3 = 0;
    uint64_t tempBitBoard4 = 0;

    tempBitBoard |= 1ULL << i;
    uint8_t col = i % 8;

    for (int k = 0; k < 8; k++) {
      tempBitBoard1 |= tempBitBoard >> (8 + k * 8);
      tempBitBoard2 |= ((tempBitBoard << (8 + k * 8)) & ALL_ONES);
    }

    for (int k = col; k < 8; k++) {
      if (k == 7) {
        break;
      }
      else {
        tempBitBoard3 |= ((tempBitBoard << (1 + 1 * (k - col))) & ALL_ONES);
      }
    }

    for (int k = col; k >= 0; k--) {
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

  for (int i = 0; i < 64; i++) {
    queenMoves[i] = rookMoves[i] | bishopMoves[i];
  }

  whiteQueens = 1ULL << 3;
  blackQueens = 1ULL << 59;

}



void Bitboard::InitKingMoves() {

  for (int i = 0; i < 64; i++) {
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



uint64_t Bitboard::pawnAttacksMan(uint64_t bitboard, bool isWhite) {

  if (isWhite) {
    uint64_t tempBitBoard1 = (bitboard << 9);
    uint64_t tempBitBoard2 = (bitboard << 7);

    tempBitBoard1 &= LEFT_MASK;
    tempBitBoard2 &= RIGHT_MASK;
    return tempBitBoard1 | tempBitBoard2;
  }
  else {
    uint64_t tempBitBoard1 = (bitboard >> 9);
    uint64_t tempBitBoard2 = (bitboard >> 7);

    tempBitBoard1 &= RIGHT_MASK;
    tempBitBoard2 &= LEFT_MASK;
    return tempBitBoard1 | tempBitBoard2;
  }

}



uint64_t Bitboard::pawnAttacks(bool isWhite) {

  if (isWhite) {
    uint64_t bitboard = pieces[0] & whites;
    uint64_t tempBitBoard1 = (bitboard << 9);
    uint64_t tempBitBoard2 = (bitboard << 7);

    tempBitBoard1 &= LEFT_MASK;
    tempBitBoard2 &= RIGHT_MASK;
    return tempBitBoard1 | tempBitBoard2;
  }
  else {
    uint64_t bitboard = pieces[0] & blacks;
    uint64_t tempBitBoard1 = (bitboard >> 9);
    uint64_t tempBitBoard2 = (bitboard >> 7);

    tempBitBoard1 &= RIGHT_MASK;
    tempBitBoard2 &= LEFT_MASK;
    return tempBitBoard1 | tempBitBoard2;
  }

}



uint64_t Bitboard::xrayAttackRook(uint64_t blockers, int index) {
  uint64_t bb = magics->rookAttacksMask(occupied, index);
  bb &= blockers;
  return magics->rookAttacksMask(occupied ^ bb, index);
}



uint64_t Bitboard::xrayAttackBishop(uint64_t blockers, int index) {
  uint64_t bb = magics->bishopAttacksMask(occupied, index);
  bb &= blockers;
  return magics->bishopAttacksMask(occupied ^ bb, index);
}



uint64_t Bitboard::xrayAttackQueen(uint64_t blockers, int index) {
  uint64_t bb = magics->bishopAttacksMask(occupied, index) | magics->rookAttacksMask(occupied, index);
  bb &= blockers;
  return magics->bishopAttacksMask(occupied & bb, index) | magics->rookAttacksMask(occupied & bb, index);
}



uint64_t Bitboard::xrayAttackRookSee(uint64_t blockers, int index) {
  uint64_t bb = magics->rookAttacksMask(occupied, index);
  return bb ^ magics->rookAttacksMask(occupied ^ (bb & blockers), index);
}



uint64_t Bitboard::xrayAttackBishopSee(uint64_t blockers, int index) {
  uint64_t bb = magics->bishopAttacksMask(occupied, index);
  return bb ^ magics->bishopAttacksMask(occupied ^ (bb & blockers), index);
}



uint64_t Bitboard::xrayAttackQueenSee(uint64_t blockers, int index) {
  uint64_t bb = magics->bishopAttacksMask(occupied, index) | magics->rookAttacksMask(occupied, index);
  bb &= blockers;
  return magics->bishopAttacksMask(occupied & bb, index) | magics->rookAttacksMask(occupied & bb, index);
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
int Bitboard::bitScanR(uint64_t bitboard) {
  return MSB_TABLE[((bitboard ^ (bitboard - 1)) * 0x03f79d71b4cb0a89) >> 58];
}



int Bitboard::count_population(uint64_t bitboard) {

  int count = 0;
  while (bitboard) {
    count++;
    bitboard &= bitboard - 1;
  }

  return count;
}



void Bitboard::printBoard(uint64_t board) {

  std::bitset<64> x(board);
  for (int i = 7; i >= 0; i--) {
    for (int j = 7; j >= 0; j--) {
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

  for (int i = 7; i >= 0; i--) {
    for (int j = 7; j >= 0; j--) {
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

    uint64_t indexP = pieces[5] & whites;
    int index = bitScanR(indexP);
    assert ((1ULL << index) == indexP);

    uint64_t bishopAttacksMaskI = magics->bishopAttacksMask(occupied, index);
    uint64_t rookAttacksMaskI = magics->rookAttacksMask(occupied, index);
    ret = pieces[0] & whitePawnAttacks[index];
    ret |= pieces[1] & knightMoves[index];
    ret |= pieces[2] & bishopAttacksMaskI;
    ret |= pieces[3] & rookAttacksMaskI;
    ret |= pieces[4] & (bishopAttacksMaskI | rookAttacksMaskI);
    ret |= pieces[5] & kingMoves[index];
    ret &= blacks;

  }
  else {

    uint64_t indexP = pieces[5] & blacks;
    int index = bitScanR(indexP);
    assert ((1ULL << index) == indexP);

    uint64_t bishopAttacksMaskI = magics->bishopAttacksMask(occupied, index);
    uint64_t rookAttacksMaskI = magics->rookAttacksMask(occupied, index);
    ret = pieces[0] & blackPawnAttacks[index];
    ret |= pieces[1] & knightMoves[index];
    ret |= pieces[2] & bishopAttacksMaskI;
    ret |= pieces[3] & rookAttacksMaskI;
    ret |= pieces[4] & (bishopAttacksMaskI | rookAttacksMaskI);
    ret |= pieces[5] & kingMoves[index];
    ret &= whites;

  }

  return ret == 0;

}



bool Bitboard::isAttacked(int index, bool color) {

  uint64_t ret = 0;
  uint64_t bishopAttacksMaskI = magics->bishopAttacksMask(occupied, index);
  uint64_t rookAttacksMaskI = magics->rookAttacksMask(occupied, index);
  ret = pieces[0] & color? whitePawnAttacks[index] : blackPawnAttacks[index];
  ret |= pieces[1] & knightMoves[index];
  ret |= pieces[2] & bishopAttacksMaskI;
  ret |= pieces[3] & rookAttacksMaskI;
  ret |= pieces[4] & (bishopAttacksMaskI | rookAttacksMaskI);
  ret |= pieces[5] & kingMoves[index];
  ret &= color? whites : blacks;

  return !ret;

}



uint64_t Bitboard::isAttackedSee(int index) {

  uint64_t ret = 0;
  uint64_t bishopAttacksMaskI = magics->bishopAttacksMask(occupied, index);
  uint64_t rookAttacksMaskI = magics->rookAttacksMask(occupied, index);
  ret |= pieces[0] & whitePawnAttacks[index];
  ret |= pieces[0] & blackPawnAttacks[index];
  ret |= pieces[1] & knightMoves[index];
  ret |= pieces[2] & bishopAttacksMaskI;
  ret |= pieces[3] & rookAttacksMaskI;
  ret |= pieces[4] & (bishopAttacksMaskI | rookAttacksMaskI);
  ret |= pieces[5] & kingMoves[index];

  return ret;

}



std::vector<Bitboard::Move> Bitboard::allValidMoves(bool color) {

  std::vector<Move> ret = {};
  ret.reserve(128);

  bool quiet = true;
  bool isEnpassant;

  int pieceFC = -1;
  int pieceTC = -1;
  int promotion = 0;

  if (color == 0){

    std::vector<int> loc = whitePiecesLoc();
    for (int i : loc) {

      if ((1ULL << i) & pieces[0]) {
        pieceFC = 0;
      }
      else if ((1ULL << i) & pieces[1]) {
        pieceFC = 1;
      }
      else if ((1ULL << i) & pieces[2]) {
        pieceFC = 2;
      }
      else if ((1ULL << i) & pieces[3]) {
        pieceFC = 3;
      }
      else if ((1ULL << i) & pieces[4]) {
        pieceFC = 4;
      }
      else if ((1ULL << i) & pieces[5]) {
        pieceFC = 5;
      }

      std::vector<int> move = validMovesWhite(i);

      for (int j : move) {

        if ((1ULL << j) & occupied) {

          if ((1ULL << j) & pieces[0]) {
            pieceTC = 0;
          }
          else if ((1ULL << j) & pieces[1]) {
            pieceTC = 1;
          }
          else if ((1ULL << j) & pieces[2]) {
            pieceTC = 2;
          }
          else if ((1ULL << j) & pieces[3]) {
            pieceTC = 3;
          }
          else if ((1ULL << j) & pieces[4]) {
            pieceTC = 4;
          }
          else if ((1ULL << j) & pieces[5]) {
            pieceTC = 5;
          }

          quiet = false;
          promotion = (pieceFC == 0 && j > 55) ? 4 : 0;
          isEnpassant = false;

        }
        else if (pieceFC == 0 && j > 55) {
          quiet = false;
          pieceTC = -1;
          promotion = 4;
          isEnpassant = false;
        }
        else if (pieceFC == 0 && (j == i + 7 || j == i + 9) && ((1ULL << j) & occupied) == 0) {
          quiet = false;
          pieceTC = -1;
          promotion = 0;
          isEnpassant = true;
        }
        else {
          quiet = true;
          pieceTC = -1;
          promotion = 0;
          isEnpassant = false;
        }

        ret.emplace_back(i, j, quiet, pieceFC, pieceTC, 0, promotion, isEnpassant);
      }

    }

  }
  else {

    std::vector<int> loc = blackPiecesLoc();
    for (int i : loc) {

      uint64_t i64 = 1ULL << i;

      if (i64 & pieces[0]) {
        pieceFC = 0;
      }
      else if (i64 & pieces[1]) {
        pieceFC = 1;
      }
      else if (i64 & pieces[2]) {
        pieceFC = 2;
      }
      else if (i64 & pieces[3]) {
        pieceFC = 3;
      }
      else if (i64 & pieces[4]) {
        pieceFC = 4;
      }
      else if (i64 & pieces[5]) {
        pieceFC = 5;
      }

      std::vector<int> move = validMovesBlack(i);

      for (int j : move) {

        uint64_t j64 = 1ULL << j;

        if (j64 & occupied) {

          if (j64 & pieces[0]) {
            pieceTC = 0;
          }
          else if (j64 & pieces[1]) {
            pieceTC = 1;
          }
          else if (j64 & pieces[2]) {
            pieceTC = 2;
          }
          else if (j64 & pieces[3]) {
            pieceTC = 3;
          }
          else if (j64 & pieces[4]) {
            pieceTC = 4;
          }
          else if (j64 & pieces[5]) {
            pieceTC = 5;
          }

          quiet = false;
          promotion = (pieceFC == 0 && j < 8) ? 4 : 0;
          isEnpassant = false;
        }
        else if (pieceFC == 0 && j < 8) {
          quiet = false;
          pieceTC = -1;
          promotion = 4;
          isEnpassant = false;
        }
        else if (pieceFC == 0 && (j == i - 7 || j == i - 9) && ((1ULL << j) & occupied) == 0) {
          quiet = false;
          pieceTC = -1;
          promotion = 0;
          isEnpassant = true;
        }
        else {
          quiet = true;
          pieceTC = -1;
          promotion = 0;
          isEnpassant = false;
        }

        ret.emplace_back(i, j, quiet, pieceFC, pieceTC, 0, promotion, isEnpassant);
      }

    }

  }

  return ret;

}



std::vector<Bitboard::Move> Bitboard::allValidCaptures(bool color) {

  std::vector<Move> ret = {};
  ret.reserve(128);

  bool quiet = true;
  int pieceFC = -1;
  int pieceTC = -1;
  int promotion = 0;
  bool isEnpassant;

  if (color == 0){

    std::vector<int> loc = whitePiecesLoc();
    for (int i : loc) {

      uint64_t i64 = 1ULL << i;

      if (i64 & pieces[0]) {
        pieceFC = 0;
      }
      else if (i64 & pieces[1]) {
        pieceFC = 1;
      }
      else if (i64 & pieces[2]) {
        pieceFC = 2;
      }
      else if (i64 & pieces[3]) {
        pieceFC = 3;
      }
      else if (i64 & pieces[4]) {
        pieceFC = 4;
      }
      else if (i64 & pieces[5]) {
        pieceFC = 5;
      }

      std::vector<int> move = validMovesWhite(i);

      for (int j : move) {

        uint64_t j64 = 1ULL << j;

        if (j64 & occupied) {

          if (j64 & pieces[0]) {
            pieceTC = 0;
          }
          else if (j64 & pieces[1]) {
            pieceTC = 1;
          }
          else if (j64 & pieces[2]) {
            pieceTC = 2;
          }
          else if (j64 & pieces[3]) {
            pieceTC = 3;
          }
          else if (j64 & pieces[4]) {
            pieceTC = 4;
          }
          else if (j64 & pieces[5]) {
            pieceTC = 5;
          }

          quiet = false;
          promotion = (pieceFC == 0 && j > 55) ? 4 : 0;
          isEnpassant = false;
          ret.emplace_back(i, j, quiet, pieceFC, pieceTC, 0, promotion, isEnpassant);

        }
        else if (pieceFC == 0 && j > 55) {
          quiet = false;
          pieceTC = -1;
          promotion = 4;
          isEnpassant = false;
          ret.emplace_back(i, j, quiet, pieceFC, pieceTC, 0, promotion, isEnpassant);
        }
        else if (pieceFC == 0 && (j == i + 7 || j == i + 9) && ((1ULL << j) & occupied) == 0) {
          quiet = false;
          pieceTC = -1;
          promotion = 0;
          isEnpassant = true;
          ret.emplace_back(i, j, quiet, pieceFC, pieceTC, 0, promotion, isEnpassant);
        }

      }

    }

  }
  else {

    std::vector<int> loc = blackPiecesLoc();
    for (int i : loc) {

      if ((1ULL << i) & pieces[0]) {
        pieceFC = 0;
      }
      else if ((1ULL << i) & pieces[1]) {
        pieceFC = 1;
      }
      else if ((1ULL << i) & pieces[2]) {
        pieceFC = 2;
      }
      else if ((1ULL << i) & pieces[3]) {
        pieceFC = 3;
      }
      else if ((1ULL << i) & pieces[4]) {
        pieceFC = 4;
      }
      else if ((1ULL << i) & pieces[5]) {
        pieceFC = 5;
      }

      std::vector<int> move = validMovesBlack(i);

      for (int j : move) {

        if ((1ULL << j) & occupied) {

          if ((1ULL << j) & pieces[0]) {
            pieceTC = 0;
          }
          else if ((1ULL << j) & pieces[1]) {
            pieceTC = 1;
          }
          else if ((1ULL << j) & pieces[2]) {
            pieceTC = 2;
          }
          else if ((1ULL << j) & pieces[3]) {
            pieceTC = 3;
          }
          else if ((1ULL << j) & pieces[4]) {
            pieceTC = 4;
          }
          else if ((1ULL << j) & pieces[5]) {
            pieceTC = 5;
          }

          quiet = false;
          promotion = (pieceFC == 0 && j < 8) ? 4 : 0;
          isEnpassant = false;
          ret.emplace_back(i, j, quiet, pieceFC, pieceTC, 0, promotion, isEnpassant);
        }
        else if (pieceFC == 0 && j < 8) {
          quiet = false;
          pieceTC = -1;
          promotion = 4;
          isEnpassant = false;
          ret.emplace_back(i, j, quiet, pieceFC, pieceTC, 0, promotion, isEnpassant);
        }
        else if (pieceFC == 0 && (j == i - 7 || j == i - 9) && ((1ULL << j) & occupied) == 0) {
          quiet = false;
          pieceTC = -1;
          promotion = 0;
          isEnpassant = true;
          ret.emplace_back(i, j, quiet, pieceFC, pieceTC, 0, promotion, isEnpassant);
        }

      }

    }

  }

  return ret;

}



std::vector<int> Bitboard::whitePiecesLoc() {
  std::vector<int> ret = {};
  ret.reserve(16);
  uint64_t bitboard = whites;
  while (bitboard != 0){
    int toApp = bitScanR(bitboard);
    bitboard &= bitboard - 1;
    ret.emplace_back(toApp);
  }
  return ret;
}



std::vector<int> Bitboard::blackPiecesLoc() {
  std::vector<int> ret = {};
  ret.reserve(16);
  uint64_t bitboard = blacks;
  while (bitboard != 0){
    int toApp = bitScanR(bitboard);
    bitboard &= bitboard - 1;
    ret.emplace_back(toApp);
  }
  return ret;
}



std::vector<int> Bitboard::validMovesWhite(int index) {

  uint64_t whitesIndex = 1ULL << index;
  std::vector<int> ret = {};
  ret.reserve(32);


  if (pieces[0] & whitesIndex) {

    uint64_t base = (whitePawnAttacks[index] & blacks) | (whitePawnMoves[index] & ~occupied);
    if (occupied & (1ULL << (index + 8))){
      base &= (1ULL << (index + 16)) ^ 18446744073709551615ULL;
    }

    int enpass = enpassantConditions(true, index);
    if (enpass) {
      ret.emplace_back(enpass);
    }

    while (base) {
      int toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }

    return ret;
  }
  else if (pieces[1] & whitesIndex) {

    uint64_t base = knightMoves[index] & ~whites;
    while (base) {
      int toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if (pieces[2] & whitesIndex) {

    uint64_t base = magics->bishopAttacksMask(occupied, index) & ~whites;
    while (base) {
      int toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if (pieces[3] & whitesIndex) {

    uint64_t base = magics->rookAttacksMask(occupied, index) & ~whites;
    while (base) {
      int toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if (pieces[4] & whitesIndex) {

    uint64_t base = (magics->bishopAttacksMask(occupied, index) | magics->rookAttacksMask(occupied, index)) & ~whites;
    while (base) {
      int toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if (pieces[5] & whitesIndex) {

    uint64_t base = kingMoves[index] & ~whites;
    if (canCastleK(true)) {
      ret.emplace_back(6);
    }

    if (canCastleQ(true)) {
      ret.emplace_back(2);
    }

    while (base) {
      int toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }
    return ret;
  }

  return ret;
}



bool Bitboard::IsMoveWhite(Move &move) {

  int index = move.fromLoc;
  int index2 = move.toLoc;
  uint64_t indexP2 = 1ULL << index2;
  uint64_t whitesIndex = 1ULL << index;

  if (pieces[0] & whitesIndex) {

    uint64_t base = (whitePawnAttacks[index] & blacks) | (whitePawnMoves[index] & ~occupied);
    if ((occupied & (1ULL << (index + 8))) != 0){
      base &= (1ULL << (index + 16)) ^ 18446744073709551615U;
    }

    int enpass = enpassantConditions(true, index);
    if (enpass && index2 == enpass) {
      return true;
    }

    return (base & indexP2) != 0;
  }
  else if (pieces[1] & whitesIndex) {
    uint64_t base = knightMoves[index] & ~whites;
    return (base & indexP2) != 0;
  }
  else if (pieces[2] & whitesIndex) {
    uint64_t base = magics->bishopAttacksMask(occupied, index) & ~whites;
    return (base & indexP2) != 0;
  }
  else if (pieces[3] & whitesIndex) {
    uint64_t base = magics->rookAttacksMask(occupied, index) & ~whites;
    return (base & indexP2) != 0;
  }
  else if (pieces[4] & whitesIndex) {
    uint64_t base = (magics->bishopAttacksMask(occupied, index) | magics->rookAttacksMask(occupied, index)) & ~whites;
    return (base & indexP2) != 0;
  }
  else if (pieces[5] & whitesIndex) {
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



std::vector<int> Bitboard::validMovesBlack(int index) {

  uint64_t blacksIndex = 1ULL << index;
  std::vector<int> ret = {};
  ret.reserve(32);

  if (pieces[0] & blacksIndex) {

    uint64_t base = (blackPawnAttacks[index] & whites) | (blackPawnMoves[index] & ~occupied);
    if ((index >= 16 && (occupied & (1ULL << (index - 8))) != 0)){
      base &= (1ULL << (index - 16)) ^ 18446744073709551615U;
    }

    int enpass = enpassantConditions(false, index);
    if (enpass) {
      ret.emplace_back(enpass);
    }

    while (base) {
      int toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if (pieces[1] & blacksIndex) {

    uint64_t base = knightMoves[index] & ~blacks;
    while (base) {
      int toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if (pieces[2] & blacksIndex) {

    uint64_t base = magics->bishopAttacksMask(occupied, index) & ~blacks;
    while (base) {
      int toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if (pieces[3] & blacksIndex) {

    uint64_t base = magics->rookAttacksMask(occupied, index) & ~blacks;
    while (base) {
      int toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if (pieces[4] & blacksIndex) {

    uint64_t base = (magics->bishopAttacksMask(occupied, index) | magics->rookAttacksMask(occupied, index)) & ~blacks;
    while (base) {
      int toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }
    return ret;
  }
  else if (pieces[5] & blacksIndex) {
    uint64_t base = kingMoves[index] & ~blacks;

    if (canCastleK(false)) {
        ret.emplace_back(62);
    }

    if (canCastleQ(false)) {
        ret.emplace_back(58);
    }

    while (base) {
      int toApp = bitScanR(base);
      base &= base - 1;
      ret.emplace_back(toApp);
    }
    return ret;
  }

  return ret;
}



bool Bitboard::IsMoveBlack(Move &move) {
  int index = move.fromLoc;
  int index2 = move.toLoc;
  uint64_t indexP2 = 1ULL << index2;
  uint64_t blacksIndex = 1ULL << index;


  if (pieces[0] & blacksIndex) {
    uint64_t base = (blackPawnAttacks[index] & whites) | (blackPawnMoves[index] & ~occupied);
    if ((index >= 16 && (occupied & (1ULL << (index - 8))) != 0)){
      base &= (1ULL << (index - 16)) ^ 18446744073709551615U;
    }

    int enpass = enpassantConditions(false, index);
    if (enpass && index2 == enpass) {
      return true;
    }

    return (base & indexP2) != 0;
  }
  else if (pieces[1] & blacksIndex) {
    uint64_t base = knightMoves[index] & ~blacks;
    return (base & indexP2) != 0;
  }
  else if (pieces[2] & blacksIndex) {
    uint64_t base = magics->bishopAttacksMask(occupied, index) & ~blacks;
    return (base & indexP2) != 0;
  }
  else if (pieces[3] & blacksIndex) {
    uint64_t base = magics->rookAttacksMask(occupied, index) & ~blacks;
    return (base & indexP2) != 0;
  }
  else if (pieces[4] & blacksIndex) {
    uint64_t base = (magics->bishopAttacksMask(occupied, index) | magics->rookAttacksMask(occupied, index)) & ~blacks;
    return (base & indexP2) != 0;
  }
  else if (pieces[5] & blacksIndex) {
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
  if (move.fromLoc == 65) {
    prevPositions.emplace_back(zobrist.hashBoardM(prevPositions.back(), move.fromLoc, move.toLoc, 0, 0, whiteToMove, false));
    moveStack.emplace_back(65, 65, 10, 10, 0, false, false, false, 0, 0);
    return;
  }

  uint64_t i1 = 1ULL << move.fromLoc;
  uint64_t i2 = 1ULL << move.toLoc;
  uint64_t i1i2 = i1 ^ i2;
  int i = move.pieceFrom;
  int k = move.pieceTo;
  bool useWhite;

  int rookTypeMoved = 0;
  int castled = 0;
  int enpassant = 0;
  bool kingMoved = false;
  bool promotion = false;

  assert(i != -1);

  useWhite = whites & i1 ? true : false;


  // Any pawn moves. Captures may happen here
  if (i == 0) {

    // STEP 1: Check for any promotions
    if (move.toLoc > 55) {
      pieces[i] ^= i2;
      pieces[4] ^= i2;
      promotion = true;
      materialScore += pieceValues[4] - pieceValues[0];
      countPawnsW--;
      countQueensW++;
    }
    else if (move.toLoc < 8) {
      pieces[i] ^= i2;
      pieces[4] ^= i2;
      promotion = true;
      materialScore -= pieceValues[4] - pieceValues[0];
      countPawnsB--;
      countQueensB++;
    }



    // STEP 2: Check for any enpassants
    if (useWhite && move.isEnpassant){

      blacks ^= 1ULL << (move.toLoc - 8);
      pieces[0] ^= 1ULL << (move.toLoc - 8);
      occupied ^= 1ULL << (move.toLoc - 8);
      materialScore += pieceValues[0];
      countPawnsB--;

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
      countPawnsW--;

      if (move.toLoc == move.fromLoc - 7) {
        enpassant = 3;
      }
      else if (move.toLoc == move.fromLoc - 9) {
        enpassant = 4;
      }
    }
  }



  // STEP 3: Check if any rooks moved
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


  // STEP 4: Check for white castling or if its a king move
  else if (i == 5){
    if (useWhite && !kingMovedWhite) {

      kingMovedWhite = true;
      kingMoved = true;

      // King side castle
      if (move.toLoc == 6) {
        whites ^= (1ULL << 7) | (1ULL << 5);
        pieces[3] ^= (1ULL << 7) | (1ULL << 5);
        occupied ^= (1ULL << 7) | (1ULL << 5);
        whiteCastled = true;
        castled = 1;
      }
      // Queen side Castle
      else if (move.toLoc == 2) {
        whites ^= 1ULL | (1ULL << 3);
        pieces[3] ^= 1ULL | (1ULL << 3);
        occupied ^= 1ULL | (1ULL << 3);
        whiteCastled = true;
        castled = 2;
      }
    }
    else if (!useWhite && !kingMovedBlack){

      kingMovedBlack = true;
      kingMoved = true;

      // King side castle
      if (move.toLoc == 62) {
        blacks ^= (1ULL << 63) | (1ULL << 61);
        pieces[3] ^= (1ULL << 63) | (1ULL << 61);
        occupied ^= (1ULL << 63) | (1ULL << 61);
        blackCastled = true;
        castled = 3;
      }
      // Queen side Castle
      else if (move.toLoc == 58) {
        blacks ^= (1ULL << 56) | (1ULL << 59);
        pieces[3] ^= (1ULL << 56) | (1ULL << 59);
        occupied ^= (1ULL << 56) | (1ULL << 59);
        blackCastled = true;
        castled = 4;
      }
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

      switch (k) {
        case 0:
          countPawnsB--;
          break;
        case 1:
          countKnightsB--;
          break;
        case 2:
          countBishopsB--;
          break;
        case 3:
          countRooksB--;
          break;
        case 4:
          countQueensB--;
          break;
      }
    }
    else {
      blacks ^= i1i2;
      whites ^= i2;
      materialScore -= pieceValues[k];

      switch (k) {
        case 0:
          countPawnsW--;
          break;
        case 1:
          countKnightsW--;
          break;
        case 2:
          countBishopsW--;
          break;
        case 3:
          countRooksW--;
          break;
        case 4:
          countQueensW--;
          break;
      }
    }

    pieces[k] ^= i2;
    pieces[i] ^= i1i2;
    occupied ^= i1;

  }

  moveStack.emplace_back(i1, i2, i, k, !useWhite, promotion, kingMoved, rookTypeMoved, castled, enpassant);
  prevPositions.emplace_back(zobrist.hashBoardM(prevPositions.back(), move.fromLoc, move.toLoc, i, k, useWhite, move.isEnpassant));

}



void Bitboard::undoMove() {

  whiteToMove = !whiteToMove;
  MoveStack m = moveStack.back();
  uint64_t fromLoc = m.fromLoc;
  uint64_t toLoc = m.toLoc;

  if (fromLoc == 65) {
    moveStack.pop_back();
    prevPositions.pop_back();
    return;
  }


  int movePiece = m.movePiece;
  int capturePiece = m.capturePiece;
  bool promotion = m.promote;

  bool kingMoved = m.kingMoved;
  int rookMoved = m.rookMoved;
  int castled = m.castled;
  int enpassant = m.enpassant;

  uint64_t i1i2 = fromLoc ^ toLoc;

  // STEP 1: Check if Enpassants
  if (enpassant){
    if (enpassant == 1 || enpassant == 2) {
      blacks ^= toLoc >> 8;
      pieces[0] ^= toLoc >> 8;
      occupied ^= toLoc >> 8;
      materialScore -= pieceValues[0];
      countPawnsB++;
    }
    else if (enpassant == 3 || enpassant == 4) {
      whites ^= toLoc << 8;
      pieces[0] ^= toLoc << 8;
      occupied ^= toLoc << 8;
      materialScore += pieceValues[0];
      countPawnsW++;
    }
  }

  // STEP 2: Check if castling
  if (castled){
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
  }

  // STEP 3: Check if King moved
  if (kingMoved){
    if (!m.color) {
      kingMovedWhite = false;
    }
    else if (m.color) {
      kingMovedBlack = false;
    }
  }

  // STEP 4: Check if rook moved
  if (rookMoved){
    switch (rookMoved) {

    case 1:
      rookMovedWhiteA = false;
      break;
    case 2:
      rookMovedWhiteH = false;
      break;
    case 3:
      rookMovedBlackA = false;
      break;
    case 4:
      rookMovedBlackH = false;
      break;
    }
  }

  // STEP 5: Check for promotions
  if (promotion){
    if (!m.color) {
      pieces[4] ^= toLoc;
      pieces[0] ^= toLoc;
      materialScore -= pieceValues[4] - pieceValues[0];
      countPawnsW++;
      countQueensW--;
    }
    else if (m.color) {
      pieces[4] ^= toLoc;
      pieces[0] ^= toLoc;
      materialScore += pieceValues[4] - pieceValues[0];
      countPawnsB++;
      countQueensB--;
    }
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

        switch (capturePiece) {
          case 0:
            countPawnsB++;
            break;
          case 1:
            countKnightsB++;
            break;
          case 2:
            countBishopsB++;
            break;
          case 3:
            countRooksB++;
            break;
          case 4:
            countQueensB++;
            break;
        }
      }
      else {
        blacks ^= i1i2;
        whites ^= toLoc;
        materialScore += pieceValues[capturePiece];

        switch (capturePiece) {
          case 0:
            countPawnsW++;
            break;
          case 1:
            countKnightsW++;
            break;
          case 2:
            countBishopsW++;
            break;
          case 3:
            countRooksW++;
            break;
          case 4:
            countQueensW++;
            break;
        }
      }
      pieces[capturePiece] ^= toLoc;
      occupied ^= fromLoc;
    }
  }

  moveStack.pop_back();
  prevPositions.pop_back();

}



int Bitboard::evaluate() {

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

  ret += whiteToMove? 12 : -12;
  ret += evaluateImbalance();


  bool endgame = false;
  int whiteKingIndex = bitScanR(whites & pieces[5]);
  int blackKingIndex = bitScanR(blacks & pieces[5]);

  uint64_t whiteRooks = whites & pieces[3];
  uint64_t blackRooks = blacks & pieces[3];

  uint64_t whiteQueens = whites & pieces[4];
  uint64_t blackQueens = blacks & pieces[4];

  if (countKnightsW + countBishopsW + countRooksW + countQueensW <= 2 && countKnightsB + countBishopsB + countRooksB + countQueensB <= 2) {
    endgame = true;
  }

  ret += evaluateMobility(whiteKingIndex, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, endgame, true) - evaluateMobility(blackKingIndex, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, endgame, false);
  ret += evaluateKingSafety(blackKingIndex, ~(whitePawns | pawnAttacks(false)), whiteKnights, whiteBishops, whiteRooks, whiteQueens, true) - evaluateKingSafety(whiteKingIndex, ~(blackPawns | pawnAttacks(true)), blackKnights, blackBishops, blackRooks, blackQueens, false);
  ret += evaluatePawns(whitePawns, blackPawns, endgame, true) - evaluatePawns(blackPawns, whitePawns, endgame, false);
  ret += evaluateOutposts(whiteKnights, whiteBishops, blackPawns, endgame, true) - evaluateOutposts(blackKnights, blackBishops, whitePawns, endgame, false);
  ret += evaluateThreats(whitePawns, endgame, true) - evaluateThreats(blackPawns, endgame, false);
  ret += evaluateTrappedPieces(whiteKingIndex, whiteRooks, true) - evaluateTrappedPieces(blackKingIndex, blackRooks, false);
  ret += evaluateKingPawnEndgame(whiteKingIndex, whitePawns, blackPawns, endgame, true) - evaluateKingPawnEndgame(blackKingIndex, blackPawns, whitePawns, endgame, false);

  ret += knightWeight[countPawnsW] * countKnightsW;
  ret -= knightWeight[countPawnsB] * countKnightsB;

  ret += rookWeight[countPawnsW] * countRooksW;
  ret -= rookWeight[countPawnsB] * countRooksB;


  return whiteToMove? ret : -ret;
}



void Bitboard::evaluateDebug() {

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

  bool endgame = false;
  if (countKnightsW + countBishopsW + countRooksW + countQueensW <= 2 && countKnightsB + countBishopsB + countRooksB + countQueensB <= 2) {
    endgame = true;
  }

  int ret = materialScore;

  std::vector<Move> moves = allValidMoves(false);
  for (Move move : moves) {
    std::cout << TO_ALG[move.fromLoc] << TO_ALG[move.toLoc] << std::endl;
    if (move.toLoc == 36 && move.pieceTo != -1) {
      std::cout << TO_ALG[move.fromLoc] << TO_ALG[move.toLoc] << std::endl;
      std::cout << "See: " << seeCaptureNew(move) << std::endl;
    }
  }

  uint64_t whitePawns = whites & pieces[0];
  uint64_t blackPawns = blacks & pieces[0];

  uint64_t whiteKnights = whites & pieces[1];
  uint64_t blackKnights = blacks & pieces[1];

  uint64_t whiteBishops = whites & pieces[2];
  uint64_t blackBishops = blacks & pieces[2];

  int whiteKingIndex = bitScanR(whites & pieces[5]);
  int blackKingIndex = bitScanR(blacks & pieces[5]);

  uint64_t whiteRooks = whites & pieces[3];
  uint64_t blackRooks = blacks & pieces[3];

  uint64_t whiteQueens = whites & pieces[4];
  uint64_t blackQueens = blacks & pieces[4];

  ret += whiteToMove? 12 : -12;
  ret += evaluateImbalance();
  std::cout << canCastleK(true) << std::endl;
  std::cout << canCastleK(false) << std::endl;



  std::cout << "----------------------------------------------------------" << std::endl;
  std::cout << "White mobility:    " << evaluateMobility(whiteKingIndex, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, endgame, true) << std::endl;
  std::cout << "white King Safety: " << evaluateKingSafety(blackKingIndex, ~(whitePawns | pawnAttacks(false)), whiteKnights, whiteBishops, whiteRooks, whiteQueens, true) << std::endl;
  std::cout << "White Pawns:       " << evaluatePawns(whitePawns, blackPawns, endgame, true) << std::endl;
  std::cout << "White Outposts:    " << evaluateOutposts(whiteKnights, whiteBishops, blackPawns, endgame, true) << std::endl;
  std::cout << "White Threats:     " << evaluateThreats(whitePawns, endgame, true) << std::endl;
  std::cout << "White trapped:     " << evaluateTrappedPieces(whiteKingIndex, whiteRooks, true) << std::endl;
  std::cout << "----------------------------------------------------------" << std::endl;

  std::cout << "----------------------------------------------------------" << std::endl;
  std::cout << "Black mobility:    " << evaluateMobility(blackKingIndex, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, endgame, false) << std::endl;
  std::cout << "Black King Safety: " << evaluateKingSafety(whiteKingIndex, ~(blackPawns | pawnAttacks(true)), blackKnights, blackBishops, blackRooks, blackQueens, false) << std::endl;
  std::cout << "Black Pawns:       " << evaluatePawns(blackPawns, whitePawns, endgame, false) << std::endl;
  std::cout << "Black Outposts:    " << evaluateOutposts(blackKnights, blackBishops, whitePawns, endgame, false) << std::endl;
  std::cout << "Black Threats:     " << evaluateThreats(blackPawns, endgame, false) << std::endl;
  std::cout << "Black trapped:     " << evaluateTrappedPieces(blackKingIndex, blackRooks, false) << std::endl;
  std::cout << "----------------------------------------------------------" << std::endl;

  std::cout << "----------------------------------------------------------" << std::endl;
  std::cout << "Total mobility:    " << evaluateMobility(whiteKingIndex, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, endgame, true) - evaluateMobility(blackKingIndex, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, endgame, false) << std::endl;
  std::cout << "Total King Safety: " << evaluateKingSafety(blackKingIndex, ~(whitePawns | pawnAttacks(false)), whiteKnights, whiteBishops, whiteRooks, whiteQueens, true) - evaluateKingSafety(whiteKingIndex, ~(blackPawns | pawnAttacks(true)), blackKnights, blackBishops, blackRooks, blackQueens, false) << std::endl;
  std::cout << "Total Pawns:       " << evaluatePawns(whitePawns, blackPawns, endgame, true) - evaluatePawns(blackPawns, whitePawns, endgame, false) << std::endl;
  std::cout << "Total Outposts:    " << evaluateOutposts(whiteKnights, whiteBishops, blackPawns, endgame, true) - evaluateOutposts(blackKnights, blackBishops, whitePawns, endgame, false) << std::endl;
  std::cout << "Total threats:     " << evaluateThreats(whitePawns, endgame, true) - evaluateThreats(blackPawns, endgame, false) << std::endl;
  std::cout << "Total trapped:     " << evaluateTrappedPieces(whiteKingIndex, whiteRooks, true) - evaluateTrappedPieces(blackKingIndex, blackRooks, false) << std::endl;
  std::cout << "----------------------------------------------------------" << std::endl;


}



int Bitboard::evaluateImbalance() {

  int ret = 0;

  if (countBishopsW >= 2) {
    ret += 35;
  }

  if (countBishopsB >= 2) {
    ret -= 35;
  }

  if (countKnightsW >= 2) {
    ret -= 16;
  }

  if (countKnightsB >= 2) {
    ret += 16;
  }

  if (countPawnsW == 0) {
    ret -= 50;
  }

  if (countPawnsB == 0) {
    ret += 50;
  }

  return ret;
}



int Bitboard::evaluateOutposts(uint64_t knights, uint64_t bishops, uint64_t pawns, bool endgame, bool isWhite) {

  int ret = 0;
  int bscan;
  uint64_t territory = isWhite? enemyTerritoryWhite & pawnAttacks(true) : enemyTerritoryBlack & pawnAttacks(false);
  uint64_t *passedPawnMask = isWhite? whitePassedPawnMask : blackPassedPawnMask;

  knights &= territory;
  bishops &= territory;

  while (knights) {
    bscan = bitScanR(knights);
    if (!(passedPawnMask[bscan] & isolatedPawnMask[bscan] & pawns)) {
      ret += 25;
    }
    knights &= knights - 1;
  }

  while (bishops) {
    bscan = bitScanR(bishops);
    if (!(passedPawnMask[bscan] & isolatedPawnMask[bscan] & pawns)) {
      ret += 12;
    }
    bishops &= bishops - 1;
  }

  return endgame? ret / 8 : ret;
}



int Bitboard::evaluateKingSafety(int kingIndex, uint64_t color, uint64_t knights, uint64_t bishops, uint64_t rooks, uint64_t queens, bool isWhite) {

  int ret = 0;
  int bscan;
  int valueOfAttacks = 0;
  int attackingPiecesCount = 0;

  uint64_t bishopsCopy = bishops;
  uint64_t rooksCopy = rooks;
  uint64_t kingZoneMask = isWhite? kingZoneMaskBlack[kingIndex] : kingZoneMaskWhite[kingIndex];
  kingZoneMask &= color;


  while (knights) {
    bscan = bitScanR(knights);
    ret -= (chebyshevArray[kingIndex][bscan] * pieceValues[1] / 512);
    if (kingZoneMask & knightAttacks(1ULL << bscan)) {
      attackingPiecesCount++;
      valueOfAttacks += 30;
    }
    knights &= knights - 1;
  }

  while (bishops) {
    bscan = bitScanR(bishops);
    ret -= (chebyshevArray[kingIndex][bscan] * pieceValues[2] / 512);
    if (kingZoneMask & xrayAttackBishop(bishopsCopy | queens, bscan)) {
      attackingPiecesCount++;
      valueOfAttacks += 20;
    }
    bishops &= bishops - 1;
  }

  while (rooks) {
    bscan = bitScanR(rooks);
    ret -= (manhattanArray[kingIndex][bscan] * pieceValues[3] / 512);
    if (kingZoneMask & xrayAttackRook(rooksCopy | queens, bscan)) {
      attackingPiecesCount++;
      valueOfAttacks += 50;
    }
    rooks &= rooks - 1;
  }

  while (queens) {
    bscan = bitScanR(queens);
    ret -= (chebyshevArray[kingIndex][bscan] * pieceValues[4] / 512);
    if (kingZoneMask & (magics->rookAttacksMask(occupied, bscan) | magics->bishopAttacksMask(occupied, bscan))) {
      attackingPiecesCount++;
      valueOfAttacks += 80;
    }
    queens &= queens - 1;
  }

  ret += (valueOfAttacks * attackWeight[attackingPiecesCount]) / 100;
  return ret;
}



int Bitboard::evaluateMobility(int kingIndex, uint64_t pawns, uint64_t knights, uint64_t bishops, uint64_t rooks, uint64_t queens, bool endgame, bool isWhite) {

  int ret = 0;
  int board = 0;
  int bscan;

  int8_t *pawnTable;
  int8_t *knightTable;
  int8_t *bishopTable;
  int8_t *rookTable;
  int8_t *queenTable;
  int8_t *kingTable;
  uint64_t freeSquares;

  uint64_t bishopsCopy = bishops;
  uint64_t rooksCopy = rooks;

  if (isWhite) {
    pawnTable = endgame? whitePawnTableEG : whitePawnTable;
    knightTable = whiteKnightTable;
    bishopTable = whiteBishopTable;
    rookTable = whiteRookTable;
    queenTable = whiteQueenTable;
    kingTable = endgame? whiteKingTableEG : whiteKingTable;
    freeSquares = ~((pieces[5] & whites) | queens | pawns | pawnAttacks(false));
  }
  else {
    pawnTable = endgame? blackPawnTableEG : blackPawnTable;;
    knightTable = blackKnightTable;
    bishopTable = blackBishopTable;
    rookTable = blackRookTable;
    queenTable = blackQueenTable;
    kingTable = endgame? blackKingTableEG : blackKingTable;
    freeSquares = ~((pieces[5] & blacks) | queens | pawns | pawnAttacks(true));
  }



  while (pawns) {
    board += pawnTable[bitScanR(pawns)];
    pawns &= pawns - 1;
  }

  ret += count_population(knightAttacks(knights) & freeSquares);
  while (knights) {
    board += knightTable[bitScanR(knights)];
    knights &= knights - 1;
  }

  while (bishops) {
    bscan = bitScanR(bishops);
    ret += count_population(xrayAttackBishop(bishopsCopy | queens, bscan) & freeSquares);
    board += bishopTable[bscan];
    bishops &= bishops - 1;
  }

  while (rooks) {
    bscan = bitScanR(rooks);
    ret += count_population(xrayAttackRook(rooksCopy | queens, bscan) & freeSquares);
    board += rookTable[bscan];
    rooks &= rooks - 1;
  }

  while (queens) {
    bscan = bitScanR(queens);
    board += queenTable[bscan];
    ret += count_population((magics->rookAttacksMask(occupied, bscan) | magics->bishopAttacksMask(occupied, bscan)) & freeSquares) / 2.5;
    queens &= queens - 1;
  }

  board += kingTable[kingIndex];
  return ret + board;

}



int Bitboard::evaluatePawns(uint64_t allyPawns, uint64_t enemyPawns, bool endgame, bool isWhite) {

  int ret = 0;
  int ret2 = 0;
  int bscan;
  uint64_t pawnsCopy = allyPawns;
  uint64_t *passedPawnMask = isWhite? whitePassedPawnMask : blackPassedPawnMask;
  uint64_t supportedPawns = allyPawns & pawnAttacks(isWhite);
  uint64_t adjPawns = (allyPawns << 1) & ~columnMask[0];
  adjPawns |= (allyPawns >> 1) & ~columnMask[7];
  adjPawns &= allyPawns;

  if (isWhite) {
    for (int i = 1; i < 7; i++) {
      ret2 += count_population(adjPawns & rowMask[i]) * i;
      ret2 += count_population(supportedPawns & rowMask[i]) * i * 2;
    }
  }
  else {
    for (int i = 1; i < 7; i++) {
      ret2 += count_population(adjPawns & rowMask[i]) * (7 - i);
      ret2 += count_population(supportedPawns & rowMask[i]) * (7 - i) * 2;
    }
  }


  // Doubled Pawns
  for (int i = 0; i < 8; i++) {
    int popCount = count_population(allyPawns & columnMask[i]);
    if (popCount >= 2) {
      ret -= (popCount - 1) * 7;
    }
  }

  // Passed Pawns and Isolated Pawns
  while (allyPawns) {
    bscan = bitScanR(allyPawns);

    if ((pawnsCopy & isolatedPawnMask[bscan]) == 0) {
      ret -= 9;
    }

    if ((enemyPawns & passedPawnMask[bscan]) == 0) {
      ret += 20;
    }
    allyPawns &= allyPawns - 1;
  }

  return ret2 + ret;

}



int Bitboard::evaluateThreats(uint64_t pawns, bool endgame, bool isWhite) {

  int ret = 0;
  int bscan;
  uint64_t validPawns = 0;
  uint64_t color;
  uint64_t colorP;
  uint64_t colorB;
  uint64_t colorN;
  uint64_t threats = 0;

  if (isWhite) {
    pawns <<= 8;
    color = blacks & ~pieces[0];
    colorN = whites & pieces[1];
    colorB = whites & pieces[2];
    colorP = blacks;
  }
  else {
    pawns >>= 8;
    color = whites & ~pieces[0];
    colorN = blacks & pieces[1];
    colorB = blacks & pieces[2];
    colorP = whites;
  }

  pawns &= ~(occupied | pawnAttacks(!isWhite));

  while (pawns) {
    bscan = bitScanR(pawns);
    if (!isAttacked(bscan, isWhite)) {
      validPawns |= 1ULL << bscan;
    }
    pawns &= pawns - 1;
  }

  if (validPawns) {
    ret += count_population(pawnAttacksMan(validPawns, isWhite) & color);
    ret *= 4;
  }

  threats |= knightAttacks(colorN) & colorP;

  while (colorB) {
    int bscan = bitScanR(colorB);
    threats |= magics->bishopAttacksMask(occupied, bscan) & colorP;
    colorB &= colorB - 1;
  }

  ret += count_population(threats & pieces[0]);
  ret += count_population(threats & pieces[1]) * 2;
  ret += count_population(threats & pieces[2]) * 3;
  ret += count_population(threats & pieces[3]) * 4;
  ret += count_population(threats & pieces[4]) * 5;
  ret += count_population(threats & pieces[5]) * 6;


  return ret;

}



int Bitboard::evaluateTrappedPieces(int kingIndex, uint64_t rooks, bool isWhite) {

  int bscan;
  int ret = 0;
  if (isWhite) {
    while (rooks) {
      bscan = bitScanR(rooks);
      if (bscan < 8) {
        if (kingIndex > 3 && bscan > kingIndex) {
          ret -= 40;
        }
        if (kingIndex < 4 && bscan < kingIndex) {
          ret -= 40;
        }
      }
      rooks &= rooks - 1;
    }
  }
  else {
    while (rooks) {
      bscan = bitScanR(rooks);
      if (bscan > 55) {
        if (kingIndex > 59 && bscan > kingIndex) {
          ret -= 40;
        }
        if (kingIndex < 60 && bscan < kingIndex) {
          ret -= 40;
        }
      }
      rooks &= rooks - 1;
    }
  }
  return ret;
}



int Bitboard::evaluateKingPawnEndgame(int kingIndex, uint64_t allyPawns, uint64_t enemyPawns, bool endgame, bool isWhite) {

  if (!endgame) {
    return 0;
  }

  int ret = 0;
  int count = 0;
  int bscan;
  uint64_t *passedPawnMaskA;
  uint64_t *passedPawnMaskE;

  uint64_t allyPawnsCopy = allyPawns;
  uint64_t enemyPawnsCopy = enemyPawns;

  if (isWhite) {
    passedPawnMaskA = whitePassedPawnMask;
    passedPawnMaskE = blackPassedPawnMask;
  }
  else {
    passedPawnMaskA = blackPassedPawnMask;
    passedPawnMaskE = whitePassedPawnMask;
  }

  // Passed Pawns and Isolated Pawns
  while (allyPawns) {
    bscan = bitScanR(allyPawns);

    int tempret = 16 - manhattanArray[bscan][kingIndex];
    if ((allyPawnsCopy & isolatedPawnMask[bscan]) == 0) {
      tempret *= 2;
    }

    if ((enemyPawnsCopy & passedPawnMaskA[bscan]) == 0) {
      tempret *= 3;
    }

    ret += tempret;
    count++;
    allyPawns &= allyPawns - 1;
  }

  // Passed Pawns and Isolated Pawns
  while (enemyPawns) {
    bscan = bitScanR(enemyPawns);

    int tempret = 16 - manhattanArray[bscan][kingIndex];
    if ((enemyPawnsCopy & isolatedPawnMask[bscan]) == 0) {
      tempret *= 2;
    }

    if ((allyPawnsCopy & passedPawnMaskE[bscan]) == 0) {
      tempret *= 3;
    }

    ret += tempret;
    count++;
    enemyPawns &= enemyPawns - 1;
  }

  return count? ret / count : 0;

}



// Give a score to each move in the list
void Bitboard::scoreMoves(std::vector<Move> &moveList, Move &move, int depth, bool isWhite) {

  if (moveList.size() <= 1) {
    return;
  }

  std::vector<Move>::iterator p = std::find(moveList.begin(), moveList.end(), move);
  if (p != moveList.end()) {
    moveList.erase(p);
  }

  int see;
  //MVV/LVA
  for (std::vector<Move>::iterator it = moveList.begin(); it != moveList.end(); ++it) {

    if (it->score < 3000000) {

      if (it->promotion == 4) {
        it->score = 1700000;
      }

      if (it->quiet){

        if (*it == killerMoves[whiteToMove][depth][0]) {
          it->score = 1000000;
        }
        else if (*it == killerMoves[whiteToMove][depth][1]) {
          it->score = 900000;
        }
        else {
          it->score += history[whiteToMove][it->pieceFrom][it->toLoc];
        }

      }
      else {

        if (depth > 0) {
          see = seeCaptureNew(*it);
          if (see >= -75) {
            it->score = it->score = 1500000 + see;
          }
          else if (see >= -300) {
            it->score = it->score = 800000 + see;
          }
          else {
            it->score = it->score = -800000 + see;
          }
        }
        else {
          if (mvvlva[it->pieceFrom][it->pieceTo] >= 1000) {
            it->score = 1500000 + mvvlva[it->pieceFrom][it->pieceTo];
          }
          else {
            it->score = 800000 + mvvlva[it->pieceFrom][it->pieceTo];
          }
        }

      }
    }

  }

}



// Choose the max score of the move and delete it from the list
Bitboard::Move Bitboard::pickMove(std::vector<Move> &moveList) {
  std::vector<Move>::iterator p = min_element(moveList.begin(), moveList.end());
  Move ret = *p;
  *p = moveList.back();
  moveList.pop_back();
  return ret;
}



void Bitboard::InsertKiller(Move &move, int &depth) {
  killerMoves[whiteToMove][depth][1] = killerMoves[whiteToMove][depth][0];
  killerMoves[whiteToMove][depth][0] = move;
}



uint64_t Bitboard::hashBoard(bool turn) {
  return zobrist.hashBoard(pieces, occupied, blacks, turn);
}



bool Bitboard::canCastleQ(bool isWhite) {
  if (isWhite) {
    uint64_t canNotBeAttacked = 14ULL & occupied;
    if (!kingMovedWhite && !rookMovedWhiteA && canNotBeAttacked == 0 && isAttacked(4, 0) && isAttacked(3, 0) && isAttacked(2, 0) && pieces[3] & whites & 1) {
      return true;
    }
  }
  else {
    uint64_t canNotBeAttacked = 1008806316530991104ULL & occupied;
    if (!kingMovedBlack && !rookMovedBlackA && canNotBeAttacked == 0 && isAttacked(60, 1) && isAttacked(59, 1) && isAttacked(58, 1) && pieces[3] & blacks & (1ULL << 56)) {
      return true;
    }
  }

  return false;
}



bool Bitboard::canCastleK(bool isWhite) {
  if (isWhite) {
    uint64_t canNotBeAttacked = 96ULL & occupied;
    if (!kingMovedWhite && !rookMovedWhiteH && canNotBeAttacked == 0 && isAttacked(4, 0) && isAttacked(5, 0) && isAttacked(6, 0) && pieces[3] & whites & (1ULL << 7)) {
      return true;
    }
  }
  else {
    uint64_t canNotBeAttacked = 6917529027641081856ULL & occupied;
    if (!kingMovedBlack && !rookMovedBlackH && canNotBeAttacked == 0 && isAttacked(60, 1) && isAttacked(61, 1) && isAttacked(62, 1) && pieces[3] & blacks & (1ULL << 63)) {
      return true;
    }
  }

  return false;
}



int Bitboard::enpassantConditions(bool isWhite, int pawnLocation) {
  if (moveStack.empty()) {
    return 0;
  }

  MoveStack lastMove = moveStack.back();
  int fromLoc = bitScanR(lastMove.fromLoc);
  int toLoc = bitScanR(lastMove.toLoc);
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



// Given an index and color of piece, determine if it is a passed pawn
bool Bitboard::isPassedPawn(int index, bool color) {



  if (color == 0) {

    uint64_t mask = (1ULL << index) & pieces[0] & whites;

    if (mask == 0) {
      return false;
    }

    return (whitePassedPawnMask[bitScanR(mask)] & (blacks & pieces[0])) == 0;

  }
  else {

    uint64_t mask = (1ULL << index) & pieces[0] & blacks;

    if (mask == 0) {
      return false;
    }

    return (blackPassedPawnMask[bitScanR(mask)] & (whites & pieces[0])) == 0;

  }
}



Bitboard::Move Bitboard::smallestAttacker(int index, bool isWhite) {

  int8_t pieceFC = -1;

  if (!isWhite) {
    uint64_t piece = (1ULL << index) & whites;

    if (piece & pieces[0]) {
      pieceFC = 0;
    }
    else if (piece & pieces[1]) {
      pieceFC = 1;
    }
    else if (piece & pieces[2]) {
      pieceFC = 2;
    }
    else if (piece & pieces[3]) {
      pieceFC = 3;
    }
    else if (piece & pieces[4]) {
      pieceFC = 4;
    }
    else if (piece & pieces[5]) {
      pieceFC = 5;
    }

    assert(pieceFC != -1);

    uint64_t index2 = whitePawnAttacks[index] & blacks & pieces[0];
    if (index2) {
      return Move(bitScanR(index2), index, false, pieceFC, 0, 0, 0, false);
    }

    index2 = knightMoves[index] & blacks & pieces[1];
    if (index2) {
      return Move(bitScanR(index2), index, false, pieceFC, 1, 0, 0, false);
    }

    index2 = magics->bishopAttacksMask(occupied, index) & blacks & pieces[2];
    if (index2) {
      return Move(bitScanR(index2), index, false, pieceFC, 2, 0, 0, false);
    }

    index2 = magics->rookAttacksMask(occupied, index) & blacks & pieces[3];
    if (index2) {
      return Move(bitScanR(index2), index, false, pieceFC, 3, 0, 0, false);
    }

    index2 = (magics->bishopAttacksMask(occupied, index) | magics->rookAttacksMask(occupied, index)) & blacks & pieces[4];
    if (index2) {
      return Move(bitScanR(index2), index, false, pieceFC, 4, 0, 0, false);
    }

    index2 = kingMoves[index] & blacks & pieces[5];
    if (index2) {
      return Move(bitScanR(index2), index, false, pieceFC, 5, 0, 0, false);
    }

  }
  else {

    uint64_t piece = (1ULL << index) & blacks;

    if (piece & pieces[0]) {
      pieceFC = 0;
    }
    else if (piece & pieces[1]) {
      pieceFC = 1;
    }
    else if (piece & pieces[2]) {
      pieceFC = 2;
    }
    else if (piece & pieces[3]) {
      pieceFC = 3;
    }
    else if (piece & pieces[4]) {
      pieceFC = 4;
    }
    else if (piece & pieces[5]) {
      pieceFC = 5;
    }

    assert(pieceFC !=-1);

    uint64_t index2 = blackPawnAttacks[index] & whites & pieces[0];
    if (index2) {
      return Move(bitScanR(index2), index, false, pieceFC, 0, 0, 0, false);
    }

    index2 = knightMoves[index] & whites & pieces[1];
    if (index2) {
      return Move(bitScanR(index2), index, false, pieceFC, 1, 0, 0, false);
    }

    index2 = magics->bishopAttacksMask(occupied, index) & whites & pieces[2];
    if (index2) {
      return Move(bitScanR(index2), index, false, pieceFC, 2, 0, 0, false);
    }

    index2 = magics->rookAttacksMask(occupied, index) & whites & pieces[3];
    if (index2) {
      return Move(bitScanR(index2), index, false, pieceFC, 3, 0, 0, false);
    }

    index2 = (magics->bishopAttacksMask(occupied, index) | magics->rookAttacksMask(occupied, index)) & whites & pieces[4];
    if (index2) {
      return Move(bitScanR(index2), index, false, pieceFC, 4, 0, 0, false);
    }

    index2 = kingMoves[index] & whites & pieces[5];
    if (index2) {
      return Move(bitScanR(index2), index, false, pieceFC, 5, 0, 0, false);
    }

  }

  return Move();

}


int Bitboard::see(int index, bool isWhite) {

  int value = 0;
  Move piece = smallestAttacker(index, isWhite);

  if (piece != Move()) {
    movePieceCapture(piece);
    value = std::max(0, pieceValues[piece.pieceTo] - see(index, !isWhite));
    undoMoveCapture();
  }

  return value;

}



int Bitboard::seeCapture(Move &capture, bool isWhite) {

  int value = 0;

  movePieceCapture(capture);
  value = pieceValues[capture.pieceTo] - see(capture.toLoc, !isWhite);
  undoMoveCapture();

  return value;

}


uint64_t Bitboard::getLeastValuablePiece(uint64_t attadef, bool isWhite, int &piece) {

  attadef &= isWhite? whites : blacks;
  for (piece = 0; piece <= 5; piece++) {
    uint64_t subset = attadef & pieces[piece];
    if (subset) {
      return subset & -subset;
    }
  }

  return 0;

}



int Bitboard::seeCaptureNew(Move &capture) {

  int gain[32], d = 0;
  uint64_t mayXray = pieces[0] | pieces[2] | pieces[3] | pieces[4];

  uint64_t fromSet = 1ULL << capture.fromLoc;

  uint64_t occ = occupied;
  uint64_t attadef = isAttackedSee(capture.toLoc);
  int aPiece = capture.pieceFrom;
  bool isWhite = fromSet & whites? true : false;
  gain[d] = pieceValues[capture.pieceTo];

  do {

    d++;
    gain[d] = pieceValues[aPiece] - gain[d - 1];
    isWhite = !isWhite;

    if (std::max(-gain[d - 1], gain[d]) < 0) {
      break;
    }

    attadef ^= fromSet;
    occ ^= fromSet;

    if (fromSet & mayXray) {
      attadef |= xrayAttackRookSee(fromSet, capture.toLoc) & (pieces[3] | pieces[4]);
      attadef |= xrayAttackBishopSee(fromSet, capture.toLoc) & (pieces[2] | pieces[4]);
    }

    fromSet = getLeastValuablePiece(attadef, isWhite, aPiece);


  } while (fromSet);

  while (--d) {
    gain[d - 1] = -std::max(-gain[d - 1], gain[d]);
  }

  return gain[0];

}



void Bitboard::movePieceCapture(Move& move) {

  uint64_t i1 = 1ULL << move.fromLoc;
  uint64_t i2 = 1ULL << move.toLoc;
  uint64_t i1i2 = i1 ^ i2;
  int i = move.pieceFrom;
  int k = move.pieceTo;
  bool useWhite;

  assert(i != -1);
  assert(k != -1);

  useWhite = whites & i1 ? true : false;

  if (useWhite) {
    whites ^= i1i2;
    blacks ^= i2;
  }
  else {
    blacks ^= i1i2;
    whites ^= i2;
  }

  pieces[k] ^= i2;
  pieces[i] ^= i1i2;
  occupied ^= i1;


  moveStack.emplace_back(i1, i2, i, k, !useWhite, 0, false, 0, false, false);

}



void Bitboard::undoMoveCapture() {

  MoveStack m = moveStack.back();
  uint64_t fromLoc = m.fromLoc;
  uint64_t toLoc = m.toLoc;
  uint64_t i1i2 = fromLoc ^ toLoc;

  // STEP 6: Undo the move
  if (toLoc != fromLoc) {
    pieces[m.movePiece] ^= i1i2;

    if (!m.color) {
      whites ^= i1i2;
      blacks ^= toLoc;
    }
    else {
      blacks ^= i1i2;
      whites ^= toLoc;
    }
    pieces[m.capturePiece] ^= toLoc;
    occupied ^= fromLoc;

  }

  moveStack.pop_back();
}



bool Bitboard::isDraw() {

  int occupiedCount = count_population(occupied);
  if (occupiedCount == 2) {
    return true;
  }

  if (occupiedCount == 3 && (pieces[1] | pieces[2])) {
    return true;
  }

  // int totalPop = countPawnsB + countPawnsW + countKnightsW + countKnightsB + countBishopsW + countBishopsW + countRooksW + countRooksB + countQueensW + countQueensB;
  //
  // if (totalPop >= 2) {
  //   return false;
  // }
  //
  // if (totalPop == 0) {
  //   return true;
  // }
  //
  // if (countKnightsW + countKnightsB == 1) {
  //   return true;
  // }
  //
  // if (countBishopsW + countBishopsB == 1) {
  //   return true;
  // }

  return false;
}





































//
