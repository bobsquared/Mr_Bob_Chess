CXXFLAGS := -Wall -Wextra -std=c++11 -static -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
CXXFLAGS += -msse3 -mpopcnt -DPOPCOUNT

all:
	g++ -O3 -o network nnue.cpp main.cpp layer.cpp KPNNUE.cpp
extractor:
	g++ -O3 -march=native -fopenmp $(CXXFLAGS) -o trainer trainer.cpp main.cpp layer.cpp KPNNUE.cpp ../bitboard.cpp ../magic_bitboards.cpp ../defs.cpp ../dumb7flooding.cpp ../movegen.cpp ../perft.cpp ../eval.cpp ../search.cpp ../uci.cpp ../movepick.cpp ../zobrist_hashing.cpp ../transpositionTable.cpp ../timeman.cpp accumulator.cpp
trainer:
	g++ -O3 -march=native -fopenmp $(CXXFLAGS) -o trainer trainer.cpp main.cpp layer.cpp KPNNUE.cpp ../bitboard.cpp ../magic_bitboards.cpp ../defs.cpp ../dumb7flooding.cpp ../movegen.cpp ../perft.cpp ../eval.cpp ../search.cpp ../uci.cpp ../movepick.cpp ../zobrist_hashing.cpp ../transpositionTable.cpp ../timeman.cpp accumulator.cpp
