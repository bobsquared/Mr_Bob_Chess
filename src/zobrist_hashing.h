
#pragma once
#include <unordered_map>
#include <stdlib.h>


// Zobrist hashing
class Zobrist {

public:

    Zobrist();

    // Determine hashing key
    uint64_t hashBoard(uint64_t *pieces, uint8_t castleFlag, int enpassantSq, bool col);
    uint64_t hashBoardPawns(uint64_t *pieces);
    uint64_t hashBoardM(uint64_t board, int pieceFrom, int pieceTo, int moveType, int captureType, bool turn, bool isEnpassant, bool whiteCastleK, bool whiteCastleQ, bool blackCastleK, bool blackCastleQ);

    void hashBoard_quiet(uint64_t &board, int from, int to, int pieceFrom);
    void hashBoard_capture(uint64_t &board, int from, int to, int pieceFrom, int pieceTo);
    void hashBoard_square(uint64_t &board, int square, int piece);
    void hashBoard_turn(uint64_t &board);
    void hashBoard_capture_promotion(uint64_t &board, int from, int to, int pieceFrom, int pieceTo, int promotionPiece);
    void hashBoard_promotion(uint64_t &board, int from, int to, int pieceFrom, int promotionPiece);
    void hashBoard_castle(uint64_t &board, uint8_t castleFlag);
    void hashBoard_enpassant(uint64_t &board, int square);


private:

    // table and flags

    uint64_t table[64][12];
    uint64_t enpassant[8];
    uint64_t castle[4];
    uint64_t blackTurn;
    uint64_t whiteKingCastle;
    uint64_t blackKingCastle;
    uint64_t whiteQueenCastle;
    uint64_t blackQueenCastle;

    bool whiteKingCastleFlag;
    bool blackKingCastleFlag;
    bool whiteQueenCastleFlag;
    bool blackQueenCastleFlag;

};
