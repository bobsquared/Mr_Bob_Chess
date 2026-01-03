#include "bitboard.h"


namespace BITBOARD {
    
    PieceMoves pieceMoves;

    // Initialize features
    void InitFeatures(BoardState& bs, Accumulator& acc) {
        acc.refresh_accumulator();
        for (int i = 0; i < 12; i++) {
            uint64_t piece = bs.pieces[i];
            while (piece) {
                acc.accumulate_add(i, bitScan(piece));
                piece &= piece - 1;
            }
        }
    }



    // Initialize all black pawn attacks squares
    void InitBlackPawnAttacks() {
        for (int i = 0; i < 64; i++) {
            uint64_t tempBitBoard = 1ULL << i;
            pieceMoves.pawnAttacks[i][1] = pawnAttacksAll(tempBitBoard, 1);
        }
    }



    // Initialize all white pawn attacks squares
    void InitWhitePawnAttacks() {
        for (int i = 0; i < 64; i++) {
            uint64_t tempBitBoard = 1ULL << i;
            pieceMoves.pawnAttacks[i][0] = pawnAttacksAll(tempBitBoard, 0);
        }
    }



    // Initialize all knight moves
    void InitKnightMoves() {

        for (int i = 0; i < 64; i++) {
            uint64_t tempBitBoard = 0;
            uint64_t tempBitBoard1 = 0;
            uint64_t tempBitBoard2 = 0;
            uint64_t tempBitBoard3 = 0;
            uint64_t tempBitBoard4 = 0;

            tempBitBoard |= 1ULL << i;
            tempBitBoard1 |= (tempBitBoard << 15);
            tempBitBoard1 |= tempBitBoard >> 17;
            tempBitBoard1 &= 9187201950435737471U;

            tempBitBoard2 |= (tempBitBoard << 6);
            tempBitBoard2 |= tempBitBoard >> 10;
            tempBitBoard2 &= 13816973012072644543U;
            tempBitBoard2 &= 9187201950435737471U;

            tempBitBoard3 |= (tempBitBoard << 10);
            tempBitBoard3 |= tempBitBoard >> 6;
            tempBitBoard3 &= 18374403900871474942U;
            tempBitBoard3 &= 18302063728033398269U;

            tempBitBoard4 |= (tempBitBoard << 17);
            tempBitBoard4 |= tempBitBoard >> 15;
            tempBitBoard4 &= 18374403900871474942U;

            pieceMoves.knightMoves[i] = tempBitBoard1 | tempBitBoard2 | tempBitBoard3 | tempBitBoard4;

        }
        
    }



    // Initialize all king moves
    void InitKingMoves() {

        for (int i = 0; i < 64; i++) {
            uint64_t tempBitBoard = 0;
            uint64_t tempBitBoard1 = 0;
            uint64_t tempBitBoard2 = 0;

            tempBitBoard = 1ULL << i;

            tempBitBoard |= tempBitBoard >> 8;
            tempBitBoard |= tempBitBoard << 8;

            tempBitBoard1 |= tempBitBoard >> 1;
            tempBitBoard1 &= ~columnMask[7];

            tempBitBoard2 |= tempBitBoard << 1;
            tempBitBoard2 &= ~columnMask[0];

            tempBitBoard |= tempBitBoard1 | tempBitBoard2;
            tempBitBoard &= ~(1ULL << i);

            pieceMoves.kingMoves[i] = tempBitBoard;
        }

    }



