// #include "bitboard.cpp"
#pragma once
#include <unordered_map>
#include <vector>
#include "zobrist_hashing.h"



class Bitboard{

public:

  const int pieceValues[6] = {10, 30, 30, 50, 90, 2000};

  struct Move {
    uint8_t fromLoc;
    uint8_t toLoc;
    bool quiet;
    int pieceFrom;
    int pieceTo;
  };

  struct KillerMove {
    Move move;
    int depth;
  };

  struct ZobristVal {
    Move move;
    int score;
    int alpha;
    int beta;
    int depth;
    uint8_t flag;
  };

  struct SortMove {
    Move move;
    int eval;
  };

  Bitboard();
  void printBoard(uint64_t board);
  void printPretty();

  std::vector<uint8_t> whitePiecesLoc();
  std::vector<uint8_t> blackPiecesLoc();
  std::vector<uint8_t> validMovesWhite(uint8_t index);
  std::vector<uint8_t> validMovesBlack(uint8_t index);
  bool IsMoveWhite(uint8_t index, uint8_t index2);
  bool IsMoveBlack(uint8_t index, uint8_t index2);
  std::vector<Move> allValidMoves(bool color);

  bool isAttacked(uint8_t index, bool color);
  bool filterCheck(bool color);
  uint8_t sortMoves(std::vector<Move> &moveList, Move move, int depth);
  void movePiece(uint8_t index1, uint8_t index2);
  void undoMove();

  int evaluate();
  int evaluateMobility();
  void resetBoard();

  uint64_t rookAttacksMask(uint64_t occupations, uint8_t index);
  uint64_t bishopAttacksMask(uint64_t occupations, uint8_t index);



  std::unordered_map<uint64_t, ZobristVal> lookup = {}; // Transpostion table
  // std::vector<ZobristVal> lookup = {}; // Transpostion table
  KillerMove killerMoves[1024][2];  // Killer Moves
  void InsertKiller(Move move, int depth);
  void InsertLookup(Move move, int score, int alpha, int beta, int depth, uint8_t flag, uint64_t key);
  uint64_t hashBoard(bool turn);

  std::vector<Move> PVMoves = {};




private:
  const uint64_t ALL_ONES = 18446744073709551615U;
  const uint64_t INNER_MASK = 35604928818740736U;
  const uint64_t RIGHT_MASK = 9187201950435737471U;
  const uint64_t LEFT_MASK = 18374403900871474942U;
  const uint64_t DOWN_MASK = 18446744073709551360U;
  const uint64_t UP_MASK = 72057594037927935U;



  // std::unordered_map<uint8_t, uint8_t> LSB_TABLE = {
  //   {0, 63},{1, 30},{2, 3},{3, 32},{4, 59},{5, 14},{6, 11},{7, 33},
  //   {8, 60},{9, 24},{10, 50},{11, 37},{12, 55},{13, 19},{14, 21},{15, 34},
  //   {16, 61},{17, 29},{18, 2},{19, 53},{20, 51},{21, 23},{22, 41},{23, 18},
  //   {24, 56},{25, 28},{26, 1},{27, 43},{28, 46},{29, 27},{30, 0},{31, 35},
  //   {32, 62},{33, 31},{34, 58},{35, 4},{36, 5},{37, 49},{38, 54},{39, 6},
  //   {40, 15},{41, 52},{42, 12},{43, 43},{44, 7},{45, 42},{46, 45},{47, 16},
  //   {48, 25},{49, 57},{50, 48},{51, 13},{52, 10},{53, 39},{54, 8},{55, 44},
  //   {56, 20},{57, 47},{58, 38},{59, 22},{60, 17},{61, 37},{62, 36},{63, 26}
  // };

  const int MSB_TABLE[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
   57, 49, 41, 37, 28, 16,  3, 61,
   54, 58, 35, 52, 50, 42, 21, 44,
   38, 32, 29, 23, 17, 11,  4, 62,
   46, 55, 26, 59, 40, 36, 15, 53,
   34, 51, 20, 43, 31, 22, 10, 45,
   25, 39, 14, 33, 19, 30,  9, 24,
   13, 18,  8, 12,  7,  6,  5, 63
};

const int LSB_TABLE[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
   57, 49, 41, 37, 28, 16,  3, 61,
   54, 58, 35, 52, 50, 42, 21, 44,
   38, 32, 29, 23, 17, 11,  4, 62,
   46, 55, 26, 59, 40, 36, 15, 53,
   34, 51, 20, 43, 31, 22, 10, 45,
   25, 39, 14, 33, 19, 30,  9, 24,
   13, 18,  8, 12,  7,  6,  5, 63
};


  // std::unordered_map<uint8_t, uint8_t> MSB_TABLE = {
  //   {0, 0},{1, 47},{2, 1},{3, 56},{4, 48},{5, 27},{6, 2},{7, 60},
  //   {8, 57},{9, 49},{10, 41},{11, 37},{12, 28},{13, 16},{14, 3},{15, 61},
  //   {16, 54},{17, 58},{18, 35},{19, 52},{20, 50},{21, 42},{22, 21},{23, 44},
  //   {24, 38},{25, 32},{26, 29},{27, 23},{28, 17},{29, 11},{30, 4},{31, 62},
  //   {32, 46},{33, 55},{34, 26},{35, 59},{36, 40},{37, 36},{38, 15},{39, 53},
  //   {40, 34},{41, 51},{42, 20},{43, 43},{44, 31},{45, 22},{46, 10},{47, 45},
  //   {48, 25},{49, 39},{50, 14},{51, 33},{52, 19},{53, 30},{54, 9},{55, 24},
  //   {56, 13},{57, 18},{58, 8},{59, 12},{60, 7},{61, 6},{62, 5},{63, 63}
  // };


