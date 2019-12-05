import pygame as pg
import os
import math as m
import params
import copy
import random as r
import pandas as pd
import numpy as np
import datetime
import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim


def position2Location(h, w, side, position):

	if len(position) != 2:
		return None

	h2 = h / 8
	w2 = w / 8

	col = position[0]
	row = position[1]
	row = int(row)

	if col == "A":
		col = 0
	elif col == "B":
		col = 1
	elif col == "C":
		col = 2
	elif col == "D":
		col = 3
	elif col == "E":
		col = 4
	elif col == "F":
		col = 5
	elif col == "G":
		col = 6
	elif col == "H":
		col = 7

	if side == "white":
		res_col = m.floor(col * w2)
		res_row = m.floor(h - row * h2)
	else:
		res_col = m.floor(w - w2 - col * w2)
		res_row = m.floor(row * h2 - h2)

	return (int(res_col + w2 / 2), int(res_row + h2 / 2))


def getPosition(h, w, side, mouse_pos):

	if mouse_pos == None:
		return None

	h2 = h / 8
	w2 = w / 8


	if side == "white":
		col = m.floor(mouse_pos[0] / w2)
		row = m.floor(8 - mouse_pos[1] / h2) + 1
	else:
		col = m.floor(8 - mouse_pos[0] / w2)
		row = m.floor(mouse_pos[1] / h2) + 1

	res = ""

	if col == 0:
		res = "A"
	elif col == 1:
		res = "B"
	elif col == 2:
		res = "C"
	elif col == 3:
		res = "D"
	elif col == 4:
		res = "E"
	elif col == 5:
		res = "F"
	elif col == 6:
		res = "G"
	elif col == 7:
		res = "H"

	res = res + str(row)

	return res


def colorBoard(boardHeight, boardWidth, screen, colorBoard, validMoves=None):

	squareWidth = boardWidth / 8
	squareHeight = boardHeight / 8

	lightSquareColor = params.lightSquareColor
	darkSquareColor = params.darkSquareColor
	lightSquareColorSelected = params.lightSquareColorSelected
	darkSquareColorSelected = params.darkSquareColorSelected

	# Set Dark squares
	screen.fill(darkSquareColor)


	# Set light squares
	for height in range(8):
		for width in range(8):

			if height % 2 == 0 and width % 2 == 0:
				pg.draw.rect(screen, lightSquareColor, (squareWidth * width, squareHeight * height, squareWidth, squareHeight))

			if height % 2 == 1 and width % 2 == 1:
				pg.draw.rect(screen, lightSquareColor, (squareWidth * width, squareHeight * height, squareWidth, squareHeight))


	if validMoves is not None and validMoves.shape[0] > 0:
		validMoves = validMoves.astype('U2')
		if colorBoard == "white":
			for height in range(8):
				for width in range(8):
					c = width + 65
					r = 7 - height + 1
					if chr(c) + str(r) in validMoves:
						if (height % 2 == 0 and width % 2 == 0) or (height % 2 == 1 and width % 2 == 1):
							pg.draw.rect(screen, lightSquareColorSelected, (squareWidth * width, squareHeight * height, squareWidth, squareHeight))
						else:
							pg.draw.rect(screen, darkSquareColorSelected, (squareWidth * width, squareHeight * height, squareWidth, squareHeight))
		else:
			for height in range(8):
				for width in range(8):
					c = 7 - width + 65
					r = height + 1
					if chr(c) + str(r) in validMoves:
						if (height % 2 == 0 and width % 2 == 0) or (height % 2 == 1 and width % 2 == 1):
							pg.draw.rect(screen, lightSquareColorSelected, (squareWidth * width, squareHeight * height, squareWidth, squareHeight))
						else:
							pg.draw.rect(screen, darkSquareColorSelected, (squareWidth * width, squareHeight * height, squareWidth, squareHeight))



	return screen


def createBoard(boardHeight, boardWidth):
	pg.init()



	gameDisplay = pg.display.set_mode((boardWidth, boardHeight))
	pg.display.set_caption('Chess')

	gameDisplay = colorBoard(boardHeight, boardWidth, gameDisplay, 'white')

	return gameDisplay


def initPositions(h, w):
	resW = {}
	resB = {}
	for i in range(65, 73):
		for j in range(1, 9):
			pos = chr(i) + str(j)
			resW[pos] = position2Location(h, w, "white", pos)
			resB[pos] = position2Location(h, w, "black", pos)

	return (resW, resB)



class Piece:
	def __init__(self, color, color_side, initPos, type, img):

		self.color = color
		self.color_side = color_side
		self.pos   = initPos
		self.type  = type
		self.image = img
		shape = self.image.get_rect()
		shape.center = self.pos
		self.rect = shape
		self.moved = False


	def show(self, board):
		board.blit(self.image, self.rect)

	def movePiece(self, loc):
		if self.pos != loc:
			self.moved = True
		self.pos = loc
		self.rect.center = loc

	def dragPiece(self, loc):
		self.rect.center = loc

	def validMoves(self, boardPos):
		return []



