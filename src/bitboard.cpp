#include "bitboard.h"

std::regex fenNumbers(".*\\s+(\\d+)\\s+(\\d+)");
extern Magics *magics;
extern int pieceValues[6];
extern Zobrist *zobrist;




Bitboard::Bitboard() {

    whitePawns = (1ULL << 8) | (1ULL << 9) | (1ULL << 10) | (1ULL << 11) | (1ULL << 12) | (1ULL << 13) | (1ULL << 14) | (1ULL << 15);
    blackPawns = (1ULL << 48) | (1ULL << 49) | (1ULL << 50) | (1ULL << 51) | (1ULL << 52) | (1ULL << 53) | (1ULL << 54) | (1ULL << 55);
    whiteQueens = 1ULL << 3;
    blackQueens = 1ULL << 59;
    InitBlackPawnAttacks();
    InitWhitePawnAttacks();
    InitKnightMoves();
    InitBishopMoves();
    InitRookMoves();
    InitKingMoves();

    reset();
    InitRookCastleFlags(pieces[6], pieces[7]);
}



Bitboard::Bitboard(const Bitboard &b) {

    whitePawns = (1ULL << 8) | (1ULL << 9) | (1ULL << 10) | (1ULL << 11) | (1ULL << 12) | (1ULL << 13) | (1ULL << 14) | (1ULL << 15);
    blackPawns = (1ULL << 48) | (1ULL << 49) | (1ULL << 50) | (1ULL << 51) | (1ULL << 52) | (1ULL << 53) | (1ULL << 54) | (1ULL << 55);
    whiteQueens = 1ULL << 3;
    blackQueens = 1ULL << 59;
    InitBlackPawnAttacks();
    InitWhitePawnAttacks();
    InitKnightMoves();
    InitBishopMoves();
    InitRookMoves();
    InitKingMoves();

    std::copy(b.pieceAt, b.pieceAt + 64, pieceAt);
    std::copy(b.pieces, b.pieces + 12, pieces);
    std::copy(b.color, b.color + 2, color);
    std::copy(b.pieceCount, b.pieceCount + 12, pieceCount);

    moveHistory.count = b.moveHistory.count;
    enpassantSq = b.enpassantSq;
    occupied = b.occupied;
    toMove = b.toMove;
    castleRights = b.castleRights;
    halfMoves = b.halfMoves;
    fullMoves = b.fullMoves;
    kingLoc[0] = b.kingLoc[0];
    kingLoc[1] = b.kingLoc[1];

    std::copy(b.moveHistory.move, b.moveHistory.move + b.moveHistory.count, moveHistory.move);

    posKey = b.getPosKey();

    InitRookCastleFlags(whiteRooks, blackRooks);
    InitFeatures();
}



std::vector<Accumulator::Features>* Bitboard::getAddFeatures() {
    return acc.getAddFeatures();
}



std::vector<Accumulator::Features>* Bitboard::getRemoveFeatures() {
    return acc.getRemoveFeatures();
}



float* Bitboard::getFeatures() {
    return acc.getFeatures();
}


bool Bitboard::getResetFlag() {
    return acc.resetFlag;
}

void Bitboard::setResetFlag(bool f) {
    acc.resetFlag = f;
}



/************************************************************************************************
**  Initialization section
**  Initialize the position and all bitboards, pointers, etc..
*************************************************************************************************/


// Reset the position to the standard chess position
void Bitboard::reset() {

    color[0] = whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKings;
    color[1] = blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKings;
    occupied = color[0] | color[1];

    pieces[0] = whitePawns;
    pieces[1] = blackPawns;
    pieces[2] = whiteKnights;
    pieces[3] = blackKnights;
    pieces[4] = whiteBishops;
    pieces[5] = blackBishops;
    pieces[6] = whiteRooks;
    pieces[7] = blackRooks;
    pieces[8] = whiteQueens;
    pieces[9] = blackQueens;
    pieces[10] = whiteKings;
    pieces[11] = blackKings;

    enpassantSq = 0;
    fullMoves = 1;
    halfMoves = 0;
    castleRights = 15;

    toMove = false;
    InitPieceAt();
    InitPieceCount();
    moveHistory.clear();
    posKey = zobrist->hashBoard(pieces, castleRights, enpassantSq, toMove);
    kingLoc[0] = bitScan(pieces[10]);
    kingLoc[1] = bitScan(pieces[11]);

    InitFeatures();
}



// Initialize features
void Bitboard::InitFeatures() {
    acc.refresh_accumulator();
    for (int i = 0; i < 12; i++) {
        uint64_t piece = pieces[i];
        while (piece) {
            acc.accumulate_add(i, bitScan(piece));
            piece &= piece - 1;
        }
    }
}



