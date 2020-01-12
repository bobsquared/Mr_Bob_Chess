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

std::string search(Bitboard &bitboard, int depth, bool color) {

  float branchingFactor = 0;
  int prevNodes = 0;
  std::string bestMove = "";

  for (uint8_t i = 1; i < depth + 1; i++) {

    auto t1 = std::chrono::high_resolution_clock::now();
    bestMove = alphabetaRoot(color, bitboard, i, depth);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << (double)(pruning) / (double)(pruningTotal) << std::endl;
    pruning = 0;
    pruningTotal = 0;
    // bool c = color;
    // for (uint8_t j = 0; j < i; j++) {
    //   std::string bestMovePV = alphabetaRoot(c, bitboard, j + 1);
    //   c = !c;
    //   std::cout << bestMovePV << " ";
    //
    //   std::string bestMoveNum1 = bestMovePV.substr(0, 2);
    //   std::string bestMoveNum2 = bestMovePV.substr(2, 2);
    //   bitboard.movePiece(TO_NUM[bestMoveNum1], TO_NUM[bestMoveNum2]);
    //
    // }
    //
    // for (uint8_t j = 0; j < i; j++) {
    //   bitboard.undoMove();
    //
    // }


    // std::string bestMoveNum1 = bestMove.substr(0, 2);
    // std::string bestMoveNum2 = bestMove.substr(2, 2);
    // std::cout << bestMoveNum1 << bestMoveNum2 << unsigned(TO_NUM[bestMoveNum1]) << unsigned(TO_NUM[bestMoveNum2]) << std::endl;
    // bitboard.movePiece(TO_NUM[bestMoveNum1], TO_NUM[bestMoveNum1]);
    //
    // bitboard.PVMoves.push_back(Bitboard::Move{TO_NUM[bestMoveNum1], TO_NUM[bestMoveNum2]});
    // std::cout << TO_ALG[bitboard.PVMoves[0].fromLoc] << TO_ALG[bitboard.PVMoves[0].toLoc] << " ";
    //
    // bool c = color;
    // uint64_t hashF = bitboard.hashBoard(c);
    // c = !c;
    // int toundo = 1;
    //
    // while (bitboard.lookup.find(hashF) != bitboard.lookup.end()) {
    //   std::cout << "hey" << std::endl;
    //   bitboard.PVMoves.push_back(bitboard.lookup[hashF].move);
    //
    //
    //   bitboard.movePiece(bitboard.lookup[hashF].move.fromLoc, bitboard.lookup[hashF].move.toLoc);
    //   hashF = bitboard.hashBoard(c);
    //   c = !c;
    //   toundo++;
    // }


    // for (uint64_t j = 0; j < bitboard.PVMoves.size(); j++) {
    //   std::cout << TO_ALG[bitboard.PVMoves[i].fromLoc] << TO_ALG[bitboard.PVMoves[i].toLoc] << " ";
    // }
    // std::cout << std::endl;
    //
    // if (bitboard.PVMoves.size() > 0){
    //   bitboard.PVMoves.clear();
    // }


    if (branchingFactor == 0.0) {
      branchingFactor = traversedNodes;
      prevNodes = traversedNodes;
    }
    else {
      branchingFactor = (float)(traversedNodes) / (float)(prevNodes);
      prevNodes = traversedNodes;
    }

    auto diff = std::chrono::duration_cast<std::chrono::nanoseconds> (t2 - t1).count();
    printInfo(i, bestMove, branchingFactor, diff);
    traversedNodes = 0;

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
  // search(x, 10, 0);

  std::regex r("\\d+");
  bool color = true;

  // std::vector<Bitboard::Move> moves = x.allValidMoves(1);
  // x.sortMoves(moves, (Bitboard::Move){53, 37});
  // uint8_t s = moves.size();
  //
  // for (uint8_t i = 0; i < s; i++) {
  //   std::cout << unsigned((moves.back()).fromLoc) << " " << unsigned((moves.back()).toLoc) << std::endl;
  //   moves.pop_back();
  // }
  // x.printBoard(knightAttacks(1 | 1ULL << 21));


  // return 0;

  while (1) {


    std::smatch m;
    std::string move = "";

    std::getline(std::cin, move);
    if (move == "isready") {
      std::cout << "readyok" << std::endl;
      continue;
    }

    if (move.substr(0, 3) == "uci") {
      std::cout << "id name 'Mr Bob' v0.1.1" << std::endl;
      std::cout << "id author Vincent Yu" << std::endl;
      std::cout << "uciok" << std::endl;
      continue;
    }

    if (move.substr(0, 5) == "color") {
      std::regex_search(move, m, r);
      color = std::stoi(m[0]);
      continue;
    }

    if (move.substr(0, 11) == "go infinite") {
      // std::thread th1(search,x,  std::stoi(m[0]), color);
      search(x, 512, color);
      continue;
    }

    if (move.substr(0, 2) == "go") {
      std::cout << move << std::endl;
      std::regex_search(move, m, r);
      // std::thread th1(search,x,  std::stoi(m[0]), color);
      // search(x, std::stoi(m[0]), color);
      search(x, 10, color);
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

    if (move.length() >= 4) {
      std::string loc = move.substr(0, 2);
      std::string loc2 = move.substr(2, 2);
      x.movePiece(TO_NUM[loc], TO_NUM[loc2]);

    }


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
