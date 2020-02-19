# Chess-Program
A Chess program made using Python 3 and pygame using FIDE rules

**GUI:**

The GUI runs the latest version of Mr Bob from the binary in the same folder. To run the GUI, pygame must be installed with python 3 or newer.
Then run 

`python3 main.py`


**Engine:**

You can call the makefile in the engine folder to create the binary.
You can then use the binary in any UCI GUI to use it.

**Techniques Used in Engine:**

- Null Move Pruning.
- Late moves reduction.
- Transposition table (default 2GB memory allocated).
- Most valuable victim, least valuable attacker (Mvv Lva) move ordering.
- Principal variation search

- Bitboards
- Magic bitboards for sliding pieces
