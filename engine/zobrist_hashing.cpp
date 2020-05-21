


// Adapted from https://en.wikipedia.org/wiki/Zobrist_hashing

#include "zobrist_hashing.h"
#include <iostream>
#include <stdlib.h>



// Initialize zobrist hashing table
// Random 64 bit number for each square and pieces
// Collisions are possible but rare.
Zobrist::Zobrist() {

  for (int i = 0; i < 64; i++) {
    for (int j = 0; j < 12; j++) {
      table[i][j] = (uint64_t)((rand() & 0xffff) | ((rand() & 0xffff) << 16) | (((uint64_t)rand() & 0xffff) << 32) | (((uint64_t)rand() & 0xffff) << 48));
    }
  }

  // Flags for castling, and black turn
  blackTurn = (uint64_t)((rand() & 0xffff) | ((rand() & 0xffff) << 16) | (((uint64_t)rand() & 0xffff) << 32) | (((uint64_t)rand() & 0xffff) << 48));
  whiteKingCastle = (uint64_t)((rand() & 0xffff) | ((rand() & 0xffff) << 16) | (((uint64_t)rand() & 0xffff) << 32) | (((uint64_t)rand() & 0xffff) << 48));
  blackKingCastle = (uint64_t)((rand() & 0xffff) | ((rand() & 0xffff) << 16) | (((uint64_t)rand() & 0xffff) << 32) | (((uint64_t)rand() & 0xffff) << 48));
  whiteQueenCastle = (uint64_t)((rand() & 0xffff) | ((rand() & 0xffff) << 16) | (((uint64_t)rand() & 0xffff) << 32) | (((uint64_t)rand() & 0xffff) << 48));
  blackQueenCastle = (uint64_t)((rand() & 0xffff) | ((rand() & 0xffff) << 16) | (((uint64_t)rand() & 0xffff) << 32) | (((uint64_t)rand() & 0xffff) << 48));

  whiteKingCastleFlag = false;
  blackKingCastleFlag = false;
  whiteQueenCastleFlag = false;
  blackQueenCastleFlag = false;

}



uint64_t Zobrist::test(uint64_t h) {
  h ^= table[5][3];
  h ^= table[7][3];
  return h;
}



// Non iterative way to determine the hash key: loop through the board
// Used to initialize the board's starting position
uint64_t Zobrist::hashBoard(uint64_t *pieces, uint64_t& occupied, uint64_t& blacks, bool turn) {

  uint64_t ret = 0;

  if (!turn) {
    ret ^= blackTurn;
  }


  for (int i = 0; i < 64; ++i) {
    uint64_t shiftI = 1ULL << i;

    if ((occupied & shiftI) != 0) {
      int j = 0;

      if ((blacks & shiftI) != 0) {
        j += 6;
      }

      if ((pieces[0] & shiftI) != 0) {
        j += 0;
      }
      else if ((pieces[1] & shiftI) != 0) {
        j += 1;
      }
      else if ((pieces[2] & shiftI) != 0) {
        j += 2;
      }
      else if ((pieces[3] & shiftI) != 0) {
        j += 3;
      }
      else if ((pieces[4] & shiftI) != 0) {
        j += 4;
      }
      else if ((pieces[5] & shiftI) != 0) {
        j += 5;
      }

      ret ^= table[i][j];
    }
  }

  return ret;
}



// Iterative way to determine hash key:
// Used in making moves to keep track of the key much faster
uint64_t Zobrist::hashBoardM(uint64_t board, int pieceFrom, int pieceTo, int i, int k, bool useWhite, bool isEnpassant, bool whiteCastleK, bool whiteCastleQ, bool blackCastleK, bool blackCastleQ) {

  // Change the turn to move no matter what.
  uint64_t ret = board ^ blackTurn;

  if (whiteCastleK != whiteKingCastleFlag) {
    ret ^= whiteKingCastle;
  }
  if (whiteCastleQ != whiteQueenCastleFlag) {
    ret ^= whiteQueenCastle;
  }
  if (blackCastleK != blackKingCastleFlag) {
    ret ^= blackKingCastle;
  }
  if (blackCastleQ != blackQueenCastleFlag) {
    ret ^= blackQueenCastle;
  }

  if (pieceFrom == 65) {
    return ret;
  }



  // Any pawn moves. Captures may happen here
  if (i == 0) {

    // STEP 1: Check for any promotions
    if (pieceTo > 55) {
      // Remove Pawn
      ret ^= table[pieceTo][0];
      // Add queen
      ret ^= table[pieceTo][4];
    }
    else if (pieceTo < 8) {
      // Remove Pawn
      ret ^= table[pieceTo][6];
      // Add queen
      ret ^= table[pieceTo][10];
    }



    // STEP 2: Check for any enpassants
    if (useWhite && isEnpassant){
      ret ^= table[pieceTo - 8][6];
    }
    else if (isEnpassant) {
      ret ^= table[pieceTo + 8][0];
    }
  }



  // STEP 3: Check for white castling or if its a king move
  else if (i == 5){
    if (useWhite && pieceFrom == 4) {
      // King side castle
      if (pieceTo == 6) {
        // Move rook:
        ret ^= table[5][3];
        ret ^= table[7][3];

      }
      // Queen side Castle
      else if (pieceTo == 2) {
        // Move rook:
        ret ^= table[3][3];
        ret ^= table[0][3];
      }
    }
    else if (!useWhite && pieceFrom == 60){
      // King side castle
      if (pieceTo == 62) {
        // Move rook:
        ret ^= table[61][9];
        ret ^= table[63][9];
      }
      // Queen side Castle
      else if (pieceTo == 58) {
        // Move rook:
        ret ^= table[59][9];
        ret ^= table[56][9];
      }
    }

  }



  // STEP 4: Finally make the move
  if (k == -1) {

    if (useWhite) {
      ret ^= table[pieceTo][i];
      ret ^= table[pieceFrom][i];
    }
    else {
      ret ^= table[pieceTo][i + 6];
      ret ^= table[pieceFrom][i + 6];
    }

  }
  else {

    if (useWhite) {
      ret ^= table[pieceTo][i];
      ret ^= table[pieceFrom][i];
      ret ^= table[pieceTo][k + 6];
    }
    else {
      ret ^= table[pieceTo][i + 6];
      ret ^= table[pieceFrom][i + 6];
      ret ^= table[pieceTo][k];
    }

  }

  return ret;
}
