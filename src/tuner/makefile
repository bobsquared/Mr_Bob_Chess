CXXFLAGS := -Wall -Wextra -std=c++11 -static -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
CXXFLAGS += -msse3 -mpopcnt -DPOPCOUNT


all:
	g++ -O3 -DTUNER $(CXXFLAGS) -o tuner tuner.cpp main.cpp ../nnue/KPNNUE.cpp ../nnue/layer.cpp ../bitboard.cpp ../magic_bitboards.cpp ../defs.cpp ../dumb7flooding.cpp ../movegen.cpp ../perft.cpp ../eval.cpp ../search.cpp ../uci.cpp ../movepick.cpp ../zobrist_hashing.cpp ../transpositionTable.cpp ../timeman.cpp
