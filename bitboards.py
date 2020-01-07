import time
import numpy as np
import math
import copy
import itertools

ALL_ONES = 18446744073709551615
INNER_MASK = 35604928818740736
RIGHT_MASK = 9187201950435737471
LEFT_MASK = 18374403900871474942
DOWN_MASK = 18446744073709551360
UP_MASK = 72057594037927935

LSB_TABLE = [0, 47,  1, 56, 48, 27,  2, 60,
   57, 49, 41, 37, 28, 16,  3, 61,
   54, 58, 35, 52, 50, 42, 21, 44,
   38, 32, 29, 23, 17, 11,  4, 62,
   46, 55, 26, 59, 40, 36, 15, 53,
   34, 51, 20, 43, 31, 22, 10, 45,
   25, 39, 14, 33, 19, 30,  9, 24,
   13, 18,  8, 12,  7,  6,  5, 63]


ATTACKS_BOARD = dict()

rookBlockBoard = dict()
bishopBlockBoard = dict()

attacksR = dict()
attacksB = dict()


class Move:
    def __init__(self, index1, index2, pieceT, capture, color):
        self.fromLoc = index1
        self.toLoc = index2
        self.pieceT = pieceT
        self.capture = capture
        self.color = color



# Stores in dictionary instead. to get white pawns, or any color pieces, just use and operations
class MovePro:
    def __init__(self, whites, blacks, occupied):
        self.pieces = dict()
        self.whites = whites
        self.blacks = blacks
        self.occupied = occupied
        self.values = [1, 3 ,3 ,5, 9, 200]

    def setWhites(self, white):
        self.whites = white

    def setBlacks(self, black):
        self.blacks = black

    def setPieces(self, index, bitboard):
        self.pieces[index] = bitboard

    def setOccupied(self, index):
        self.occupied ^= index

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


