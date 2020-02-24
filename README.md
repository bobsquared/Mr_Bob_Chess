# Chess-Program
A Chess GUI made using Python 3 and pygame using FIDE rules.

A Chess engine made using C++.

**GUI:**

The GUI runs the latest version of Mr Bob from the binary in the same folder. To run the GUI, pygame must be installed with python 3 or newer.
Then run 

`python3 main.py`


**Engine:**

Universal Chess Interface (UCI)
You can call the makefile in the engine folder to create the binary.
You can then use the binary in any UCI GUI to use it.

**Techniques Used in Engine:**

*Move Ordering:*

1. Principal variation move
2. Most valuable victim, least valuable attacker (Mvv Lva) move ordering.
3. Killer moves heurstic.
4. History Heuristic.

*Pruning and Reductions:*

- Null Move Pruning.
- Late moves reduction.

*Pure speed improvements:*

- Transposition table (default 2GB memory allocated).
- Iterative deepening
- Principal variation search.
- Bitboards.
- Magic bitboards for sliding pieces.

*Accuracy:*

- Quiescence search (Mitigates the horizon effect).

**TODO:**

- More pruning techniques such as futility pruning.
- Effectively implement internal iterative deepening
- Lazy evaluation.
- Many more.
