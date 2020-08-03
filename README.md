# Mr Bob - Chess Engine
A Chess engine made using C++.

**Engine:**

Universal Chess Interface (UCI)
You can call the makefile in the src folder to create the binary.
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

- Transposition table (default 256MB memory allocated).
- Iterative deepening
- Principal variation search.
- Bitboards.
- Magic bitboards for sliding pieces.

*Accuracy:*

- Quiescence search (Mitigates the horizon effect).
