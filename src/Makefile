
debug = no
debug_gdb = no
debug_zobrist = no
popcount = yes
avx2 = yes
native = yes
EXE = ../bob


CXXFLAGS := -Wall -Wextra -std=c++20 -static -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
OBCXXFLAGS := -Wall -Wextra -std=c++20 -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive


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
CXXFLAGS += -DUSE_AVX2
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

CXX := syzygy/fathom/src/tbprobe.c nnue/layers/concatenate.cpp board/syzygy_probe.cpp board/detections.cpp board/debug.cpp board/see.cpp board/fen.cpp board/move.cpp board/bitboard.cpp board/magic_bitboards.cpp defs.cpp board/movegen.cpp board/legality.cpp perft.cpp eval.cpp search.cpp uci.cpp uci_commands.cpp board/movepick.cpp transpositionTable.cpp timeman.cpp thread_search.cpp

openbench: $(CXX)
	g++ -O3 -o $(EXE) $(OBCXXFLAGS) main.cpp $(CXX)
engine: $(CXX)
	g++ -O3 -o $(EXE) $(CXXFLAGS) main.cpp $(CXX)
tuner2: $(CXX)
	g++ -O3 -fopenmp -o tuner -DTUNER $(CXXFLAGS) tuner.cpp $(CXX) 
