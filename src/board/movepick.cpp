/**
* A move picker.
*
* Used mainly to give moves a score.
*/


#include "movepick.h"
#include "legality.h"
#include "movegen.h"


namespace MOVEPICK {



    void scoreMoves(MoveList &moveList, Board &b, PrevMoveInfo &prev, ThreadData &td) {

        MOVE move;
        int from;
        int to;
        MOVE prevMove = prev.prevMove;
        bool isValidPrevMove = (prevMove != NO_MOVE && prevMove != NULL_MOVE);

        for (int i = 0; i < moveList.count; i++) {
            move = moveList.get_index_move(i);
            int moveTo = get_move_to(move);
            int moveFrom = get_move_from(move);

            if (move & CAPTURE_FLAG) {

                from = b.state.pieceAt[moveFrom] / 2;
                to = b.state.pieceAt[moveTo] / 2;

                if ((move & MOVE_FLAGS) == ENPASSANT_FLAG) {
                    moveList.set_score_index(i, 1000000 + mvvlva[from][0]);
                }
                else {
                    int see = BITBOARD::seeCapture(b.state, move);
                    int score = (see > 0? 1000000 : (see == 0? 950000 : -1000000));
                    moveList.set_score_index(i, score + mvvlva[from][to] + THREAD::getHistory(td.historyData, b.state.toMove, false, moveFrom, moveTo) / 256);
                }

            }
            else if (move & PROMOTION_FLAG) {
                if ((move & QUEEN_PROMOTION_FLAG) == QUEEN_PROMOTION_FLAG) {
                    moveList.set_score_index(i, 1000000);
                }
                else if ((move & QUEEN_PROMOTION_FLAG) == KNIGHT_PROMOTION_FLAG) {
                    moveList.set_score_index(i, 650000);
                }
                else {
                    moveList.set_score_index(i, 0);
                }
            }
            else {
                int cmh = isValidPrevMove * THREAD::getCounterHistory(b, prev, td.historyData,  moveFrom, moveTo);
                moveList.set_score_index(i, THREAD::getHistory(td.historyData, b.state.toMove, true, moveFrom, moveTo) + cmh);
            }
        }

    }



    bool pick_move(MOVE &move, Board &b, PrevMoveInfo &prev, ThreadData &td, MovePickData &mpd) {

        switch (mpd.stage) {
            case PV_MOVE: {
                mpd.stage = PV_MOVE2;
                if (BITBOARD::isPseudoLegal(b.state, mpd.pvMove)) {
                    move = mpd.pvMove;
                    return true;
                }
                [[fallthrough]];
            }
            case PV_MOVE2: {
                mpd.stage = PV_MOVE3;
                if (BITBOARD::isPseudoLegal(b.state, mpd.pvMove2)) {
                    move = mpd.pvMove2;
                    return true;
                }
                [[fallthrough]];
            }
            case PV_MOVE3: {
                mpd.stage = GEN_CAPTURES;
                if (BITBOARD::isPseudoLegal(b.state, mpd.pvMove3)) {
                    move = mpd.pvMove3;
                    return true;
                }
                [[fallthrough]];
            }
            case GEN_CAPTURES: {
                mpd.stage = GOOD_CAPTURES;
                MOVE_GEN::generate_captures_promotions(mpd.moveListCaptures, b.state);
                scoreMoves(mpd.moveListCaptures, b, prev, td);
                [[fallthrough]];
            }
            case GOOD_CAPTURES: {
                int score = 0;
                while (mpd.moveListCaptures.get_next_move_score(move, score) && score >= 500000) {
                    mpd.moveListCaptures.remove_move(move);
                    if (skip_move(move, mpd)) {
                        continue;
                    }
                    return true;
                }
                mpd.stage = KILLER1;
                [[fallthrough]];
            }
            case KILLER1: {
                mpd.stage = KILLER2;
                if (mpd.killMove1 != mpd.pvMove && mpd.killMove1 != mpd.pvMove2 && mpd.killMove1 != mpd.pvMove3) {
                    if (BITBOARD::isPseudoLegal(b.state, mpd.killMove1)) {
                        move = mpd.killMove1;
                        return true;
                    }
                }
                
                [[fallthrough]];
            }
            case KILLER2: {
                mpd.stage = COUNTER_MOVE;
                if (mpd.killMove2 != mpd.pvMove && mpd.killMove2 != mpd.pvMove2 && mpd.killMove2 != mpd.pvMove3) {
                    if (BITBOARD::isPseudoLegal(b.state, mpd.killMove2)) {
                        move = mpd.killMove2;
                        return true;
                    }
                }
                [[fallthrough]];
            }
            case COUNTER_MOVE: {
                mpd.stage = GEN_QUIETS;
                if (mpd.counterMove != mpd.pvMove && mpd.counterMove != mpd.pvMove2 && mpd.counterMove != mpd.pvMove3 && mpd.counterMove != mpd.killMove1 && mpd.counterMove != mpd.killMove2) {
                    if (BITBOARD::isPseudoLegal(b.state, mpd.counterMove)) {
                        move = mpd.counterMove;
                        return true;
                    }
                }
                [[fallthrough]];
            }
            case GEN_QUIETS: {
                mpd.stage = QUIETS;
                MOVE_GEN::generate_quiets(mpd.moveListQuiets, b.state);
                scoreMoves(mpd.moveListQuiets, b, prev, td);
                [[fallthrough]];
            }
            case QUIETS: {
                while (mpd.moveListQuiets.get_next_move(move)) {
                    if (skip_move(move, mpd)) {
                        continue;
                    }
                    return true;
                }
                mpd.stage = BAD_CAPTURES;
                [[fallthrough]];
            }
            case BAD_CAPTURES: {
                while (mpd.moveListCaptures.get_next_move(move)) {
                    if (skip_move(move, mpd)) {
                        continue;
                    }
                    return true;
                }
                mpd.stage = DONE;
                [[fallthrough]];
            }
            case DONE: {
                return false;
            }
            default: {
                return false;
            }

        }
    }