// Initialize all black pawn attacks squares
void Bitboard::InitBlackPawnAttacks() {
    for (int i = 0; i < 64; i++) {
        uint64_t tempBitBoard = 1ULL << i;
        pawnAttacks[i][1] = pawnAttacksAll(tempBitBoard, 1);
    }
}



// Initialize all white pawn attacks squares
void Bitboard::InitWhitePawnAttacks() {
    for (int i = 0; i < 64; i++) {
        uint64_t tempBitBoard = 1ULL << i;
        pawnAttacks[i][0] = pawnAttacksAll(tempBitBoard, 0);
    }
}



// Initialize all knight moves
void Bitboard::InitKnightMoves() {

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

        knightMoves[i] = tempBitBoard1 | tempBitBoard2 | tempBitBoard3 | tempBitBoard4;

    }

    whiteKnights = (1ULL << 1) | (1ULL << 6);
    blackKnights = (1ULL << 62) | (1ULL << 57);
}



// Initialize all bishop moves
void Bitboard::InitBishopMoves() {
    whiteBishops = (1ULL << 2) | (1ULL << 5);
    blackBishops = (1ULL << 61) | (1ULL << 58);
}



// Initialize all rook moves
void Bitboard::InitRookMoves() {
    whiteRooks = 1 | (1ULL << 7);
    blackRooks = (1ULL << 56) | (1ULL << 63);
}



// Initialize all king moves
void Bitboard::InitKingMoves() {

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

        kingMoves[i] = tempBitBoard;
    }

    whiteKings = 1ULL << 4;
    blackKings = 1ULL << 60;

}



// Initialize all piece square locations
void Bitboard::InitPieceAt() {
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 12; j++) {
            pieceAt[i] = -1;
            if (pieces[j] & (1ULL << i)) {
                pieceAt[i] = j;
                break;
            }
        }
    }

    for (int i = 0; i < 64; i++) {
        if (pieceAt[i] != -1) {
            assert(pieces[pieceAt[i]] & (1ULL << i));
        }
    }
}



// Initialize all piece counts
void Bitboard::InitPieceCount() {
    for (int i = 0; i < 12; i++) {
        pieceCount[i] = count_population(pieces[i]);
    }
}



// Initialize castling flags
void Bitboard::InitRookCastleFlags(uint64_t whiteRooks, uint64_t blackRooks) {
    for (int i = 0; i < 64; i++) {
        rookCastleFlagMask[i] = 15;
    }

    int count = 0;
    while (whiteRooks) {
        rookCastleFlagMask[bitScan(whiteRooks)] ^= count == 0? CASTLE_FLAG_QUEEN_WHITE : CASTLE_FLAG_KING_WHITE;
        count++;
        whiteRooks &= whiteRooks - 1;
    }

    count = 0;
    while (blackRooks) {
        rookCastleFlagMask[bitScan(blackRooks)] ^= count == 0? CASTLE_FLAG_QUEEN_BLACK : CASTLE_FLAG_KING_BLACK;
        count++;
        blackRooks &= blackRooks - 1;
    }
}




/************************************************************************************************
**  Moves section
**  Changing the position of the board.
*************************************************************************************************/


// Move a piece that doesn't involve captures.
void Bitboard::move_quiet(int from, int to, int piece, uint64_t i1i2) {
    color[toMove] ^= i1i2;
    pieces[piece] ^= i1i2;
    occupied      ^= i1i2;
    pieceAt[to]    = piece;
    pieceAt[from]  = -1;
    acc.accumulate_add(piece, to);
    acc.accumulate_remove(piece, from);
}



// Make null move.
void Bitboard::make_null_move() {
    toMove = !toMove;
    moveHistory.insert(MoveInfo(0, enpassantSq, halfMoves, castleRights, posKey, NULL_MOVE));

    if (enpassantSq) {
        zobrist->hashBoard_enpassant(posKey, enpassantSq);
        enpassantSq = 0;
    }

    zobrist->hashBoard_turn(posKey);
    return;
}


// Undo null move
void Bitboard::undo_null_move() {
    toMove = !toMove;
    MoveInfo moveInfo = moveHistory.pop();
    halfMoves = moveInfo.halfMoves;
    castleRights = moveInfo.castleRights;
    enpassantSq = moveInfo.enpassantSq;
    posKey = moveInfo.posKey;
    return;
}