class King(Piece):
	def __init__(self, color, color_side, initPos, type, img):
		super().__init__(color, color_side, initPos, type, img)
		self.value = 10000


	def validMoves(self, boardPos):
		res = np.array([], dtype='<S2')
		currentPos = getPosition(params.board_height, params.board_width, self.color_side, self.pos)
		row = int(currentPos[1])
		col = currentPos[0]

		col_n = ord(col)
		if col_n + 1 < 73:
			pos = boardPos[chr(col_n + 1) + str(row)]
			if pos[0]:
				if pos[1].color != self.color:
					res = np.insert(res, 0, chr(col_n + 1) + str(row))
			else:
				res = np.insert(res, 0, chr(col_n + 1) + str(row))

			if row + 1 < 9:
				pos = boardPos[chr(col_n + 1) + str(row + 1)]
				if pos[0]:
					if pos[1].color != self.color:
						res = np.insert(res, 0, chr(col_n + 1) + str(row + 1))
				else:
					res = np.insert(res, 0, chr(col_n + 1) + str(row + 1))

			if row - 1 > 0:
				pos = boardPos[chr(col_n + 1) + str(row - 1)]
				if pos[0]:
					if pos[1].color != self.color:
						res = np.insert(res, 0, chr(col_n + 1) + str(row - 1))
				else:
					res = np.insert(res, 0, chr(col_n + 1) + str(row - 1))


		if col_n - 1 > 64:
			pos = boardPos[chr(col_n - 1) + str(row)]
			if pos[0]:
				if pos[1].color != self.color:
					res = np.insert(res, 0, chr(col_n - 1) + str(row))
			else:
				res = np.insert(res, 0, chr(col_n - 1) + str(row))

			if row + 1 < 9:
				pos = boardPos[chr(col_n - 1) + str(row + 1)]
				if pos[0]:
					if pos[1].color != self.color:
						res = np.insert(res, 0, chr(col_n - 1) + str(row + 1))
				else:
					res = np.insert(res, 0, chr(col_n - 1) + str(row + 1))

			if row - 1 > 0:
				pos = boardPos[chr(col_n - 1) + str(row - 1)]
				if pos[0]:
					if pos[1].color != self.color:
						res = np.insert(res, 0, chr(col_n - 1) + str(row - 1))
				else:
					res = np.insert(res, 0, chr(col_n - 1) + str(row - 1))


		if row + 1 < 9:
			pos = boardPos[chr(col_n) + str(row + 1)]
			if pos[0]:
				if pos[1].color != self.color:
					res = np.insert(res, 0, chr(col_n) + str(row + 1))
			else:
				res = np.insert(res, 0, chr(col_n) + str(row + 1))

		if row - 1 > 0:
			pos = boardPos[chr(col_n) + str(row - 1)]
			if pos[0]:
				if pos[1].color != self.color:
					res = np.insert(res, 0, chr(col_n) + str(row - 1))
			else:
				res = np.insert(res, 0, chr(col_n) + str(row - 1))

		return res


class Queen(Piece):
	def __init__(self, color, color_side, initPos, type, img):
		super().__init__(color, color_side, initPos, type, img)
		self.value = 9


	def validMoves(self, boardPos):
		res = np.array([], dtype='<S2')
		currentPos = getPosition(params.board_height, params.board_width, self.color_side, self.pos)
		row = int(currentPos[1])
		col = currentPos[0]

		for i in range(row + 1, 9, 1):
			pos = boardPos[col + str(i)]
			if pos[0]:
				if pos[1].color != self.color:
					res = np.insert(res, 0, col + str(i))
					break
				else:
					break
			else:
				res = np.insert(res, 0, col + str(i))

		for i in range(row - 1, 0, -1):
			pos = boardPos[col + str(i)]
			if pos[0]:
				if pos[1].color != self.color:
					res = np.insert(res, 0, col + str(i))
					break
				else:
					break
			else:
				res = np.insert(res, 0, col + str(i))

		for i in range(ord(col) + 1, 73, 1):
			pos = boardPos[chr(i) + str(row)]
			if pos[0]:
				if pos[1].color != self.color:
					res = np.insert(res, 0, chr(i) + str(row))
					break
				else:
					break
			else:
				res = np.insert(res, 0, chr(i) + str(row))

		for i in range(ord(col) - 1, 64, -1):
			pos = boardPos[chr(i) + str(row)]
			if pos[0]:
				if pos[1].color != self.color:
					res = np.insert(res, 0, chr(i) + str(row))
					break
				else:
					break
			else:
				res = np.insert(res, 0, chr(i) + str(row))

		col_n = ord(col)

		count = 1
		for i in range(col_n + 1, 73):
			if row + count < 9:
				pos = boardPos[chr(i) + str(row + count)]
				if pos[0]:
					if pos[1].color != self.color:
						res = np.insert(res, 0, chr(i) + str(row + count))
						break
					else:
						break
				else:
					res = np.insert(res, 0, chr(i) + str(row + count))
				count = count + 1
			else:
				break

		count = 1
		for i in range(col_n - 1, 64, -1):
			if row + count < 9:
				pos = boardPos[chr(i) + str(row + count)]
				if pos[0]:
					if pos[1].color != self.color:
						res = np.insert(res, 0, chr(i) + str(row + count))
						break
					else:
						break
				else:
					res = np.insert(res, 0, chr(i) + str(row + count))
				count = count + 1
			else:
				break

		count = -1
		for i in range(col_n + 1, 73):
			if row + count > 0:
				pos = boardPos[chr(i) + str(row + count)]
				if pos[0]:
					if pos[1].color != self.color:
						res = np.insert(res, 0, chr(i) + str(row + count))
						break
					else:
						break
				else:
					res = np.insert(res, 0, chr(i) + str(row + count))
				count = count - 1
			else:
				break

		count = -1
		for i in range(col_n - 1, 64, -1):
			if row + count > 0:
				pos = boardPos[chr(i) + str(row + count)]
				if pos[0]:
					if pos[1].color != self.color:
						res = np.insert(res, 0, chr(i) + str(row + count))
						break
					else:
						break
				else:
					res = np.insert(res, 0, chr(i) + str(row + count))
				count = count - 1
			else:
				break

		return np.unique(res)

class Rook(Piece):
	def __init__(self, color, color_side, initPos, type, img):
		super().__init__(color, color_side, initPos, type, img)
		self.value = 5


	def validMoves(self, boardPos):
		res = np.array([], dtype='<S2')
		currentPos = getPosition(params.board_height, params.board_width, self.color_side, self.pos)
		row = int(currentPos[1])
		col = currentPos[0]

		for i in range(row + 1, 9, 1):
			pos = boardPos[col + str(i)]
			if pos[0]:
				if pos[1].color != self.color:
					res = np.insert(res, 0, col + str(i))
					break
				else:
					break
			else:
				res = np.insert(res, 0, col + str(i))

		for i in range(row - 1, 0, -1):
			pos = boardPos[col + str(i)]
			if pos[0]:
				if pos[1].color != self.color:
					res = np.insert(res, 0, col + str(i))
					break
				else:
					break
			else:
				res = np.insert(res, 0, col + str(i))

		for i in range(ord(col) + 1, 73, 1):
			pos = boardPos[chr(i) + str(row)]
			if pos[0]:
				if pos[1].color != self.color:
					res = np.insert(res, 0, chr(i) + str(row))
					break
				else:
					break
			else:
				res = np.insert(res, 0, chr(i) + str(row))

		for i in range(ord(col) - 1, 64, -1):
			pos = boardPos[chr(i) + str(row)]
			if pos[0]:
				if pos[1].color != self.color:
					res = np.insert(res, 0, chr(i) + str(row))
					break
				else:
					break
			else:
				res = np.insert(res, 0, chr(i) + str(row))
		return res

