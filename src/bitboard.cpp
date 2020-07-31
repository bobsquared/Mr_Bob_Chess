#include "bitboard.h"

std::regex fenNumbers(".*(\\d+)\\s+(\\d+)");
extern Magics *magics;
extern int pieceValues[6];




Bitboard::Bitboard() {

    InitWhitePawnMoves();
    InitBlackPawnMoves();
    InitBlackPawnAttacks();
    InitWhitePawnAttacks();
    InitKnightMoves();
    InitBishopMoves();
    InitRookMoves();
    InitQueenMoves();
    InitKingMoves();

    moveGen = new MoveGen();
    magics = new Magics(rookMoves, bishopMoves);
    eval = new Eval();
    movePick = new MovePick();
    zobrist = new Zobrist();
    tt = new TranspositionTable();

    reset();
    InitRookCastleFlags(pieces[6], pieces[7]);
}



Bitboard::~Bitboard() {
    delete moveGen;
    delete magics;
    delete eval;
    delete movePick;
    delete zobrist;
    delete tt;
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
    InitMaterial();
    InitPieceAt();
    InitPieceCount();
    moveHistory.clear();
    posKey = zobrist->hashBoard(pieces, castleRights, enpassantSq, toMove);
}



// Initialize all black pawn moves
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

    blackPawns = (1ULL << 48) | (1ULL << 49) | (1ULL << 50) | (1ULL << 51) | (1ULL << 52) | (1ULL << 53) | (1ULL << 54) | (1ULL << 55);

}



// Initialize all white pawn moves
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

    whitePawns = (1ULL << 8) | (1ULL << 9) | (1ULL << 10) | (1ULL << 11) | (1ULL << 12) | (1ULL << 13) | (1ULL << 14) | (1ULL << 15);

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

        bishopMoves[i] = tempBitBoard1 | tempBitBoard2 | tempBitBoard4 | tempBitBoard3;
    }

    whiteBishops = (1ULL << 2) | (1ULL << 5);
    blackBishops = (1ULL << 61) | (1ULL << 58);

}



// Initialize all rook moves
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
            tempBitBoard2 |= (tempBitBoard << (8 + k * 8));
        }

        for (int k = col; k < 8; k++) {
            if (k == 7) {
                break;
            }
            else {
                tempBitBoard3 |= (tempBitBoard << (1 + 1 * (k - col)));
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

        rookMoves[i] = (tempBitBoard1 | tempBitBoard2 | tempBitBoard4 | tempBitBoard3) & (~tempBitBoard);
    }

    whiteRooks = 1 | (1ULL << 7);
    blackRooks = (1ULL << 56) | (1ULL << 63);
}