// Make a move or null move.
// This involves any legal moves in the game of chess.
void Bitboard::make_move(MOVE move) {

    int from = get_move_from(move);
    int to = get_move_to(move);

    int fromPiece = pieceAt[from];
    int toPiece = pieceAt[to];

    uint64_t i1 = 1ULL << from;
    uint64_t i2 = 1ULL << to;
    uint64_t i1i2 = i1 | i2;

    uint8_t crights = castleRights;
    int hmoves = halfMoves;
    int enSq = enpassantSq;
    uint64_t prevPosKey = posKey;
    int moveFlags = move & MOVE_FLAGS;
    uint8_t cflag = toMove? CASTLE_FLAG_BLACK : CASTLE_FLAG_WHITE;

    halfMoves++;
    fullMoves += toMove;

    if (enpassantSq) {
        zobrist->hashBoard_enpassant(posKey, enpassantSq);
        enpassantSq = 0;
    }

    // Update half moves
    if (fromPiece == toMove) {
        halfMoves = 0;
    }

    // Update castling rights
    if (fromPiece == 6 + toMove) {
        zobrist->hashBoard_castle(posKey, castleRights & ~rookCastleFlagMask[from]);
        castleRights &= rookCastleFlagMask[from];
    }
    else if (fromPiece == 10 + toMove) {
        if (cflag) {
            zobrist->hashBoard_castle(posKey, castleRights & cflag);
            castleRights &= ~cflag;
        }
        kingLoc[toMove] = to;
    }

    assert(fromPiece != -1);
    assert(from != 0 || to != 0);

    if (moveFlags == QUIET_MOVES_FLAG) {
        assert(toPiece == -1);
        move_quiet(from, to, fromPiece, i1i2);
        zobrist->hashBoard_quiet(posKey, from, to, fromPiece);
    }
    else if (moveFlags == ENPASSANT_FLAG) {
        assert(toPiece == -1);
        assert(fromPiece == toMove);
        move_quiet(from, to, fromPiece, i1i2);

        uint64_t toCap = to + (toMove * 2 - 1) * 8;
        zobrist->hashBoard_quiet(posKey, from, to, fromPiece);
        zobrist->hashBoard_square(posKey, toCap, !toMove);
        color[!toMove] ^= 1ULL << toCap;
        pieces[!toMove] ^= 1ULL << toCap;
        occupied ^= 1ULL << toCap;
        pieceAt[toCap] = -1;
        pieceCount[!toMove]--;
        acc.accumulate_remove(!toMove, toCap);
    }
    else if (move & CAPTURE_FLAG) {
        assert(toPiece != -1);

        if (toPiece == 6 + !toMove) {
            zobrist->hashBoard_castle(posKey, castleRights & ~rookCastleFlagMask[to]);
            castleRights &= rookCastleFlagMask[to];
        }

        color[toMove] ^= i1i2;
        color[!toMove] ^= i2;
        pieceAt[from] = -1;
        pieces[toPiece] ^= i2;
        occupied ^= i1;
        pieceCount[toPiece]--;
        acc.accumulate_remove(fromPiece, from);
        acc.accumulate_remove(toPiece, to);

        if (move & PROMOTION_FLAG) {
            assert(fromPiece == toMove);
            int pieceVal = (moveFlags - 11);
            int promotePiece = pieceVal * 2 + toMove;
            pieces[fromPiece] ^= i1;
            pieces[promotePiece] ^= i2;
            pieceAt[to] = promotePiece;
            pieceCount[toMove]--;
            pieceCount[promotePiece]++;
            zobrist->hashBoard_capture_promotion(posKey, from, to, fromPiece, toPiece, promotePiece);
            acc.accumulate_add(promotePiece, to);
        }
        else {
            pieceAt[to] = fromPiece;
            pieces[fromPiece] ^= i1i2;
            zobrist->hashBoard_capture(posKey, from, to, fromPiece, toPiece);
            acc.accumulate_add(fromPiece, to);
        }

        halfMoves = 0;
    }
    else if (moveFlags == DOUBLE_PAWN_PUSH_FLAG) {
        assert(toPiece == -1);
        move_quiet(from, to, fromPiece, i1i2);
        enpassantSq = to + (toMove * 2 - 1) * 8;
        zobrist->hashBoard_quiet(posKey, from, to, fromPiece);
        zobrist->hashBoard_enpassant(posKey, enpassantSq);
    }
    else if (move & PROMOTION_FLAG) {
        assert(toPiece == -1);
        assert(fromPiece == toMove);
        color[toMove] ^= i1i2;
        pieceAt[from] = -1;
        pieces[fromPiece] ^= i1;
        occupied ^= i1i2;

        int pieceVal = (moveFlags - 7);
        int promotePiece = pieceVal * 2 + toMove;
        pieces[promotePiece] ^= i2;
        pieceAt[to] = promotePiece;
        pieceCount[toMove]--;
        pieceCount[promotePiece]++;
        zobrist->hashBoard_promotion(posKey, from, to, fromPiece, promotePiece);
        acc.accumulate_add(promotePiece, to);
        acc.accumulate_remove(fromPiece, from);
    }
    else if (moveFlags == KING_CASTLE_FLAG) {
        assert(toPiece == -1);
        assert(fromPiece == 10 + toMove);
        move_quiet(from, to, fromPiece, i1i2);
        move_quiet(to + 1, to - 1, 6 + toMove, 1ULL << (to - 1) | 1ULL << (to + 1));
        zobrist->hashBoard_quiet(posKey, from, to, fromPiece);
        zobrist->hashBoard_quiet(posKey, to + 1, to - 1, 6 + toMove);
    }
    else if (moveFlags == QUEEN_CASTLE_FLAG) {
        assert(toPiece == -1);
        assert(fromPiece == 10 + toMove);
        move_quiet(from, to, fromPiece, i1i2);
        move_quiet(to - 2, to + 1, 6 + toMove, 1ULL << (to - 2) | 1ULL << (to + 1));
        zobrist->hashBoard_quiet(posKey, from, to, fromPiece);
        zobrist->hashBoard_quiet(posKey, to - 2, to + 1, 6 + toMove);
    }

    toMove = !toMove;
    zobrist->hashBoard_turn(posKey);
    moveHistory.insert(MoveInfo(toPiece, enSq, hmoves, crights, prevPosKey, move));
}


