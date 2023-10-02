#include <iostream>
#include <cstdint>
#include <regex>
#include <thread>
#include <climits>
#include "magic_bitboards.h"
#include "eval.h"
#include "movegen.h"
#include "defs.h"
#include "perft.h"
#include "bitboard.h"
#include "search.h"
#include "uci_commands.h"
#include "uci.h"


extern char _binary_bob_brain_041523e378_nnue_start[];
extern char _binary_bob_brain_041523e378_nnue_size[];


Zobrist *zobrist = new Zobrist();
Magics *magics;


void Bench(Bitboard &b, Search &s) {
    // positions from Ethereal
    static const char* Benchmarks[] = {
    #include "bench.csv"
        ""};

    int nodes = 0;
    int nn = 0;
    int time  = 0;

    s.willPrintInfo(false);
    for (int i = 0; strcmp(Benchmarks[i], ""); i++) {
        s.setSearch();
        b.setPosFen(Benchmarks[i]);
        int eva = s.beginSearch(b, 13, INT_MAX, INT_MAX, 0, 0, 0, true);
        nodes = s.getTotalNodesSearched();
        printf("Bench [# %2d] %12d nodes %8d nps %8d CP\n", i + 1, (int) nodes, (int) (1000.0f * nodes / (s.getTotalTime() + 1)), eva);
        nn += nodes;
        time += s.getTotalTime();
        b.reset();
    }
    s.willPrintInfo(true);

    printf("OVERALL: %53d nodes %8d nps\n", nn, (int) (1000.0f * nn / (time + 1)));
}



