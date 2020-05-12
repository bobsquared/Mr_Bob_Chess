// #include "bitboard.cpp"
#pragma once
#include <unordered_map>
#include <vector>
#include "zobrist_hashing.h"
#include "magic_bitboards.h"



class Bitboard{

public:

  // All information about a move
  struct Move {
    uint8_t fromLoc;
    uint8_t toLoc;
    bool quiet;
    int8_t pieceFrom;
    int8_t pieceTo;
    int score;
    uint8_t promotion;
    bool isEnpassant;

    bool operator<(const Move& a) const { return score > a.score; }
    bool operator==(const Move& rhs) {
      return (fromLoc == rhs.fromLoc) && (toLoc == rhs.toLoc);
    }
    Move() :
      fromLoc(0), toLoc(0), quiet(true), pieceFrom(0), pieceTo(-1), score(0), promotion(0), isEnpassant(false) {}
    Move(uint8_t fromLoc, uint8_t toLoc, bool quiet, int8_t pieceFrom, int8_t pieceTo, int score, uint8_t promotion, bool isEnpassant) :
      fromLoc(fromLoc), toLoc(toLoc), quiet(quiet), pieceFrom(pieceFrom), pieceTo(pieceTo), score(score), promotion(promotion), isEnpassant(isEnpassant) {}
  };


  uint32_t history[2][64][64]; // For history heuristic


  Bitboard();
  ~Bitboard();

  // Insert into killer moves and transposition table
  void InsertKiller(Move &move, int &depth);
  uint64_t getPosKey();

  // Printing utilities
  void printBoard(uint64_t board);
  void printPretty();

  // Checking and generating moves
  bool IsMoveWhite(Move &move);
  bool IsMoveBlack(Move &move);
  std::vector<Move> allValidMoves(bool color);
  std::vector<Move> allValidCaptures(bool color);

  // Filtering and scoring moves
  bool filterCheck(bool color);
  void scoreMoves(std::vector<Move> &moveList, Move &move, int depth, bool isWhite);
  Move pickMove(std::vector<Move> &moveList);

  // Checking moves
  bool canNullMove();
  bool isThreeFold();
  bool isDraw();
  bool isPassedPawn(int index, bool color);

  // Moving pieces
  void movePiece(Move &move);
  void undoMove();

  // Evaluate position
  int evaluate();
  void evaluateDebug();

  // Reseting position to original
  void resetBoard();

  // Extras
  std::string posToFEN();
  void updateHalfMove();
  int seeCapture(Move &capture, bool isWhite);


private:

  const uint64_t ALL_ONES = 18446744073709551615U;
  const uint64_t INNER_MASK = 35604928818740736U;
  const uint64_t RIGHT_MASK = 9187201950435737471U;
  const uint64_t LEFT_MASK = 18374403900871474942U;
  const uint64_t DOWN_MASK = 18446744073709551360U;
  const uint64_t UP_MASK = 72057594037927935U;

  const int pieceValues[6] = {100, 321, 333, 500, 900, 20000};
  const std::string NUM_TO_STR[9] = {"0", "1", "2", "3", "4", "5", "6", "7", "8"};

  const int attackWeight[8] = {
    12, 25, 50, 75, 88, 94, 97, 99
  };

  const int knightWeight[9] = {
    -20, -16, -12, -8, -4, 0, 4, 8, 12
  };