// Undo a move or null move.
// This involves any legal moves in the game of chess.
void Bitboard::undo_move(MOVE move) {

    toMove = !toMove;
    zobrist->hashBoard_turn(posKey);
    int from = get_move_from(move);
    int to = get_move_to(move);
    int toPiece = pieceAt[to];

    uint64_t i1 = 1ULL << from;
    uint64_t i2 = 1ULL << to;
    uint64_t i1i2 = i1 | i2;
    int moveFlags = move & MOVE_FLAGS;

    MoveInfo moveInfo = moveHistory.pop();
    halfMoves = moveInfo.halfMoves;
    castleRights = moveInfo.castleRights;
    enpassantSq = moveInfo.enpassantSq;
    posKey = moveInfo.posKey;

    fullMoves -= toMove;

    if (toPiece == 10 + toMove) {
        kingLoc[toMove] = from;
    }

    if (moveFlags == QUIET_MOVES_FLAG) {
        move_quiet(to, from, toPiece, i1i2);
    }
    else if (moveFlags == ENPASSANT_FLAG) {
        assert(toPiece == toMove);
        move_quiet(to, from, toPiece, i1i2);

        uint64_t toCap = to + (toMove * 2 - 1) * 8;
        color[!toMove] ^= 1ULL << toCap;
        pieces[!toMove] ^= 1ULL << toCap;
        occupied ^= 1ULL << toCap;
        pieceAt[toCap] = !toMove;
        pieceCount[!toMove]++;
        acc.accumulate_add(!toMove, toCap);
    }
    else if (move & CAPTURE_FLAG) {

        color[toMove] ^= i1i2;
        color[!toMove] ^= i2;
        pieces[moveInfo.captureType] ^= i2;
        pieceAt[to] = moveInfo.captureType;
        occupied ^= i1;
        pieceCount[moveInfo.captureType]++;
        acc.accumulate_add(moveInfo.captureType, to);

        if (move & PROMOTION_FLAG) {
            int pieceVal = (moveFlags - 11);
            int promotePiece = pieceVal * 2 + toMove;
            pieceAt[from] = toMove;
            pieces[toMove] ^= i1;
            pieces[promotePiece] ^= i2;
            pieceCount[toMove]++;
            pieceCount[promotePiece]--;
            acc.accumulate_remove(promotePiece, to);
            acc.accumulate_add(toMove, from);
        }
        else {
            pieces[toPiece] ^= i1i2;
            pieceAt[from] = toPiece;
            acc.accumulate_remove(toPiece, to);
            acc.accumulate_add(toPiece, from);
        }

    }
    else if (moveFlags == DOUBLE_PAWN_PUSH_FLAG) {
        assert(toPiece == toMove);
        move_quiet(to, from, toPiece, i1i2);
    }
    else if (move & PROMOTION_FLAG) {
        color[toMove] ^= i1i2;
        pieceAt[from] = 0 + toMove;
        pieceAt[to] = -1;
        pieces[0 + toMove] ^= i1;
        occupied ^= i1i2;

        int pieceVal = (moveFlags - 7);
        int promotePiece = pieceVal * 2 + toMove;
        pieces[promotePiece] ^= i2;
        pieceCount[toMove]++;
        pieceCount[promotePiece]--;
        acc.accumulate_remove(promotePiece, to);
        acc.accumulate_add(toMove, from);

    }
    else if (moveFlags == KING_CASTLE_FLAG) {
        assert(toPiece == 10 + toMove);
        move_quiet(to, from, toPiece, i1i2);
        move_quiet(to - 1, to + 1, 6 + toMove, 1ULL << (to - 1) | 1ULL << (to + 1));
    }
    else if (moveFlags == QUEEN_CASTLE_FLAG) {
        assert(toPiece == 10 + toMove);
        move_quiet(to, from, toPiece, i1i2);
        move_quiet(to + 1, to - 2, 6 + toMove, 1ULL << (to - 2) | 1ULL << (to + 1));
    }
}


