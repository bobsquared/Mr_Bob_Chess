#ifndef BITACCU_H
#define BITACCU_H

#include <cstdint>


namespace BITACCU {

    struct BitAccu {
        uint64_t addsWhites[12] = {0};
        uint64_t addsBlacks[12] = {0};
        uint64_t removesWhites[12] = {0};
        uint64_t removesBlacks[12] = {0};
    };
    

    inline void Refresh(BitAccu &bitAcc) {
        for (int i = 0; i < 12; i++) {
            bitAcc.addsWhites[i] = 0;
        }
        for (int i = 0; i < 12; i++) {
            bitAcc.addsBlacks[i] = 0;
        }
        for (int i = 0; i < 12; i++) {
            bitAcc.removesWhites[i] = 0;
        }
        for (int i = 0; i < 12; i++) {
            bitAcc.removesBlacks[i] = 0;
        }
    }

    inline void Add(int piece, int loc, BitAccu &bitAcc) {
        int blackPiece = piece + ((piece & 1) * -2) + 1;

        bitAcc.addsWhites[piece] |= (1ULL << loc);
        bitAcc.addsBlacks[blackPiece] |= (1ULL << (loc ^ 56));

        uint64_t maskWhite = bitAcc.addsWhites[piece] & bitAcc.removesWhites[piece];
        uint64_t maskBlack = bitAcc.addsBlacks[blackPiece] & bitAcc.removesBlacks[blackPiece];

        bitAcc.addsWhites[piece] ^= maskWhite;
        bitAcc.addsBlacks[blackPiece] ^= maskBlack;
        bitAcc.removesWhites[piece] ^= maskWhite;
        bitAcc.removesBlacks[blackPiece] ^= maskBlack;
    }

    inline void Remove(int piece, int loc, BitAccu &bitAcc) {
        int blackPiece = piece + ((piece & 1) * -2) + 1;

        bitAcc.removesWhites[piece] |= (1ULL << loc);
        bitAcc.removesBlacks[blackPiece] |= (1ULL << (loc ^ 56));

        uint64_t maskWhite = bitAcc.addsWhites[piece] & bitAcc.removesWhites[piece];
        uint64_t maskBlack = bitAcc.addsBlacks[blackPiece] & bitAcc.removesBlacks[blackPiece];

        bitAcc.addsWhites[piece] ^= maskWhite;
        bitAcc.addsBlacks[blackPiece] ^= maskBlack;
        bitAcc.removesWhites[piece] ^= maskWhite;
        bitAcc.removesBlacks[blackPiece] ^= maskBlack;
    }

}




#endif