// Initialize all queen moves
void Bitboard::InitQueenMoves() {

    for (int i = 0; i < 64; i++) {
        queenMoves[i] = rookMoves[i] | bishopMoves[i];
    }

    whiteQueens = 1ULL << 3;
    blackQueens = 1ULL << 59;

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



// Initialize material scores
void Bitboard::InitMaterial() {
    material[0] = count_population(pieces[0]) * pieceValues[0];
    material[0] += count_population(pieces[2]) * pieceValues[1];
    material[0] += count_population(pieces[4]) * pieceValues[2];
    material[0] += count_population(pieces[6]) * pieceValues[3];
    material[0] += count_population(pieces[8]) * pieceValues[4];
    material[0] += count_population(pieces[10]) * pieceValues[5];

    material[1] = count_population(pieces[1]) * pieceValues[0];
    material[1] += count_population(pieces[3]) * pieceValues[1];
    material[1] += count_population(pieces[5]) * pieceValues[2];
    material[1] += count_population(pieces[7]) * pieceValues[3];
    material[1] += count_population(pieces[9]) * pieceValues[4];
    material[1] += count_population(pieces[11]) * pieceValues[5];
}



/************************************************************************************************
**  Move Generation section
**  Generates all pseudo or legal moves from current position.
*************************************************************************************************/


// Generate all pseudo-legal moves
void Bitboard::generate(MoveList &moveList, int depth, MOVE pvMove) {
    moveGen->generate_all_moves(moveList, pieces, color, pawnAttacks, knightMoves, magics, kingMoves, occupied, enpassantSq, castleRights, toMove);
    movePick->scoreMoves(moveList, moveHistory, pieceAt, depth, toMove, pvMove);
}



// Generate all pseudo-legal captures
void Bitboard::generate_captures_promotions(MoveList &moveList, MOVE pvMove) {
    moveGen->generate_captures_promotions(moveList, pieces, color, pawnAttacks, knightMoves, magics, kingMoves, occupied, enpassantSq, castleRights, toMove);
    movePick->scoreMoves(moveList, moveHistory, pieceAt, 0, toMove, pvMove);
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
}



// Make a move or null move.
// This involves any legal moves in the game of chess.
void Bitboard::make_move(MOVE move) {

    if (move == NULL_MOVE) {
        toMove = !toMove;
        moveHistory.insert(MoveInfo(0, enpassantSq, halfMoves, castleRights, posKey, move));

        if (enpassantSq) {
            zobrist->hashBoard_enpassant(posKey, enpassantSq);
            enpassantSq = 0;
        }
        zobrist->hashBoard_turn(posKey);
        return;
    }

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

    halfMoves++;

    if (enpassantSq) {
        zobrist->hashBoard_enpassant(posKey, enpassantSq);
        enpassantSq = 0;
    }

    // Update half moves
    if (fromPiece == toMove) {
        halfMoves = 0;
    }

    // Update full moves
    if (toMove) {
        fullMoves++;
    }

    // Update castling rights
    if (fromPiece == 6 + toMove) {
        zobrist->hashBoard_castle(posKey, castleRights & ~rookCastleFlagMask[from]);
        castleRights &= rookCastleFlagMask[from];
    }
    else if (fromPiece == 10 + toMove) {
        if (toMove) {
            zobrist->hashBoard_castle(posKey, castleRights & CASTLE_FLAG_BLACK);
            castleRights &= ~CASTLE_FLAG_BLACK;
        }
        else {
            zobrist->hashBoard_castle(posKey, castleRights & CASTLE_FLAG_WHITE);
            castleRights &= ~CASTLE_FLAG_WHITE;
        }
    }

    assert(fromPiece != -1);
    assert(from != 0 || to != 0);

    // Make quiet moves
    if ((move & MOVE_FLAGS) == QUIET_MOVES_FLAG) {
        assert(toPiece == -1);
        move_quiet(from, to, fromPiece, i1i2);
        zobrist->hashBoard_quiet(posKey, from, to, fromPiece);
    }
    // Make enpassant move
    else if ((move & MOVE_FLAGS) == ENPASSANT_FLAG) {
        assert(toPiece == -1);
        assert(fromPiece == toMove);
        move_quiet(from, to, fromPiece, i1i2);

        uint64_t toCap = toMove? (to + 8) : (to - 8);
        zobrist->hashBoard_quiet(posKey, from, to, fromPiece);
        zobrist->hashBoard_square(posKey, toCap, !toMove);
        color[!toMove] ^= 1ULL << toCap;
        pieces[!toMove] ^= 1ULL << toCap;
        occupied ^= 1ULL << toCap;
        pieceAt[toCap] = -1;
        material[!toMove] -= pieceValues[0];
        pieceCount[!toMove]--;
    }
    // Make a capture
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
        material[!toMove] -= pieceValues[toPiece / 2];
        pieceCount[toPiece]--;

        switch (move & MOVE_FLAGS) {
            case CAPTURES_NORMAL_FLAG:
                pieceAt[to] = fromPiece;
                pieces[fromPiece] ^= i1i2;
                zobrist->hashBoard_capture(posKey, from, to, fromPiece, toPiece);
                break;
            case QUEEN_PROMOTION_CAPTURE_FLAG:
                assert(fromPiece == toMove);
                pieces[fromPiece] ^= i1;
                pieces[8 + toMove] ^= i2;
                pieceAt[to] = 8 + toMove;
                material[toMove] += pieceValues[4] - pieceValues[0];
                pieceCount[toMove]--;
                pieceCount[8 + toMove]++;
                zobrist->hashBoard_capture_promotion(posKey, from, to, fromPiece, toPiece, 8 + toMove);
                break;
            case ROOK_PROMOTION_CAPTURE_FLAG:
                assert(fromPiece == toMove);
                pieces[fromPiece] ^= i1;
                pieces[6 + toMove] ^= i2;
                pieceAt[to] = 6 + toMove;
                material[toMove] += pieceValues[3] - pieceValues[0];
                pieceCount[toMove]--;
                pieceCount[6 + toMove]++;
                zobrist->hashBoard_capture_promotion(posKey, from, to, fromPiece, toPiece, 6 + toMove);
                break;
            case BISHOP_PROMOTION_CAPTURE_FLAG:
                assert(fromPiece == toMove);
                pieces[fromPiece] ^= i1;
                pieces[4 + toMove] ^= i2;
                pieceAt[to] = 4 + toMove;
                material[toMove] += pieceValues[2] - pieceValues[0];
                pieceCount[toMove]--;
                pieceCount[4 + toMove]++;
                zobrist->hashBoard_capture_promotion(posKey, from, to, fromPiece, toPiece, 4 + toMove);
                break;
            case KNIGHT_PROMOTION_CAPTURE_FLAG:
                assert(fromPiece == toMove);
                pieces[fromPiece] ^= i1;
                pieces[2 + toMove] ^= i2;
                pieceAt[to] = 2 + toMove;
                material[toMove] += pieceValues[1] - pieceValues[0];
                pieceCount[toMove]--;
                pieceCount[2 + toMove]++;
                zobrist->hashBoard_capture_promotion(posKey, from, to, fromPiece, toPiece, 2 + toMove);
                break;
        }

        halfMoves = 0;
    }
    // Make a double pawn push
    else if ((move & MOVE_FLAGS) == DOUBLE_PAWN_PUSH_FLAG) {
        assert(toPiece == -1);
        move_quiet(from, to, fromPiece, i1i2);
        enpassantSq = toMove? to + 8 : to - 8;
        zobrist->hashBoard_quiet(posKey, from, to, fromPiece);
        zobrist->hashBoard_enpassant(posKey, enpassantSq);
    }
    // Make a promotion
    else if (move & PROMOTION_FLAG) {
        assert(toPiece == -1);
        assert(fromPiece == toMove);
        color[toMove] ^= i1i2;
        pieceAt[from] = -1;
        pieces[fromPiece] ^= i1;
        occupied ^= i1i2;

        switch (move & MOVE_FLAGS) {
            case QUEEN_PROMOTION_FLAG:
                pieces[8 + toMove] ^= i2;
                pieceAt[to] = 8 + toMove;
                material[toMove] += pieceValues[4] - pieceValues[0];
                pieceCount[toMove]--;
                pieceCount[8 + toMove]++;
                zobrist->hashBoard_promotion(posKey, from, to, fromPiece, 8 + toMove);
                break;
            case ROOK_PROMOTION_FLAG:
                pieces[6 + toMove] ^= i2;
                pieceAt[to] = 6 + toMove;
                material[toMove] += pieceValues[3] - pieceValues[0];
                pieceCount[toMove]--;
                pieceCount[6 + toMove]++;
                zobrist->hashBoard_promotion(posKey, from, to, fromPiece, 6 + toMove);
                break;
            case BISHOP_PROMOTION_FLAG:
                pieces[4 + toMove] ^= i2;
                pieceAt[to] = 4 + toMove;
                material[toMove] += pieceValues[2] - pieceValues[0];
                pieceCount[toMove]--;
                pieceCount[4 + toMove]++;
                zobrist->hashBoard_promotion(posKey, from, to, fromPiece, 4 + toMove);
                break;
            case KNIGHT_PROMOTION_FLAG:
                pieces[2 + toMove] ^= i2;
                pieceAt[to] = 2 + toMove;
                material[toMove] += pieceValues[1] - pieceValues[0];
                pieceCount[toMove]--;
                pieceCount[2 + toMove]++;
                zobrist->hashBoard_promotion(posKey, from, to, fromPiece, 2 + toMove);
                break;
        }
    }
    // Castle kingside
    else if ((move & MOVE_FLAGS) == KING_CASTLE_FLAG) {
        assert(toPiece == -1);
        assert(fromPiece == 10 + toMove);
        move_quiet(from, to, fromPiece, i1i2);
        move_quiet(to + 1, to - 1, 6 + toMove, 1ULL << (to - 1) | 1ULL << (to + 1));
        zobrist->hashBoard_quiet(posKey, from, to, fromPiece);
        zobrist->hashBoard_quiet(posKey, to + 1, to - 1, 6 + toMove);
    }
    // Castle queenside
    else if ((move & MOVE_FLAGS) == QUEEN_CASTLE_FLAG) {
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

    if (move == NULL_MOVE) {
        toMove = !toMove;
        MoveInfo moveInfo = moveHistory.pop();
        halfMoves = moveInfo.halfMoves;
        castleRights = moveInfo.castleRights;
        enpassantSq = moveInfo.enpassantSq;
        posKey = moveInfo.posKey;
        return;
    }

    toMove = !toMove;
    zobrist->hashBoard_turn(posKey);
    int from = get_move_from(move);
    int to = get_move_to(move);
    int toPiece = pieceAt[to];

    uint64_t i1 = 1ULL << from;
    uint64_t i2 = 1ULL << to;
    uint64_t i1i2 = i1 | i2;

    MoveInfo moveInfo = moveHistory.pop();
    halfMoves = moveInfo.halfMoves;
    castleRights = moveInfo.castleRights;
    enpassantSq = moveInfo.enpassantSq;
    posKey = moveInfo.posKey;

    if (toMove) {
        fullMoves--;
    }

    if ((move & MOVE_FLAGS) == QUIET_MOVES_FLAG) {
        move_quiet(to, from, toPiece, i1i2);
    }
    else if ((move & MOVE_FLAGS) == ENPASSANT_FLAG) {
        assert(toPiece == toMove);
        move_quiet(to, from, toPiece, i1i2);

        uint64_t toCap = toMove? (to + 8) : (to - 8);
        color[!toMove] ^= 1ULL << toCap;
        pieces[!toMove] ^= 1ULL << toCap;
        occupied ^= 1ULL << toCap;
        pieceAt[toCap] = !toMove;
        material[!toMove] += pieceValues[0];
        pieceCount[!toMove]++;
    }
    else if (move & CAPTURE_FLAG) {

        color[toMove] ^= i1i2;
        color[!toMove] ^= i2;
        pieces[moveInfo.captureType] ^= i2;
        pieceAt[to] = moveInfo.captureType;
        occupied ^= i1;
        material[!toMove] += pieceValues[moveInfo.captureType / 2];
        pieceCount[moveInfo.captureType]++;

        switch (move & MOVE_FLAGS) {
            case CAPTURES_NORMAL_FLAG:
                pieces[toPiece] ^= i1i2;
                pieceAt[from] = toPiece;
                break;
            case QUEEN_PROMOTION_CAPTURE_FLAG:
                pieceAt[from] = 0 + toMove;
                pieces[0 + toMove] ^= i1;
                pieces[8 + toMove] ^= i2;
                material[toMove] -= pieceValues[4] - pieceValues[0];
                pieceCount[toMove]++;
                pieceCount[8 + toMove]--;
                break;
            case ROOK_PROMOTION_CAPTURE_FLAG:
                pieceAt[from] = 0 + toMove;
                pieces[0 + toMove] ^= i1;
                pieces[6 + toMove] ^= i2;
                material[toMove] -= pieceValues[3] - pieceValues[0];
                pieceCount[toMove]++;
                pieceCount[6 + toMove]--;
                break;
            case BISHOP_PROMOTION_CAPTURE_FLAG:
                pieceAt[from] = 0 + toMove;
                pieces[0 + toMove] ^= i1;
                pieces[4 + toMove] ^= i2;
                material[toMove] -= pieceValues[2] - pieceValues[0];
                pieceCount[toMove]++;
                pieceCount[4 + toMove]--;
                break;
            case KNIGHT_PROMOTION_CAPTURE_FLAG:
                pieceAt[from] = 0 + toMove;
                pieces[0 + toMove] ^= i1;
                pieces[2 + toMove] ^= i2;
                material[toMove] -= pieceValues[1] - pieceValues[0];
                pieceCount[toMove]++;
                pieceCount[2 + toMove]--;
                break;
        }

    }
    else if ((move & MOVE_FLAGS) == DOUBLE_PAWN_PUSH_FLAG) {
        assert(toPiece == toMove);
        move_quiet(to, from, toPiece, i1i2);
    }
    else if (move & PROMOTION_FLAG) {
        color[toMove] ^= i1i2;
        pieceAt[from] = 0 + toMove;
        pieceAt[to] = -1;
        pieces[0 + toMove] ^= i1;
        occupied ^= i1i2;

        switch (move & MOVE_FLAGS) {
            case QUEEN_PROMOTION_FLAG:
                pieces[8 + toMove] ^= i2;
                material[toMove] -= pieceValues[4] - pieceValues[0];
                pieceCount[toMove]++;
                pieceCount[8 + toMove]--;
                break;
            case ROOK_PROMOTION_FLAG:
                pieces[6 + toMove] ^= i2;
                material[toMove] -= pieceValues[3] - pieceValues[0];
                pieceCount[toMove]++;
                pieceCount[6 + toMove]--;
                break;
            case BISHOP_PROMOTION_FLAG:
                pieces[4 + toMove] ^= i2;
                material[toMove] -= pieceValues[2] - pieceValues[0];
                pieceCount[toMove]++;
                pieceCount[4 + toMove]--;
                break;
            case KNIGHT_PROMOTION_FLAG:
                pieces[2 + toMove] ^= i2;
                material[toMove] -= pieceValues[1] - pieceValues[0];
                pieceCount[toMove]++;
                pieceCount[2 + toMove]--;
                break;
        }
    }
    else if ((move & MOVE_FLAGS) == KING_CASTLE_FLAG) {
        assert(toPiece == 10 + toMove);
        move_quiet(to, from, toPiece, i1i2);
        move_quiet(to - 1, to + 1, 6 + toMove, 1ULL << (to - 1) | 1ULL << (to + 1));
    }
    else if ((move & MOVE_FLAGS) == QUEEN_CASTLE_FLAG) {
        assert(toPiece == 10 + toMove);
        move_quiet(to, from, toPiece, i1i2);
        move_quiet(to + 1, to - 2, 6 + toMove, 1ULL << (to - 2) | 1ULL << (to + 1));
    }
}


/************************************************************************************************
**  Checks section
**  Used for determining whether a player is in check.
*************************************************************************************************/


// Determines whether the other player is in check.
bool Bitboard::InCheckOther() {

    assert(pieces[10 + !toMove] != 0);
    uint64_t ret = 0;
    int index = bitScan(pieces[10 + !toMove]);

    ret = pieces[toMove] & pawnAttacksAll(pieces[10 + !toMove], !toMove);
    ret |= pieces[2 + toMove] & knightMoves[index];
    ret |= pieces[4 + toMove] & magics->bishopAttacksMask(occupied, index);
    ret |= pieces[6 + toMove] & magics->rookAttacksMask(occupied, index);
    ret |= pieces[8 + toMove] & magics->queenAttacksMask(occupied, index);
    ret |= pieces[10 + toMove] & kingMoves[index];
    ret &= color[toMove];

    return ret != 0;

}



// Determines whether the current player is in check.
bool Bitboard::InCheck() {

    assert(pieces[10 + toMove] != 0);
    uint64_t ret = 0;
    int index = bitScan(pieces[10 + toMove]);

    ret = pieces[!toMove] & pawnAttacksAll(pieces[10 + toMove], toMove);
    ret |= pieces[2 + !toMove] & knightMoves[index];
    ret |= pieces[4 + !toMove] & magics->bishopAttacksMask(occupied, index);
    ret |= pieces[6 + !toMove] & magics->rookAttacksMask(occupied, index);
    ret |= pieces[8 + !toMove] & magics->queenAttacksMask(occupied, index);
    ret |= pieces[10 + !toMove] & kingMoves[index];
    ret &= color[!toMove];

    return ret != 0;

}



/************************************************************************************************
**  Evaluation section
**  Used for determining the value of the position and who is winning.
*************************************************************************************************/

// Returns the heuristic evaluation for the current position.
int Bitboard::evaluate() {
    return toMove? -eval->evaluate(material, pieces, magics, knightMoves, pieceCount, occupied, toMove) : eval->evaluate(material, pieces, magics, knightMoves, pieceCount, occupied, toMove);
}



// Returns the heuristic evaluation for the current position.
// Prints the evaluation with different sections for debugging
int Bitboard::evaluate_debug() {
    return eval->evaluate_debug(material, pieces, magics, knightMoves, pieceCount, occupied);
}



/************************************************************************************************
**  Draws and Repetition section
**  Used for determining draws.
*************************************************************************************************/


// Returns a boolean on whether the position is a draw.
bool Bitboard::isDraw() {

    // 50 move rule
    if (moveHistory.count > 0 && moveHistory.move[moveHistory.count - 1].halfMoves >= 100) {
        return true;
    }

    // Repetition
    if (moveHistory.count >= 3 && std::count(moveHistory.move, moveHistory.move + moveHistory.count, posKey) >= 2) {
        return true;
    }

    // Material draw.
    if (pieceCount[0] + pieceCount[1] + pieceCount[6] + pieceCount[7] + pieceCount[8] + pieceCount[9] > 0) {
        return false;
    }

    // Material draw.
    if (pieceCount[2] + pieceCount[3] + pieceCount[4] + pieceCount[5] == 1) {
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



// Determines if there is one repetition in the position
bool Bitboard::isRepetition() {
    if (std::count(moveHistory.move, moveHistory.move + moveHistory.count, posKey) >= 1) {
        return true;
    }
    return false;
}



/************************************************************************************************
**  Zobrist Hashing and Transposition table Section
**  Used for move ordering and massive search improvements.
*************************************************************************************************/


// Return the current position key
uint64_t Bitboard::getPosKey() {
    return posKey;
}



// Probe the transposition table for duplicate positions
bool Bitboard::probeTT(uint64_t posKey, ZobristVal &hashedBoard, int depth, bool &ttRet, int &alpha, int &beta) {
    return tt->probeTT(posKey, hashedBoard, depth, ttRet, alpha, beta);
}



// Save the current searched position into the transposition table
void Bitboard::saveTT(MOVE move, int score, int depth, uint8_t flag, uint64_t key) {
    assert (move != 0);
    tt->saveTT(move, score, depth, flag, key);
}



// Save the current searched position into the transposition table
int Bitboard::getHashFull() {
    return tt->getHashFull();
}



// Save the current searched position into the transposition table
void Bitboard::clearHashStats() {
    tt->clearHashStats();
}



// Save the current searched position into the transposition table
void Bitboard::clearHashTable() {
    tt->clearHashTable();
}



// Return the principal variation as a string.
// It returns the string as a list of moves, (ex. 'e2e4 e7e5 d2d4 e5d4')
std::string Bitboard::getPv() {

    std::string pv = "";
    std::vector<uint64_t> loopChecker;
    std::stack<MOVE> movesToUndo;

    while (true) {
        uint64_t posKey = getPosKey();
        loopChecker.push_back(posKey);

        if (std::count(loopChecker.begin(), loopChecker.end(), loopChecker.back()) >= 3) {
            break;
        }

        ZobristVal hashedBoard = tt->getHashValue(posKey);
        if (hashedBoard.posKey == posKey) {
            movesToUndo.push(hashedBoard.move);
            pv += " " + TO_ALG[get_move_from(hashedBoard.move)] + TO_ALG[get_move_to(hashedBoard.move)];

            switch (hashedBoard.move & MOVE_FLAGS) {
                case QUEEN_PROMOTION_FLAG:
                    pv += "q";
                    break;
                case QUEEN_PROMOTION_CAPTURE_FLAG:
                    pv += "q";
                    break;
                case ROOK_PROMOTION_FLAG:
                    pv += "r";
                    break;
                case ROOK_PROMOTION_CAPTURE_FLAG:
                    pv += "r";
                    break;
                case BISHOP_PROMOTION_FLAG:
                    pv += "b";
                    break;
                case BISHOP_PROMOTION_CAPTURE_FLAG:
                    pv += "b";
                    break;
                case KNIGHT_PROMOTION_FLAG:
                    pv += "n";
                    break;
                case KNIGHT_PROMOTION_CAPTURE_FLAG:
                    pv += "n";
                    break;
            }

            make_move(hashedBoard.move);
        }
        else {
            break;
        }
    }

    while (!movesToUndo.empty()) {
        undo_move(movesToUndo.top());
        movesToUndo.pop();
    }

    return pv;
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
    ret |= (pieces[4] | pieces[5]) & magics->bishopAttacksMask(occupied, index);
    ret |= (pieces[6] | pieces[7]) & magics->rookAttacksMask(occupied, index);
    ret |= (pieces[8] | pieces[9]) & magics->queenAttacksMask(occupied, index);
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
    const int pvals[6] = {100, 350, 350, 500, 900, 5000};

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


// Return the side to move
bool Bitboard::getSideToMove() {
    return toMove;
}



// Insert killer moves into array
void Bitboard::insertKiller(int depth, MOVE move) {
    if (killers[toMove][depth][0] == move) {
        return;
    }
    killers[toMove][depth][1] = killers[toMove][depth][0];
    killers[toMove][depth][0] = move;
}



// remove killer moves
void Bitboard::removeKiller(int depth) {
    killers[!toMove][depth][1] = NO_MOVE;
    killers[!toMove][depth][0] = NO_MOVE;
    killers[toMove][depth][1] = NO_MOVE;
    killers[toMove][depth][0] = NO_MOVE;
}



// Insert counter move into array
void Bitboard::insertCounterMove(MOVE move) {
    MOVE prevMove = moveHistory.move[moveHistory.count - 1].move;
    counterMove[toMove][get_move_from(prevMove)][get_move_to(prevMove)] = move;
}



// Checks to see if a move (opposite side) is a killer move
bool Bitboard::isKiller(int depth, MOVE move) {
    return killers[!toMove][depth][0] == move || killers[!toMove][depth][1] == move? true : false;
}



// Returns true if null move pruning is allowed in the position
bool Bitboard::nullMoveable() {
    if (pieceCount[2] + pieceCount[3] + pieceCount[4] + pieceCount[5] + pieceCount[6] + pieceCount[7] + pieceCount[8] + pieceCount[9] > 0) {
        return true;
    }
    return false;
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

    for (int i = 0; i < 12; i++) {
        pieces[i] = 0;
    }

    int lineOffset = 0;
    size_t posIndex = fen.find(" ");

    for (size_t i = 0; i < fen.length(); i++) {

        if (posIndex == i) {
            break;
        }
        else if (fen.find("/", i) == i) {
            lineOffset++;
        }
        else if (fen.find("2", i) == i) {
            lineOffset -= 1;
        }
        else if (fen.find("3", i) == i) {
            lineOffset -= 2;
        }
        else if (fen.find("4", i) == i) {
            lineOffset -= 3;
        }
        else if (fen.find("5", i) == i) {
            lineOffset -= 4;
        }
        else if (fen.find("6", i) == i) {
            lineOffset -= 5;
        }
        else if (fen.find("7", i) == i) {
            lineOffset -= 6;
        }
        else if (fen.find("8", i) == i) {
            lineOffset -= 7;
        }
        else if (fen.find("P", i) == i) {
            pieces[0] |= 1ULL << (((i - lineOffset) % 8) + 8 * (7 - (i - lineOffset) / 8));
        }
        else if (fen.find("p", i) == i) {
            pieces[1] |= 1ULL << (((i - lineOffset) % 8) + 8 * (7 - (i - lineOffset) / 8));
        }
        else if (fen.find("N", i) == i) {
            pieces[2] |= 1ULL << (((i - lineOffset) % 8) + 8 * (7 - (i - lineOffset) / 8));
        }
        else if (fen.find("n", i) == i) {
            pieces[3] |= 1ULL << (((i - lineOffset) % 8) + 8 * (7 - (i - lineOffset) / 8));
        }
        else if (fen.find("B", i) == i) {
            pieces[4] |= 1ULL << (((i - lineOffset) % 8) + 8 * (7 - (i - lineOffset) / 8));
        }
        else if (fen.find("b", i) == i) {
            pieces[5] |= 1ULL << (((i - lineOffset) % 8) + 8 * (7 - (i - lineOffset) / 8));
        }
        else if (fen.find("R", i) == i) {
            pieces[6] |= 1ULL << (((i - lineOffset) % 8) + 8 * (7 - (i - lineOffset) / 8));
        }
        else if (fen.find("r", i) == i) {
            pieces[7] |= 1ULL << (((i - lineOffset) % 8) + 8 * (7 - (i - lineOffset) / 8));
        }
        else if (fen.find("Q", i) == i) {
            pieces[8] |= 1ULL << (((i - lineOffset) % 8) + 8 * (7 - (i - lineOffset) / 8));
        }
        else if (fen.find("q", i) == i) {
            pieces[9] |= 1ULL << (((i - lineOffset) % 8) + 8 * (7 - (i - lineOffset) / 8));
        }
        else if (fen.find("K", i) == i) {
            pieces[10] |= 1ULL << (((i - lineOffset) % 8) + 8 * (7 - (i - lineOffset) / 8));
        }
        else if (fen.find("k", i) == i) {
            pieces[11] |= 1ULL << (((i - lineOffset) % 8) + 8 * (7 - (i - lineOffset) / 8));
        }
    }

    color[0] = pieces[0] | pieces[2] | pieces[4] | pieces[6] | pieces[8] | pieces[10];
    color[1] = pieces[1] | pieces[3] | pieces[5] | pieces[7] | pieces[9] | pieces[11];
    occupied = color[0] | color[1];
    InitPieceAt();
    InitMaterial();
    InitPieceCount();


    if (fen.find("w", posIndex) == posIndex + 1) {
        toMove = false;
    }
    else if (fen.find("b", posIndex) == posIndex + 1) {
        toMove = true;
    }

    castleRights = 0;
    size_t posIndex2 = std::string::npos;
    size_t tempIndex = fen.find("K", posIndex);

    if (tempIndex != std::string::npos) {
        posIndex2 = tempIndex;
        castleRights |= 8;
    }

    tempIndex = fen.find("Q", posIndex);
    if (tempIndex != std::string::npos) {
        posIndex2 = tempIndex;
        castleRights |= 4;
    }

    tempIndex = fen.find("k", posIndex);
    if (tempIndex != std::string::npos) {
        posIndex2 = tempIndex;
        castleRights |= 2;
    }

    tempIndex = fen.find("q", posIndex);
    if (tempIndex != std::string::npos) {
        posIndex2 = tempIndex;
        castleRights |= 1;
    }

    if (posIndex2 != std::string::npos) {
        posIndex = posIndex2;
    }
    else {
        posIndex = fen.find("-", posIndex);
    }

    enpassantSq = TO_NUM[fen.substr(posIndex + 2, 2)];

    std::smatch m;
    if (std::regex_search(fen, m, fenNumbers)) {
        halfMoves = std::stoi(m[1]);
        fullMoves = std::stoi(m[2]);
    }
    else {
        halfMoves = 0;
        fullMoves = 1;
    }

    posKey = zobrist->hashBoard(pieces, castleRights, enpassantSq, toMove);

}

















































//
