

debug = no
debug_zobrist = no
tuner = no


CXXFLAGS := -Wall -Wextra -std=c++11 -static -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive

ifeq ($(debug), no)
CXXFLAGS += -DNDEBUG
endif

ifeq ($(debug_zobrist), yes)
CXXFLAGS += -DDEBUGHASH
endif


ifeq ($(tuner), yes)
CXXFLAGS += -o "tuner"
CXX := tuner.cpp bitboard.cpp magic_bitboards.cpp defs.cpp dumb7flooding.cpp movegen.cpp perft.cpp eval.cpp search.cpp uci.cpp movepick.cpp zobrist_hashing.cpp transpositionTable.cpp timeman.cpp
else
CXXFLAGS += -o "bob"
CXX := main.cpp bitboard.cpp magic_bitboards.cpp defs.cpp dumb7flooding.cpp movegen.cpp perft.cpp eval.cpp search.cpp uci.cpp movepick.cpp zobrist_hashing.cpp transpositionTable.cpp timeman.cpp
endif

all: $(CXX)
	g++ -O3 $(CXXFLAGS) $(CXX)
