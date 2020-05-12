#include "eval.h"


Eval::Eval() {

  initDistanceArray();
  InitPieceBoards();
  InitPassedPawnsMask();
  InitIsolatedPawnsMask();
  InitKingZoneMask();
  InitColumnsMask();

}



void Eval::InitDistanceArray() {

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



void Eval::InitPieceBoards() {

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



void Eval::InitPassedPawnsMask() {

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



void Eval::InitIsolatedPawnsMask() {

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



void Eval::InitKingZoneMask() {

  for (int i = 0; i < 64; i++) {

    kingZoneMaskWhite[i] = kingMoves[i] | (1ULL << i);
    kingZoneMaskWhite[i] |= kingZoneMaskWhite[i] << 8;
    kingZoneMaskWhite[i] |= kingZoneMaskWhite[i] << 8;
    kingZoneMaskWhite[i] |= kingZoneMaskWhite[i] << 8;

    kingZoneMaskBlack[i] = kingMoves[i] | (1ULL << i);
    kingZoneMaskBlack[i] |= kingZoneMaskBlack[i] >> 8;
    kingZoneMaskBlack[i] |= kingZoneMaskBlack[i] >> 8;
    kingZoneMaskBlack[i] |= kingZoneMaskBlack[i] >> 8;

  }

}



void Eval::InitColumnsMask() {

  for (int i = 0; i < 8; i++) {
    columnMask[i] = 1ULL << i;
    columnMask[i] |= columnMask[i] << 8;
    columnMask[i] |= columnMask[i] << 16;
    columnMask[i] |= columnMask[i] << 32;
  }

}



int Eval::evaluateKingSafety(uint8_t whiteKingIndex, uint8_t blackKingIndex, uint64_t whiteKnights, uint64_t blackKnights,
  uint64_t whiteBishops, uint64_t blackBishops, uint64_t whiteRooks, uint64_t blackRooks, uint64_t whiteQueens, uint64_t blackQueens) {

  int ret = 0;
  int bscan;

  int valueOfAttacksWhite = 0;
  int attackingPiecesCountWhite = 0;
  int valueOfAttacksBlack = 0;
  int attackingPiecesCountBlack = 0;

  while (whiteKnights) {
    bscan = bitScanR(whiteKnights);
    ret -= (chebyshevArray[blackKingIndex][bscan] * pieceValues[1] / 375);

    if (kingZoneMaskBlack[blackKingIndex] & knightAttacks(bscan) & ~whites) {
      attackingPiecesCountWhite++;
      valueOfAttacksWhite += 22;
    }

    whiteKnights &= whiteKnights - 1;
  }

  while (blackKnights) {
    bscan = bitScanR(blackKnights);
    ret += (chebyshevArray[whiteKingIndex][bscan] * pieceValues[1] / 375);

    if (kingZoneMaskWhite[whiteKingIndex] & knightAttacks(bscan) & ~blacks) {
      attackingPiecesCountBlack++;
      valueOfAttacksBlack -= 22;
    }

    blackKnights &= blackKnights - 1;
  }

  while (whiteBishops) {
    bscan = bitScanR(whiteBishops);
    ret -= (chebyshevArray[blackKingIndex][bscan] * pieceValues[2] / 512);

    if (kingZoneMaskBlack[blackKingIndex] & magics->bishopAttacksMask(occupied, bscan) & ~whites) {
      attackingPiecesCountWhite++;
      valueOfAttacksWhite += 20;
    }

    whiteBishops &= whiteBishops - 1;
  }

  while (blackBishops) {
    bscan = bitScanR(blackBishops);
    ret += (chebyshevArray[whiteKingIndex][bscan] * pieceValues[2] / 512);

    if (kingZoneMaskWhite[whiteKingIndex] & magics->bishopAttacksMask(occupied, bscan) & ~blacks) {
      attackingPiecesCountBlack++;
      valueOfAttacksBlack -= 20;
    }

    blackBishops &= blackBishops - 1;
  }

  while (whiteRooks) {
    bscan = bitScanR(whiteRooks);
    ret -= (manhattanArray[blackKingIndex][bscan] * pieceValues[3] / 512);

    if (kingZoneMaskBlack[blackKingIndex] & magics->rookAttacksMask(occupied, bscan) & ~whites) {
      attackingPiecesCountWhite++;
      valueOfAttacksWhite += 35;
    }

    whiteRooks &= whiteRooks - 1;
  }

  while (blackRooks) {
    bscan = bitScanR(blackRooks);
    ret += (manhattanArray[whiteKingIndex][bscan] * pieceValues[3] / 512);

    if (kingZoneMaskWhite[whiteKingIndex] & magics->rookAttacksMask(occupied, bscan) & ~blacks) {
      attackingPiecesCountBlack++;
      valueOfAttacksBlack -= 35;
    }

    blackRooks &= blackRooks - 1;
  }

  while (whiteQueens) {
    bscan = bitScanR(whiteQueens);
    ret -= (chebyshevArray[blackKingIndex][bscan] * pieceValues[4] / 512);

    if (kingZoneMaskBlack[blackKingIndex] & (magics->rookAttacksMask(occupied, bscan) | magics->bishopAttacksMask(occupied, bscan)) & ~whites) {
      attackingPiecesCountWhite++;
      valueOfAttacksWhite += 47;
    }

    whiteQueens &= whiteQueens - 1;
  }

  while (blackQueens) {
    bscan = bitScanR(whiteQueens);
    ret += (chebyshevArray[whiteKingIndex][bscan] * pieceValues[4] / 512);

    if (kingZoneMaskWhite[whiteKingIndex] & (magics->rookAttacksMask(occupied, bscan) | magics->bishopAttacksMask(occupied, bscan)) & ~blacks) {
      attackingPiecesCountBlack++;
      valueOfAttacksBlack -= 47;
    }

    blackQueens &= blackQueens - 1;
  }

  ret += (valueOfAttacksWhite * attackWeight[attackingPiecesCountWhite]) / 100;
  ret += (valueOfAttacksBlack * attackWeight[attackingPiecesCountBlack]) / 100;

  return ret;
}



int Eval::evaluateMobility(uint64_t whitePawns, uint64_t blackPawns, uint64_t whiteKnights, uint64_t blackKnights,
  uint64_t whiteBishops, uint64_t blackBishops, uint64_t whiteRooks, uint64_t blackRooks, uint64_t whiteQueens, uint64_t blackQueens, bool endgame) {

  int ret = 0;
  int board = 0;
  int bscan;

  if (endgame) {

    uint64_t whiteKing = whites & pieces[5];
    uint64_t blackKing = blacks & pieces[5];

    while (whitePawns) {
      board += whitePawnTable[bitScanR(whitePawns)];
      whitePawns &= whitePawns - 1;
    }


    // countPiece = count_population(pawnsB);
    while (blackPawns) {
      board -= blackPawnTable[bitScanR(blackPawns)];
      blackPawns &= blackPawns - 1;
    }

    board += whiteKingTableEG[bitScanR(whiteKing)];
    board -= blackKingTableEG[bitScanR(blackKing)];

  }
  else {

    uint64_t whiteKing = whites & pieces[5];
    uint64_t blackKing = blacks & pieces[5];

    while (whitePawns) {
      board += whitePawnTable[bitScanR(whitePawns)];
      whitePawns &= whitePawns - 1;
    }


    // countPiece = count_population(pawnsB);
    while (blackPawns) {
      board -= blackPawnTable[bitScanR(blackPawns)];
      blackPawns &= blackPawns - 1;
    }

    board += whiteKingTable[bitScanR(whiteKing)];
    board -= blackKingTable[bitScanR(blackKing)];

  }

  ret += count_population(knightAttacks(whiteKnights) & (pawnAttacksBlack(blacks) ^ ALL_ONES)) * 2;
  ret -= count_population(knightAttacks(blackKnights) & (pawnAttacksWhite(whites) ^ ALL_ONES)) * 2;

  while (whiteKnights) {
    board += whiteKnightTable[bitScanR(whiteKnights)];
    whiteKnights &= whiteKnights - 1;
  }

  while (blackKnights) {
    board -= blackKnightTable[bitScanR(blackKnights)];
    blackKnights &= blackKnights - 1;
  }

  while (whiteBishops) {
    bscan = bitScanR(whiteBishops);
    ret += count_population(magics->bishopAttacksMask(occupied, bscan));
    board += whiteBishopTable[bscan];
    whiteBishops &= whiteBishops - 1;
  }

  while (blackBishops) {
    bscan = bitScanR(blackBishops);
    ret -= count_population(magics->bishopAttacksMask(occupied, bscan));
    board -= blackBishopTable[bscan];
    blackBishops &= blackBishops - 1;
  }

  while (whiteRooks) {
    bscan = bitScanR(whiteRooks);
    ret += count_population(magics->rookAttacksMask(occupied, bscan));
    board += whiteRookTable[bscan];
    whiteRooks &= whiteRooks - 1;
  }

  while (blackRooks) {
    bscan = bitScanR(blackRooks);
    ret -= count_population(magics->rookAttacksMask(occupied, bscan));
    board -= blackRookTable[bscan];
    blackRooks &= blackRooks - 1;
  }

  while (whiteQueens) {
    bscan = bitScanR(whiteQueens);
    board += whiteQueenTable[bscan];
    ret += count_population((magics->rookAttacksMask(occupied, bscan) | magics->bishopAttacksMask(occupied, bscan))) / 2;
    whiteQueens &= whiteQueens - 1;
  }

  while (blackQueens) {
    bscan = bitScanR(blackQueens);
    board -= blackQueenTable[bscan];
    ret -= count_population((magics->rookAttacksMask(occupied, bscan) | magics->bishopAttacksMask(occupied, bscan))) / 2;
    blackQueens &= blackQueens - 1;
  }

  return ret + board;

}



int Eval::evaluatePawns(uint64_t whitePawns, uint64_t blackPawns) {

  int ret = 0;
  int bscan;
  uint64_t whitePawnsCopy = whitePawns;
  uint64_t blackPawnsCopy = blackPawns;

  // Doubled Pawns
  for (int i = 0; i < 8; i++) {

    int popCountWhite = count_population(whitePawns & columnMask[i]);
    int popCountBlack = count_population(blackPawns & columnMask[i]);

    if (popCountWhite >= 2) {
      ret -= (popCountWhite - 1) * 15;
    }

    if (popCountBlack >= 2) {
      ret += (popCountBlack - 1) * 15;
    }
  }



  // Passed Pawns and Isolated Pawns
  while (whitePawns) {
    bscan = bitScanR(whitePawns);

    if ((whitePawnsCopy & isolatedPawnMask[bscan]) == 0) {
      ret -= 10;
    }

    if ((blackPawns & whitePassedPawnMask[bscan]) == 0) {
      ret += 20;
    }
    whitePawns &= whitePawns - 1;
  }

  while (blackPawns) {
    bscan = bitScanR(blackPawns);

    if ((blackPawnsCopy & isolatedPawnMask[bscan]) == 0) {
      ret += 10;
    }

    if ((whitePawnsCopy & blackPassedPawnMask[bscan]) == 0) {
      ret -= 20;
    }
    blackPawns &= blackPawns - 1;
  }

  return ret;

}



int Eval::evaluate(int materialScore, bool whiteToMove, int *pieces, int countPawnsW, int countPawnsB, int countKnightsW, int countKnightsB, int countBishopsW, int countBishopsB, int countRooksW, int countRooksB) {

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
    ret += 12;
  }
  else {
    ret -= 12;
  }

  if (countBishopsW >= 2) {
    ret += 25;
  }

  if (countBishopsB >= 2) {
    ret -= 25;
  }

  if (countKnightsW >= 2) {
    ret -= 20;
  }

  if (countKnightsB >= 2) {
    ret += 20;
  }

  if (countPawnsW == 0) {
    ret -= 50;
  }

  if (countPawnsB == 0) {
    ret += 50;
  }


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

  ret += evaluateMobility(whitePawns, blackPawns, whiteKnights, blackKnights, whiteBishops, blackBishops, whiteRooks, blackRooks, whiteQueens, blackQueens, endgame);
  ret += evaluateKingSafety(whiteKingIndex, blackKingIndex, whiteKnights, blackKnights, whiteBishops, blackBishops, whiteRooks, blackRooks, whiteQueens, blackQueens);
  ret += evaluatePawns(whitePawns, blackPawns);

  ret += knightWeight[countPawnsW] * countKnightsW;
  ret -= knightWeight[countPawnsB] * countKnightsB;

  ret += rookWeight[countPawnsW] * countRooksW;
  ret -= rookWeight[countPawnsB] * countRooksB;


  return whiteToMove? ret : -ret;
}