class Bishop(Piece):
	def __init__(self, color, color_side, initPos, type, img):
		super().__init__(color, color_side, initPos, type, img)
		self.value = 3.5

	def validMoves(self, boardPos):
		res = np.array([], dtype='<S2')
		currentPos = getPosition(params.board_height, params.board_width, self.color_side, self.pos)
		row = int(currentPos[1])
		col = currentPos[0]

		col_n = ord(col)

		count = 1
		for i in range(col_n + 1, 73):
			if row + count < 9:
				pos = boardPos[chr(i) + str(row + count)]
				if pos[0]:
					if pos[1].color != self.color:
						res = np.insert(res, 0, chr(i) + str(row + count))
						break
					else:
						break
				else:
					res = np.insert(res, 0, chr(i) + str(row + count))
				count = count + 1
			else:
				break

		count = 1
		for i in range(col_n - 1, 64, -1):
			if row + count < 9:
				pos = boardPos[chr(i) + str(row + count)]
				if pos[0]:
					if pos[1].color != self.color:
						res = np.insert(res, 0, chr(i) + str(row + count))
						break
					else:
						break
				else:
					res = np.insert(res, 0, chr(i) + str(row + count))
				count = count + 1
			else:
				break

		count = -1
		for i in range(col_n + 1, 73):
			if row + count > 0:
				pos = boardPos[chr(i) + str(row + count)]
				if pos[0]:
					if pos[1].color != self.color:
						res = np.insert(res, 0, chr(i) + str(row + count))
						break
					else:
						break
				else:
					res = np.insert(res, 0, chr(i) + str(row + count))
				count = count - 1
			else:
				break

		count = -1
		for i in range(col_n - 1, 64, -1):
			if row + count > 0:
				pos = boardPos[chr(i) + str(row + count)]
				if pos[0]:
					if pos[1].color != self.color:
						res = np.insert(res, 0, chr(i) + str(row + count))
						break
					else:
						break
				else:
					res = np.insert(res, 0, chr(i) + str(row + count))
				count = count - 1
			else:
				break

		return res

class Knight(Piece):
	def __init__(self, color, color_side, initPos, type, img):
		super().__init__(color, color_side, initPos, type, img)
		self.value = 3

	def validMoves(self, boardPos):
		res = np.array([], dtype='<S2')
		currentPos = getPosition(params.board_height, params.board_width, self.color_side, self.pos)
		row = int(currentPos[1])
		col = currentPos[0]

		col_n = ord(col)

		if row + 1 < 9:
			if col_n + 2 < 73:
				pos = boardPos[chr(col_n + 2) + str(row + 1)]
				if pos[0] and pos[1].color != self.color:
					res = np.insert(res, 0, chr(col_n + 2) + str(row + 1))
				elif pos[0] == False:
					res = np.insert(res, 0, chr(col_n + 2) + str(row + 1))
			if col_n - 2 > 64:
				pos = boardPos[chr(col_n - 2) + str(row + 1)]
				if pos[0] and pos[1].color != self.color:
					res = np.insert(res, 0, chr(col_n - 2) + str(row + 1))
				elif pos[0] == False:
					res = np.insert(res, 0, chr(col_n - 2) + str(row + 1))
		if row + 2 < 9:
			if col_n + 1 < 73:
				pos = boardPos[chr(col_n + 1) + str(row + 2)]
				if pos[0] and pos[1].color != self.color:
					res = np.insert(res, 0, chr(col_n + 1) + str(row + 2))
				elif pos[0] == False:
					res = np.insert(res, 0, chr(col_n + 1) + str(row + 2))
			if col_n - 1 > 64:
				pos = boardPos[chr(col_n - 1) + str(row + 2)]
				if pos[0] and pos[1].color != self.color:
					res = np.insert(res, 0, chr(col_n - 1) + str(row + 2))
				elif pos[0] == False:
					res = np.insert(res, 0, chr(col_n - 1) + str(row + 2))
		if row - 1 > 0:
			if col_n + 2 < 73:
				pos = boardPos[chr(col_n + 2) + str(row - 1)]
				if pos[0] and pos[1].color != self.color:
					res = np.insert(res, 0, chr(col_n + 2) + str(row - 1))
				elif pos[0] == False:
					res = np.insert(res, 0, chr(col_n + 2) + str(row - 1))
			if col_n - 2 > 64:
				pos = boardPos[chr(col_n - 2) + str(row - 1)]
				if pos[0] and pos[1].color != self.color:
					res = np.insert(res, 0, chr(col_n - 2) + str(row - 1))
				elif pos[0] == False:
					res = np.insert(res, 0, chr(col_n - 2) + str(row - 1))
		if row - 2 > 0:
			if col_n + 1 < 73:
				pos = boardPos[chr(col_n + 1) + str(row - 2)]
				if pos[0] and pos[1].color != self.color:
					res = np.insert(res, 0, chr(col_n + 1) + str(row - 2))
				elif pos[0] == False:
					res = np.insert(res, 0, chr(col_n + 1) + str(row - 2))
			if col_n - 1 > 64:
				pos = boardPos[chr(col_n - 1) + str(row - 2)]
				if pos[0] and pos[1].color != self.color:
					res = np.insert(res, 0, chr(col_n - 1) + str(row - 2))
				elif pos[0] == False:
					res = np.insert(res, 0, chr(col_n - 1) + str(row - 2))


		return res




