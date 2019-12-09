import time
import numpy as np

ALL_ONES = 18446744073709551615
ATTACKS_BOARD = dict()


class MagicPro:
    def __init__(self, bitboard, shift, magic, mask):
        self.board = Board()
        self.bitboard = bitboard
        self.shift = shift
        self.magic = magic
        self.mask = mask

    def modifyMagic(self, index, value):
        self.magic[index] = value

    def modifyShift(self, index, value):
        self.shift[index] = value

    def modifyBitboard(self, index, value):
        self.bitboard[index] = value




attacksR = dict()
attacksB = dict()

class Magic:
    def __init__(self):
        self.board = Board()
        self.magicR = dict()
        self.magicB = dict()
        self.shiftR = dict()
        self.shiftB = dict()
        self.mask = flipBits(0) & self.board.colZeros[0] & self.board.colZeros[7]
        self.mask = unSetCellState1D(self.mask, 1)
        self.mask = unSetCellState1D(self.mask, 2)
        self.mask = unSetCellState1D(self.mask, 3)
        self.mask = unSetCellState1D(self.mask, 4)
        self.mask = unSetCellState1D(self.mask, 5)
        self.mask = unSetCellState1D(self.mask, 6)

        self.mask = unSetCellState1D(self.mask, 62)
        self.mask = unSetCellState1D(self.mask, 61)
        self.mask = unSetCellState1D(self.mask, 60)
        self.mask = unSetCellState1D(self.mask, 59)
        self.mask = unSetCellState1D(self.mask, 58)
        self.mask = unSetCellState1D(self.mask, 57)



        self.optimalMagicRook()
        self.optimalMagicBishop()

        for i in range(64):
            mrMasked = self.mask & self.board.rookMoves[i]
            count = count_population(mrMasked)
            mrShift = 64 - count
            mrMagic = self.magicR[i]
            attacksR[i] = MagicPro(self.board.rookMoves[i], mrShift, mrMagic, mrMasked)

        for i in range(64):
            mrMasked = self.mask & self.board.bishopMoves[i]
            count = count_population(mrMasked)
            mrShift = 64 - count
            mrMagic = self.magicB[i]
            attacksB[i] = MagicPro(self.board.bishopMoves[i], mrShift, mrMagic, mrMasked)

        print(attacksB[23].magic)


    def optimalMagicRook(self):
        self.magicR[0] = 0x2080020500400f0
        self.magicR[1] = 0x28444000400010
        self.magicR[2] = 0x20000a1004100014
        self.magicR[3] = 0x20010c090202006
        self.magicR[4] = 0x8408008200810004
        self.magicR[5] = 0x1746000808002
        self.magicR[6] = 0x2200098000808201
        self.magicR[7] = 0x12c0002080200041
        self.magicR[8] = 0x104000208e480804
        self.magicR[9] = 0x8084014008281008
        self.magicR[10] = 0x4200810910500410
        self.magicR[11] = 0x100014481c20400c
        self.magicR[12] = 0x4014a4040020808
        self.magicR[13] = 0x401002001010a4
        self.magicR[14] = 0x202000500010001
        self.magicR[15] = 0x8112808005810081
        self.magicR[16] = 0x40902108802020
        self.magicR[17] = 0x42002101008101
        self.magicR[18] = 0x459442200810c202
        self.magicR[19] = 0x81001103309808
        self.magicR[20] = 0x8110000080102
        self.magicR[21] = 0x8812806008080404
        self.magicR[22] = 0x104020000800101
        self.magicR[23] = 0x40a1048000028201
        self.magicR[24] = 0x4100ba0000004081
        self.magicR[25] = 0x44803a4003400109
        self.magicR[26] = 0xa010a00000030443
        self.magicR[27] = 0x91021a000100409
        self.magicR[28] = 0x4201e8040880a012
        self.magicR[29] = 0x22a000440201802
        self.magicR[30] = 0x30890a72000204
        self.magicR[31] = 0x10411402a0c482
        self.magicR[32] = 0x40004841102088
        self.magicR[33] = 0x40230000100040
        self.magicR[34] = 0x40100010000a0488
        self.magicR[35] = 0x1410100200050844
        self.magicR[36] = 0x100090808508411
        self.magicR[37] = 0x1410040024001142
        self.magicR[38] = 0x8840018001214002
        self.magicR[39] = 0x410201000098001
        self.magicR[40] = 0x8400802120088848
        self.magicR[41] = 0x2060080000021004
        self.magicR[42] = 0x82101002000d0022
        self.magicR[43] = 0x1001101001008241
        self.magicR[44] = 0x9040411808040102
        self.magicR[45] = 0x600800480009042
        self.magicR[46] = 0x1a020000040205
        self.magicR[47] = 0x4200404040505199
        self.magicR[48] = 0x2020081040080080
        self.magicR[49] = 0x40a3002000544108
        self.magicR[50] = 0x4501100800148402
        self.magicR[51] = 0x81440280100224
        self.magicR[52] = 0x88008000000804
        self.magicR[53] = 0x8084060000002812
        self.magicR[54] = 0x1840201000108312
        self.magicR[55] = 0x5080202000000141
        self.magicR[56] = 0x1042a180880281
        self.magicR[57] = 0x900802900c01040
        self.magicR[58] = 0x8205104104120
        self.magicR[59] = 0x9004220000440a
        self.magicR[60] = 0x8029510200708
        self.magicR[61] = 0x8008440100404241
        self.magicR[62] = 0x2420001111000bd
        self.magicR[63] = 0x4000882304000041

    def optimalMagicBishop(self):
        self.magicB[0] = 0x100420000431024
        self.magicB[1] = 0x280800101073404
        self.magicB[2] = 0x42000a00840802
        self.magicB[3] = 0xca800c0410c2
        self.magicB[4] = 0x81004290941c20
        self.magicB[5] = 0x400200450020250
        self.magicB[6] = 0x444a019204022084
        self.magicB[7] = 0x88610802202109a
        self.magicB[8] = 0x11210a0800086008
        self.magicB[9] = 0x400a08c08802801
        self.magicB[10] = 0x1301a0500111c808
        self.magicB[11] = 0x1280100480180404
        self.magicB[12] = 0x720009020028445
        self.magicB[13] = 0x91880a9000010a01
        self.magicB[14] = 0x31200940150802b2
        self.magicB[15] = 0x5119080c20000602
        self.magicB[16] = 0x242400a002448023
        self.magicB[17] = 0x4819006001200008
        self.magicB[18] = 0x222c10400020090
        self.magicB[19] = 0x302008420409004
        self.magicB[20] = 0x504200070009045
        self.magicB[21] = 0x210071240c02046
        self.magicB[22] = 0x1182219000022611
        self.magicB[23] = 0x400c50000005801
        self.magicB[24] = 0x4004010000113100
        self.magicB[25] = 0x2008121604819400
        self.magicB[26] = 0xc4a4010000290101
        self.magicB[27] = 0x404a000888004802
        self.magicB[28] = 0x8820c004105010
        self.magicB[29] = 0x28280100908300
        self.magicB[30] = 0x4c013189c0320a80
        self.magicB[31] = 0x42008080042080
        self.magicB[32] = 0x90803000c080840
        self.magicB[33] = 0x2180001028220
        self.magicB[34] = 0x1084002a040036
        self.magicB[35] = 0x212009200401
        self.magicB[36] = 0x128110040c84a84
        self.magicB[37] = 0x81488020022802
        self.magicB[38] = 0x8c0014100181
        self.magicB[39] = 0x2222013020082
        self.magicB[40] = 0xa00100002382c03
        self.magicB[41] = 0x1000280001005c02
        self.magicB[42] = 0x84801010000114c
        self.magicB[43] = 0x480410048000084
        self.magicB[44] = 0x21204420080020a
        self.magicB[45] = 0x2020010000424a10
        self.magicB[46] = 0x240041021d500141
        self.magicB[47] = 0x420844000280214
        self.magicB[48] = 0x29084a280042108
        self.magicB[49] = 0x84102a8080a20a49
        self.magicB[50] = 0x104204908010212
        self.magicB[51] = 0x40a20280081860c1
        self.magicB[52] = 0x3044000200121004
        self.magicB[53] = 0x1001008807081122
        self.magicB[54] = 0x50066c000210811
        self.magicB[55] = 0xe3001240f8a106
        self.magicB[56] = 0x940c0204030020d4
        self.magicB[57] = 0x619204000210826a
        self.magicB[58] = 0x2010438002b00a2
        self.magicB[59] = 0x884042004005802
        self.magicB[60] = 0xa90240000006404
        self.magicB[61] = 0x500d082244010008
        self.magicB[62] = 0x28190d00040014e0
        self.magicB[63] = 0x825201600c082444



