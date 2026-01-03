#include "perft.h"


// Static exchange evaluation test
void SeeTest(Board &x, std::string fen, int from, int to, int result, MOVE flags) {

    MOVE move;
    MoveList moveList;
    std::string resultPass;

    BITBOARD::setPosFen(x.state, x.acc, fen);
    MOVE_GEN::generate_all_moves(moveList, x.state);
    while (moveList.get_next_move(move)) {
        if (get_move_from(move) == from && get_move_to(move) == to) {
            if (flags && (move & MOVE_FLAGS) != flags) {
                continue;
            }
            int seeRes = BITBOARD::seeCapture(x.state, move);
            if (seeRes == result) {
                resultPass = "PASS";
            }
            else {
                resultPass = "FAIL";
            }
            std::cout << "Result - " << fen << " " << seeRes  << " "  << result << " " << resultPass << std::endl;
        }
    }

}



// Perft root call
void Perft(Board & x, int depth) {

    uint64_t nodes = 0;
    for (int i = 0; i <= depth; i++) {
        auto t1 = std::chrono::high_resolution_clock::now();
        nodes= PerftCall(x, i);
        std::cout << "Depth: " << i << ", Nodes: " << nodes;
        auto t2 = std::chrono::high_resolution_clock::now();

        auto diff = std::chrono::duration_cast<std::chrono::nanoseconds> (t2 - t1).count();
        if ((double) diff == 0) {
            std::cout << ", NPS: 0" << std::endl;
        }
        else {
            std::cout << ", NPS: " << (int) (((double) nodes * 1000000000.0) / ((double) diff)) << std::endl;
        }

    }

    std::cout << "Done" << std::endl;

}



// Perft recursive call
uint64_t PerftCall(Board & b, int depth) {

    if (depth == 0) {
        return 1;
    }

    uint64_t nodes = 0;
    MOVE move;
    MoveList moveList;

    MOVE_GEN::generate_all_moves(moveList, b.state);
    if (depth == 1) {
        int count = 0;

        for (int i = 0; i < moveList.count; i++) {
            move = moveList.moves[i];
            if (BITBOARD::isLegal(b, move)) {
                count++;
            }
        }
        return count;
    }

    for (int i = 0; i < moveList.count; i++) {
        move = moveList.moves[i];
        if (BITBOARD::isLegal(b, move)) {
            BITBOARD::make_move(b, move);
            nodes += PerftCall(b, depth - 1);
            BITBOARD::undo_move(b, move);
        }
    }

    return nodes;
}
