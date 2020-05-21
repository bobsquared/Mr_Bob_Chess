

debug = no


CXXFLAGS := -Wall -Wextra -march=native -std=c++11 -static -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive

ifeq ($(debug), no)
CXXFLAGS += -DDEBUGFLAG
endif

CXX := main.cpp bitboard.cpp dumb7flooding.cpp zobrist_hashing.cpp config.cpp magic_bitboards.cpp search.cpp transpositionTable.cpp defs.cpp

all: $(CXX)
	g++ -O3 $(CXXFLAGS) -o "../Mr Bob" $(CXX)