    void scoreMovesQS(MoveList &moveList, Board &b) {

        MOVE move;
        int from;
        int to;

        for (int i = 0; i < moveList.count; i++) {
            move = moveList.get_index_move(i);

            if (move & CAPTURE_FLAG) {

                from = b.state.pieceAt[get_move_from(move)] / 2;
                to = b.state.pieceAt[get_move_to(move)] / 2;

                if ((move & MOVE_FLAGS) == ENPASSANT_FLAG) {
                    moveList.set_score_index(i, 1000000 + mvvlva[from][0]);
                }
                else {
                    int see = BITBOARD::seeCapture(b.state, move);
                    int score = see > 0? 1000000 : (see == 0? 900000 : 0);
                    moveList.set_score_index(i, score + mvvlva[from][to]);
                }

            }
            else if (move & PROMOTION_FLAG) {
                if ((move & QUEEN_PROMOTION_FLAG) == QUEEN_PROMOTION_FLAG) {
                    moveList.set_score_index(i, 1000000);
                }
                else if ((move & QUEEN_PROMOTION_FLAG) == KNIGHT_PROMOTION_FLAG) {
                    moveList.set_score_index(i, 650000);
                }
                else {
                    moveList.set_score_index(i, 0);
                }
            }
        }

    }



    bool pick_move_qs(MOVE &move, Board &b, MovePickDataQS &mpd, bool inCheck) {

        switch (mpd.stage) {
            case PV_MOVE: {
                mpd.stage = GEN_CAPTURES;
                if ((inCheck || !isQuietMove(mpd.pvMove)) && BITBOARD::isPseudoLegal(b.state, mpd.pvMove)) {
                    move = mpd.pvMove;
                    return true;
                }
                [[fallthrough]];
            }
            case GEN_CAPTURES: {
                mpd.stage = GOOD_CAPTURES;
                inCheck? MOVE_GEN::generate_all_moves(mpd.moveList, b.state) : MOVE_GEN::generate_captures_promotions(mpd.moveList, b.state);
                scoreMovesQS(mpd.moveList, b);
                [[fallthrough]];
            }
            case GOOD_CAPTURES: {
                while (mpd.moveList.get_next_move(move)) {
                    if (skip_move_qs(move, mpd)) {
                        continue;
                    }
                    return true;
                }
                return false;
            }
            default: {
                return false;
            }

        }
    }
}
