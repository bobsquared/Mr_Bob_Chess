#include "perft.h"



// Static exchange evaluation test
void SeeTest(Bitboard &x, std::string fen, int from, int to, int result, MOVE flags) {

    MOVE move;
    MoveList moveList;
    std::string resultPass;

    x.setPosFen(fen);
    x.generate(moveList, 0, NO_MOVE);
    while (moveList.get_next_move(move)) {
        if (get_move_from(move) == from && get_move_to(move) == to) {
            if (flags && (move & MOVE_FLAGS) != flags) {
                continue;
            }
            int seeRes = x.seeCapture(move);
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



// Perft test positions
void PerftTest(Bitboard & x) {

    Perft(x, 5, 4865609, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    Perft(x, 5, 193690690, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
    Perft(x, 5, 674624, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
    Perft(x, 5, 15833292, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    Perft(x, 5, 15833292, "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1");
    Perft(x, 5, 89941194, "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    Perft(x, 5, 164075551, "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");

}



// Perft root call
void Perft(Bitboard & x, int depth, uint64_t numNodes, std::string fen) {

    x.setPosFen(fen);
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

    if (nodes == numNodes)  {
        std::cout << "Passed." << std::endl;
    }
    else {
        std::cout << "FAILED." << std::endl;
    }

    std::cout << std::endl;

}



// Perft recursive call
uint64_t PerftCall(Bitboard & b, int depth) {

    uint64_t nodes = 0;
    MOVE move;
    MoveList moveList;

    if (depth == 0) {
        return 1;
    }

    b.generate_unsorted(moveList, depth, NO_MOVE);
    while (moveList.get_next_move(move)) {

        // if (b.isLegal(move)) {
            b.make_move(move);

            if (!b.InCheckOther()) {
                nodes += PerftCall(b, depth - 1);
            }
            b.undo_move(move);
        // }
    }

    return nodes;
}
