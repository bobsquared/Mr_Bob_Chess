#include "uci.h"



UCI::UCI() {}

// Prints the commands
void UCI::startMessage() {
    std::cout << NAME << " " << VERSION << " UCI chess engine by " << AUTHOR << std::endl;
}


// Prints the commands
void UCI::uciCommand() {
    std::cout << "id name " << NAME << " " << VERSION << std::endl;
    std::cout << "id author " << AUTHOR << std::endl;
    std::cout << std::endl;

    std::cout << "option name Hash type spin default 256 min 1 max 2054" << std::endl;
    std::cout << "uciok" << std::endl;
}



// Prints the commands
void UCI::readyCommand() {
    std::cout << "readyok" << std::endl;
}



// New game has started, clear hash, killers, histories, etc.
void UCI::newGameCommand(Bitboard &b) {
    InitHistory();
    InitKillers();
    InitCounterMoves();
    b.clearHashTable();
}



// Universal Chess Interface position startpos moves command
// Moves the pieces on the board according to the command.
void UCI::startPosMoves(Bitboard & b, std::string moves) {

    // Make all the moves.
    while (moves.find(' ') != std::string::npos) {

        MOVE move;
        MoveList moveList;

        b.generate(moveList, 0, NO_MOVE);
        while (moveList.get_next_move(move)) {
            if (get_move_from(move) == TO_NUM[moves.substr(0, 2)] && get_move_to(move) == TO_NUM[moves.substr(2, 2)]) {
                if (moves.substr(4, 1) == "q") {
                    if ((move & MOVE_FLAGS) != QUEEN_PROMOTION_FLAG && (move & MOVE_FLAGS) != QUEEN_PROMOTION_CAPTURE_FLAG) {
                        continue;
                    }
                }
                else if (moves.substr(4, 1) == "r"){
                    if ((move & MOVE_FLAGS) != ROOK_PROMOTION_FLAG && (move & MOVE_FLAGS) != ROOK_PROMOTION_CAPTURE_FLAG) {
                        continue;
                    }
                }
                else if (moves.substr(4, 1) == "b"){
                    if ((move & MOVE_FLAGS) != BISHOP_PROMOTION_FLAG && (move & MOVE_FLAGS) != BISHOP_PROMOTION_CAPTURE_FLAG) {
                        continue;
                    }
                }
                else if (moves.substr(4, 1) == "n"){
                    if ((move & MOVE_FLAGS) != KNIGHT_PROMOTION_FLAG && (move & MOVE_FLAGS) != KNIGHT_PROMOTION_CAPTURE_FLAG) {
                        continue;
                    }
                }
                b.make_move(move);
                break;
            }
        }

        moves = moves.erase(0, moves.find(' ') + 1);
    }

    // If only one more move in the list
    if (moves.find(' ') == std::string::npos && (moves.size() >= 4)) {
        MOVE move;
        MoveList moveList;
        b.generate(moveList, 0, NO_MOVE);
        while (moveList.get_next_move(move)) {
            if (get_move_from(move) == TO_NUM[moves.substr(0, 2)] && get_move_to(move) == TO_NUM[moves.substr(2, 2)]) {
                if (moves.size() >= 5) {
                    if (moves.substr(4, 1) == "q") {
                        if ((move & MOVE_FLAGS) != QUEEN_PROMOTION_FLAG && (move & MOVE_FLAGS) != QUEEN_PROMOTION_CAPTURE_FLAG) {
                            continue;
                        }
                    }
                    else if (moves.substr(4, 1) == "r"){
                        if ((move & MOVE_FLAGS) != ROOK_PROMOTION_FLAG && (move & MOVE_FLAGS) != ROOK_PROMOTION_CAPTURE_FLAG) {
                            continue;
                        }
                    }
                    else if (moves.substr(4, 1) == "b"){
                        if ((move & MOVE_FLAGS) != BISHOP_PROMOTION_FLAG && (move & MOVE_FLAGS) != BISHOP_PROMOTION_CAPTURE_FLAG) {
                            continue;
                        }
                    }
                    else if (moves.substr(4, 1) == "n"){
                        if ((move & MOVE_FLAGS) != KNIGHT_PROMOTION_FLAG && (move & MOVE_FLAGS) != KNIGHT_PROMOTION_CAPTURE_FLAG) {
                            continue;
                        }
                    }
                }
                b.make_move(move);
                break;
            }
        }
    }

}