class Magic:
    def __init__(self):
        self.board = Board()
        self.magicR = dict()
        self.magicB = dict()
        self.shiftR = dict()
        self.shiftB = dict()



        self.optimalMagicRook()
        self.optimalMagicBishop()

        for i in range(64):
            mrMasked = 0
            if i == 0:
                mrMasked = UP_MASK & RIGHT_MASK
            elif i == 7:
                mrMasked = UP_MASK & LEFT_MASK
            elif i == 63:
                mrMasked = DOWN_MASK & LEFT_MASK
            elif i == 56:
                mrMasked = DOWN_MASK & RIGHT_MASK
            elif i < 7 and i > 0:
                mrMasked = UP_MASK & LEFT_MASK & RIGHT_MASK
            elif i < 63 and i > 56:
                mrMasked = DOWN_MASK & LEFT_MASK & RIGHT_MASK
            elif i == 8 or i == 16 or i == 24 or i == 32 or i == 40 or i == 48:
                mrMasked = UP_MASK & RIGHT_MASK & DOWN_MASK
            elif i == 15 or i == 23 or i == 31 or i == 39 or i == 47 or i == 55:
                mrMasked = UP_MASK & LEFT_MASK & DOWN_MASK
            else:
                mrMasked = INNER_MASK

            mrMasked &= self.board.rookMoves[i]
            count = count_population(mrMasked)
            mrShift = 64 - count
            mrMagic = self.magicR[i]
            attacksR[i] = MagicPro(self.board.rookMoves[i], mrShift, mrMagic, mrMasked)

        for i in range(64):
            mrMasked = INNER_MASK & self.board.bishopMoves[i]
            count = count_population(mrMasked)
            mrShift = 64 - count
            mrMagic = self.magicB[i]
            attacksB[i] = MagicPro(self.board.bishopMoves[i], mrShift, mrMagic, mrMasked)



    def optimalMagicRook(self):
        self.magicR[0] = 0x2080020500400f0
        self.magicR[1] = 0x28444000400010
        self.magicR[2] = 0x20000a1004100014
        self.magicR[3] = 0x20010c090202006
        self.magicR[4] = 0x8408008200810004
        self.magicR[5] = 0x1746000808002468 # added 468 which fixes a bug
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

        self.LSB_TABLE = dict()

        for i in range(len(LSB_TABLE)):
            self.LSB_TABLE[i] = LSB_TABLE[i]

        self.board = 0
        self.colZeros = dict()
        self.rowZeros = dict()
        # self.moveStack = np.array([], dtype=np.object)
        self.moveStack = []

        # Do this for now
        self.undoIndex1 = -1
        self.undoIndex2 = -1
        self.prevPiece1 = -1
        self.prevPiece2 = -1

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

        # Mask Rows
        for i in range(8):
            tempBitSet = ALL_ONES
            tempBitSet = unSetCellState1D(tempBitSet, 0 + i * 8)
            tempBitSet = unSetCellState1D(tempBitSet, 1 + i * 8)
            tempBitSet = unSetCellState1D(tempBitSet, 2 + i * 8)
            tempBitSet = unSetCellState1D(tempBitSet, 3 + i * 8)
            tempBitSet = unSetCellState1D(tempBitSet, 4 + i * 8)
            tempBitSet = unSetCellState1D(tempBitSet, 5 + i * 8)
            tempBitSet = unSetCellState1D(tempBitSet, 6 + i * 8)
            tempBitSet = unSetCellState1D(tempBitSet, 7 + i * 8)
            self.rowZeros[i] = tempBitSet

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

        occupationWhite = self.whitePawns | self.whiteKnights | self.whiteBishops | self.whiteRooks | self.whiteQueens | self.whiteKings
        occupationBlack = self.blackPawns | self.blackKnights | self.blackBishops | self.blackRooks | self.blackQueens | self.blackKings
        self.movePro = MovePro(occupationWhite, occupationBlack, occupationWhite | occupationBlack)
        self.movePro.setPieces(0, self.whitePawns | self.blackPawns)
        self.movePro.setPieces(1, self.whiteKnights | self.blackKnights)
        self.movePro.setPieces(2, self.whiteBishops | self.blackBishops)
        self.movePro.setPieces(3, self.whiteRooks | self.blackRooks)
        self.movePro.setPieces(4, self.whiteQueens | self.blackQueens)
        self.movePro.setPieces(5, self.whiteKings | self.blackKings)

        # Material scoring
        self.materialScore = 0
        for i in range(6):
            self.materialScore -= count_population(self.movePro.pieces[i] & self.movePro.blacks) * self.movePro.values[i]
            self.materialScore += count_population(self.movePro.pieces[i] & self.movePro.whites) * self.movePro.values[i]

        self.populationWhites =  count_population(self.movePro.whites)
        self.populationBlacks =  count_population(self.movePro.blacks)

        self.populationPieces = dict()
        self.populationPieces[0] = count_population(self.whitePawns)
        self.populationPieces[1] = count_population(self.whiteKnights)
        self.populationPieces[2] = count_population(self.whiteBishops)
        self.populationPieces[3] = count_population(self.whiteRooks)
        self.populationPieces[4] = count_population(self.whiteQueens)
        self.populationPieces[5] = count_population(self.whiteKings)
        self.populationPieces[6] = count_population(self.blackPawns)
        self.populationPieces[7] = count_population(self.blackKnights)
        self.populationPieces[8] = count_population(self.blackBishops)
        self.populationPieces[9] = count_population(self.blackRooks)
        self.populationPieces[10] = count_population(self.blackQueens)
        self.populationPieces[11] = count_population(self.blackKings)


        self.kingMovedWhite = False
        self.kingMovedBlack = False

        self.rookMovedWhiteH = False
        self.rookMovedWhiteA = False
        self.rookMovedBlackH = False
        self.rookMovedBlackA = False

        self.castled = 0
        self.castleValue = 0.5



    def __copy__(self):
        newone = type(self)()
        newone.__dict__.update(self.__dict__)
        return newone

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
        board = bishopBlockBoard[index]
        occupations &= attacksB[index].mask
        occupations *= attacksB[index].magic
        occupations >>= attacksB[index].shift
        return board[occupations]

    def rookAttacksMask(self, occupations, index):
        board = rookBlockBoard[index]
        occupations &= attacksR[index].mask
        occupations *= attacksR[index].magic
        occupations >>= attacksR[index].shift
        return board[occupations]

    def filterCheck(self, color):
        movePro = self.movePro
        pieces = movePro.pieces
        whites = movePro.whites
        blacks = movePro.blacks
        occupied = movePro.occupied

        whiteKing = pieces[5] & whites
        blackKing = pieces[5] & blacks

        if color == 0:
            if whiteKing != 0:
                kingPos = self.LSB_TABLE[(((whiteKing ^ (whiteKing - 1)) * 0x03f79d71b4cb0a89) & ALL_ONES) >> 58]
                # kingPos = bitScan(whiteKing)
                rookAttacksMask = self.rookAttacksMask(occupied, kingPos)
                bishopAttacksMask = self.bishopAttacksMask(occupied, kingPos)
                ret = blacks & pieces[0] & self.whitePawnAttacks[kingPos]
                ret |= blacks & pieces[1] & self.knightMoves[kingPos]
                ret |= blacks & pieces[2] & bishopAttacksMask
                ret |= blacks & pieces[3] & rookAttacksMask
                ret |= blacks & pieces[4] & (bishopAttacksMask | rookAttacksMask)
                ret |= blacks & pieces[5] & self.kingMoves[kingPos]
            else:
                return False
                # print(kingPos)
                # printBoard((self.bishopAttacksMask(self.movePro.occupied, kingPos) | self.rookAttacksMask(self.movePro.occupied, kingPos)))
                # print()
        else:
            if blackKing != 0:
                kingPos = self.LSB_TABLE[(((blackKing ^ (blackKing - 1)) * 0x03f79d71b4cb0a89) & ALL_ONES) >> 58]
                # kingPos = bitScan(blackKing)
                rookAttacksMask = self.rookAttacksMask(occupied, kingPos)
                bishopAttacksMask = self.bishopAttacksMask(occupied, kingPos)
                ret = whites & pieces[0] & self.blackPawnAttacks[kingPos]
                ret |= whites & pieces[1] & self.knightMoves[kingPos]
                ret |= whites & pieces[2] & bishopAttacksMask
                ret |= whites & pieces[3] & rookAttacksMask
                ret |= whites & pieces[4] & (bishopAttacksMask | rookAttacksMask)
                ret |= whites & pieces[5] & self.kingMoves[kingPos]
            else:
                return False

        return ret == 0

    def isAttacked(self, index, color):
        movePro = self.movePro
        pieces = movePro.pieces
        whites = movePro.whites
        blacks = movePro.blacks
        occupied = movePro.occupied

        indexP = 1 << index

        if color == 0:
            if indexP != 0:
                pos = self.LSB_TABLE[(((indexP ^ (indexP - 1)) * 0x03f79d71b4cb0a89) & ALL_ONES) >> 58]
                # kingPos = bitScan(whiteKing)
                rookAttacksMask = self.rookAttacksMask(occupied, pos)
                bishopAttacksMask = self.bishopAttacksMask(occupied, pos)
                ret = blacks & pieces[0] & self.whitePawnAttacks[pos]
                ret |= blacks & pieces[1] & self.knightMoves[pos]
                ret |= blacks & pieces[2] & bishopAttacksMask
                ret |= blacks & pieces[3] & rookAttacksMask
                ret |= blacks & pieces[4] & (bishopAttacksMask | rookAttacksMask)
                ret |= blacks & pieces[5] & self.kingMoves[pos]
            else:
                return False
                # print(kingPos)
                # printBoard((self.bishopAttacksMask(self.movePro.occupied, kingPos) | self.rookAttacksMask(self.movePro.occupied, kingPos)))
                # print()
        else:
            if indexP != 0:
                pos = self.LSB_TABLE[(((indexP ^ (indexP - 1)) * 0x03f79d71b4cb0a89) & ALL_ONES) >> 58]
                # kingPos = bitScan(blackKing)
                rookAttacksMask = self.rookAttacksMask(occupied, pos)
                bishopAttacksMask = self.bishopAttacksMask(occupied, pos)
                ret = whites & pieces[0] & self.blackPawnAttacks[pos]
                ret |= whites & pieces[1] & self.knightMoves[pos]
                ret |= whites & pieces[2] & bishopAttacksMask
                ret |= whites & pieces[3] & rookAttacksMask
                ret |= whites & pieces[4] & (bishopAttacksMask | rookAttacksMask)
                ret |= whites & pieces[5] & self.kingMoves[pos]
            else:
                return False

        return ret == 0

    def validMovesWhite(self, index):
        movePro = self.movePro
        pieces = movePro.pieces
        whites = movePro.whites
        blacks = movePro.blacks
        occupied = movePro.occupied

        indexP = 1 << index

        if pieces[0] & whites & indexP:
            base = (self.whitePawnAttacks[index] & blacks) | (self.whitePawnMoves[index] & ~occupied)
            if occupied & (1 << (index + 8)) != 0:
                base &= (1 << (index + 16)) ^ 18446744073709551615
            ret = []
            append = ret.append
            table = self.LSB_TABLE
            while base != 0:
                toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & ALL_ONES) >> 58]
                base ^= 1 << toApp
                append(toApp)
            return ret, 0
            # return fillMoves(base), 0
            # Pawn moves
        elif pieces[1] & whites & indexP:
            base = self.knightMoves[index] & ~whites
            ret = []
            append = ret.append
            table = self.LSB_TABLE
            while base != 0:
                toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & ALL_ONES) >> 58]
                base ^= 1 << toApp
                append(toApp)
            return ret, 1
            # return fillMoves(base), 1
            # Knight moves
        elif pieces[2] & whites & indexP:
            base = self.bishopAttacksMask(occupied, index) & ~whites
            ret = []
            append = ret.append
            table = self.LSB_TABLE
            while base != 0:
                toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & ALL_ONES) >> 58]
                base ^= 1 << toApp
                append(toApp)
            return ret, 2
            # return fillMoves(base), 2
            # Knight moves
        elif pieces[3] & whites & indexP:
            base = self.rookAttacksMask(occupied, index) & ~whites
            ret = []
            append = ret.append
            table = self.LSB_TABLE
            while base != 0:
                toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & ALL_ONES) >> 58]
                base ^= 1 << toApp
                append(toApp)
            return ret, 3
            # return fillMoves(base), 3
            # Knight moves
        elif pieces[4] & whites & indexP:
            base = (self.bishopAttacksMask(occupied, index) | self.rookAttacksMask(occupied, index)) & ~whites
            ret = []
            append = ret.append
            table = self.LSB_TABLE
            while base != 0:
                toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & ALL_ONES) >> 58]
                base ^= 1 << toApp
                append(toApp)
            return ret, 4
            # return fillMoves(base), 4
            # Knight moves
        elif pieces[5] & whites & indexP:
            base = self.kingMoves[index] & ~whites
            ret = []
            append = ret.append
            table = self.LSB_TABLE
            while base != 0:
                toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & ALL_ONES) >> 58]
                base ^= 1 << toApp
                append(toApp)
            return ret, 5
            # return fillMoves(base), 5
            # Knight moves

        return []

    def validMovesBlack(self, index):
        # Move these two lines one level higher once done.
        movePro = self.movePro
        pieces = movePro.pieces
        whites = movePro.whites
        blacks = movePro.blacks
        occupied = movePro.occupied
        indexP = 1 << index

        if pieces[0] & blacks & indexP:
            base = (self.blackPawnAttacks[index] & whites) | (self.blackPawnMoves[index] & ~occupied)
            if index >= 16 and occupied & (1 << (index - 8)) != 0:
                base &= (1 << (index - 16)) ^ 18446744073709551615

            ret = []
            append = ret.append
            table = self.LSB_TABLE
            while base != 0:
                toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & ALL_ONES) >> 58]
                base ^= 1 << toApp
                append(toApp)
            return ret, 0
            # return fillMoves(base), 0
            # Pawn moves
        elif pieces[1] & blacks & indexP:
            base = self.knightMoves[index] & ~blacks
            ret = []
            append = ret.append
            table = self.LSB_TABLE
            while base != 0:
                toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & ALL_ONES) >> 58]
                base ^= 1 << toApp
                append(toApp)
            return ret, 1
            # return fillMoves(base), 1
            # Knight moves
        elif pieces[2] & blacks & indexP:
            base = self.bishopAttacksMask(occupied, index) & ~blacks
            ret = []
            append = ret.append
            table = self.LSB_TABLE
            while base != 0:
                toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & ALL_ONES) >> 58]
                base ^= 1 << toApp
                append(toApp)
            return ret, 2
            # return fillMoves(base), 2
            # Knight moves
        elif pieces[3] & blacks & indexP:
            base = self.rookAttacksMask(occupied, index) & ~blacks
            ret = []
            append = ret.append
            table = self.LSB_TABLE
            while base != 0:
                toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & ALL_ONES) >> 58]
                base ^= 1 << toApp
                append(toApp)
            return ret, 3
            # return fillMoves(base), 3
            # Knight moves
        elif pieces[4] & blacks & indexP:
            base = (self.bishopAttacksMask(occupied, index) | self.rookAttacksMask(occupied, index)) & ~blacks
            ret = []
            append = ret.append
            table = self.LSB_TABLE
            while base != 0:
                toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & ALL_ONES) >> 58]
                base ^= 1 << toApp
                append(toApp)
            return ret, 4
            # return fillMoves(base), 4
            # Knight moves
        elif pieces[5] & blacks & indexP:
            base = self.kingMoves[index] & ~blacks
            ret = []
            append = ret.append
            table = self.LSB_TABLE
            while base != 0:
                toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & ALL_ONES) >> 58]
                base ^= 1 << toApp
                append(toApp)
            return ret, 5
            # return fillMoves(base), 5
            # Knight moves

        return []

    def whitePiecesLoc(self):
        ret = []
        append = ret.append
        bitboard = self.movePro.whites
        table = self.LSB_TABLE
        while bitboard != 0:
            toApp = table[(((bitboard ^ (bitboard - 1)) * 0x03f79d71b4cb0a89) & ALL_ONES) >> 58]
            # toApp = bitScan(bitboard)
            bitboard ^= 1 << toApp
            append(toApp)
        return ret
        # return fillMoves(self.movePro.whites, self.populationWhites)

    def blackPiecesLoc(self):
        ret = []
        append = ret.append
        bitboard = self.movePro.blacks
        table = self.LSB_TABLE
        while bitboard != 0:
            toApp = table[(((bitboard ^ (bitboard - 1)) * 0x03f79d71b4cb0a89) & ALL_ONES) >> 58]
            # toApp = bitScan(bitboard)
            bitboard ^= 1 << toApp
            append(toApp)
        return ret
        # return fillMoves(self.movePro.blacks, self.populationBlacks)

    def allValidMoves(self, color):
        ret = []
        ret2 = []
        movePro = self.movePro
        append = ret.append
        append2 = ret2.append
        occupied = movePro.occupied
        pieces = movePro.pieces
        whites = movePro.whites
        blacks = movePro.blacks
        # validMovesWhite = self.validMovesWhite
        # validMovesBlack = self.validMovesBlack
        table = self.LSB_TABLE

        knightMoves = self.knightMoves
        bishopAttacksMask = self.bishopAttacksMask
        rookAttacksMask = self.rookAttacksMask
        kingMoves = self.kingMoves

        if color == 0:

            whitePawnAttacks = self.whitePawnAttacks
            whitePawnMoves = self.whitePawnMoves
            mrStart = []
            appendS = mrStart.append
            bitboard = self.movePro.whites
            while bitboard != 0:
                toApp = table[(((bitboard ^ (bitboard - 1)) * 0x03f79d71b4cb0a89) & ALL_ONES) >> 58]
                # toApp = bitScan(bitboard)
                bitboard ^= 1 << toApp
                appendS(toApp)

            for start in mrStart:
                vMoves = None

                index = start
                indexP = 1 << index
                whitesIndex = whites & indexP

                if pieces[0] & whitesIndex:
                    base = (whitePawnAttacks[index] & blacks) | (whitePawnMoves[index] & ~occupied)
                    if occupied & (1 << (index + 8)) != 0:
                        base &= (1 << (index + 16)) ^ 18446744073709551615
                    del ret2[:]
                    while base != 0:
                        toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & 18446744073709551615) >> 58]
                        base ^= 1 << toApp
                        append2(toApp)
                    vMoves = (ret2, 0)
                    # return fillMoves(base), 0
                    # Pawn moves
                elif pieces[1] & whitesIndex:
                    base = knightMoves[index] & ~whites
                    del ret2[:]
                    while base != 0:
                        toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & 18446744073709551615) >> 58]
                        base ^= 1 << toApp
                        append2(toApp)
                    vMoves = (ret2, 1)
                    # return fillMoves(base), 1
                    # Knight moves
                elif pieces[2] & whitesIndex:
                    base = bishopAttacksMask(occupied, index) & ~whites
                    del ret2[:]
                    while base != 0:
                        toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & 18446744073709551615) >> 58]
                        base ^= 1 << toApp
                        append2(toApp)
                    vMoves = (ret2, 2)
                    # return fillMoves(base), 2
                    # Knight moves
                elif pieces[3] & whitesIndex:
                    base = rookAttacksMask(occupied, index) & ~whites
                    del ret2[:]
                    while base != 0:
                        toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & 18446744073709551615) >> 58]
                        base ^= 1 << toApp
                        append2(toApp)
                    vMoves = (ret2, 3)
                    # return fillMoves(base), 3
                    # Knight moves
                elif pieces[4] & whitesIndex:
                    base = (bishopAttacksMask(occupied, index) | rookAttacksMask(occupied, index)) & ~whites
                    del ret2[:]
                    while base != 0:
                        toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & 18446744073709551615) >> 58]
                        base ^= 1 << toApp
                        append2(toApp)
                    vMoves = (ret2, 4)
                    # return fillMoves(base), 4
                    # Knight moves
                elif pieces[5] & whitesIndex:
                    base = kingMoves[index] & ~whites
                    del ret2[:]

                    occupiedToCastleOO = occupied & ((1 << 5) | (1 << 6))
                    occupiedToCastleOOO = occupied & ((1 << 3) | (1 << 2) | (1 << 1))
                    if not self.kingMovedWhite:
                        if not self.rookMovedWhiteH and occupiedToCastleOO == 0 and self.isAttacked(4, 0) and self.isAttacked(5, 0) and self.isAttacked(6, 0):
                            append2(6)
                        if not self.rookMovedWhiteA and occupiedToCastleOOO == 0 and self.isAttacked(4, 0) and self.isAttacked(3, 0) and self.isAttacked(2, 0):
                            append2(2)

                    while base != 0:
                        toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & 18446744073709551615) >> 58]
                        base ^= 1 << toApp
                        append2(toApp)

                    vMoves = (ret2, 5)

                for end in vMoves[0]:
                    if (1 << end) & occupied != 0:
                        shiftEnd = (1 << end)
                        # ret += [(start, end, vMoves[1], i) for i in range(6) if pieces[i] & (1 << end) != 0]
                        i = -1
                        if pieces[0] & shiftEnd != 0:
                            i = 0
                        elif pieces[1] & shiftEnd != 0:
                            i = 1
                        elif pieces[2] & shiftEnd != 0:
                            i = 2
                        elif pieces[3] & shiftEnd != 0:
                            i = 3
                        elif pieces[4] & shiftEnd != 0:
                            i = 4
                        elif pieces[5] & shiftEnd != 0:
                            i = 5

                        append((start, end, vMoves[1], i))

                    else:
                        append((start, end, vMoves[1], None))
        else:

            blackPawnAttacks = self.blackPawnAttacks
            blackPawnMoves = self.blackPawnMoves
            mrStart = []
            appendS = mrStart.append
            bitboard = movePro.blacks
            while bitboard != 0:
                toApp = table[(((bitboard ^ (bitboard - 1)) * 0x03f79d71b4cb0a89) & 18446744073709551615) >> 58]
                # toApp = bitScan(bitboard)
                bitboard ^= 1 << toApp
                appendS(toApp)

            # print(mrStart)

            for start in mrStart:
                vMoves = None

                index = start
                indexP = 1 << index
                blacksIndex = blacks & indexP

                if pieces[0] & blacksIndex:
                    base = (blackPawnAttacks[index] & whites) | (blackPawnMoves[index] & ~occupied)
                    if index >= 16 and occupied & (1 << (index - 8)) != 0:
                        base &= (1 << (index - 16)) ^ 18446744073709551615

                    del ret2[:]
                    while base != 0:
                        toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & 18446744073709551615) >> 58]
                        base ^= 1 << toApp
                        append2(toApp)
                    vMoves = (ret2, 0)
                    # return fillMoves(base), 0
                    # Pawn moves
                elif pieces[1] & blacksIndex:
                    base = knightMoves[index] & ~blacks
                    del ret2[:]
                    while base != 0:
                        toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & 18446744073709551615) >> 58]
                        base ^= 1 << toApp
                        append2(toApp)
                    vMoves = (ret2, 1)
                    # return fillMoves(base), 1
                    # Knight moves
                elif pieces[2] & blacksIndex:
                    base = bishopAttacksMask(occupied, index) & ~blacks
                    del ret2[:]
                    while base != 0:
                        toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & 18446744073709551615) >> 58]
                        base ^= 1 << toApp
                        append2(toApp)
                    vMoves = (ret2, 2)
                    # return fillMoves(base), 2
                    # Knight moves
                elif pieces[3] & blacksIndex:
                    base = rookAttacksMask(occupied, index) & ~blacks
                    del ret2[:]
                    while base != 0:
                        toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & 18446744073709551615) >> 58]
                        base ^= 1 << toApp
                        append2(toApp)
                    vMoves = (ret2, 3)
                    # return fillMoves(base), 3
                    # Knight moves
                elif pieces[4] & blacksIndex:
                    base = (bishopAttacksMask(occupied, index) | rookAttacksMask(occupied, index)) & ~blacks
                    del ret2[:]
                    while base != 0:
                        toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & 18446744073709551615) >> 58]
                        base ^= 1 << toApp
                        append2(toApp)
                    vMoves = (ret2, 4)
                    # return fillMoves(base), 4
                    # Knight moves
                elif pieces[5] & blacksIndex:
                    base = kingMoves[index] & ~blacks
                    del ret2[:]

                    occupiedToCastleOO = occupied & ((1 << 62) | (1 << 61))
                    occupiedToCastleOOO = occupied & ((1 << 59) | (1 << 58) | (1 << 57))
                    if not self.kingMovedBlack:
                        if not self.rookMovedBlackH and occupiedToCastleOO == 0 and self.isAttacked(60, 1) and self.isAttacked(61, 1) and self.isAttacked(62, 1):
                            append2(62)
                        if not self.rookMovedBlackA and occupiedToCastleOOO == 0 and self.isAttacked(60, 1) and self.isAttacked(59, 1) and self.isAttacked(58, 1):
                            append2(58)

                    while base != 0:
                        toApp = table[(((base ^ (base - 1)) * 0x03f79d71b4cb0a89) & 18446744073709551615) >> 58]
                        base ^= 1 << toApp
                        append2(toApp)
                    vMoves = (ret2, 5)


                    # return fillMoves(base), 5
                    # Knight moves

                # print(vMoves)
                for end in vMoves[0]:
                    if (1 << end) & occupied != 0:
                        shiftEnd = (1 << end)
                        # ret += [(start, end, vMoves[1], i) for i in range(6) if pieces[i] & (1 << end) != 0]
                        i = -1
                        if pieces[0] & shiftEnd != 0:
                            i = 0
                        elif pieces[1] & shiftEnd != 0:
                            i = 1
                        elif pieces[2] & shiftEnd != 0:
                            i = 2
                        elif pieces[3] & shiftEnd != 0:
                            i = 3
                        elif pieces[4] & shiftEnd != 0:
                            i = 4
                        elif pieces[5] & shiftEnd != 0:
                            i = 5

                        append((start, end, vMoves[1], i))
                    else:
                        append((start, end, vMoves[1], None))

        return ret

    def sortMoves(self, moves, elements, killers, prevMove=None):

        # newList = []
        # append = newList.append

        if 0 in killers:
            killersFirst = killers[0][5]
            killersFirstMove = (killersFirst[0], killersFirst[1], killersFirst[2], None)
            if killersFirstMove in moves:
                moves.insert(0, moves.pop(moves.index(killersFirstMove)))

            if 1 in killers:
                killersSecond =  killers[1][5]
                killersSecondMove = (killersSecond[0], killersSecond[1], killersSecond[2], None)
                if 1 in killers and killersSecondMove in moves:
                    moves.insert(0, moves.pop(moves.index(killersSecondMove)))




        for i in moves:
            if i[3] and i[2] <= i[3]:
                moves.insert(0, moves.pop(moves.index(i)))

        if prevMove:
            for i in moves:
                if i[1] == prevMove[1]:
                    moves.insert(0, moves.pop(moves.index(i)))



        for i in elements:
            if i in moves:
                moves.insert(0, moves.pop(moves.index(i)))


        # newList = list(dict.fromkeys(newList))
        # notMoves = [i for i in moves if i not in newList]
        # newList = [x for x in itertools.chain(newList, notMoves)]
        # for i in moves:
        #     if i not in newList:
        #         append(i)
        # print(notMoves)

        return moves

    def movePiece(self, index1, index2):

        movePro = self.movePro
        # setPieces = movePro.setPieces
        # setOccupied = movePro.setOccupied
        # setWhites = movePro.setWhites
        # setBlacks = movePro.setBlacks
        # whites = movePro.whites
        # blacks = movePro.blacks
        pieces = movePro.pieces
        # occupied = movePro.occupied
        values = movePro.values
        moveStack = self.moveStack
        append = moveStack.append

        i1 = 1 << index1
        i2 = 1 << index2
        i1i2 = i1 ^ i2
        moved = False

        i = -1
        if pieces[0] & i1 != 0:
            i = 0
        elif pieces[1] & i1 != 0:
            i = 1
        elif pieces[2] & i1 != 0:
            i = 2
        elif pieces[3] & i1 != 0:
            i = 3
        elif pieces[4] & i1 != 0:
            i = 4
        elif pieces[5] & i1 != 0:
            i = 5

        if i == -1:
            print("Cannot make move")
            return

        if not (i2 & movePro.occupied):
            movePro.pieces[i] ^= i1i2
            # setPieces(i, pieces[i] ^ i1i2)

            if movePro.whites & i1:
                movePro.whites ^= i1i2
                kingTrue = 0
                rookTrue = 0
                castle = 0

                if i == 5 and not self.kingMovedWhite and index2 == 6:
                    movePro.whites ^= (1 << 7) | (1 << 5)
                    movePro.pieces[3] ^= (1 << 7) | (1 << 5)
                    movePro.occupied ^= (1 << 7) | (1 << 5)
                    self.castled += self.castleValue
                    castle = 1

                if i == 5 and not self.kingMovedWhite and index2 == 2:
                    movePro.whites ^= (1 << 0) | (1 << 3)
                    movePro.pieces[3] ^= (1 << 0) | (1 << 3)
                    movePro.occupied ^= (1 << 0) | (1 << 3)
                    self.castled += self.castleValue
                    castle = 2

                if i == 5 and not self.kingMovedWhite:
                    self.kingMovedWhite = self.castleValue
                    kingTrue = 1


                if i == 3 and not self.rookMovedWhiteA and index1 == 0:
                    self.rookMovedWhiteA = True
                    rookTrue = 1

                if i == 3 and not self.rookMovedWhiteH and index1 == 7:
                    self.rookMovedWhiteH = True
                    rookTrue = 2

                # setWhites(whites ^ i1i2)
                if i == 0 and index2 > 55:
                    movePro.pieces[i] ^= i2
                    movePro.pieces[4] ^= i2
                    self.materialScore += values[4] - values[0]
                    self.populationPieces[i] -= 1
                    self.populationPieces[4] += 1
                    append((i1, i2, i, -1, 0, 4, kingTrue, rookTrue, castle))
                else:
                    append((i1, i2, i, -1, 0, 0, kingTrue, rookTrue, castle))
            elif movePro.blacks & i1:
                movePro.blacks ^= i1i2

                kingTrue = 0
                rookTrue = 0
                castle = 0

                if i == 5 and not self.kingMovedBlack and index2 == 62:
                    movePro.blacks ^= (1 << 63) | (1 << 61)
                    movePro.pieces[3] ^= (1 << 63) | (1 << 61)
                    movePro.occupied ^= (1 << 63) | (1 << 61)
                    self.castled -= self.castleValue
                    castle = 3

                if i == 5 and not self.kingMovedBlack and index2 == 58:
                    movePro.whites ^= (1 << 56) | (1 << 59)
                    movePro.pieces[3] ^= (1 << 56) | (1 << 59)
                    movePro.occupied ^= (1 << 56) | (1 << 59)
                    self.castled -= self.castleValue
                    castle = 4

                if i == 5 and not self.kingMovedBlack:
                    self.kingMovedBlack = True
                    kingTrue = 2


                if i == 3 and not self.rookMovedBlackA and index1 == 56:
                    self.rookMovedBlackA = True
                    rookTrue = 3

                if i == 3 and not self.rookMovedBlackH and index1 == 63:
                    self.rookMovedBlackH = True
                    rookTrue = 4
                # setBlacks(blacks ^ i1i2)
                if i == 0 and index2 < 8:
                    movePro.pieces[i] ^= i2
                    movePro.pieces[4] ^= i2
                    self.materialScore -= values[4] - values[0]
                    self.populationPieces[i + 6] -= 1
                    self.populationPieces[4 + 6] += 1
                    append((i1, i2, i, -1, 1, 4, kingTrue, rookTrue, castle))
                else:
                    append((i1, i2, i, -1, 1, 0, kingTrue, rookTrue, castle))



            movePro.occupied ^= i1i2
            # setOccupied(i1i2)

        else:
            if movePro.whites & i1:
                movePro.whites ^= i1i2
                kingTrue = 0
                if i == 5 and not self.kingMovedWhite:
                    self.kingMovedWhite = True
                    kingTrue = 1

                rookTrue = 0
                if i == 3 and not self.rookMovedWhiteA and index1 == 0:
                    self.rookMovedWhiteA = True
                    rookTrue = 1

                if i == 3 and not self.rookMovedWhiteH and index1 == 7:
                    self.rookMovedWhiteH = True
                    rookTrue = 2

                # setWhites(whites ^ i1i2)
                if movePro.blacks & i2:
                    k = -1
                    if pieces[0] & i2 != 0:
                        k = 0
                    elif pieces[1] & i2 != 0:
                        k = 1
                    elif pieces[2] & i2 != 0:
                        k = 2
                    elif pieces[3] & i2 != 0:
                        k = 3
                    elif pieces[4] & i2 != 0:
                        k = 4
                    elif pieces[5] & i2 != 0:
                        k = 5

                    movePro.pieces[k] ^= i2
                    movePro.blacks ^= i2
                    # setPieces(k, pieces[k] ^ i2)
                    # setBlacks(blacks ^ i2)
                    if i == 0 and index2 > 55:
                        movePro.pieces[0] ^= i2
                        movePro.pieces[4] ^= i2
                        self.materialScore += values[4] - values[0]
                        self.populationPieces[0] -= 1
                        self.populationPieces[4] += 1
                        append((i1, i2, i, k, 0, 4, kingTrue, rookTrue, 0))
                    else:
                        append((i1, i2, i, k, 0, 0, kingTrue, rookTrue, 0))

                    self.materialScore += values[k]
                    self.populationBlacks -= 1
                    self.populationPieces[k + 6] -= 1

                else:
                    append((i1, i2, i, -1, 0, 0, kingTrue, rookTrue, 0))
            elif movePro.blacks & i1:
                movePro.blacks ^= i1i2
                kingTrue = 0
                if i == 5 and not self.kingMovedBlack:
                    self.kingMovedBlack = True
                    kingTrue = 2

                rookTrue = 0
                if i == 3 and not self.rookMovedBlackA and index1 == 56:
                    self.rookMovedBlackA = True
                    rookTrue = 3

                if i == 3 and not self.rookMovedBlackH and index1 == 63:
                    self.rookMovedBlackH = True
                    rookTrue = 4
                # setBlacks(blacks ^ i1i2)
                if movePro.whites & i2:
                    k = -1
                    if pieces[0] & i2 != 0:
                        k = 0
                    elif pieces[1] & i2 != 0:
                        k = 1
                    elif pieces[2] & i2 != 0:
                        k = 2
                    elif pieces[3] & i2 != 0:
                        k = 3
                    elif pieces[4] & i2 != 0:
                        k = 4
                    elif pieces[5] & i2 != 0:
                        k = 5

                    movePro.pieces[k] ^= i2
                    movePro.whites ^= i2
                    # setPieces(k, pieces[k] ^ i2)
                    # setWhites(whites ^ i2)
                    if i == 0 and index2 < 8:
                        movePro.pieces[0] ^= i2
                        movePro.pieces[4] ^= i2
                        self.materialScore -= values[4] - values[0]
                        self.populationPieces[0 + 6] -= 1
                        self.populationPieces[4 + 6] += 1
                        append((i1, i2, i, k, 1, 4, kingTrue, rookTrue, 0))
                    else:
                        append((i1, i2, i, k, 1, 0, kingTrue, rookTrue, 0))

                    # append((i1, i2, i, k, 1, 0))
                    self.materialScore -= values[k]
                    self.populationWhites -= 1
                    self.populationPieces[k] -= 1
                else:
                    append((i1, i2, i, -1, 1, 0, kingTrue, rookTrue, 0))

            movePro.occupied ^= i1
            movePro.pieces[i] ^= i1i2
            # setOccupied(i1)
            # setPieces(i, pieces[i] ^ i1i2)


        return

    def undoMove(self):
        # printBoard(self.moveStack[-1].occupied)
        movePro = self.movePro
        # setPieces = movePro.setPieces
        # setOccupied = movePro.setOccupied
        # setWhites = movePro.setWhites
        # setBlacks = movePro.setBlacks
        # whites = movePro.whites
        # blacks = movePro.blacks
        # pieces = movePro.pieces
        # occupied = movePro.occupied
        values = movePro.values
        moveStack = self.moveStack

        # print(len(moveStack))
        m = moveStack[-1]
        fromLoc = m[0]
        toLoc = m[1]
        movingPiece = m[2]
        capturedPiece = m[3]
        promotionPiece = m[5]

        if m[8] == 1:
            movePro.whites ^= (1 << 7) | (1 << 5)
            movePro.pieces[3] ^= (1 << 7) | (1 << 5)
            movePro.occupied ^= (1 << 7) | (1 << 5)
            self.castled = 0

        elif m[8] == 2:
            movePro.whites ^= (1 << 0) | (1 << 3)
            movePro.pieces[3] ^= (1 << 0) | (1 << 3)
            movePro.occupied ^= (1 << 0) | (1 << 3)
            self.castled = 0

        elif m[8] == 3:
            movePro.blacks ^= (1 << 63) | (1 << 61)
            movePro.pieces[3] ^= (1 << 63) | (1 << 61)
            movePro.occupied ^= (1 << 63) | (1 << 61)
            self.castled = 0

        elif m[8] == 4:
            movePro.whites ^= (1 << 56) | (1 << 59)
            movePro.pieces[3] ^= (1 << 56) | (1 << 59)
            movePro.occupied ^= (1 << 56) | (1 << 59)
            self.castled = 0

        if m[6] == 1:
            self.kingMovedWhite = False
        elif m[6] == 2:
            self.kingMovedBlack = False

        if m[7] != 0:
            if m[7] == 1:
                self.rookMovedWhiteA = False
            elif m[7] == 2:
                self.rookMovedWhiteH = False
            elif m[7] == 3:
                self.rookMovedBlackA = False
            elif m[7] == 4:
                self.rookMovedBlackH = False


        i1i2 = fromLoc ^ toLoc
        if toLoc != fromLoc:
            movePro.pieces[movingPiece] ^= i1i2
            # setPieces(movingPiece, pieces[movingPiece] ^ i1i2)
            if m[4] == 0:
                movePro.whites ^= i1i2
                # setWhites(whites ^ i1i2)
                if capturedPiece != -1:
                    movePro.pieces[capturedPiece] ^= toLoc
                    movePro.blacks ^= toLoc
                    movePro.occupied ^= fromLoc
                    # setPieces(capturedPiece, pieces[capturedPiece] ^ toLoc)
                    # setBlacks(blacks ^ toLoc)
                    # setOccupied(fromLoc)
                    self.materialScore -= values[capturedPiece]
                    self.populationBlacks += 1
                    self.populationPieces[capturedPiece + 6] += 1
                else:
                    movePro.occupied ^= i1i2

                if promotionPiece != 0:
                    movePro.pieces[promotionPiece] ^= toLoc
                    movePro.pieces[0] ^= toLoc
                    self.materialScore -= values[4] - values[0]
                    self.populationPieces[4] -= 1
                    # setOccupied(i1i2)
            else:
                movePro.blacks ^= i1i2
                # setBlacks(blacks ^ i1i2)
                if capturedPiece != -1:
                    movePro.pieces[capturedPiece] ^= toLoc
                    movePro.whites ^= toLoc
                    movePro.occupied ^= fromLoc
                    # setPieces(capturedPiece, pieces[capturedPiece] ^ toLoc)
                    # setWhites(whites ^ toLoc)
                    # setOccupied(fromLoc)
                    self.materialScore += values[capturedPiece]
                    self.populationWhites += 1
                    self.populationPieces[capturedPiece] += 1
                else:
                    movePro.occupied ^= i1i2

                if promotionPiece != 0:
                    movePro.pieces[promotionPiece] ^= toLoc
                    movePro.pieces[0] ^= toLoc
                    self.materialScore += values[4] - values[0]
                    self.populationPieces[4 + 6] -= 1
                    # setOccupied(i1i2)

        del moveStack[-1]
        # self.moveStack = np.delete(self.moveStack, -1)
        return

    def evaluate(self):
        ret = 0
        ret += self.materialScore + self.castled
        # for i in range(6):
        #     ret -= count_population(self.movePro.pieces[i] & self.movePro.blacks) * self.movePro.values[i]
        #     ret += count_population(self.movePro.pieces[i] & self.movePro.whites) * self.movePro.values[i]
        #     # ret -= len(self.allValidMoves(1)) * 0.001
        #     # ret += len(self.allValidMoves(0)) * 0.001

        return ret

    def printPretty(self):
        x = '{:064b}'.format(self.movePro.occupied)
        whites = '{:064b}'.format(self.movePro.whites)
        blacks = '{:064b}'.format(self.movePro.blacks)
        pawns = '{:064b}'.format(self.movePro.pieces[0])
        knights = '{:064b}'.format(self.movePro.pieces[1])
        bishops = '{:064b}'.format(self.movePro.pieces[2])
        rooks = '{:064b}'.format(self.movePro.pieces[3])
        queens = '{:064b}'.format(self.movePro.pieces[4])
        kings = '{:064b}'.format(self.movePro.pieces[5])
        for i in range(8):
            for j in range(8):
                s = ""
                if x[(7 - j) + 8 * i] != "0":
                    if whites[(7 - j) + 8 * i] != "0":
                        s += "w"
                    if blacks[(7 - j) + 8 * i] != "0":
                        s += "b"
                    if pawns[(7 - j) + 8 * i] != "0":
                        s += "P"
                    if knights[(7 - j) + 8 * i] != "0":
                        s += "N"
                    if bishops[(7 - j) + 8 * i] != "0":
                        s += "B"
                    if rooks[(7 - j) + 8 * i] != "0":
                        s += "R"
                    if queens[(7 - j) + 8 * i] != "0":
                        s += "Q"
                    if kings[(7 - j) + 8 * i] != "0":
                        s += "K"
                if s == "":
                    s += "  "
                print(s, end = " ")
            print()





