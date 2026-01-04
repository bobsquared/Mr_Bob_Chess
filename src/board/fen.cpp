#include "fen.h"

namespace BITBOARD {

     // Converts the current position to FEN
    std::string posToFEN(BoardState& bs) {

        std::string fen = "";
        std::string cStr = "";
        const std::string NUM_TO_STR[9] = {"0", "1", "2", "3", "4", "5", "6", "7", "8"};

        for (uint8_t i = 0; i < 8; i++) {
            std::string rank = "";
            uint8_t count = 0;


            for (uint8_t j = 0; j < 8; j++) {
                uint64_t calc = (1ULL << ((i * 8) + j));

                if (calc & bs.occupied) {

                    if (count > 0) {
                        cStr = NUM_TO_STR[count];
                        rank += cStr;
                    }
                    count = 0;
                    if (calc & bs.pieces[0]) {
                        rank += "P";
                    }
                    else if (calc & bs.pieces[2]) {
                        rank += "N";
                    }
                    else if (calc & bs.pieces[4]) {
                        rank += "B";
                    }
                    else if (calc & bs.pieces[6]) {
                        rank += "R";
                    }
                    else if (calc & bs.pieces[8]) {
                        rank += "Q";
                    }
                    else if (calc & bs.pieces[10]) {
                        rank += "K";
                    }
                    else if (calc & bs.pieces[1]) {
                        rank += "p";
                    }
                    else if (calc & bs.pieces[3]) {
                        rank += "n";
                    }
                    else if (calc & bs.pieces[5]) {
                        rank += "b";
                    }
                    else if (calc & bs.pieces[7]) {
                        rank += "r";
                    }
                    else if (calc & bs.pieces[9]) {
                        rank += "q";
                    }
                    else if (calc & bs.pieces[11]) {
                        rank += "k";
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

        fen += bs.toMove? " b" : " w";
        std::string crights = "";
        if (bs.castleRights & 8) {
            crights += "K";
        }
        if (bs.castleRights & 4) {
            crights += "Q";
        }
        if (bs.castleRights & 2) {
            crights += "k";
        }
        if (bs.castleRights & 1) {
            crights += "q";
        }

        fen += bs.castleRights? " " + crights : " -";
        fen += bs.enpassantSq? " " + TO_ALG[bs.enpassantSq] : " -";
        fen += " " + std::to_string(bs.halfMoves);
        fen += " "  + std::to_string(bs.fullMoves);

        return fen;
    }





    // Take a FEN position as a string and set the posiiton.
    void setPosFen(BoardState& bs, Accumulator& acc, std::string fen) {

        // Clear all bitboards
        std::string pieceIndex = "PpNnBbRrQqKk";
        bs.color[0] = 0;
        bs.color[1] = 0;
        bs.occupied = 0;

        bs.posKey = 0;

        bs.halfMoves = 0;
        bs.fullMoves = 1;
        bs.enpassantSq = 0;
        bs.toMove = false;
        acc.refresh_accumulator();

        for (int i = 0; i < 12; i++) {
            bs.pieces[i] = 0;
            bs.pieceCount[i] = 0;
        }

        for (int i = 0; i < 64; i++) {
            bs.pieceAt[i] = -1;
        }

        int lineOffset = 56;
        size_t posIndex = fen.find(" ");
        for (size_t i = 0; i < posIndex; i++) {
            if (std::isdigit(fen[i])) {
                lineOffset += fen[i] - 48;
            }
            else if (fen[i] == '/') {
                lineOffset -= 16;
            }
            else {
                int piece = pieceIndex.find(fen[i]);
                bs.pieces[piece] |= 1ULL << lineOffset;
                bs.pieceCount[piece]++;
                bs.pieceAt[lineOffset] = piece;
                ZOBRIST::hashBoard_square(bs.posKey, lineOffset, piece);
                acc.accumulate_add(piece, lineOffset);

                if (piece / 2 == 5) {
                    bs.kingLoc[piece - 10] = lineOffset;
                }

                lineOffset++;
            }
        }


        bs.color[0] = bs.pieces[0] | bs.pieces[2] | bs.pieces[4] | bs.pieces[6] | bs.pieces[8] | bs.pieces[10];
        bs.color[1] = bs.pieces[1] | bs.pieces[3] | bs.pieces[5] | bs.pieces[7] | bs.pieces[9] | bs.pieces[11];
        bs.occupied = bs.color[0] | bs.color[1];


        posIndex++;
        if (fen[posIndex] == 'b') {
            bs.toMove = true;
            ZOBRIST::hashBoard_turn(bs.posKey);
        }
        posIndex++;


        posIndex++;
        bs.castleRights = 0;
        if (fen[posIndex] != '-') {

            if (fen[posIndex] == 'K') {
                bs.castleRights ^= 8;
                posIndex++;
            }

            if (fen[posIndex] == 'Q') {
                bs.castleRights ^= 4;
                posIndex++;
            }

            if (fen[posIndex] == 'k') {
                bs.castleRights ^= 2;
                posIndex++;
            }

            if (fen[posIndex] == 'q') {
                bs.castleRights ^= 1;
                posIndex++;
            }

            posIndex--;

        }

        ZOBRIST::hashBoard_castle(bs.posKey, bs.castleRights ^ 15);

        posIndex += 2;
        if (fen[posIndex] != '-') {
            bs.enpassantSq = TO_NUM[fen.substr(posIndex, 2)];
            posIndex++;
            if (bs.enpassantSq) {
                ZOBRIST::hashBoard_enpassant(bs.posKey, bs.enpassantSq);
            }
        }
        posIndex += 2;

        bs.halfMoves = std::isdigit(fen[posIndex + 1])? 10 * (fen[posIndex] - 48) + (fen[posIndex + 1] - 48) : fen[posIndex] - 48;
        posIndex += std::isdigit(fen[posIndex + 1])? 3 : 2;

        // fullMoves = std::isdigit(fen[posIndex + 1])? 10 * (fen[posIndex] - 48) + (fen[posIndex + 1] - 48) : fen[posIndex] - 48;

    }
}