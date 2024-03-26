#include "uci_commands.h"
#include "defs.h"
#include "perft.h"
#include <iostream>
#include <thread>
#include <climits>



void startPosMoves(Bitboard &b, std::string moves);



void StartCommand::execute() {
    std::cout << ENGINE_NAME << " " << ENGINE_VERSION << " UCI chess engine by " << AUTHOR << std::endl;
}



void UCICommand::execute() {
    std::cout << "id name " << ENGINE_NAME << " " << ENGINE_VERSION << std::endl;
    std::cout << "id author  " << AUTHOR << std::endl << std::endl;

    options->printAllOptions();

    std::cout << "uciok" << std::endl;
}



void ReadyCommand::execute() {
    std::cout << "readyok" << std::endl;
}


void NewGameCommand::execute() {
    ThreadSearch *thread = s.getThreads();
    for (int id = 0; id < s.getNThreads(); id++) {
        thread[id].ResetHistories();
        thread[id].InitKillers();
        thread[id].InitCounterMoves();
        thread[id].ttWrites = 0;
    }
    s.clearTT();
}



void GoCommand::execute() {
    std::thread thr;
    int wtime = INT_MAX;
    int btime = INT_MAX;
    int winc = 0;
    int binc = 0;
    int movestogo = 0;
    int depth = 99;
    int perft = -1;
    bool analysis = false;

    if (command == "go infinite") {
        analysis = true;
    }
    else {
        for (auto param : params->getParameters(command)) {
            if (param.name == "wtime") {
                wtime = param.val;
            }
            else if (param.name == "btime") {
                btime = param.val;
            }
            else if (param.name == "winc") {
                winc = param.val;
            }
            else if (param.name == "binc") {
                binc = param.val;
            }
            else if (param.name == "movestogo") {
                movestogo = param.val;
            }
            else if (param.name == "depth") {
                analysis = true;
                depth = param.val;
            }
            else if (param.name == "perft") {
                perft = param.val;
            }
        }
    }

    if (perft >= 0) {
        Perft(b, perft);
    }
    else {
        thr = std::thread(&Search::beginSearch, &s, std::ref(b), depth, wtime, btime, winc, binc, movestogo, analysis);
        thr.detach();
    }
    
}



void PositionCommand::execute() {
    // Reset postion to starting position
    if (command == "position startpos") {
        b.reset();
    }

    // Make all moves in the list
    else if (command.substr(0, 24) == "position startpos moves ") {
        b.reset();
        startPosMoves(b, command.substr(24, command.size() - 24));
    }

    // Set position to FEN position
    else if (command.substr(0, 13) == "position fen ") {
        b.reset();
        size_t indexMoves = command.find("moves ");
        b.setPosFen(command.substr(13, indexMoves));

        if (indexMoves != std::string::npos) {
            startPosMoves(b, command.substr(indexMoves, command.size() - indexMoves));
        }
    }
}



void startPosMoves(Bitboard &b, std::string moves) {

    MoveGen moveGen = MoveGen();

    // Make all the moves.
    while (moves.find(' ') != std::string::npos) {

        MOVE move;
        MoveList moveList;

        moveGen.generate_all_moves(moveList, b);
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
        moveGen.generate_all_moves(moveList, b);
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