def fillMoves(bitboard, population=None):

    # if population != None:
    #     ret = []
    #     append = ret.append
    #     for i in range(population):
    #         toApp = bitScan(bitboard)
    #         bitboard ^= 1 << toApp
    #         append(toApp)
        # bb = [bitboard]
        # ret = [fillMovesI(bb) for i in range(population)]
        # return ret
    # else:
    ret = []
    append = ret.append
    while bitboard != 0:
        toApp = LSB_TABLE[(((bitboard ^ (bitboard - 1)) * 0x03f79d71b4cb0a89) & ALL_ONES) >> 58]
        # toApp = bitScan(bitboard)
        bitboard ^= 1 << toApp
        append(toApp)
    return ret
        # bitboard = unSetCellState1D(bitboard, toApp)

    # return ret

def fillMovesI(bitboard):
    toApp = bitScan(bitboard[0])
    bitboard[0] ^= 1 << toApp
    return toApp

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


#I honestly tried to implement myself but failed.
#Adapted from https://stackoverflow.com/questions/30680559/how-to-find-magic-bitboards
def bitCombinations(index, bitboard):
    bindex = 0
    board = bitboard
    for i in range(64):
        bitToFlip = 1 << i
        if bitToFlip & bitboard != 0:
            if (index & (1 << bindex) == 0):
                board &= ~bitToFlip
            bindex += 1
    return board


