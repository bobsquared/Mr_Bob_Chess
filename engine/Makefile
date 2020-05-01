
all: main.cpp bitboard.cpp dumb7flooding.cpp zobrist_hashing.cpp config.cpp magic_bitboards.cpp search.cpp transpositionTable.cpp defs.cpp
	g++ -O3 -Wall -Wextra -march=native -std=c++11 -static -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive -o "../Mr Bob" main.cpp search.cpp bitboard.cpp dumb7flooding.cpp zobrist_hashing.cpp config.cpp magic_bitboards.cpp transpositionTable.cpp defs.cpp
