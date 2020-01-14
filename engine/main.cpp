#include <iostream>
#include "bitboard.h"
#include <time.h>
#include <string>
#include <chrono>
#include <unordered_map>
#include <regex>
#include <vector>
#include "search.h"
#include <thread>
#include <cmath>



void printInfo(int depth, std::string move, float branchingFactor, long long int time) {
  if ((double) (time / 1000000000.0) == 0){
    std::cout << "Iterative Deepening: Depth " << depth << " Number of Nodes traversed: " << traversedNodes << " NPS: " << " inf " << " time: " <<  time / 1000000.0 << "ms" << std::endl;
    std::cout << "	Best move found: " << move <<  " Effective Branching Factor: " << branchingFactor << std::endl;
  }
  else {
    std::cout << "Iterative Deepening: Depth " << depth << " Number of Nodes traversed: " << traversedNodes << " KNPS: " << (uint64_t)((traversedNodes / 1000.0) / (double) (time / 1000000000.0)) << " time: " << time / 1000000.0 << "ms" << std::endl;
    std::cout << "	Best move found: " << move <<  " Effective Branching Factor: " << branchingFactor << std::endl;
  }
}

void printInfoUCI(int depth, long long int time, int cp, int mateInPlies) {
  std::cout << "info depth " << depth << " score cp " << cp;

  if (mateInPlies) {
    std::cout << " score mate " << std::ceil((double)(mateInPlies / 2.0));
  }

  std::cout << " nodes " << traversedNodes << " nps " << (uint64_t)(traversedNodes / (double)(time / 1000000000.0)) << " time " <<  time / 1000000.0 << std::endl;
    // std::cout << "	Best move found: " << move <<  " Effective Branching Factor: " << branchingFactor << std::endl;

}

std::string search(Bitboard &bitboard, int depth, bool color) {

  float branchingFactor = 0;
  int prevNodes = 0;
  std::string prevBestMove = "";
  std::string bestMove = "";
  int cp;
  int prevCp;
  int mateInPlies;
  int prevMateInPlies;
  ReturnInfo bMove;

  for (uint8_t i = 1; i < depth + 1; i++) {
    if (exit_thread_flag) {
      break;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    bMove = alphabetaRoot(color, bitboard, i, depth);
    bestMove = bMove.bestMove;
    cp = bMove.score * 10.0;

    if (bMove.mateIn) {
      mateInPlies = depth + 1 - bMove.mateIn;
    }
    else {
      mateInPlies = 0;
    }

    if (!exit_thread_flag) {
      prevBestMove = bestMove;
      prevCp = cp;
      prevMateInPlies = mateInPlies;
    }


    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << (double)(pruning) / (double)(pruningTotal) << std::endl;
    pruning = 0;
    pruningTotal = 0;


    if (branchingFactor == 0.0) {
      branchingFactor = traversedNodes;
      prevNodes = traversedNodes;
    }
    else {
      branchingFactor = (float)(traversedNodes) / (float)(prevNodes);
      prevNodes = traversedNodes;
    }

    auto diff = std::chrono::duration_cast<std::chrono::nanoseconds> (t2 - t1).count();
    printInfoUCI(i, diff, cp, mateInPlies);
    traversedNodes = 0;

  }

  if (exit_thread_flag) {
    bestMove = prevBestMove;
    cp = prevCp;
    mateInPlies = prevMateInPlies;
  }
  // std::cout << bitboard.lookup.bucket_count() << std::endl;
  // std::cout << bitboard.lookup.max_size() << std::endl;
  std::cout << "end" << std::endl;
  std::cout << "bestmove " << bestMove << std::endl;

  bitboard.lookup.clear();
  return bestMove;

}

void startPosMoves(bool &color, Bitboard & bitboard, std::string moves) {
  bitboard.resetBoard();
  color = true;
  while (moves.find(' ') != std::string::npos) {
    // std::cout << moves.substr(0, moves.find(' ')) << std::endl;
    bitboard.movePiece(TO_NUM[moves.substr(0, 2)], TO_NUM[moves.substr(2, 2)]);
    color = !color;
    moves = moves.erase(0, moves.find(' ') + 1);
  }

  if (moves.find(' ') == std::string::npos && (moves.size() >= 4)) {
    bitboard.movePiece(TO_NUM[moves.substr(0, 2)], TO_NUM[moves.substr(2, 2)]);
    color = !color;
  }
}




int main() {
  srand(time(NULL));
  Bitboard x = Bitboard();

  std::regex r("\\d+");
  bool color = true;
  std::thread th1;

  std::cout << "'Mr Bob' UCI engine by Vincent Yu" << std::endl;

  while (1) {


    std::smatch m;
    std::string move = "";

    std::getline(std::cin, move);
    if (move == "isready") {
      std::cout << "readyok" << std::endl;
      continue;
    }

    if (move.substr(0, 3) == "uci") {
      std::cout << "id name 'Mr Bob' v0.2.1" << std::endl;
      std::cout << "id author Vincent Yu" << std::endl;
      std::cout << "uciok" << std::endl;
      continue;
    }

    if (move.substr(0, 5) == "color") {
      std::regex_search(move, m, r);
      color = std::stoi(m[0]);
      continue;
    }

    if (move.substr(0, 11) == "stop") {
      if (th1.joinable()) {
        exit_thread_flag = true;
        th1.join();
        exit_thread_flag = false;
      }
      continue;
    }

    if (move.substr(0, 11) == "go infinite") {
      // std::thread th1(search,x,  std::stoi(m[0]), color);
      th1 = std::thread(search, std::ref(x),  512, color);
      continue;
    }

    if (move.substr(0, 2) == "go") {
      std::cout << move << std::endl;
      std::regex_search(move, m, r);
      // th1 = std::thread(search, std::ref(x),  std::stoi(m[0]), color);
      th1 = std::thread(search, std::ref(x),  10, color);
      th1.join();
      // search(x, std::stoi(m[0]), color);
      // search(x, 10, color);
      continue;
    }

    if (move == "position startpos") {
      color = true;
      x.resetBoard();
      continue;
    }

    if (move.substr(0, 24) == "position startpos moves ") {
      color = true;
      startPosMoves(color, x, move.substr(24, move.size() - 24));
      continue;
    }

    if (move.substr(0, 5) == "print") {
      x.printPretty();
      continue;
    }

    if (move.substr(0, 8) == "evaluate") {
      std::cout << x.evaluate() << std::endl;
      continue;
    }

    if (move.substr(0, 4) == "undo") {
      x.undoMove();
      continue;
    }

    if (move.substr(0, 11) == "white moves") {

      std::vector<Bitboard::Move> mrMoves = x.allValidMoves(0);
      for (Bitboard::Move mrMove : mrMoves) {
        std::cout << TO_ALG[mrMove.fromLoc] << TO_ALG[mrMove.toLoc] << " ";
      }
      std::cout << std::endl;
      continue;
    }

    // if (move.length() >= 4) {
    //   std::string loc = move.substr(0, 2);
    //   std::string loc2 = move.substr(2, 2);
    //   x.movePiece(TO_NUM[loc], TO_NUM[loc2]);
    //
    // }


  }
  // x.printPretty();
  // x.movePiece(5, 40);
  // x.printPretty();
  // x.movePiece(57, 40);
  // x.printPretty();
  // x.undoMove();
  // x.printPretty();


  return 0;
}
