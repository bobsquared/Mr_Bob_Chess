
debug = no
debug_zobrist = no
popcount = yes
native = no
EXE = ../bob


CXXFLAGS := -Wall -Wextra -std=c++11 -static -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
OBCXXFLAGS := -Wall -Wextra -std=c++11 -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive


ifeq ($(debug), no)
CXXFLAGS += -DNDEBUG
OBCXXFLAGS += -DNDEBUG
endif

ifeq ($(popcount), yes)
CXXFLAGS += -msse3 -mpopcnt -DPOPCOUNT
OBCXXFLAGS += -msse3 -mpopcnt -DPOPCOUNT
endif

ifeq ($(native), yes)
CXXFLAGS += -march=native
OBCXXFLAGS += -march=native
endif

ifeq ($(debug_zobrist), yes)
CXXFLAGS += -DDEBUGHASH
endif

CXX := bitboard.cpp magic_bitboards.cpp defs.cpp dumb7flooding.cpp movegen.cpp perft.cpp eval.cpp search.cpp uci.cpp movepick.cpp zobrist_hashing.cpp transpositionTable.cpp timeman.cpp

openbench: $(CXX)
	g++ -O3 -o $(EXE) $(OBCXXFLAGS) main.cpp $(CXX)
engine: $(CXX)
	g++ -O3 -o $(EXE) $(CXXFLAGS) main.cpp $(CXX)
tuner: $(CXX)
	g++ -O3 -o tuner -DTUNER $(CXXFLAGS) tuner.cpp $(CXX)
