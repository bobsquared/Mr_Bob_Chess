

debug = yes
debug_zobrist = no


CXXFLAGS := -Wall -Wextra -march=native -std=c++11 -static -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive

ifeq ($(debug), no)
CXXFLAGS += -DNDEBUG
endif

ifeq ($(debug_zobrist), yes)
CXXFLAGS += -DDEBUGHASH
endif

CXX := main.cpp bitboard.cpp magic_bitboards.cpp defs.cpp dumb7flooding.cpp movegen.cpp perft.cpp eval.cpp search.cpp uci.cpp movepick.cpp zobrist_hashing.cpp transpositionTable.cpp

all: $(CXX)
	g++ -O3 $(CXXFLAGS) -o "bob" $(CXX)
