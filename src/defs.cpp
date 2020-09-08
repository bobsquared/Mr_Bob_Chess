#include "defs.h"

#ifdef _WIN32
#include <intrin.h>
#endif

uint64_t columnMask[64];
uint64_t rowMask[64];
int history[2][64][64];
MOVE killers[128][2];
MOVE counterMove[2][64][64];
int evalStack[128];


// Algebra to number
std::unordered_map<std::string, uint8_t> TO_NUM = {
    {"a1", 0},{"b1", 1},{"c1", 2},{"d1", 3},{"e1", 4},{"f1", 5},{"g1", 6},{"h1", 7},
    {"a2", 8},{"b2", 9},{"c2", 10},{"d2", 11},{"e2", 12},{"f2", 13},{"g2", 14},{"h2", 15},
    {"a3", 16},{"b3", 17},{"c3", 18},{"d3", 19},{"e3", 20},{"f3", 21},{"g3", 22},{"h3", 23},
    {"a4", 24},{"b4", 25},{"c4", 26},{"d4", 27},{"e4", 28},{"f4", 29},{"g4", 30},{"h4", 31},
    {"a5", 32},{"b5", 33},{"c5", 34},{"d5", 35},{"e5", 36},{"f5", 37},{"g5", 38},{"h5", 39},
    {"a6", 40},{"b6", 41},{"c6", 42},{"d6", 43},{"e6", 44},{"f6", 45},{"g6", 46},{"h6", 47},
    {"a7", 48},{"b7", 49},{"c7", 50},{"d7", 51},{"e7", 52},{"f7", 53},{"g7", 54},{"h7", 55},
    {"a8", 56},{"b8", 57},{"c8", 58},{"d8", 59},{"e8", 60},{"f8", 61},{"g8", 62},{"h8", 63}
};



// Get the move from location
int get_move_from(uint16_t move) {
    return (move & FROM_LOC_MOVE) >> 10;
}



// Get the move to location
int get_move_to(uint16_t move) {
    return (move & TO_LOC_MOVE) >> 4;
}



// Scan the least significant bit
int bitScan(const uint64_t bitboard) {

    #if defined(_MSC_VER) || defined(__MINGW32__)
    unsigned long ret;
    _BitScanForward64(&ret, bitboard);
    return (int) ret;

    #elif defined(__GNUC__)
    return __builtin_ctzll(bitboard);
    #endif

    return MSB_TABLE[((bitboard ^ (bitboard - 1)) * 0x03f79d71b4cb0a89) >> 58];
}



// Initialize the columns bitboard
void InitColumnsMask() {

    for (int i = 0; i < 64; i++) {
        columnMask[i] = 1ULL << i;
        columnMask[i] |= columnMask[i] << 8;
        columnMask[i] |= columnMask[i] << 16;
        columnMask[i] |= columnMask[i] << 32;

        columnMask[i] |= columnMask[i] >> 8;
        columnMask[i] |= columnMask[i] >> 16;
        columnMask[i] |= columnMask[i] >> 32;
    }

}



// Initialize the rows bitboard
void InitRowsMask() {

    for (int i = 0; i < 64; i++) {
        rowMask[i] = 1ULL << (((i / 8) % 8) * 8);
        rowMask[i] |= rowMask[i] << 1;
        rowMask[i] |= rowMask[i] << 2;
        rowMask[i] |= rowMask[i] << 4;
    }

}



// Initialize the history
void InitHistory() {

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 64; j++) {
            for (int k = 0; k < 64; k++) {
                history[i][j][k] = 0;
            }
        }
    }

}



// Initialize the history
void InitKillers() {

    for (int i = 0; i < 128; i++) {
        for (int j = 0; j < 2; j++) {
            killers[i][j] = 0;
        }
    }

}



// Initialize the counter moves
void InitCounterMoves() {

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 64; j++) {
            for (int k = 0; k < 64; k++) {
                counterMove[i][j][k] = 0;
            }
        }
    }

}



// All pawn attacks
// Useful for obtaining bitboard for multiple pawn attacks
uint64_t pawnAttacksAll(uint64_t bitboard, bool colorFlag) {

    if (!colorFlag) {
        uint64_t tempBitBoard1 = (bitboard << 9);
        uint64_t tempBitBoard2 = (bitboard << 7);

        tempBitBoard1 &= ~columnMask[0];
        tempBitBoard2 &= ~columnMask[7];
        return tempBitBoard1 | tempBitBoard2;
    }
    else {
        uint64_t tempBitBoard1 = (bitboard >> 9);
        uint64_t tempBitBoard2 = (bitboard >> 7);

        tempBitBoard1 &= ~columnMask[7];
        tempBitBoard2 &= ~columnMask[0];
        return tempBitBoard1 | tempBitBoard2;
    }

}



// All knight attacks
// Useful for obtaining all knight attacks
uint64_t knightAttacks(uint64_t knights) {
    uint64_t l1 = (knights >> 1) & 0x7f7f7f7f7f7f7f7f;
    uint64_t l2 = (knights >> 2) & 0x3f3f3f3f3f3f3f3f;
    uint64_t r1 = (knights << 1) & 0xfefefefefefefefe;
    uint64_t r2 = (knights << 2) & 0xfcfcfcfcfcfcfcfc;
    uint64_t h1 = l1 | r1;
    uint64_t h2 = l2 | r2;
    return (h1 << 16) | (h1 >> 16) | (h2 << 8) | (h2 >> 8);
}



// Counts the number of one bits
int count_population(uint64_t bitboard) {

    #ifdef POPCOUNT

    #if defined(__MINGW32__) || defined(_MSC_VER)
    return _mm_popcnt_u64(bitboard);
    #elif defined(__GNUC__)
    return __builtin_popcountll(bitboard);
    #endif

    #endif

    int count = 0;
    while (bitboard) {
        count++;
        bitboard &= bitboard - 1;
    }

    return count;

}



// Prints the bitboard in 1s and 0s
void printBoard(const uint64_t board) {

    std::bitset<64> x(board);
    for (int i = 7; i >= 0; i--) {
        for (int j = 7; j >= 0; j--) {
            std::cout << x[(7 - j) + 8 * i] << " ";
        }
        std::cout << std::endl;
    }

}
