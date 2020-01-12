
board_height = 600
board_width  = 600

# lightSquareColor = (162, 162, 162)
# darkSquareColor  = (98, 98, 98)

lightSquareColor = (172, 172, 162)
darkSquareColor  = (108, 108, 98)

# lightSquareColorSelected = (162, 162, 192)
# darkSquareColorSelected  = (98, 98, 128)
lightSquareColorSelected = (192, 182, 162)
darkSquareColorSelected  = (128, 118, 98)


color_side = 'black' # The side of the board in which you are viewing
is_playing_white = False # Manual control over white pieces, False = Engine
is_playing_black = True # Manual control over white pieces, False = Engine

# Engine depth (Obviously, More depth is better, but lower depth is faster)
white_Max_Depth = 10
black_Max_Depth = 10
Late_Move_Reduction = 2
Late_Move_Reduction_Depth = 1

killerSlots = 2
searchmethod = 0 # 0 = alphabeta, 1 = minimax