/************************************************************************************************
**  Checks and legal moves section
**  Used for determining whether a player is in check.
*************************************************************************************************/


// Determines whether the other player is in check.
bool Bitboard::InCheckOther() {

    assert(pieces[10 + !toMove] != 0);
    uint64_t ret = 0;
    int index = kingLoc[!toMove];

    ret = pieces[toMove] & pawnAttacks[index][!toMove];
    ret |= pieces[2 + toMove] & knightMoves[index];
    ret |= pieces[10 + toMove] & kingMoves[index];
    ret |= (pieces[4 + toMove] | pieces[8 + toMove]) & magics->bishopAttacksMask(occupied, index);
    ret |= (pieces[6 + toMove] | pieces[8 + toMove]) & magics->rookAttacksMask(occupied, index);

    return ret != 0;

}



// Determines whether the current player is in check.
bool Bitboard::InCheck() {

    assert(pieces[10 + toMove] != 0);
    uint64_t ret = 0;
    int index = kingLoc[toMove];

    ret = pieces[!toMove] & pawnAttacks[index][toMove];
    ret |= pieces[2 + !toMove] & knightMoves[index];
    ret |= pieces[10 + !toMove] & kingMoves[index];
    ret |= (pieces[4 + !toMove] | pieces[8 + !toMove]) & magics->bishopAttacksMask(occupied, index);
    ret |= (pieces[6 + !toMove] | pieces[8 + !toMove]) & magics->rookAttacksMask(occupied, index);

    return ret != 0;

}



// Determines if a move is legal
bool Bitboard::isLegal(MOVE move) {

    // Special cases
    if ((MOVE_FLAGS & move) == ENPASSANT_FLAG) {
        make_move(move);
        bool legal = InCheckOther();
        undo_move(move);
        return !legal;
    }

    // Move to and from square, and if its a capture
    int from = get_move_from(move);
    int to = get_move_to(move);
    int kingSide = 10 + toMove;
    uint64_t to64 = (1ULL << to);
    uint64_t from64 = (1ULL << from);
    uint64_t tofrom = to64 | from64;

    bool attacked = false;
    bool kingMove = pieceAt[from] / 2 == 5;

    // If the moving piece is a king
    pieces[kingSide] ^= kingMove * tofrom;

    if (kingMove) {
        kingLoc[toMove] = to;
    }

    // If it is a capture
    if (move & CAPTURE_FLAG) {
        occupied ^= from64;
        pieces[pieceAt[to]] ^= to64;
        attacked = InCheck();
        pieces[pieceAt[to]] ^= to64;
        occupied ^= from64;
    }
    else {
        occupied ^= tofrom;
        attacked = InCheck();
        occupied ^= tofrom;
    }

    // If the moving piece is a king
    pieces[kingSide] ^= kingMove * tofrom;

    if (kingMove) {
        kingLoc[toMove] = from;
    }

    return !attacked;
}



// Determines if a move is pseudo legal
bool Bitboard::isPseudoLegal(MOVE move) {

    int from = get_move_from(move);
    int to = get_move_to(move);
    int pieceMoved = pieceAt[from];

    // who to move and is there piece
    if (pieceMoved == -1 || pieceMoved % 2 != toMove || move == NULL_MOVE || move == NO_MOVE) {
        return false;
    }

    // Enpassant
    if ((MOVE_FLAGS & move) == ENPASSANT_FLAG && pieceMoved / 2 == 0 && (pawnAttacks[from][toMove] & (1ULL << enpassantSq))) {
        return true;
    }

    if (move & CAPTURE_FLAG) {
        if (((1ULL << to) & color[!toMove]) == 0 || ((1ULL << to) & color[toMove])) {
            return false;
        }

        switch (pieceMoved / 2) {
            case 0:
                return (pawnAttacks[from][toMove] & (1ULL << to)) != 0;
            case 1:
                return (knightMoves[from] & (1ULL << to)) != 0;
            case 2:
                return (magics->bishopAttacksMask(occupied, from) & (1ULL << to)) != 0;
            case 3:
                return (magics->rookAttacksMask(occupied, from) & (1ULL << to)) != 0;
            case 4:
                return (magics->queenAttacksMask(occupied, from) & (1ULL << to)) != 0;
            case 5:
                return (kingMoves[from] & (1ULL << to)) != 0;
        }
    }

    if (pieceMoved / 2 == 0) {
        if (((toMove? (1ULL << (from - 8)) : (1ULL << (from + 8))) & occupied)) {
            return false;
        }

        if ((move & MOVE_FLAGS) == DOUBLE_PAWN_PUSH_FLAG) {
            if ((rowMask[8 + toMove * 40] & (1ULL << from)) && ((toMove? (1ULL << (from - 16)) : (1ULL << (from + 16))) & occupied)) {
                return false;
            }
        }

        return true;
    }



    if (pieceMoved / 2 == 5) {
        if ((MOVE_FLAGS & move) == KING_CASTLE_FLAG || (MOVE_FLAGS & move) == QUEEN_CASTLE_FLAG) {
            if (from != 4 && from != 60) {
                return false;
            }

            if ((MOVE_FLAGS & move) == KING_CASTLE_FLAG && can_castle_king()) {
                return true;
            }

            if ((MOVE_FLAGS & move) == QUEEN_CASTLE_FLAG && can_castle_queen()) {
                return true;
            }
        }
    }


    if ((move & MOVE_FLAGS) == QUIET_MOVES_FLAG) {

        if (((1ULL << to) & occupied) != 0) {
            return false;
        }

        assert (pieceMoved / 2 != 0);
        switch (pieceMoved / 2) {
            case 1:
                return (knightMoves[from] & (1ULL << to)) != 0;
            case 2:
                return (magics->bishopAttacksMask(occupied, from) & (1ULL << to)) != 0;
            case 3:
                return (magics->rookAttacksMask(occupied, from) & (1ULL << to)) != 0;
            case 4:
                return (magics->queenAttacksMask(occupied, from) & (1ULL << to)) != 0;
            case 5:
                return (kingMoves[from] & (1ULL << to)) != 0;
        }
    }


    return false;
}