def blocksRook(bitboard, index):
    bitboardMasked = bitboard & attacksR[index].mask
    indexP = index
    index = 1 << index
    ret = dict()
    countMasked = count_population(bitboardMasked)



    for i in range(1 << countMasked):
        r = bitCombinations(i, bitboardMasked)
        res = (dumb7FloodingN(index, r) | dumb7FloodingE(index, r) | dumb7FloodingS(index, r) | dumb7FloodingW(index, r)) & flipBits(index)

        prevR = r


        r *= attacksR[indexP].magic
        r >>= attacksR[indexP].shift

        if indexP == 5 and r == 0:
            printBoard(r)
            print(prevR)

        ret[r] = res


    return ret

def blocksBishop(bitboard, index):
    bitboardMasked = bitboard & INNER_MASK
    indexP = index
    index = 1 << index
    ret = dict()
    count = count_population(bitboard)
    for i in range(1 << count):
        r = bitCombinations(i, bitboard)
        res = (dumb7FloodingNE(index, r) | dumb7FloodingSE(index, r) | dumb7FloodingSW(index, r) | dumb7FloodingNW(index, r)) & flipBits(index)

        r *= attacksB[indexP].magic
        r >>= attacksB[indexP].shift

        ret[r] = res

    return ret

def dumb7FloodingN(index, empty):
    empty = flipBits(empty)
    flood = index
    index = (index << 8) & empty
    flood |= index
    index = (index << 8) & empty
    flood |= index
    index = (index << 8) & empty
    flood |= index
    index = (index << 8) & empty
    flood |= index
    index = (index << 8) & empty
    flood |= index
    index = (index << 8) & empty
    flood |= index
    index = (index << 8) & empty
    flood |= index
    return (flood << 8) & ALL_ONES