class Pawn(Piece):
	def __init__(self, color, color_side, initPos, type, img):
		super().__init__(color, color_side, initPos, type, img)
		self.value = 1


	def validMoves(self, boardPos):
		res = np.array([], dtype='<S2')
		currentPos = getPosition(params.board_height, params.board_width, self.color_side, self.pos)
		row = int(currentPos[1])
		col = currentPos[0]


		if self.color == "white":
			if row + 1 > 8:
				return res

			pos_1 = boardPos[col + str(row + 1)]
			if pos_1[0] == False:
				res = np.insert(res, 0, col + str(row + 1))

			if row + 2 < 9:
				pos_2 = boardPos[col + str(row + 2)]
				if pos_1[0] == False and pos_2[0] == False and self.moved == False:
					res = np.insert(res, 0, col + str(row + 2))

			if ord(col) + 1 < 73:
				pos_3 = boardPos[chr(ord(col) + 1) + str(row + 1)]
				if pos_3[0] and pos_3[1].color != self.color:
					res = np.insert(res, 0, chr(ord(col) + 1) + str(row + 1))

			if ord(col) - 1 > 64:
				pos_4 = boardPos[chr(ord(col) - 1) + str(row + 1)]
				if pos_4[0] and pos_4[1].color != self.color:
					res = np.insert(res, 0, chr(ord(col) - 1) + str(row + 1))


		else:
			if row - 1 < 1:
				return res

			pos_1 = boardPos[col + str(row - 1)]
			if pos_1[0] == False:
				res = np.insert(res, 0, col + str(row - 1))

			if row - 2 > 0:
				pos_2 = boardPos[col + str(row - 2)]
				if pos_1[0] == False and pos_2[0] == False and self.moved == False:
					res = np.insert(res, 0, col + str(row - 2))

			if ord(col) + 1 < 73:
				pos_3 = boardPos[chr(ord(col) + 1) + str(row - 1)]
				if pos_3[0] and pos_3[1].color != self.color:
					res = np.insert(res, 0, chr(ord(col) + 1) + str(row - 1))

			if ord(col) - 1 > 64:
				pos_4 = boardPos[chr(ord(col) - 1) + str(row - 1)]
				if pos_4[0] and pos_4[1].color != self.color:
					res = np.insert(res, 0, chr(ord(col) - 1) + str(row - 1))



		return res