  // Moves
  std::unordered_map<uint8_t, uint64_t> whitePawnMoves = {};
  std::unordered_map<uint8_t, uint64_t> blackPawnMoves = {};
  std::unordered_map<uint8_t, uint64_t> whitePawnAttacks = {};
  std::unordered_map<uint8_t, uint64_t> blackPawnAttacks = {};
  void InitWhitePawnMoves();
  void InitBlackPawnMoves();
  uint64_t pawnAttacksWhite(uint64_t bitboard);
  uint64_t pawnAttacksBlack(uint64_t bitboard);
  uint64_t whitePawns;
  uint64_t blackPawns;

  std::unordered_map<uint8_t, uint64_t> knightMoves = {};
  void InitKnightMoves();
  uint64_t knightAttacks(uint64_t knights);
  uint64_t whiteKnights;
  uint64_t blackKnights;

  std::unordered_map<uint8_t, uint64_t> bishopMoves = {};
  void InitBishopMoves();
  uint64_t whiteBishops;
  uint64_t blackBishops;

  std::unordered_map<uint8_t, uint64_t> rookMoves = {};
  void InitRookMoves();
  uint64_t whiteRooks;
  uint64_t blackRooks;

  std::unordered_map<uint8_t, uint64_t> queenMoves = {};
  void InitQueenMoves();
  uint64_t whiteQueens;
  uint64_t blackQueens;

  std::unordered_map<uint8_t, uint64_t> kingMoves = {};
  void InitKingMoves();
  uint64_t whiteKings;
  uint64_t blackKings;




  bool InitBlocksRook(uint64_t bitboard, uint64_t index, uint64_t magic);
  bool InitBlocksBishop(uint64_t bitboard, uint8_t index, uint64_t magic);




  uint8_t bitScanF(uint64_t bitboard);
  uint8_t bitScanR(uint64_t bitboard);
  uint64_t bitCombinations(uint64_t index, uint64_t bitboard);


  // Magic Bitboards -------------

  struct MagicPro {
    uint64_t bitboard;
    uint32_t shift;
    uint64_t magic;
    uint64_t mask;
  };

  std::unordered_map<uint8_t, uint64_t> shiftR = {};
  std::unordered_map<uint8_t, uint64_t> shiftB = {};
  std::unordered_map<uint8_t, uint64_t> magicR = {};
  std::unordered_map<uint8_t, uint64_t> magicB = {};

  std::unordered_map<uint8_t, MagicPro> attacksR = {};
  std::unordered_map<uint8_t, MagicPro> attacksB = {};

  std::unordered_map<uint8_t, std::unordered_map<uint64_t, uint64_t>> rookComb = {};
  std::unordered_map<uint8_t, std::unordered_map<uint64_t, uint64_t>> bishopComb = {};







  void optimalMagicRook();
  void optimalMagicBishop();
  void Generate_Magic_Rooks();
  void Generate_Magic_Bishops();

  std::unordered_map<uint8_t, std::unordered_map<uint64_t, uint64_t>> rayAttacks = {};
  void InitRayAttacks();

  // ---------------------------------------------


  // Moves and Pieces -------------------------------------

  struct MoveStack {
    uint64_t fromLoc;
    uint64_t toLoc;
    int8_t movePiece;
    int8_t capturePiece;
    bool color;
    bool promote;
    bool kingMoved;
    uint8_t rookMoved;
    uint8_t castled;
    uint8_t enpassant;
  };

  std::unordered_map<uint8_t, uint64_t> pieces = {};
  uint64_t whites;
  uint64_t blacks;
  uint64_t occupied;

  int materialScore;
  bool kingMovedWhite;
  bool kingMovedBlack;
  bool rookMovedWhiteA;
  bool rookMovedWhiteH;
  bool rookMovedBlackA;
  bool rookMovedBlackH;
  bool whiteCastled;
  bool blackCastled;

  std::vector<MoveStack> moveStack = {};
  uint8_t enpasssantFlag;
  bool endgameFlag;
  uint8_t enpassantConditions(bool isWhite, uint8_t pawnLocation);


  bool canCastleQ(bool isWhite);
  bool canCastleK(bool isWhite);



  // ----------------------------------------------

  uint64_t CRayAttacksN(uint64_t occupation, uint64_t index);
  uint64_t CRayAttacksNE(uint64_t occupation, uint64_t index);
  uint64_t CRayAttacksE(uint64_t occupation, uint64_t index);
  uint64_t CRayAttacksSE(uint64_t occupation, uint64_t index);
  uint64_t CRayAttacksS(uint64_t occupation, uint64_t index);
  uint64_t CRayAttacksSW(uint64_t occupation, uint64_t index);
  uint64_t CRayAttacksW(uint64_t occupation, uint64_t index);
  uint64_t CRayAttacksNW(uint64_t occupation, uint64_t index);


  Zobrist zobrist;

  void test();


  uint8_t count_population(uint64_t bitboard);



};
