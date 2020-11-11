


// Adapted from https://en.wikipedia.org/wiki/Zobrist_hashing
#include "zobrist_hashing.h"



// Initialize zobrist hashing table
// Random 64 bit number for each square and pieces
// Collisions are possible but rare.
Zobrist::Zobrist() {

    std::mt19937_64 rng;
    rng.seed(0);

    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 12; j++) {
            table[i][j] = (uint64_t)((rng() & 0xffff) | ((rng() & 0xffff) << 16) | (((uint64_t)rng() & 0xffff) << 32) | (((uint64_t)rng() & 0xffff) << 48));
        }
    }

    for (int i = 0; i < 8; i++) {
        enpassant[i] = (uint64_t)((rng() & 0xffff) | ((rng() & 0xffff) << 16) | (((uint64_t)rng() & 0xffff) << 32) | (((uint64_t)rng() & 0xffff) << 48));
    }

    for (int i = 0; i < 4; i++) {
        castle[i] = (uint64_t)((rng() & 0xffff) | ((rng() & 0xffff) << 16) | (((uint64_t)rng() & 0xffff) << 32) | (((uint64_t)rng() & 0xffff) << 48));
    }

    // Flags for castling, and black turn
    blackTurn = (uint64_t)((rng() & 0xffff) | ((rng() & 0xffff) << 16) | (((uint64_t)rng() & 0xffff) << 32) | (((uint64_t)rng() & 0xffff) << 48));

}



// Non iterative way to determine the hash key: loop through the board
// Used to initialize the board's starting position
uint64_t Zobrist::hashBoard(uint64_t *pieces, uint8_t castleFlag, int enpassantSq, bool col) {

    uint64_t ret = 0;

    if (col) {
        ret ^= blackTurn;
    }

    hashBoard_castle(ret, 15);
    hashBoard_castle(ret, castleFlag);
    if (enpassantSq) {
        hashBoard_enpassant(ret, enpassantSq % 8);
    }


    for (int i = 0; i < 64; ++i) {
        uint64_t shiftI = 1ULL << i;
        for (int j = 0; j < 12; j++) {
            if (pieces[j] & shiftI) {
                ret ^= table[i][j];
                break;
            }
        }
    }

    return ret;
}



// Non iterative way to determine the hash key: loop through the board
// Used to initialize the board's starting position
uint64_t Zobrist::hashBoardPawns(uint64_t *pieces) {

    uint64_t ret = 0;

    for (int i = 0; i < 64; ++i) {
        uint64_t shiftI = 1ULL << i;
        for (int j = 0; j < 2; j++) {
            if (pieces[j] & shiftI) {
                ret ^= table[i][j];
                break;
            }
        }
    }

    return ret;
}



// Iterative way to determine hash key:
// Used in making moves to keep track of the key much faster
void Zobrist::hashBoard_quiet(uint64_t &board, int from, int to, int pieceFrom) {
    board ^= table[from][pieceFrom];
    board ^= table[to][pieceFrom];
}


// Iterative way to determine hash key:
// Update captures
void Zobrist::hashBoard_capture(uint64_t &board, int from, int to, int pieceFrom, int pieceTo) {
    board ^= table[from][pieceFrom];
    board ^= table[to][pieceFrom];
    board ^= table[to][pieceTo];
}



// Iterative way to determine hash key:
// Update specific square
void Zobrist::hashBoard_square(uint64_t &board, int square, int piece) {
    board ^= table[square][piece];
}



// Iterative way to determine hash key:
// Update capture promotions
void Zobrist::hashBoard_capture_promotion(uint64_t &board, int from, int to, int pieceFrom, int pieceTo, int promotionPiece) {
    board ^= table[from][pieceFrom];
    board ^= table[to][pieceTo];
    board ^= table[to][promotionPiece];
}



// Iterative way to determine hash key:
// Update promotions
void Zobrist::hashBoard_promotion(uint64_t &board, int from, int to, int pieceFrom, int promotionPiece) {
    board ^= table[from][pieceFrom];
    board ^= table[to][promotionPiece];
}



// Iterative way to determine hash key:
// Update enpassant square
void Zobrist::hashBoard_enpassant(uint64_t &board, int square) {
    board ^= enpassant[square % 8];
}



// Iterative way to determine hash key:
// Update castling rights
void Zobrist::hashBoard_castle(uint64_t &board, uint8_t castleFlag) {
    while (castleFlag) {
        board ^= castle[bitScan(castleFlag)];
        castleFlag &= castleFlag - 1;
    }
}



// Iterative way to determine hash key:
// Update turn to move
void Zobrist::hashBoard_turn(uint64_t &board) {
    board ^= blackTurn;
}
