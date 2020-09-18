
debug = no
debug_zobrist = no
popcount = yes
EXE = ../bob


CXXFLAGS := -Wall -Wextra -std=c++11 -static -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive

ifeq ($(debug), no)
CXXFLAGS += -DNDEBUG
endif

ifeq ($(popcount), yes)
CXXFLAGS += -msse3 -mpopcnt -DPOPCOUNT
endif

ifeq ($(debug_zobrist), yes)
CXXFLAGS += -DDEBUGHASH
endif

CXX := bitboard.cpp magic_bitboards.cpp defs.cpp dumb7flooding.cpp movegen.cpp perft.cpp eval.cpp search.cpp uci.cpp movepick.cpp zobrist_hashing.cpp transpositionTable.cpp timeman.cpp


engine: $(CXX)
	g++ -O3 -o $(EXE) $(CXXFLAGS) main.cpp $(CXX)

tuner: $(CXX)
	g++ -O3 -o tuner $(CXXFLAGS) tuner.cpp $(CXX)