class Board:
	def __init__(self, color, positions):

		self.pieces = []
		self.color = color
		self.positions = {}
		self.occupied_squares = {}
		self.prevPiece = None
		self.turn = 'white'
		self.moveNumber = 0
		self.traversedNodes = 0
		self.model = ChessModel()
		#self.model.load_state_dict(torch.load("model.pwf"))
		self.allSet = []
		self.optimizer = optim.SGD(self.model.parameters(), lr=0.1)


		if self.color == 'white':
			self.positions = positions[0]
		else:
			self.positions = positions[1]

		#White pieces
		for i in range(65, 73):
			pos = chr(i) + "2"
			self.pieces.append(Pawn('white', color, self.positions[pos], 'p', params.white_pawn_img))

		# White King
		self.pieces.append(King('white', color, self.positions['E1'], 'k', params.white_king_img))
		# White Queen
		self.pieces.append(Queen('white', color, self.positions['D1'], 'q', params.white_queen_img))
		# White Rooks
		self.pieces.append(Rook('white', color, self.positions['A1'], 'r', params.white_rook_img))
		self.pieces.append(Rook('white', color, self.positions['H1'], 'r', params.white_rook_img))
		# White bishops
		self.pieces.append(Bishop('white', color, self.positions['C1'], 'b', params.white_bishop_img))
		self.pieces.append(Bishop('white', color, self.positions['F1'], 'b', params.white_bishop_img))
		# White knights
		self.pieces.append(Knight('white', color, self.positions['B1'], 'n', params.white_knight_img))
		self.pieces.append(Knight('white', color, self.positions['G1'], 'n', params.white_knight_img))


		#Black pieces
		for i in range(65, 73):
			pos = chr(i) + "7"
			self.pieces.append(Pawn('black', color, self.positions[pos], 'p', params.black_pawn_img))

		# Black King
		self.pieces.append(King('black', color, self.positions['E8'], 'k', params.black_king_img))
		# Black Queen
		self.pieces.append(Queen('black', color, self.positions['D8'], 'q', params.black_queen_img))
		# Black Rooks
		self.pieces.append(Rook('black', color, self.positions['A8'], 'r', params.black_rook_img))
		self.pieces.append(Rook('black', color, self.positions['H8'], 'r', params.black_rook_img))
		# Black bishops
		self.pieces.append(Bishop('black', color, self.positions['C8'], 'b', params.black_bishop_img))
		self.pieces.append(Bishop('black', color, self.positions['F8'], 'b', params.black_bishop_img))
		# Black knights
		self.pieces.append(Knight('black', color, self.positions['B8'], 'n', params.black_knight_img))
		self.pieces.append(Knight('black', color, self.positions['G8'], 'n', params.black_knight_img))

		self.updateLocation()



	def resetBoard(self):
		self.occupied_squares = {}
		self.pieces = []
		self.moveNumber = 0
		self.turn = 'white'
		ret = self.allSet
		self.allSet = []

		#White pieces
		for i in range(65, 73):
			pos = chr(i) + "2"
			self.pieces.append(Pawn('white', self.color, self.positions[pos], 'p', params.white_pawn_img))

		# White King
		self.pieces.append(King('white', self.color, self.positions['E1'], 'k', params.white_king_img))
		# White Queen
		self.pieces.append(Queen('white', self.color, self.positions['D1'], 'q', params.white_queen_img))
		# White Rooks
		self.pieces.append(Rook('white', self.color, self.positions['A1'], 'r', params.white_rook_img))
		self.pieces.append(Rook('white', self.color, self.positions['H1'], 'r', params.white_rook_img))
		# White bishops
		self.pieces.append(Bishop('white', self.color, self.positions['C1'], 'b', params.white_bishop_img))
		self.pieces.append(Bishop('white', self.color, self.positions['F1'], 'b', params.white_bishop_img))
		# White knights
		self.pieces.append(Knight('white', self.color, self.positions['B1'], 'n', params.white_knight_img))
		self.pieces.append(Knight('white', self.color, self.positions['G1'], 'n', params.white_knight_img))


		#Black pieces
		for i in range(65, 73):
			pos = chr(i) + "7"
			self.pieces.append(Pawn('black', self.color, self.positions[pos], 'p', params.black_pawn_img))

		# Black King
		self.pieces.append(King('black', self.color, self.positions['E8'], 'k', params.black_king_img))
		# Black Queen
		self.pieces.append(Queen('black', self.color, self.positions['D8'], 'q', params.black_queen_img))
		# Black Rooks
		self.pieces.append(Rook('black', self.color, self.positions['A8'], 'r', params.black_rook_img))
		self.pieces.append(Rook('black', self.color, self.positions['H8'], 'r', params.black_rook_img))
		# Black bishops
		self.pieces.append(Bishop('black', self.color, self.positions['C8'], 'b', params.black_bishop_img))
		self.pieces.append(Bishop('black', self.color, self.positions['F8'], 'b', params.black_bishop_img))
		# Black knights
		self.pieces.append(Knight('black', self.color, self.positions['B8'], 'n', params.black_knight_img))
		self.pieces.append(Knight('black', self.color, self.positions['G8'], 'n', params.black_knight_img))

		self.updateLocation()

		return ret

	def show(self, board):
		for piece in self.pieces:
			piece.show(board)
		if self.prevPiece:
			self.prevPiece.show(board)



	def updateLocation(self):
		for i in range(65, 73):
			for j in range(1, 9):
				pos = chr(i) + str(j)
				for piece in self.pieces:
					if piece.pos == self.positions[pos]:
						self.occupied_squares[pos] = (True, piece)
						break
					else:
						self.occupied_squares[pos] = (False, None)



	def dragPiece(self, loc1, loc2):
		for i in range(len(self.pieces)):
			if self.pieces[i].pos == self.positions[loc1] and self.pieces[i].color == self.turn:
				self.pieces[i].dragPiece(loc2)
				self.prevPiece = self.pieces[i]
				break

	def movePiece(self, loc1, loc2):
		mrPiece = None
		mrPiece2 = None
		mrPiece2Loc = -1
		removed = False
		castling = 0
		notation = ""
		for i in range(len(self.pieces)):
			if len(loc2) == 2 and (int(loc2[1]) <= 8 and int(loc2[1]) >= 1) and self.pieces[i].pos == self.positions[loc2]:
				mrPiece = self.pieces[i]
			if self.pieces[i].pos == self.positions[loc1] and self.turn == self.pieces[i].color:
				mrPiece2Loc = i
				mrPiece2 = self.pieces[i]


		if not mrPiece2:
			return
		else:
			vMoves = self.getValidMoves(loc1)
			vMoves = vMoves.astype('U2')
			notation += mrPiece2.type.upper()

		if mrPiece2 and loc2 not in vMoves:
			self.pieces[mrPiece2Loc].movePiece(self.positions[loc1])
			return
		elif mrPiece2 and loc2 in vMoves:
			removed = True
			if mrPiece2.type == 'k' and not mrPiece2.moved and mrPiece2.color == 'white' and loc2 == 'A1' and 'A1' in vMoves:
				for piece in self.pieces:
					if piece.pos == self.positions['A1']:
						piece.movePiece(self.positions['D1'])
						break
				self.pieces[mrPiece2Loc].movePiece(self.positions['C1'])
				mrPiece = None
				castling = 1
			elif mrPiece2.type == 'k' and not mrPiece2.moved and mrPiece2.color == 'white' and loc2 == 'C1' and 'C1' in vMoves:
				for piece in self.pieces:
					if piece.pos == self.positions['A1']:
						piece.movePiece(self.positions['D1'])
						break
				self.pieces[mrPiece2Loc].movePiece(self.positions['C1'])
				mrPiece = None
				castling = 1
			elif mrPiece2.type == 'k' and not mrPiece2.moved and mrPiece2.color == 'white' and loc2 == 'H1' and 'H1' in vMoves:
				for piece in self.pieces:
					if piece.pos == self.positions['H1']:
						piece.movePiece(self.positions['F1'])
						break
				self.pieces[mrPiece2Loc].movePiece(self.positions['G1'])
				mrPiece = None
				castling = 2
			elif mrPiece2.type == 'k' and not mrPiece2.moved and mrPiece2.color == 'white' and loc2 == 'G1' and 'G1' in vMoves:
				for piece in self.pieces:
					if piece.pos == self.positions['H1']:
						piece.movePiece(self.positions['F1'])
						break
				self.pieces[mrPiece2Loc].movePiece(self.positions['G1'])
				mrPiece = None
				castling = 2
			elif mrPiece2.type == 'k' and not mrPiece2.moved and mrPiece2.color == 'black' and loc2 == 'H8' and 'H8' in vMoves:
				for piece in self.pieces:
					if piece.pos == self.positions['H8']:
						piece.movePiece(self.positions['F8'])
						break
				self.pieces[mrPiece2Loc].movePiece(self.positions['G8'])
				mrPiece = None
				castling = 2
			elif mrPiece2.type == 'k' and not mrPiece2.moved and mrPiece2.color == 'black' and loc2 == 'G8' and 'G8' in vMoves:
				for piece in self.pieces:
					if piece.pos == self.positions['H8']:
						piece.movePiece(self.positions['F8'])
						break
				self.pieces[mrPiece2Loc].movePiece(self.positions['G8'])
				mrPiece = None
				castling = 2
			elif mrPiece2.type == 'k' and not mrPiece2.moved and mrPiece2.color == 'black' and loc2 == 'A8' and 'A8' in vMoves:
				for piece in self.pieces:
					if piece.pos == self.positions['A8']:
						piece.movePiece(self.positions['D8'])
						break
				self.pieces[mrPiece2Loc].movePiece(self.positions['C8'])
				mrPiece = None
				castling = 1
			elif mrPiece2.type == 'k' and not mrPiece2.moved and mrPiece2.color == 'black' and loc2 == 'C8' and 'C8' in vMoves:
				for piece in self.pieces:
					if piece.pos == self.positions['A8']:
						piece.movePiece(self.positions['D8'])
						break
				self.pieces[mrPiece2Loc].movePiece(self.positions['C8'])
				mrPiece = None
				castling = 1

			else:
				self.pieces[mrPiece2Loc].movePiece(self.positions[loc2])

		if mrPiece and self.positions[loc1] != self.positions[loc2] and removed:
			self.pieces.remove(mrPiece)
			notation += "x"

		self.updateLocation()

		toA = []
		for key, value in self.occupied_squares.items():
			val = 0
			if value[0]:
				val = value[1].value
				if value[1].color == "black":
					val = -val
			toA.append(float(val))

		if self.turn == 'white':
			self.turn = 'black'
			self.moveNumber += 1
		else:
			self.turn = 'white'


		self.allSet.append(torch.tensor(toA))

		#print(self.outcomeGame())
		if castling == 1:
			notation = "O-O-O"
		elif castling == 2:
			notation = "O-O"
		else:
			notation = notation + loc2.lower()
		print(self.moveNumber, notation)
		if self.outcomeGame() == 2:
			return 2
		elif self.outcomeGame() == 3:
			return 3
		elif self.outcomeGame() == 1 or self.moveNumber > 50:
			return 1

		return 0



	def getValidMoves(self, loc, board=None):
		if not board:
			ret = np.array([], dtype='<S2')
			for piece in self.pieces:
				if piece.pos == self.positions[loc]:
					ret = piece.validMoves(self.occupied_squares)
					ret = self.filterCheck(self.occupied_squares, ret, piece)

					# Castling rights
					if piece.type == 'k' and not piece.moved:
						if piece.color == 'white':
							allVMoves = self.getAllValidMoves('black')
							isRookThere = self.occupied_squares['A1']
							isCheck = self.isInCheck('white')
							if not isCheck and self.occupied_squares['A1'][0] and not self.occupied_squares['A1'][1].moved and 'B1' not in allVMoves and 'C1' not in allVMoves and 'D1' not in allVMoves:
								if not self.occupied_squares['B1'][0] and not self.occupied_squares['C1'][0] and not self.occupied_squares['D1'][0]:
									ret = np.append(ret, ['A1', 'C1', 'D1'])
							if not isCheck and self.occupied_squares['H1'][0] and not self.occupied_squares['H1'][1].moved and 'F1' not in allVMoves and 'G1' not in allVMoves :
								if not self.occupied_squares['F1'][0] and not self.occupied_squares['G1'][0]:
									ret = np.append(ret, ['F1', 'G1', 'H1'])
						else:
							allVMoves = self.getAllValidMoves('white')
							isRookThere = self.occupied_squares['A1']
							isCheck = self.isInCheck('black')
							if not isCheck and self.occupied_squares['A8'][0] and not self.occupied_squares['A8'][1].moved and 'B8' not in allVMoves and 'C8' not in allVMoves and 'D8' not in allVMoves:
								if not self.occupied_squares['B8'][0] and not self.occupied_squares['C8'][0] and not self.occupied_squares['D8'][0]:
									ret = np.append(ret, ['A8', 'C8', 'D8'])
							if not isCheck and self.occupied_squares['H8'][0] and not self.occupied_squares['H8'][1].moved and 'F8' not in allVMoves and 'G8' not in allVMoves :
								if not self.occupied_squares['F8'][0] and not self.occupied_squares['G8'][0]:
									ret = np.append(ret, ['F8', 'G8', 'H8'])
					break
		else:
			ret = np.array([], dtype='<S2')
			for key, value in board.items():
				if value[0]:
					piece = value[1]
					if piece.pos == self.positions[loc]:
						ret = piece.validMoves(board)
						ret = self.filterCheck(board, ret, piece)

						# Castling rights
						if piece.type == 'k' and not piece.moved:
							if piece.color == 'white':
								allVMoves = self.getAllValidMoves('black')
								isRookThere = board['A1']
								isCheck = self.isInCheck('white')
								if not isCheck and board['A1'][0] and not board['A1'][1].moved and 'B1' not in allVMoves and 'C1' not in allVMoves and 'D1' not in allVMoves:
									if not board['B1'][0] and not board['C1'][0] and not board['D1'][0]:
										ret = np.append(ret, ['A1', 'C1', 'D1'])
								if not isCheck and board['H1'][0] and not board['H1'][1].moved and 'F1' not in allVMoves and 'G1' not in allVMoves :
									if not board['F1'][0] and not board['G1'][0]:
										ret = np.append(ret, ['F1', 'G1', 'H1'])
							else:
								allVMoves = self.getAllValidMoves('white')
								isRookThere = board['A1']
								isCheck = self.isInCheck('black')
								if not isCheck and board['A8'][0] and not board['A8'][1].moved and 'B8' not in allVMoves and 'C8' not in allVMoves and 'D8' not in allVMoves:
									if not board['B8'][0] and not board['C8'][0] and not board['D8'][0]:
										ret = np.append(ret, ['A8', 'C8', 'D8'])
								if not isCheck and board['H8'][0] and not board['H8'][1].moved and 'F8' not in allVMoves and 'G8' not in allVMoves :
									if not board['F8'][0] and not board['G8'][0]:
										ret = np.append(ret, ['F8', 'G8', 'H8'])

						break
		return ret

	def getAllValidMoves(self, color, board=None):
		ret = np.array([], dtype='<S2')
		if board == None:
			for piece in self.pieces:
				if piece.color == color:
					ret = np.append(ret, piece.validMoves(self.occupied_squares))
		else:

			for key, value in board.items():
				if value[0] and value[1].color == color:
					ret = np.append(ret, value[1].validMoves(board))

		return np.unique(ret)

	def isInCheck(self, color, board=None):
		ret = False
		if board == None:
			if color == "white":
				AllValidMoves = self.getAllValidMoves("black")
			else:
				AllValidMoves = self.getAllValidMoves("white")
			AllValidMoves = AllValidMoves.astype('U2')


			for piece in self.pieces:
				if piece.color == color and piece.type == 'k' and getPosition(params.board_height, params.board_width, self.color, piece.pos) in AllValidMoves:
					ret = True
					break
			piece.isInCheck = ret

		else:
			if color == "white":
				AllValidMoves = self.getAllValidMoves("black", board)
			else:
				AllValidMoves = self.getAllValidMoves("white", board)
			AllValidMoves = AllValidMoves.astype('U2')

			for key, value in board.items():
				if value[0] and value[1].color == color and value[1].type == 'k' and getPosition(params.board_height, params.board_width, self.color, value[1].pos) in AllValidMoves:
					ret = True
					break

		return ret

	def filterCheck(self, boardPositions, validMoves, piece):
		validMoves = validMoves.astype('U2')
		boardCopy = boardPositions.copy()
		mrPiecePos = getPosition(params.board_height, params.board_width, self.color, piece.pos)
		boardCopy[mrPiecePos] = (False, None)
		res = np.array([], dtype='<S2')

		for pos in validMoves:

			if piece.type == 'k':
				mrPiece = King(piece.color, self.color, self.positions[pos], 'k', params.black_king_img)
			elif piece.type == 'q':
				mrPiece = Queen(piece.color, self.color, self.positions[pos], 'q', params.black_king_img)
			elif piece.type == 'r':
				mrPiece = Rook(piece.color, self.color, self.positions[pos], 'r', params.black_king_img)
			elif piece.type == 'b':
				mrPiece = Bishop(piece.color, self.color, self.positions[pos], 'b', params.black_king_img)
			elif piece.type == 'n':
				mrPiece = Knight(piece.color, self.color, self.positions[pos], 'n', params.black_king_img)
			elif piece.type == 'p':
				mrPiece = Pawn(piece.color, self.color, self.positions[pos], 'p', params.black_king_img)

			tempBoardPos = boardCopy.copy()
			tempBoardPos[pos] = (True, mrPiece)
			if not self.isInCheck(piece.color, board=tempBoardPos):
				res = np.insert(res, 0, pos)

		return res

	def isTurn(self, loc):
		for piece in self.pieces:
			if piece.pos == self.positions[loc]:
				return piece.color == self.turn

		return False

	# 0 = ongoing, 1 = draw, 2 = white wins, 3 = black wins
	def outcomeGame(self):
		valid = False
		for piece in self.pieces:
			if piece.color == self.turn:
				allVMoves = self.getValidMoves(getPosition(params.board_height, params.board_width, self.color, piece.pos))
				if len(allVMoves) > 0:
					valid = True

		if valid:
			return 0
		elif not self.isInCheck(self.turn):
			return 1
		elif self.turn == 'white':
			return 3
		elif self.turn == 'black':
			return 2

	def minimax(self, turn):
		mrMove = None
		if turn == "white" and self.turn == 'black':
			mrMove = self.minimaxR(False, self.occupied_squares, 2)

		if turn == "black" and self.turn == 'white':
			mrMove = self.minimaxR(True, self.occupied_squares, 2)

		print('Number of Nodes traversed:', self.traversedNodes)
		self.traversedNodes = 0

		#print(mrMove, self.traversedNodes)
		if mrMove and mrMove[0] == m.inf:
			return 2
		if mrMove and mrMove[0] == -m.inf:
			return 3
		if mrMove and mrMove[1] and mrMove[2]:
			return self.movePiece(mrMove[1], mrMove[2])

		return 0

	def minimaxR(self, useMax, boardPos, maxDepth, depth=0, alpha=-m.inf, beta=m.inf):

		self.traversedNodes += 1
		if depth == maxDepth:
			return self.evaluateBoard(boardPos), None, None

		res = 0
		prevMove = ""
		bestMove = ""
		loc = ""


		if useMax:
			res = -m.inf
		else:
			res = m.inf

		if useMax:
			for key, value in boardPos.items():
				isPiece = value[0]
				piece = value[1]
				if isPiece and piece.color == 'white':
					coords = getPosition(params.board_height, params.board_width, self.color, value[1].pos)
					vMoves = self.getValidMoves(coords, boardPos)
					vMoves = vMoves.astype('U2')
					if vMoves.shape[0] > 0:
						for move in vMoves:
							tmpBoard = boardPos.copy()

							if piece.type == 'k':
								mrPiece = King(piece.color, self.color, self.positions[move], 'k', params.black_king_img)
							elif piece.type == 'q':
								mrPiece = Queen(piece.color, self.color, self.positions[move], 'q', params.black_king_img)
							elif piece.type == 'r':
								mrPiece = Rook(piece.color, self.color, self.positions[move], 'r', params.black_king_img)
							elif piece.type == 'b':
								mrPiece = Bishop(piece.color, self.color, self.positions[move], 'b', params.black_king_img)
							elif piece.type == 'n':
								mrPiece = Knight(piece.color, self.color, self.positions[move], 'n', params.black_king_img)
							elif piece.type == 'p':
								mrPiece = Pawn(piece.color, self.color, self.positions[move], 'p', params.black_king_img)

							tmpBoard[move] = (True, mrPiece)
							tmpBoard[coords] = (False, None)
							ret = self.minimaxR(False, tmpBoard, maxDepth, depth=depth+1, alpha=alpha, beta=beta)[0]

							if ret > res:
								res = ret
								loc = coords
								bestMove = move

							alpha = max(alpha, res)
							if alpha >= beta:
								break

		else:
			for key, value in boardPos.items():
				isPiece = value[0]
				piece = value[1]
				if isPiece and piece.color == 'black':
					coords = getPosition(params.board_height, params.board_width, self.color, value[1].pos)
					vMoves = self.getValidMoves(coords, boardPos)
					vMoves = vMoves.astype('U2')
					if vMoves.shape[0] > 0:
						for move in vMoves:
							tmpBoard = boardPos.copy()

							if piece.type == 'k':
								mrPiece = King(piece.color, self.color, self.positions[move], 'k', params.black_king_img)
							elif piece.type == 'q':
								mrPiece = Queen(piece.color, self.color, self.positions[move], 'q', params.black_king_img)
							elif piece.type == 'r':
								mrPiece = Rook(piece.color, self.color, self.positions[move], 'r', params.black_king_img)
							elif piece.type == 'b':
								mrPiece = Bishop(piece.color, self.color, self.positions[move], 'b', params.black_king_img)
							elif piece.type == 'n':
								mrPiece = Knight(piece.color, self.color, self.positions[move], 'n', params.black_king_img)
							elif piece.type == 'p':
								mrPiece = Pawn(piece.color, self.color, self.positions[move], 'p', params.black_king_img)

							tmpBoard[move] = (True, mrPiece)
							tmpBoard[coords] = (False, None)
							ret = self.minimaxR(True, tmpBoard, maxDepth, depth=depth+1, alpha=alpha, beta=beta)[0]
							if ret < res:
								res = ret
								loc = coords
								bestMove = move

							beta = min(beta, res)
							if alpha >= beta:
								break


		return res, loc, bestMove

	def evaluateBoard(self, boardPos):
		res = r.uniform(-0.025, 0.025)
		self.model.eval()

		toA = []
		for key, value in boardPos.items():
			val = 0
			if value[0]:
				val = value[1].value
				if value[1].color == "black":
					val = -val
			toA.append(float(val))

		ress = torch.tensor(toA)

		# for key, value in boardPos.items():
		# 	if value[0]:
		# 		if value[1].color == 'white':
		# 			#res += len(self.getValidMoves(getPosition(params.board_height, params.board_width, self.color, value[1].pos), board=boardPos)) / 10
		# 			res += value[1].value
		# 		else:
		# 			#res -= len(self.getValidMoves(getPosition(params.board_height, params.board_width, self.color, value[1].pos), board=boardPos)) / 10
		# 			res -= value[1].value
		# self.model(boardPos)

		print(float(self.model(ress)[1]) + res)
		return float(self.model(ress)[1]) + res

	def Train(self, Positions, result):
		positions = torch.stack(Positions)
		result = result - 1
		loss_function = nn.NLLLoss()

		self.model.train()
		loss = float("inf")

		for epoch in range(1):
			self.optimizer.zero_grad()
			self.model.zero_grad()
			scores = self.model(positions)
			loss = loss_function(scores, torch.tensor([result] * len(Positions)))
			loss.backward()
			self.optimizer.step()

		torch.save(self.model.state_dict(), "model.pwf")