// Determine if player can castle kingside
bool Bitboard::can_castle_king() {

    if (!(castleRights & (toMove? KING_CASTLE_RIGHTS_BLACK : KING_CASTLE_RIGHTS_WHITE))) {
        return false;
    }

    if (occupied & (toMove? KING_CASTLE_OCCUPIED_BLACK_MASK : KING_CASTLE_OCCUPIED_WHITE_MASK)) {
        return false;
    }

    if (isAttackedCastleMask(toMove? KING_CASTLE_BLACK_MASK : KING_CASTLE_WHITE_MASK)) {
        return false;
    }

    return true;
}



// Determine if player can castle queenside
bool Bitboard::can_castle_queen() {

    if (!(castleRights & (toMove? QUEEN_CASTLE_RIGHTS_BLACK : QUEEN_CASTLE_RIGHTS_WHITE))) {
        return false;
    }

    if (occupied & (toMove? QUEEN_CASTLE_OCCUPIED_BLACK_MASK : QUEEN_CASTLE_OCCUPIED_WHITE_MASK)) {
        return false;
    }

    if (isAttackedCastleMask(toMove? QUEEN_CASTLE_BLACK_MASK : QUEEN_CASTLE_WHITE_MASK)) {
        return false;
    }

    return true;
}



// Determine if playr can castle
bool Bitboard::isAttackedCastleMask(uint64_t bitboard) {

    uint64_t ret = 0;

    ret = kingMoves[bitScan(pieces[10 + !toMove])];
    ret |= knightAttacks(pieces[2 + !toMove]);
    ret |= pawnAttacksAll(pieces[!toMove], !toMove);

    if (ret & bitboard) {
        return true;
    }

    uint64_t piece = pieces[4 + !toMove];
    while (piece) {
        if (magics->bishopAttacksMask(occupied, bitScan(piece)) & bitboard) {
            return true;
        }
        piece &= piece - 1;
    }

    piece = pieces[6 + !toMove];
    while (piece) {
        if (magics->rookAttacksMask(occupied, bitScan(piece)) & bitboard) {
            return true;
        }
        piece &= piece - 1;
    }

    piece = pieces[8 + !toMove];
    while (piece) {
        if (magics->queenAttacksMask(occupied, bitScan(piece)) & bitboard) {
            return true;
        }
        piece &= piece - 1;
    }

    return false;

}


/************************************************************************************************
**  Draws and Repetition section
**  Used for determining draws.
*************************************************************************************************/


// Returns a boolean on whether the position is a draw.
bool Bitboard::isDraw(int ply) {

    // 50 move rule
    if (halfMoves >= 100) {
        return true;
    }

    // Repetition
    bool repetition = false;
    for (int i = moveHistory.count - 2; i >= std::max(0, moveHistory.count - halfMoves) ; i -= 2) {
        if (moveHistory.move[i].posKey == posKey) {
            if (repetition || ply > 1) {
                return true;
            }
            repetition = true;
        }
    }


    // Material draw.
    if (pieceCount[0] + pieceCount[1] + pieceCount[6] + pieceCount[7] + pieceCount[8] + pieceCount[9] > 0) {
        return false;
    }

    // Material draw.
    if (pieceCount[2] + pieceCount[3] + pieceCount[4] + pieceCount[5] <= 1) {
        return true;
    }

    return false;
}



// Returns true if there are no potential win material (only knight, only bishop)
bool Bitboard::noPotentialWin() {

    if (pieceCount[toMove] + pieceCount[6 + toMove] + pieceCount[8 + toMove] > 0) {
        return false;
    }

    if (pieceCount[2 + toMove] + pieceCount[4 + toMove] == 1) {
        return true;
    }

    return false;
}



