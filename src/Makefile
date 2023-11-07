
debug = no
debug_gdb = no
debug_zobrist = no
popcount = no
avx2 = yes
native = no
EXE = ../bob


CXXFLAGS := -Wall -Wextra -std=c++17 -static -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
OBCXXFLAGS := -Wall -Wextra -std=c++17 -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive


ifeq ($(debug), no)
CXXFLAGS += -DNDEBUG
OBCXXFLAGS += -DNDEBUG
endif

ifeq ($(debug_gdb), yes)
CXXFLAGS += -g
OBCXXFLAGS += -g
endif

ifeq ($(popcount), yes)
CXXFLAGS += -msse3 -mpopcnt -DPOPCOUNT
OBCXXFLAGS += -msse3 -mpopcnt -DPOPCOUNT
endif

ifeq ($(avx2), yes)
CXXFLAGS += -mavx2
OBCXXFLAGS += -mavx2
else
CXXFLAGS += -msse2
OBCXXFLAGS += -msse2
endif

ifeq ($(native), yes)
CXXFLAGS += -march=native
OBCXXFLAGS += -march=native
endif

ifeq ($(debug_zobrist), yes)
CXXFLAGS += -DDEBUGHASH
endif

CXX := bitboard.cpp magic_bitboards.cpp defs.cpp dumb7flooding.cpp movegen.cpp perft.cpp eval.cpp search.cpp uci.cpp uci_commands.cpp movepick.cpp zobrist_hashing.cpp transpositionTable.cpp timeman.cpp thread_search.cpp nnue/layer.cpp nnue/KPNNUE.cpp nnue/accumulator.cpp

openbench: $(CXX)
	g++ -O3 -o $(EXE) $(OBCXXFLAGS) main.cpp $(CXX)
engine: $(CXX)
	g++ -O3 -o $(EXE) $(CXXFLAGS) main.cpp $(CXX)
tuner2: $(CXX)
	g++ -O3 -fopenmp -o tuner -DTUNER $(CXXFLAGS) tuner.cpp $(CXX) 