class Board:
    def __init__(self):
        self.board = 0
        self.colZeros = dict()

        # Pawn moves
        self.blackPawnMoves = dict()
        self.whitePawnMoves = dict()

        # Pawn moves
        self.blackPawnAttacks = dict()
        self.whitePawnAttacks = dict()

        # Pawn occupations
        self.blackPawns = 0
        self.whitePawns = 0

        # Knight moves
        self.knightMoves = dict()

        # Knight occupations
        self.whiteKnights = 0
        self.blackKnights = 0

        # Bishop moves
        self.bishopMoves = dict()

        # Bishop occupations
        self.whiteBishops = 0
        self.blackBishops = 0

        # Bishop moves
        self.rookMoves = dict()

        # Bishop occupations
        self.whiteRooks = 0
        self.blackRooks = 0

        # Bishop moves
        self.queenMoves = dict()

        # Bishop occupations
        self.whiteQueens = 0
        self.blackQueens = 0

        # Bishop moves
        self.kingMoves = dict()

        # Bishop occupations
        self.whiteKings = 0
        self.blackKings = 0

        # Mask Columns
        for i in range(8):
            tempBitSet = ALL_ONES
            tempBitSet = unSetCellState1D(tempBitSet, 0 + i)
            tempBitSet = unSetCellState1D(tempBitSet, 8 + i)
            tempBitSet = unSetCellState1D(tempBitSet, 16 + i)
            tempBitSet = unSetCellState1D(tempBitSet, 24 + i)
            tempBitSet = unSetCellState1D(tempBitSet, 32 + i)
            tempBitSet = unSetCellState1D(tempBitSet, 40 + i)
            tempBitSet = unSetCellState1D(tempBitSet, 48 + i)
            tempBitSet = unSetCellState1D(tempBitSet, 56 + i)
            self.colZeros[i] = tempBitSet

        #Black pawn moves
        for i in range(64):
            tempBitBoard = 0
            tempBitBoard = setCellState1D(tempBitBoard, i)
            tempBitBoard |= tempBitBoard >> 8
            if i < 56 and i > 47:
                tempBitBoard |= tempBitBoard >> 8
            tempBitBoard = unSetCellState1D(tempBitBoard, i)
            self.blackPawnMoves[i] = tempBitBoard

        #White pawn moves
        for i in range(64):
            tempBitBoard = 0
            tempBitBoard = setCellState1D(tempBitBoard, i)
            tempBitBoard |= ((tempBitBoard << 8) & ALL_ONES)
            if i < 16 and i > 7:
                tempBitBoard |= ((tempBitBoard << 8) & ALL_ONES)
            tempBitBoard = unSetCellState1D(tempBitBoard, i)
            self.whitePawnMoves[i] = tempBitBoard

        #Black pawn attacks
        for i in range(64):
            tempBitBoard = 0
            tempBitBoard = setCellState1D(tempBitBoard, i)
            self.blackPawnAttacks[i] = self.pawnAttacksMask(False, tempBitBoard)

        #White pawn attacks
        for i in range(64):
            tempBitBoard = 0
            tempBitBoard = setCellState1D(tempBitBoard, i)
            self.whitePawnAttacks[i] = self.pawnAttacksMask(True, tempBitBoard)

        #Initialize white pawns
        self.whitePawns = setCellState1D(self.whitePawns, 8)
        self.whitePawns = setCellState1D(self.whitePawns, 9)
        self.whitePawns = setCellState1D(self.whitePawns, 10)
        self.whitePawns = setCellState1D(self.whitePawns, 11)
        self.whitePawns = setCellState1D(self.whitePawns, 12)
        self.whitePawns = setCellState1D(self.whitePawns, 13)
        self.whitePawns = setCellState1D(self.whitePawns, 14)
        self.whitePawns = setCellState1D(self.whitePawns, 15)

        #Initialize black pawns
        self.blackPawns = setCellState1D(self.blackPawns, 48)
        self.blackPawns = setCellState1D(self.blackPawns, 49)
        self.blackPawns = setCellState1D(self.blackPawns, 50)
        self.blackPawns = setCellState1D(self.blackPawns, 51)
        self.blackPawns = setCellState1D(self.blackPawns, 52)
        self.blackPawns = setCellState1D(self.blackPawns, 53)
        self.blackPawns = setCellState1D(self.blackPawns, 54)
        self.blackPawns = setCellState1D(self.blackPawns, 55)

        # Knight moves
        for i in range(64):
            tempBitBoard = 0
            tempBitBoard1 = 0
            tempBitBoard2 = 0
            tempBitBoard3 = 0
            tempBitBoard4 = 0

            tempBitBoard = setCellState1D(tempBitBoard, i)
            tempBitBoard1 |= ((tempBitBoard << 15) & ALL_ONES)
            tempBitBoard1 |= tempBitBoard >> 17
            tempBitBoard1 &= self.colZeros[7]

            tempBitBoard2 |= ((tempBitBoard << 6) & ALL_ONES)
            tempBitBoard2 |= tempBitBoard >> 10
            tempBitBoard2 &= self.colZeros[6]
            tempBitBoard2 &= self.colZeros[7]

            tempBitBoard3 |= ((tempBitBoard << 10) & ALL_ONES)
            tempBitBoard3 |= tempBitBoard >> 6
            tempBitBoard3 &= self.colZeros[0]
            tempBitBoard3 &= self.colZeros[1]

            tempBitBoard4 |= ((tempBitBoard << 17) & ALL_ONES)
            tempBitBoard4 |= tempBitBoard >> 15
            tempBitBoard4 &= self.colZeros[0]

            self.knightMoves[i] = tempBitBoard1 | tempBitBoard2 | tempBitBoard3 | tempBitBoard4


        # Initialize white knights
        self.whiteKnights = setCellState1D(self.whiteKnights, 1)
        self.whiteKnights = setCellState1D(self.whiteKnights, 6)

        # Initialize black knights
        self.blackKnights = setCellState1D(self.blackKnights, 62)
        self.blackKnights = setCellState1D(self.blackKnights, 57)

        # Bishop moves
        for i in range(64):
            tempBitBoard = 0
            tempBitBoard1 = 0
            tempBitBoard2 = 0
            tempBitBoard3 = 0
            tempBitBoard4 = 0

            tempBitBoard = setCellState1D(tempBitBoard, i)
            col = i % 8
            for k in range(col, 8):
                if k == 7:
                    break
                else:
                    tempBitBoard1 |= tempBitBoard << (9 + 9 * (k - col))

            for k in range(col, 8):
                if k == 7:
                    break
                else:
                    tempBitBoard2 |= tempBitBoard >> (7 + 7 * (k - col))

            for k in range(col, -1, -1):
                if k == 0:
                    break
                else:
                    tempBitBoard3 |= tempBitBoard << 7 + 7 * (col - k)

            for k in range(col, -1, -1):
                if k == 0:
                    break
                else:
                    tempBitBoard4 |= tempBitBoard >> (9 + 9 * (col - k))

            self.bishopMoves[i] = ((tempBitBoard1 & ALL_ONES) | tempBitBoard2 | tempBitBoard4 | (tempBitBoard3 & ALL_ONES))

        # Initialize White bishops
        self.whiteBishops = setCellState1D(self.whiteBishops, 2)
        self.whiteBishops = setCellState1D(self.whiteBishops, 5)

        # Initialize black bishops
        self.blackBishops = setCellState1D(self.blackBishops, 61)
        self.blackBishops = setCellState1D(self.blackBishops, 58)

        # Rook moves
        for i in range(64):
            tempBitBoard = 0
            tempBitBoard1 = 0
            tempBitBoard2 = 0
            tempBitBoard3 = 0
            tempBitBoard4 = 0

            tempBitBoard = setCellState1D(tempBitBoard, i)
            col = i % 8

            for k in range(8):
                tempBitBoard1 |= tempBitBoard >> (8 + k * 8)
                tempBitBoard2 |= ((tempBitBoard << (8 + k * 8)) & ALL_ONES)

            for k in range(col, 8):
                if k == 7:
                    break
                else:
                    tempBitBoard3 |= ((tempBitBoard << (1 + 1 * (k - col))) & ALL_ONES)

            for k in range(col, -1, -1):
                if k == 0:
                    break
                else:
                    tempBitBoard4 |= tempBitBoard >> (1 + 1 * (col - k))

            self.rookMoves[i] = (tempBitBoard1 | tempBitBoard2 | tempBitBoard4 | tempBitBoard3)

        # Initialize white rooks
        self.whiteRooks = setCellState1D(self.whiteRooks, 0)
        self.whiteRooks = setCellState1D(self.whiteRooks, 7)

        # Initialize white rooks
        self.blackRooks = setCellState1D(self.blackRooks, 63)
        self.blackRooks = setCellState1D(self.blackRooks, 56)

        # Queen moves
        for i in range(64):
            self.queenMoves[i] = self.rookMoves[i] | self.bishopMoves[i]

        # Initialize white queens
        self.whiteQueens = setCellState1D(self.whiteQueens, 3)

        # Initialize black queens
        self.blackQueens = setCellState1D(self.blackQueens, 59)

        # King moves
        for i in range(64):
            tempBitBoard = 0
            tempBitBoard1 = 0
            tempBitBoard2 = 0

            tempBitBoard = setCellState1D(tempBitBoard, i)
            col = i % 8

            tempBitBoard |= tempBitBoard >> 8
            tempBitBoard |= ((tempBitBoard << 8) & ALL_ONES)

            tempBitBoard1 |= tempBitBoard >> 1
            tempBitBoard1 &= self.colZeros[7]

            tempBitBoard2 |= ((tempBitBoard << 1 ) & ALL_ONES)
            tempBitBoard2 &= self.colZeros[0]

            tempBitBoard |= tempBitBoard1 | tempBitBoard2
            tempBitBoard = unSetCellState1D(tempBitBoard, i)

            self.kingMoves[i] = tempBitBoard

        # Initialize white kings
        self.whiteKings = setCellState1D(self.whiteKings, 4)

        # Initialize black kings
        self.blackKings = setCellState1D(self.blackKings, 60)



    def pawnAttacksMask(self, isWhite, bitboard):
        if isWhite:
            tempBitBoard1 = ((bitboard << 9) & ALL_ONES)
            tempBitBoard2 = ((bitboard << 7) & ALL_ONES)

            tempBitBoard1 &= self.colZeros[0]
            tempBitBoard2 &= self.colZeros[7]
            return tempBitBoard1 | tempBitBoard2
        else:
            tempBitBoard1 = bitboard >> 9
            tempBitBoard2 = bitboard >> 7

            tempBitBoard1 &= self.colZeros[7]
            tempBitBoard2 &= self.colZeros[0]
            return tempBitBoard1 | tempBitBoard2

    def bishopAttacksMask(self, occupations, index):
        board = attacksB[index].bitboard
        occupations &= attacksB[index].mask
        occupations *= attacksB[index].magic
        occupations >>= attacksB[index].shift

        return board[occupations]




def setCellState(bitboard, col, row):
    x = 1 << (row * 8 + col)
    return x | bitboard

def setCellState1D(bitboard, index):
    x = 1 << index
    return x | bitboard

def unSetCellState1D(bitboard, index):
    x = 1 << index
    return (~x) & bitboard

def flipBits(bitboard):
    return bitboard ^ ALL_ONES

def printBoard(board):
    x = '{:064b}'.format(board)
    for i in range(8):
        for j in range(8):
            print(x[(7 - j) + 8 * i], end = " ")
        print()

def count_population(bitboard):
    count = 0
    while (bitboard != 0):
        count += 1
        bitboard &= (bitboard - 1)
    return count


start = time.time()* 1000000
x = Board()
m = Magic()
h = setCellState1D(x.board, 28)
h = unSetCellState1D(h, 48)
h = 0
printBoard(x.bishopAttacksMask(13, 5))

end = time.time() * 1000000
print((end - start))