  const int rookWeight[9] = {
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




  // Moves
  uint64_t whitePawnMoves[64];
  uint64_t blackPawnMoves[64];
  uint64_t whitePawnAttacks[64];
  uint64_t blackPawnAttacks[64];
  void InitWhitePawnMoves();
  void InitBlackPawnMoves();
  uint64_t pawnAttacksMan(uint64_t bitboard, bool isWhite);
  uint64_t pawnAttacks(bool isWhite);
  uint64_t whitePawns;
  uint64_t blackPawns;

  uint64_t knightMoves[64];
  void InitKnightMoves();
  uint64_t knightAttacks(uint64_t knights);
  uint64_t whiteKnights;
  uint64_t blackKnights;

  uint64_t bishopMoves[64];
  void InitBishopMoves();
  uint64_t whiteBishops;
  uint64_t blackBishops;

  uint64_t rookMoves[64];
  void InitRookMoves();
  uint64_t whiteRooks;
  uint64_t blackRooks;

  uint64_t queenMoves[64];
  void InitQueenMoves();
  uint64_t whiteQueens;
  uint64_t blackQueens;

  uint64_t kingMoves[64];
  void InitKingMoves();
  uint64_t whiteKings;
  uint64_t blackKings;



  // Find 1 bit in bitboard
  int bitScanR(uint64_t bitboard);

  // Mvv Lva
  void InitMvvLva();
  int mvvlva[6][6];

  // ---------------------------------------------


  // Moves and Pieces -------------------------------------

  struct MoveStack {
    uint64_t fromLoc;
    uint64_t toLoc;
    int movePiece;
    int capturePiece;
    bool color;
    bool promote;
    bool kingMoved;
    int rookMoved;
    int castled;
    int enpassant;

    MoveStack(uint64_t fromLoc, uint64_t toLoc, int movePiece, int capturePiece, bool color, bool promote, bool kingMoved, int rookMoved, int castled, int enpassant) :
      fromLoc(fromLoc), toLoc(toLoc), movePiece(movePiece), capturePiece(capturePiece), color(color), promote(promote), kingMoved(kingMoved), rookMoved(rookMoved), castled(castled), enpassant(enpassant) {}
  };


  Zobrist zobrist;

  // Pieces bitboards
  uint64_t pieces[6];
  uint64_t whites;
  uint64_t blacks;
  uint64_t occupied;
  int materialScore; // Material score updates incrementally
  uint16_t halfMove; // Half moves (plies)

  // Game flags
  uint8_t enpasssantFlag;
  bool endgameFlag;
  bool whiteToMove;

  // Rook and king flags
  bool kingMovedWhite;
  bool kingMovedBlack;
  bool rookMovedWhiteA;
  bool rookMovedWhiteH;
  bool rookMovedBlackA;
  bool rookMovedBlackH;
  bool whiteCastled;
  bool blackCastled;

  std::vector<MoveStack> moveStack = {}; // MoveStack for undo move
  std::vector<uint64_t> prevPositions = {}; // Holds zobrist key in move/undo stack

  // Count pieces
  int countPawnsW;
  int countPawnsB;

  int countKnightsW;
  int countKnightsB;

  int countBishopsW;
  int countBishopsB;

  int countRooksW;
  int countRooksB;

  int countQueensW;
  int countQueensB;

  // Piece square tables
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
  int whiteKingTableEG[64];
  int blackKingTableEG[64];

  // Initializing masks
  void InitPieceBoards();
  void InitDistanceArray();
  void InitPassedPawnsMask();
  void InitIsolatedPawnsMask();
  void InitColumnsMask();
  void InitRowsMask();
  void InitKingZoneMask();

  uint64_t whitePassedPawnMask[64];
  uint64_t blackPassedPawnMask[64];
  uint64_t isolatedPawnMask[64];
  uint64_t columnMask[8];
  uint64_t rowMask[8];
  uint64_t kingZoneMaskWhite[64];
  uint64_t kingZoneMaskBlack[64];
  uint64_t enemyTerritoryWhite;
  uint64_t enemyTerritoryBlack;

  Magics *magics; // Magic bitboard


  // Castling rights
  bool canCastleQ(bool isWhite);
  bool canCastleK(bool isWhite);

  // ----------------------------------------------


  // Generating moves helpers
  std::vector<int> whitePiecesLoc();
  std::vector<int> blackPiecesLoc();
  std::vector<int> validMovesWhite(int index);
  std::vector<int> validMovesBlack(int index);


  bool isAttacked(int index, bool color); // Is square attacked

  // Evaluation functions
  int evaluateImbalance();
  int evaluateMobility(int kingIndex, uint64_t pawns, uint64_t knights, uint64_t bishops, uint64_t rooks, uint64_t queens, bool endgame, bool isWhite);
  int evaluateKingSafety(int kingIndex, uint64_t color, uint64_t knights, uint64_t bishops, uint64_t rooks, uint64_t queens, bool isWhite);
  int evaluatePawns(uint64_t allyPawns, uint64_t enemyPawns, bool endgame, bool isWhite);
  int evaluateOutposts(uint64_t knights, uint64_t bishops, uint64_t pawns, bool endgame, bool isWhite);
  int evaluateThreats(uint64_t pawns, bool endgame, bool isWhite);



  uint64_t hashBoard(bool turn); // zobrist hashing
  int enpassantConditions(bool isWhite, int pawnLocation); // Can enpassant


  Move killerMoves[2][1024][2];  // Killer Moves
  uint8_t manhattanArray[64][64]; // Distance arrays
  uint8_t chebyshevArray[64][64]; // Distance arrays

  int count_population(uint64_t bitboard); // Count number of 1 bits

  //See
  Move smallestAttacker(int index, bool isWhite);
  int see(int index, bool isWhite);
  void movePieceCapture(Move &move);
  void undoMoveCapture();


};