/************************************************************************************************
**  Zobrist Hashing and Transposition table Section
**  Used for move ordering and massive search improvements.
*************************************************************************************************/


// Return the current position key
uint64_t Bitboard::getPosKey() const {
    return posKey;
}




// A debugging function for zobrist hashing.
// A position key is created from scratch, and is compared to the iterative position key for confirmation.
void Bitboard::debugZobristHash() {
    if (posKey != zobrist->hashBoard(pieces, castleRights, enpassantSq, toMove)) {
        std::cout << posKey << " " << zobrist->hashBoard(pieces, castleRights, enpassantSq, toMove) << std::endl;
    }
    assert (posKey == zobrist->hashBoard(pieces, castleRights, enpassantSq, toMove));
}





/************************************************************************************************
**  Static Exchange Evaluation Section.
**  Used for additional pruning/reductions. Potentially can be used for move ordering.
*************************************************************************************************/

// Takes in the index square and returns the bitboard of pieces that can attack this square
uint64_t Bitboard::isAttackedSee(int index) {

    uint64_t ret = 0;
    ret |= pieces[0] & pawnAttacks[index][1];
    ret |= pieces[1] & pawnAttacks[index][0];
    ret |= (pieces[2] | pieces[3]) & knightMoves[index];
    ret |= (pieces[4] | pieces[5] | pieces[8] | pieces[9]) & magics->bishopAttacksMask(occupied, index);
    ret |= (pieces[6] | pieces[7] | pieces[8] | pieces[9]) & magics->rookAttacksMask(occupied, index);
    ret |= (pieces[10] | pieces[11]) & kingMoves[index];

    return ret;

}



// Return the bitboard location of the least valuable attacker.
// Returns 0 (empty bitboard) if no attackers are found.
uint64_t Bitboard::getLeastValuablePiece(uint64_t attadef, bool col, int &piece) {

    for (piece = col; piece < 12; piece += 2) {
        uint64_t subset = attadef & pieces[piece];
        if (subset) {
            return subset & -subset;
        }
    }

    return 0;
}



// Static Exchange Evaluation.
// Determines whether or not a move is a winning or losing capture.
// SEE < 0 is losing, SEE > 0 is winning
int Bitboard::seeCapture(MOVE capture) {

    int gain[32];
    int d = 0;
    uint64_t mayXray = pieces[0] | pieces[1] | pieces[4] | pieces[5] | pieces[6] | pieces[7] | pieces[8] | pieces[9];

    int from = get_move_from(capture);
    int to = get_move_to(capture);
    uint64_t fromSet = 1ULL << from;

    uint64_t occ = occupied;
    uint64_t attadef = isAttackedSee(to);
    int aPiece = pieceAt[from];
    bool isWhite = toMove;

    if (to >= 56 || to < 8) {
        return 0;
    }

    if ((capture & MOVE_FLAGS) == ENPASSANT_FLAG) {
        gain[d] = pvals[0];
    }
    else {
        gain[d] = pieceAt[to] == -1? 0 : pvals[pieceAt[to] / 2];
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
            attadef |= magics->xrayAttackBishop(occ, fromSet, to) & (pieces[4] | pieces[5] | pieces[8] | pieces[9]);
            attadef |= magics->xrayAttackRook(occ, fromSet, to) & (pieces[6] | pieces[7] | pieces[8] | pieces[9]);
        }

        fromSet = getLeastValuablePiece(attadef, isWhite, aPiece);

    } while (fromSet);

    while (--d) {
        gain[d - 1] = -std::max(-gain[d - 1], gain[d]);
    }

    return gain[0];
}



/************************************************************************************************
**  Helper/Debugging functions
*************************************************************************************************/



// Return the piece from index
int Bitboard::getPiece(int index) {
    if (pieceAt[index] == -1) {
        return -1;
    }
    return pieceAt[index] / 2;
}



// Return the rank from index depending on the side to move
int Bitboard::getRankFromSideToMove(int index) {
    return toMove? 7 - (index / 8) : index / 8;
}


// Return the side to move
bool Bitboard::getSideToMove() {
    return toMove;
}


// Insert counter move into array
void Bitboard::insertCounterMove(ThreadSearch *th, MOVE move) {
    MOVE prevMove = moveHistory.move[moveHistory.count - 1].move;

    if (prevMove != NULL_MOVE) {
        th->counterMove[toMove][get_move_from(prevMove)][get_move_to(prevMove)] = move;
    }
}



// Returns true if null move pruning is allowed in the position
bool Bitboard::nullMoveable() {
    return color[toMove] != (pieces[toMove] | pieces[10 + toMove]);
}



