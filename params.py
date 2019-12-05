import pygame as pg
import os

board_height = 600
board_width  = 600

lightSquareColor = (162, 162, 162)
darkSquareColor  = (98, 98, 98)

lightSquareColorSelected = (162, 162, 192)
darkSquareColorSelected  = (98, 98, 128)

bh = int(board_height / 8)
bw = int(board_width / 8)

imagePiecesSet = 'piece_set_2'

black_king_img   = pg.image.load(os.path.join('Images', imagePiecesSet, 'bking.png'))
black_queen_img  = pg.image.load(os.path.join('Images', imagePiecesSet, 'bqueen.png'))
black_rook_img   = pg.image.load(os.path.join('Images', imagePiecesSet, 'brook.png'))
black_bishop_img = pg.image.load(os.path.join('Images', imagePiecesSet, 'bbishop.png'))
black_knight_img = pg.image.load(os.path.join('Images', imagePiecesSet, 'bknight.png'))
black_pawn_img   = pg.image.load(os.path.join('Images', imagePiecesSet, 'bpawn.png'))

white_king_img   = pg.image.load(os.path.join('Images', imagePiecesSet, 'wking.png'))
white_queen_img  = pg.image.load(os.path.join('Images', imagePiecesSet, 'wqueen.png'))
white_rook_img   = pg.image.load(os.path.join('Images', imagePiecesSet, 'wrook.png'))
white_bishop_img = pg.image.load(os.path.join('Images', imagePiecesSet, 'wbishop.png'))
white_knight_img = pg.image.load(os.path.join('Images', imagePiecesSet, 'wknight.png'))
white_pawn_img   = pg.image.load(os.path.join('Images', imagePiecesSet, 'wpawn.png'))


black_king_img   = pg.transform.scale(black_king_img, (bw, bh))
black_queen_img  = pg.transform.scale(black_queen_img, (bw, bh))
black_rook_img   = pg.transform.scale(black_rook_img, (bw, bh))
black_bishop_img = pg.transform.scale(black_bishop_img, (bw, bh))
black_knight_img = pg.transform.scale(black_knight_img, (bw, bh))
black_pawn_img   = pg.transform.scale(black_pawn_img, (bw, bh))

white_king_img   = pg.transform.scale(white_king_img, (bw, bh))
white_queen_img  = pg.transform.scale(white_queen_img, (bw, bh))
white_rook_img   = pg.transform.scale(white_rook_img, (bw, bh))
white_bishop_img = pg.transform.scale(white_bishop_img, (bw, bh))
white_knight_img = pg.transform.scale(white_knight_img, (bw, bh))
white_pawn_img   = pg.transform.scale(white_pawn_img, (bw, bh))
