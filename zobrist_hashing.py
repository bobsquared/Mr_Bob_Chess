import random

# Adapted from https://en.wikipedia.org/wiki/Zobrist_hashing
class Zobrist:
    def __init__(self):
        self.table = dict()
        self.lookUp = dict()

        for i in range(64):
            temp = dict()
            for j in range(12):
                temp[j] = random.getrandbits(64)
            self.table[i] = temp

    def hashBoard(self, movePro, turn):
        pieces = movePro.pieces
        occupied = movePro.occupied
        blacks = movePro.blacks
        table = self.table
        
        ret = turn
        for i in range(64):
            if occupied & (1 << i) != 0:
                j = 0
                if blacks & (1 << i) != 0:
                    j += 6

                if pieces[0] & (1 << i) != 0:
                    j += 0
                elif pieces[1] & (1 << i) != 0:
                    j += 1
                elif pieces[2] & (1 << i) != 0:
                    j += 2
                elif pieces[3] & (1 << i) != 0:
                    j += 3
                elif pieces[4] & (1 << i) != 0:
                    j += 4
                elif pieces[5] & (1 << i) != 0:
                    j += 5

                ret = ret ^ table[i][j]
        return ret

zobrist = Zobrist()