// Prints the state of the board
void Bitboard::printPretty() {

    std::bitset<64> oc(occupied);
    std::bitset<64> wh(color[0]);
    std::bitset<64> bl(color[1]);
    std::bitset<64> pa(pieces[0] | pieces[1]);
    std::bitset<64> kn(pieces[2] | pieces[3]);
    std::bitset<64> bi(pieces[4] | pieces[5]);
    std::bitset<64> ro(pieces[6] | pieces[7]);
    std::bitset<64> qu(pieces[8] | pieces[9]);
    std::bitset<64> ki(pieces[10] | pieces[11]);
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



// Converts the current position to FEN
std::string Bitboard::posToFEN() {

    std::string fen = "";
    std::string cStr = "";
    const std::string NUM_TO_STR[9] = {"0", "1", "2", "3", "4", "5", "6", "7", "8"};

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
                if (calc & pieces[0]) {
                    rank += "P";
                }
                else if (calc & pieces[2]) {
                    rank += "N";
                }
                else if (calc & pieces[4]) {
                    rank += "B";
                }
                else if (calc & pieces[6]) {
                    rank += "R";
                }
                else if (calc & pieces[8]) {
                    rank += "Q";
                }
                else if (calc & pieces[10]) {
                    rank += "K";
                }
                else if (calc & pieces[1]) {
                    rank += "p";
                }
                else if (calc & pieces[3]) {
                    rank += "n";
                }
                else if (calc & pieces[5]) {
                    rank += "b";
                }
                else if (calc & pieces[7]) {
                    rank += "r";
                }
                else if (calc & pieces[9]) {
                    rank += "q";
                }
                else if (calc & pieces[11]) {
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

    fen += toMove? " b" : " w";
    std::string crights = "";
    if (castleRights & 8) {
        crights += "K";
    }
    if (castleRights & 4) {
        crights += "Q";
    }
    if (castleRights & 2) {
        crights += "k";
    }
    if (castleRights & 1) {
        crights += "q";
    }

    fen += castleRights? " " + crights : " -";
    fen += enpassantSq? " " + TO_ALG[enpassantSq] : " -";
    fen += " " + std::to_string(halfMoves);
    fen += " "  + std::to_string(fullMoves);

    return fen;
}





// Take a FEN position as a string and set the posiiton.
void Bitboard::setPosFen(std::string fen) {

    // Clear all bitboards
    color[0] = 0;
    color[1] = 0;
    occupied = 0;

    posKey = 0;

    halfMoves = 0;
    fullMoves = 1;
    enpassantSq = 0;
    toMove = false;
    acc.refresh_accumulator();

    for (int i = 0; i < 12; i++) {
        pieces[i] = 0;
        pieceCount[i] = 0;
    }

    for (int i = 0; i < 64; i++) {
        pieceAt[i] = -1;
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
            pieces[piece] |= 1ULL << lineOffset;
            pieceCount[piece]++;
            pieceAt[lineOffset] = piece;
            zobrist->hashBoard_square(posKey, lineOffset, piece);
            acc.accumulate_add(piece, lineOffset);

            if (piece / 2 == 5) {
                kingLoc[piece - 10] = lineOffset;
            }

            lineOffset++;
        }
    }


    color[0] = pieces[0] | pieces[2] | pieces[4] | pieces[6] | pieces[8] | pieces[10];
    color[1] = pieces[1] | pieces[3] | pieces[5] | pieces[7] | pieces[9] | pieces[11];
    occupied = color[0] | color[1];


    posIndex++;
    if (fen[posIndex] == 'b') {
        toMove = true;
        zobrist->hashBoard_turn(posKey);
    }
    posIndex++;


    posIndex++;
    castleRights = 0;
    if (fen[posIndex] != '-') {

        if (fen[posIndex] == 'K') {
            castleRights ^= 8;
            posIndex++;
        }

        if (fen[posIndex] == 'Q') {
            castleRights ^= 4;
            posIndex++;
        }

        if (fen[posIndex] == 'k') {
            castleRights ^= 2;
            posIndex++;
        }

        if (fen[posIndex] == 'q') {
            castleRights ^= 1;
            posIndex++;
        }

        posIndex--;

    }

    zobrist->hashBoard_castle(posKey, castleRights ^ 15);

    posIndex += 2;
    if (fen[posIndex] != '-') {
        enpassantSq = TO_NUM[fen.substr(posIndex, 2)];
        posIndex++;
        if (enpassantSq) {
            zobrist->hashBoard_enpassant(posKey, enpassantSq);
        }
    }
    posIndex += 2;

    halfMoves = std::isdigit(fen[posIndex + 1])? 10 * (fen[posIndex] - 48) + (fen[posIndex + 1] - 48) : fen[posIndex] - 48;
    posIndex += std::isdigit(fen[posIndex + 1])? 3 : 2;

    // fullMoves = std::isdigit(fen[posIndex + 1])? 10 * (fen[posIndex] - 48) + (fen[posIndex + 1] - 48) : fen[posIndex] - 48;

}






































//