def dumb7FloodingS(index, empty):
    empty = flipBits(empty)
    flood = index
    index = (index >> 8) & empty
    flood |= index
    index = (index >> 8) & empty
    flood |= index
    index = (index >> 8) & empty
    flood |= index
    index = (index >> 8) & empty
    flood |= index
    index = (index >> 8) & empty
    flood |= index
    index = (index >> 8) & empty
    flood |= index
    index = (index >> 8) & empty
    flood |= index

    return (flood >> 8) & ALL_ONES

def dumb7FloodingE(index, empty):
    empty = flipBits(empty)
    empty &= 0xfefefefefefefefe
    flood = index
    index = (index << 1) & empty
    flood |= index
    index = (index << 1) & empty
    flood |= index
    index = (index << 1) & empty
    flood |= index
    index = (index << 1) & empty
    flood |= index
    index = (index << 1) & empty
    flood |= index
    index = (index << 1) & empty
    flood |= index
    index = (index << 1) & empty
    flood |= index

    return (flood << 1) & 0xfefefefefefefefe

def dumb7FloodingW(index, empty):
    empty = flipBits(empty)
    empty &= 0x7f7f7f7f7f7f7f7f
    flood = index
    index = (index >> 1) & empty
    flood |= index
    index = (index >> 1) & empty
    flood |= index
    index = (index >> 1) & empty
    flood |= index
    index = (index >> 1) & empty
    flood |= index
    index = (index >> 1) & empty
    flood |= index
    index = (index >> 1) & empty
    flood |= index
    index = (index >> 1) & empty
    flood |= index

    return (flood >> 1) & 0x7f7f7f7f7f7f7f7f

