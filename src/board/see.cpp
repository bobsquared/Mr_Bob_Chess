#include "see.h"

namespace BITBOARD {

    /************************************************************************************************
    **  Static Exchange Evaluation Section.
    **  Used for additional pruning/reductions. Potentially can be used for move ordering.
    *************************************************************************************************/

    static constexpr int pvals[6] = {100, 450, 450, 650, 1250, 5000};

    // Return the bitboard location of the least valuable attacker.
    // Returns 0 (empty bitboard) if no attackers are found.
    uint64_t getLeastValuablePiece(BoardState& bs, uint64_t attadef, bool col, int &piece) {

        for (piece = col; piece < 12; piece += 2) {
            uint64_t subset = attadef & bs.pieces[piece];
            if (subset) {
                return subset & -subset;
            }
        }

        return 0;
    }

    // Takes in the index square and returns the bitboard of pieces that can attack this square
    uint64_t isAttackedSee(BoardState& bs, int index) {

        BITBOARD::PieceMoves& pm = BITBOARD::pieceMoves;
        uint64_t ret = 0;
        ret |= bs.pieces[0] & pm.pawnAttacks[index][1];
        ret |= bs.pieces[1] & pm.pawnAttacks[index][0];
        ret |= (bs.pieces[2] | bs.pieces[3]) & pm.knightMoves[index];
        ret |= (bs.pieces[4] | bs.pieces[5] | bs.pieces[8] | bs.pieces[9]) & MAGIC_BITBOARDS::bishopAttacksMask(bs.occupied, index);
        ret |= (bs.pieces[6] | bs.pieces[7] | bs.pieces[8] | bs.pieces[9]) & MAGIC_BITBOARDS::rookAttacksMask(bs.occupied, index);
        ret |= (bs.pieces[10] | bs.pieces[11]) & pm.kingMoves[index];

        return ret;

    }


    // Static Exchange Evaluation.
    // Determines whether or not a move is a winning or losing capture.
    // SEE < 0 is losing, SEE > 0 is winning
    int seeCapture(BoardState& bs, MOVE capture) {

        int gain[32];
        int d = 0;
        uint64_t mayXray = bs.pieces[0] | bs.pieces[1] | bs.pieces[4] | bs.pieces[5] | bs.pieces[6] | bs.pieces[7] | bs.pieces[8] | bs.pieces[9];

        int from = get_move_from(capture);
        int to = get_move_to(capture);
        uint64_t fromSet = 1ULL << from;

        uint64_t occ = bs.occupied;
        uint64_t attadef = isAttackedSee(bs, to);
        int aPiece = bs.pieceAt[from];
        bool isWhite = bs.toMove;

        if ((capture & PROMOTION_FLAG) != 0) {
            return 0;
        }

        if ((capture & MOVE_FLAGS) == ENPASSANT_FLAG) {
            gain[d] = pvals[0];
        }
        else {
            gain[d] = bs.pieceAt[to] == -1? 0 : pvals[bs.pieceAt[to] / 2];
        }

        do {
            d++;
            gain[d] = pvals[aPiece / 2] - gain[d - 1];
            isWhite = !isWhite;

            if (std::max(-gain[d - 1], gain[d]) < 0) {
                break;
            }

            attadef ^= fromSet;
            occ ^= fromSet;

            if (fromSet & mayXray) {
                attadef |= MAGIC_BITBOARDS::xrayAttackBishop(occ, fromSet, to) & (bs.pieces[4] | bs.pieces[5] | bs.pieces[8] | bs.pieces[9]);
                attadef |= MAGIC_BITBOARDS::xrayAttackRook(occ, fromSet, to) & (bs.pieces[6] | bs.pieces[7] | bs.pieces[8] | bs.pieces[9]);
            }

            fromSet = getLeastValuablePiece(bs, attadef, isWhite, aPiece);

        } while (fromSet);

        while (--d) {
            gain[d - 1] = -std::max(-gain[d - 1], gain[d]);
        }

        return gain[0];
    }

}