    // Initialize all piece square locations
    void InitPieceAt(BoardState& bs) {
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 12; j++) {
                bs.pieceAt[i] = -1;
                if (bs.pieces[j] & (1ULL << i)) {
                    bs.pieceAt[i] = j;
                    break;
                }
            }
        }

        for (int i = 0; i < 64; i++) {
            if (bs.pieceAt[i] != -1) {
                assert(bs.pieces[bs.pieceAt[i]] & (1ULL << i));
            }
        }
    }



    // Initialize all piece counts
    void InitPieceCount(BoardState& bs) {
        for (int i = 0; i < 12; i++) {
            bs.pieceCount[i] = count_population(bs.pieces[i]);
        }
    }



    // Initialize castling flags
    void InitRookCastleFlags(uint64_t whiteRooks, uint64_t blackRooks) {
        for (int i = 0; i < 64; i++) {
            pieceMoves.rookCastleFlagMask[i] = 15;
        }

        int count = 0;
        while (whiteRooks) {
            pieceMoves.rookCastleFlagMask[bitScan(whiteRooks)] ^= count == 0? CASTLE_FLAG_QUEEN_WHITE : CASTLE_FLAG_KING_WHITE;
            count++;
            whiteRooks &= whiteRooks - 1;
        }

        count = 0;
        while (blackRooks) {
            pieceMoves.rookCastleFlagMask[bitScan(blackRooks)] ^= count == 0? CASTLE_FLAG_QUEEN_BLACK : CASTLE_FLAG_KING_BLACK;
            count++;
            blackRooks &= blackRooks - 1;
        }
    }

    void InitBoard() {
        InitBlackPawnAttacks();
        InitWhitePawnAttacks();
        InitKnightMoves();
        InitKingMoves();
    }

    void CopyBoard(const BoardState &bOrig, BoardState &bTarget) {
        std::copy(bOrig.pieceAt, bOrig.pieceAt + 64, bTarget.pieceAt);
        std::copy(bOrig.pieces, bOrig.pieces + 12, bTarget.pieces);
        std::copy(bOrig.color, bOrig.color + 2, bTarget.color);
        std::copy(bOrig.pieceCount, bOrig.pieceCount + 12, bTarget.pieceCount);

        bTarget.enpassantSq = bOrig.enpassantSq;
        bTarget.occupied = bOrig.occupied;
        bTarget.toMove = bOrig.toMove;
        bTarget.castleRights = bOrig.castleRights;
        bTarget.halfMoves = bOrig.halfMoves;
        bTarget.fullMoves = bOrig.fullMoves;
        bTarget.kingLoc[0] = bOrig.kingLoc[0];
        bTarget.kingLoc[1] = bOrig.kingLoc[1];

        bTarget.posKey = bOrig.posKey;
    }


    void CopyBoardHistory(const MoveInfoStack &mOrig, MoveInfoStack &mTarget) {
        mTarget.count = mOrig.count;
        std::copy(mOrig.moves, mOrig.moves + mOrig.count, mTarget.moves);
    }

    void CopyAllBoard(const Board &bOrig, Board &bTarget) {
        CopyBoard(bOrig.state, bTarget.state);
        CopyBoardHistory(bOrig.moveHistory, bTarget.moveHistory);
        InitFeatures(bTarget.state, bTarget.acc);
    }

    // Reset the position to the standard chess position
    void reset(Board& b) {

        BoardState& bs = b.state;
        MoveInfoStack& moveHistory = b.moveHistory;
        Accumulator& acc = b.acc;

        bs.pieces[0] = (1ULL << 8) | (1ULL << 9) | (1ULL << 10) | (1ULL << 11) | (1ULL << 12) | (1ULL << 13) | (1ULL << 14) | (1ULL << 15);
        bs.pieces[1] = (1ULL << 48) | (1ULL << 49) | (1ULL << 50) | (1ULL << 51) | (1ULL << 52) | (1ULL << 53) | (1ULL << 54) | (1ULL << 55);

        bs.pieces[2] = (1ULL << 1) | (1ULL << 6);
        bs.pieces[3] = (1ULL << 62) | (1ULL << 57);

        bs.pieces[4] = (1ULL << 2) | (1ULL << 5);
        bs.pieces[5] = (1ULL << 61) | (1ULL << 58);

        bs.pieces[6] = 1 | (1ULL << 7);
        bs.pieces[7] = (1ULL << 56) | (1ULL << 63);

        bs.pieces[8] = 1ULL << 3;
        bs.pieces[9] = 1ULL << 59;

        bs.pieces[10] = 1ULL << 4;
        bs.pieces[11] = 1ULL << 60;

        bs.color[0] = bs.pieces[0] | bs.pieces[2] | bs.pieces[4] | bs.pieces[6] | bs.pieces[8] | bs.pieces[10];
        bs.color[1] = bs.pieces[1] | bs.pieces[3] | bs.pieces[5] | bs.pieces[7] | bs.pieces[9] | bs.pieces[11];
        bs.occupied = bs.color[0] | bs.color[1];

        bs.enpassantSq = 0;
        bs.fullMoves = 1;
        bs.halfMoves = 0;
        bs.castleRights = 15;

        bs.toMove = false;
        InitPieceAt(bs);
        InitPieceCount(bs);
        moveHistory.clear();
        bs.posKey = ZOBRIST::hashBoard(bs.pieces, bs.castleRights, bs.enpassantSq, bs.toMove);
        bs.kingLoc[0] = bitScan(bs.pieces[10]);
        bs.kingLoc[1] = bitScan(bs.pieces[11]);

        InitFeatures(bs, acc);
        InitRookCastleFlags(bs.pieces[6], bs.pieces[7]);
    }


}