def dumb7FloodingNE(index, empty):
    empty = flipBits(empty)
    empty &= 0xfefefefefefefefe
    flood = index
    index = (index << 9) & empty
    flood |= index
    index = (index << 9) & empty
    flood |= index
    index = (index << 9) & empty
    flood |= index
    index = (index << 9) & empty
    flood |= index
    index = (index << 9) & empty
    flood |= index
    index = (index << 9) & empty
    flood |= index
    index = (index << 9) & empty
    flood |= index

    return (flood << 9) & 0xfefefefefefefefe

def dumb7FloodingSE(index, empty):
    empty = flipBits(empty)
    empty &= 0xfefefefefefefefe
    flood = index
    index = (index >> 7) & empty
    flood |= index
    index = (index >> 7) & empty
    flood |= index
    index = (index >> 7) & empty
    flood |= index
    index = (index >> 7) & empty
    flood |= index
    index = (index >> 7) & empty
    flood |= index
    index = (index >> 7) & empty
    flood |= index
    index = (index >> 7) & empty
    flood |= index

    return (flood >> 7) & 0xfefefefefefefefe

def dumb7FloodingNW(index, empty):
    empty = flipBits(empty)
    empty &= 0x7f7f7f7f7f7f7f7f
    flood = index
    index = (index << 7) & empty
    flood |= index
    index = (index << 7) & empty
    flood |= index
    index = (index << 7) & empty
    flood |= index
    index = (index << 7) & empty
    flood |= index
    index = (index << 7) & empty
    flood |= index
    index = (index << 7) & empty
    flood |= index
    index = (index << 7) & empty
    flood |= index

    return (flood << 7) & 0x7f7f7f7f7f7f7f7f

