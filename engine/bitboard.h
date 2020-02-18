// #include "bitboard.cpp"
#pragma once
#include <unordered_map>
#include <vector>
#include "zobrist_hashing.h"
#include "magic_bitboards.h"



class Bitboard{

public:

  void updateHalfMove();
  const int pieceValues[6] = {100, 300, 325, 500, 900, 20000};
  const std::string NUM_TO_STR[9] = {"0", "1", "2", "3", "4", "5", "6", "7", "8"};

  struct Move {
    uint8_t fromLoc;
    uint8_t toLoc;
    bool quiet;
    int8_t pieceFrom;
    int8_t pieceTo;
    int score;
    uint8_t promotion;
    bool isEnpassant;
    // Move(uint8_t fromLoc, uint8_t toLoc, bool quiet, int pieceFrom, int pieceTo) : fromLoc(fromLoc), toLoc(toLoc), quiet(quiet), pieceFrom(pieceFrom), pieceTo(pieceTo) {}
    bool operator<(const Move& a) const { return score > a.score; }
    bool operator==(const Move& rhs) {
      return (fromLoc == rhs.fromLoc) && (toLoc == rhs.toLoc);
    }
    Move() :
      fromLoc(0), toLoc(0), quiet(true), pieceFrom(0), pieceTo(-1), score(0), promotion(0), isEnpassant(false) {}
    Move(uint8_t fromLoc, uint8_t toLoc, bool quiet, int8_t pieceFrom, int8_t pieceTo, int score, uint8_t promotion, bool isEnpassant) :
      fromLoc(fromLoc), toLoc(toLoc), quiet(quiet), pieceFrom(pieceFrom), pieceTo(pieceTo), score(score), promotion(promotion), isEnpassant(isEnpassant) {}
  };


  struct ZobristVal {
    Move move;
    int score;
    int depth;
    uint8_t flag;
    uint64_t posKey;
    uint16_t halfMove;

    ZobristVal() :
      move(Move()), score(0), depth(0), flag(0), posKey(0), halfMove(0) {}
  };

  struct SortMove {
    Move move;
    int eval;
  };

  Bitboard();
  ~Bitboard();
  void printBoard(uint64_t board);
  void printPretty();

  std::vector<uint8_t> whitePiecesLoc();
  std::vector<uint8_t> blackPiecesLoc();
  std::vector<uint8_t> validMovesWhite(uint8_t index);
  std::vector<uint8_t> validMovesBlack(uint8_t index);
  bool IsMoveWhite(Move &move);
  bool IsMoveBlack(Move &move);
  std::vector<Move> allValidMoves(bool color);
  std::vector<Move> allValidCaptures(bool color);

  bool isAttacked(uint8_t index, bool color);
  bool filterCheck(bool color);
  uint8_t sortMoves(std::vector<Move> &moveList, Move &move, int depth);
  void movePiece(Move& move);
  void undoMove();
  bool canNullMove();

  int evaluate(int alpha, int beta);
  int evaluateMobility(uint64_t whitePawns, uint64_t blackPawns, uint64_t whiteKnights, uint64_t blackKnights,
    uint64_t whiteBishops, uint64_t blackBishops, uint64_t whiteRooks, uint64_t blackRooks, uint64_t whiteQueens, uint64_t blackQueens, bool endgame);
  int evaluateKingSafety(uint8_t whiteKingIndex, uint8_t blackKingIndex, uint8_t whitePawns, uint8_t blackPawns, uint64_t whiteKnights, uint64_t blackKnights,
    uint64_t whiteBishops, uint64_t blackBishops, uint64_t whiteRooks, uint64_t blackRooks, uint64_t whiteQueens, uint64_t blackQueens);
  void resetBoard();
  bool isThreeFold();

  uint64_t rookAttacksMask(uint64_t occupations, uint8_t index);
  uint64_t bishopAttacksMask(uint64_t occupations, uint8_t index);
  uint8_t manhattanArray[64][64];
  uint8_t chebyshevArray[64][64];

  std::unordered_map<uint64_t, ZobristVal> lookup = {}; // Transpostion table
  ZobristVal *lookup2; // Transpostion table
  Move killerMoves[2][1024][2];  // Killer Moves
  void InsertKiller(Move move, int depth);
  void InsertLookup(Move move, int score, int depth, uint8_t flag, uint64_t key);
  uint64_t hashBoard(bool turn);

  std::vector<Move> PVMoves = {};
  std::string posToFEN();
  uint64_t getPosKey();
  uint32_t history[2][64][64];

  uint64_t numHashes;





private:
  const uint64_t ALL_ONES = 18446744073709551615U;
  const uint64_t INNER_MASK = 35604928818740736U;
  const uint64_t RIGHT_MASK = 9187201950435737471U;
  const uint64_t LEFT_MASK = 18374403900871474942U;
  const uint64_t DOWN_MASK = 18446744073709551360U;
  const uint64_t UP_MASK = 72057594037927935U;

  const uint8_t attackWeight[8] = {
    0, 0, 50, 75, 88, 94, 97, 99
};

  const int8_t knightWeight[9] = {
    -20, -16, -12, -8, -4, 0, 4, 8, 12
  };

  const int8_t rookWeight[9] = {
    15, 12, 9, 6, 3, 0, -3, -6, -9
  };




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







  void optimalMagicRook();
  void optimalMagicBishop();
  void Generate_Magic_Rooks();
  void Generate_Magic_Bishops();

  std::unordered_map<uint8_t, std::unordered_map<uint64_t, uint64_t>> rayAttacks = {};
  void InitRayAttacks();
  void InitMvvLva();
  uint16_t mvvlva[6][6];

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

    MoveStack(uint64_t fromLoc, uint64_t toLoc, int8_t movePiece, int8_t capturePiece, bool color, bool promote, bool kingMoved, uint8_t rookMoved, uint8_t castled, uint8_t enpassant) :
      fromLoc(fromLoc), toLoc(toLoc), movePiece(movePiece), capturePiece(capturePiece), color(color), promote(promote), kingMoved(kingMoved), rookMoved(rookMoved), castled(castled), enpassant(enpassant) {}
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
  std::vector<uint64_t> prevPositions = {};
  uint8_t enpasssantFlag;
  bool endgameFlag;
  bool whiteToMove;

  uint8_t countPawnsW;
  uint8_t countPawnsB;

  uint8_t countKnightsW;
  uint8_t countKnightsB;

  uint8_t countBishopsW;
  uint8_t countBishopsB;

  uint8_t countRooksW;
  uint8_t countRooksB;

  uint8_t countQueensW;
  uint8_t countQueensB;

  int whitePawnTable[64];
  int blackPawnTable[64];
  int whitePawnTableEG[64];
  int blackPawnTableEG[64];
  int whiteKnightTable[64];
  int blackKnightTable[64];
  int whiteBishopTable[64];
  int blackBishopTable[64];
  int whiteRookTable[64];
  int blackRookTable[64];
  int whiteQueenTable[64];
  int blackQueenTable[64];
  int whiteKingTable[64];
  int blackKingTable[64];
  void InitPieceBoards();
  void InitDistanceArray();
  uint8_t enpassantConditions(bool isWhite, uint8_t pawnLocation);

  Magics magics;





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
  uint16_t halfMove;



};
