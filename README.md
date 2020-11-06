
<img src="logo.png" width="480">


# Mr Bob - Chess Engine
A Chess engine made using C++.

**Engine:**

Universal Chess Interface (UCI)
You can call the makefile in the src folder to create the binary.
You can then use the binary in any UCI GUI to use it.

**Acknowledgements:**

This engine wouldn't be where it is now without [Chess Programming Wiki](https://www.chessprogramming.org/Main_Page), where I have spent many hours
looking at the documentation. I have also looked at various programming topics from [talkchess forums](http://talkchess.com/forum3/viewforum.php?f=7)
which has helped tremendously. Special thanks to the [Koivisto team](https://github.com/Luecx/Koivisto) team who are kind enough to give many advice for this engine, and Casey Chou for the logo.
And thanks to many open source engines where I've gained inspiration from, most notably [Ethereal](https://github.com/AndyGrant/Ethereal) and [Stockfish](https://github.com/official-stockfish/Stockfish).


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