def dumb7FloodingSW(index, empty):
    empty = flipBits(empty)
    empty &= 0x7f7f7f7f7f7f7f7f
    flood = index
    index = (index >> 9) & empty
    flood |= index
    index = (index >> 9) & empty
    flood |= index
    index = (index >> 9) & empty
    flood |= index
    index = (index >> 9) & empty
    flood |= index
    index = (index >> 9) & empty
    flood |= index
    index = (index >> 9) & empty
    flood |= index
    index = (index >> 9) & empty
    flood |= index

    return (flood >> 9) & 0x7f7f7f7f7f7f7f7f

# Adapted from https://www.chessprogramming.org/BitScan
def bitScan(bitboard):
    return LSB_TABLE[(((bitboard ^ (bitboard - 1)) * 0x03f79d71b4cb0a89) & ALL_ONES) >> 58]





start = time.time()* 1000000
x = Board()
m = Magic()

for i in range(64):
    rookBlockBoard[i] = blocksRook(x.rookMoves[i], i)
    bishopBlockBoard[i] = blocksBishop(x.bishopMoves[i], i)

# x.printPretty()
# print()
x.movePiece(62, 16)
x.printPretty()
print()
x.movePiece(61, 21)
x.printPretty()
print()
print(x.allValidMoves(1))
x.undoMove()
x.printPretty()
print()
# x.movePiece(2, 47)
# x.printPretty()
# print()
start = time.time()* 1000000
# for i in range(1000000):
    # x.whitePiecesLoc()
    # x.undoMove()
end = time.time() * 1000000
# x.printPretty()
# print()
# x.undoMove()
# x.printPretty()
# print()
occupations = 0
printBoard(x.rookMoves[5])
print()
printBoard(x.rookAttacksMask(occupations, 5))
# printBoard(x.rookMoves[3])


# printBoard(x.moveStack[-1].occupied)
print()

# print(x.validMovesWhite(1))

print((end - start))