int main(int argc, char* argv[]) {

    InitColumnsMask();
    InitRowsMask();

    size_t nnueBytesSize = reinterpret_cast<size_t>(&_binary_bob_brain_041523e378_nnue_size);

    magics = new Magics();
    KPNNUE *model = new KPNNUE(_binary_bob_brain_041523e378_nnue_start, nnueBytesSize);
    Eval *eval = new Eval(model);                        /**< The evaluator to score the positions*/
    TranspositionTable *tt = new TranspositionTable();
    ThreadSearch *thread = new ThreadSearch[1];

    Search s = Search(eval, tt, thread);
    Bitboard pos = Bitboard();
    std::string command = "";

    UCIOptions options = UCIOptions();
    UCIParameters params = UCIParameters();
    UCICommandInvoker uci;

    CommandInterface* c = nullptr;
    
    options.addOption([&](int n){s.setTTSize(n);}, "Hash", 256, 1, 131072);
    options.addOption([&](int n){s.setNumThreads(n);}, "Threads", 1, 1, 256);
    options.addOption([&](int n){s.setMultiPVSearch(n);}, "MultiPV", 1, 1, 256);

    options.addOption([&](int n){s.setRFPsearch(n);}, "rfp", 136, 1, 500);
    options.addOption([&](int n){s.setRazorsearch(n);}, "razor", 392, 1, 1000);
    options.addOption([&](int n){s.setProbcutsearch(n);}, "probcut", 251, 1, 1000);
    options.addOption([&](int n){s.setFutilitysearch(n);}, "futility", 328, 1, 1000);
    options.addOption([&](int n){s.setHistoryLMRsearch(n);}, "histlmr", 2084, 1, 10000);
    options.addOption([&](int n){s.setHistoryLMRNoisysearch(n);}, "histlmrnoisy", 2534, 1, 10000);

    params.addParameter("wtime");
    params.addParameter("btime");
    params.addParameter("winc");
    params.addParameter("binc");
    params.addParameter("movestogo");
    params.addParameter("depth");
    params.addParameter("perft");

    c = new StartCommand();
    uci.setCommand(c);
    uci.executeUCI();
    uci.cleanCommand();

    c = new NewGameCommand(s);
    uci.setCommand(c);
    uci.executeUCI();
    uci.cleanCommand();

    if (argc > 1 && strcmp(argv[1], "bench") == 0) {
        Bench(pos, s);
        return 0;
    }

    // Forever loop of awesomeness
    while (std::getline(std::cin, command)) {
        s.setSearch();
        std::string commandType = command.substr(0, command.find(' '));

        // Quit the program
        if (command == "quit") {
            s.stopSearch();
            break;
        }

        else if (commandType == "uci") {
            c = new UCICommand(&options);
            uci.setCommand(c);
        }

        else if (commandType == "isready") {
            c = new ReadyCommand();
            uci.setCommand(c);
        }

        else if (command == "ucinewgame") {
            c = new NewGameCommand(s);
            uci.setCommand(c);
        }

        else if (command == "stop") {
            s.stopSearch();
            continue;
        }

        else if (commandType == "go") {
            c = new GoCommand(&params, command, s, pos);
            uci.setCommand(c);
        }

        else if (commandType == "setoption") {
            options.setOption(command);
        }

        else if (commandType == "position") {
            c = new PositionCommand(command, pos);
            uci.setCommand(c);
        }

        else if (command == "see") {
            const int pvals[6] = {100, 450, 450, 650, 1250, 5000};
            SeeTest(pos, "4R3/2r3p1/5bk1/1p1r3p/p2PR1P1/P1BK1P2/1P6/8 b - -", TO_NUM["h5"], TO_NUM["g4"], 0);
            SeeTest(pos, "4R3/2r3p1/5bk1/1p1r1p1p/p2PR1P1/P1BK1P2/1P6/8 b - -", TO_NUM["h5"], TO_NUM["g4"], 0);
            SeeTest(pos, "4r1k1/5pp1/nbp4p/1p2p2q/1P2P1b1/1BP2N1P/1B2QPPK/3R4 b - -", TO_NUM["g4"], TO_NUM["f3"],0);
            SeeTest(pos, "2r1r1k1/pp1bppbp/3p1np1/q3P3/2P2P2/1P2B3/P1N1B1PP/2RQ1RK1 b - -", TO_NUM["d6"], TO_NUM["e5"], pvals[0]);
            SeeTest(pos, "7r/5qpk/p1Qp1b1p/3r3n/BB3p2/5p2/P1P2P2/4RK1R w - -", TO_NUM["e1"], TO_NUM["e8"], 0);
            SeeTest(pos, "6rr/6pk/p1Qp1b1p/2n5/1B3p2/5p2/P1P2P2/4RK1R w - -", TO_NUM["e1"], TO_NUM["e8"], -pvals[3]);
            SeeTest(pos, "7r/5qpk/2Qp1b1p/1N1r3n/BB3p2/5p2/P1P2P2/4RK1R w - -", TO_NUM["e1"], TO_NUM["e8"], -pvals[3]);
            SeeTest(pos, "6RR/4bP2/8/8/5r2/3K4/5p2/4k3 w - -", TO_NUM["f7"], TO_NUM["f8"], pvals[2] - pvals[0], QUEEN_PROMOTION_FLAG);
            SeeTest(pos, "6RR/4bP2/8/8/5r2/3K4/5p2/4k3 w - -", TO_NUM["f7"], TO_NUM["f8"], pvals[1] - pvals[0], KNIGHT_PROMOTION_FLAG);
            SeeTest(pos, "7R/5P2/8/8/8/3K2r1/5p2/4k3 w - -", TO_NUM["f7"], TO_NUM["f8"], pvals[4] - pvals[0], QUEEN_PROMOTION_FLAG);
            SeeTest(pos, "7R/5P2/8/8/8/3K2r1/5p2/4k3 w - -", TO_NUM["f7"], TO_NUM["f8"], pvals[2] - pvals[0], BISHOP_PROMOTION_FLAG);
            SeeTest(pos, "7R/4bP2/8/8/1q6/3K4/5p2/4k3 w - -", TO_NUM["f7"], TO_NUM["f8"], -pvals[0], ROOK_PROMOTION_FLAG);
            SeeTest(pos, "8/4kp2/2npp3/1Nn5/1p2PQP1/7q/1PP1B3/4KR1r b - -", TO_NUM["h1"], TO_NUM["f1"], 0);
            SeeTest(pos, "8/4kp2/2npp3/1Nn5/1p2P1P1/7q/1PP1B3/4KR1r b - -", TO_NUM["h1"], TO_NUM["f1"], 0);
            SeeTest(pos, "2r2r1k/6bp/p7/2q2p1Q/3PpP2/1B6/P5PP/2RR3K b - -", TO_NUM["c5"], TO_NUM["c1"], 2 * pvals[3] - pvals[4]);
            SeeTest(pos, "r2qk1nr/pp2ppbp/2b3p1/2p1p3/8/2N2N2/PPPP1PPP/R1BQR1K1 w kq -", TO_NUM["f3"], TO_NUM["e5"], pvals[0]);
            SeeTest(pos, "6r1/4kq2/b2p1p2/p1pPb3/p1P2B1Q/2P4P/2B1R1P1/6K1 w - -", TO_NUM["f4"], TO_NUM["e5"], 0);
            SeeTest(pos, "3q2nk/pb1r1p2/np6/3P2Pp/2p1P3/2R4B/PQ3P1P/3R2K1 w - h6", TO_NUM["g5"], TO_NUM["h6"], 0);
            SeeTest(pos, "3q2nk/pb1r1p2/np6/3P2Pp/2p1P3/2R1B2B/PQ3P1P/3R2K1 w - h6", TO_NUM["g5"], TO_NUM["h6"], pvals[0]);
            SeeTest(pos, "2r4r/1P4pk/p2p1b1p/7n/BB3p2/2R2p2/P1P2P2/4RK2 w - -", TO_NUM["c3"], TO_NUM["c8"], pvals[3]);
            SeeTest(pos, "2r5/1P4pk/p2p1b1p/5b1n/BB3p2/2R2p2/P1P2P2/4RK2 w - -", TO_NUM["c3"], TO_NUM["c8"], pvals[3]);
            SeeTest(pos, "2r4k/2r4p/p7/2b2p1b/4pP2/1BR5/P1R3PP/2Q4K w - -", TO_NUM["c3"], TO_NUM["c5"], pvals[2]);
            SeeTest(pos, "8/pp6/2pkp3/4bp2/2R3b1/2P5/PP4B1/1K6 w - -", TO_NUM["g2"], TO_NUM["c6"], pvals[0] - pvals[2]);
            SeeTest(pos, "4q3/1p1pr1k1/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - -", TO_NUM["e6"], TO_NUM["e4"], pvals[0]- pvals[3]);
            SeeTest(pos, "4q3/1p1pr1kb/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - -", TO_NUM["h7"], TO_NUM["e4"], pvals[0]);
            continue;
        }

        
        uci.executeUCI();
        uci.cleanCommand();

    }

    s.cleanUpSearch();

    delete magics;
    delete zobrist;

    return 0;
}