class ChessModel(nn.Module):
	def __init__(self):
		super(ChessModel, self).__init__()
		self.fc1 = nn.Linear(64, 128)
		self.relu1 = nn.ReLU()
		self.fc2 = nn.Linear(128, 128)
		self.relu2 = nn.ReLU()
		self.fc3 = nn.Linear(128, 3)

	def forward(self, chessFeatures):
	    # Max pooling over a (2, 2) window

		out = self.fc1(chessFeatures)
		out = self.relu1(out)
		out = self.fc3(out)
		return out



def main():

	r.seed(datetime.datetime.now())
	# Get width and height of the board
	width = params.board_width
	height = params.board_height
	color_side = 'white'


	chessBoard = createBoard(height, width) # Pygame object of the board
	positions = initPositions(height, width) # returns the coordinates of the board. for example: specifys where the A1 square is
	boardPieces = Board(color_side, positions) # Class that displays all the board pieces

	clock = pg.time.Clock()
	crashed = False


	isDragging = False
	mouse_prev = None
	showValid = None
	epochs = 30
	while not crashed:
		for event in pg.event.get():

			mouse_pos = getPosition(height, width, color_side, pg.mouse.get_pos())

			if event.type == pg.QUIT:
				crashed = True
			if event.type == pg.MOUSEBUTTONDOWN and event.button == 1:
				isDragging = True
				mouse_prev = pg.mouse.get_pos()
				showValid = boardPieces.getValidMoves(mouse_pos)
				if not boardPieces.isTurn(mouse_pos):
					showValid = None
				#showValid = boardPieces.getAllValidMoves("white")
				boardPieces.dragPiece(mouse_pos, event.pos)

			if event.type == pg.MOUSEBUTTONDOWN and event.button == 3:
				isDragging = False
				if mouse_prev:
					showValid = None
					boardPieces.movePiece(getPosition(height, width, color_side, mouse_prev), getPosition(height, width, color_side, mouse_prev))

			if event.type == pg.MOUSEBUTTONUP and event.button == 1:

				if isDragging and mouse_prev:
					showValid = None
					boardPieces.movePiece(getPosition(height, width, color_side, mouse_prev), getPosition(height, width, color_side, pg.mouse.get_pos()))
					mouse_prev = None
					isDragging = False


			if event.type == pg.MOUSEMOTION:
				if isDragging:
					boardPieces.dragPiece(getPosition(height, width, color_side, mouse_prev), event.pos)

			#print(boardPieces.outcomeGame())
			# print(getPosition(height, width, color_side, pg.mouse.get_pos()), position2Location(height, width, color_side, getPosition(height, width, color_side, pg.mouse.get_pos())), pg.mouse.get_pos())


		chessBoard = colorBoard(height, width, chessBoard, color_side, showValid)
		boardPieces.show(chessBoard)
		pg.display.flip()
		mm = 0
		if boardPieces.turn == "white":
			boardPieces.prevPiece = None
			mm = boardPieces.minimax('black')
			pg.display.flip()
		#mm = 0
		elif boardPieces.turn == "black":
			boardPieces.prevPiece = None
			mm = boardPieces.minimax('white')
			pg.display.flip()

		if mm == 2:
			print("White wins!")
			#os.system("pause")
			boardPieces.Train(boardPieces.resetBoard(), mm)
			# crashed = True
		if mm == 3:
			print("Black wins!")
			#os.system("pause")
			boardPieces.Train(boardPieces.resetBoard(), mm)
			# crashed = True
		if mm == 1:
			print("Drawn game")

			#os.system("pause")
			boardPieces.Train(boardPieces.resetBoard(), mm)
			# crashed = True

		clock.tick(144)


	pg.quit()
	quit()
	return 0


if __name__=='__main__':
    main()
