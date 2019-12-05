import pygame as pg
import os
import math as m
import params
import copy
import random as r
import pandas as pd
import numpy as np
import datetime


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


	if validMoves and len(validMoves) > 0:
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


	def validMoves(self, boardPos, newBoard=None):
		res = []
		currentPos = getPosition(params.board_height, params.board_width, self.color_side, self.pos)
		row = int(currentPos[1])
		col = currentPos[0]

		col_n = ord(col)

		if newBoard:
			if col_n + 1 < 73:
				pos = newBoard[newBoard['position'] == chr(col_n + 1) + str(row)].values
				if pos[1] != 'x':
					if pos[2] != self.color:
						res.append(chr(col_n + 1) + str(row))
				else:
					res.append(chr(col_n + 1) + str(row))

				if row + 1 < 9:
					pos = newBoard[newBoard['position'] == chr(col_n + 1) + str(row + 1)].values
					if pos[1] != 'x':
						if pos[2] != self.color:
							res.append(chr(col_n + 1) + str(row + 1))
					else:
						res.append(chr(col_n + 1) + str(row + 1))

				if row - 1 > 0:
					pos = newBoard[newBoard['position'] == chr(col_n + 1) + str(row - 1)].values
					if pos[1] != 'x':
						if pos[2] != self.color:
							res.append(chr(col_n + 1) + str(row - 1))
					else:
						res.append(chr(col_n + 1) + str(row - 1))


			if col_n - 1 > 64:
				pos = newBoard[newBoard['position'] == chr(col_n - 1) + str(row)].values
				if pos[1] != 'x':
					if pos[2] != self.color:
						res.append(chr(col_n - 1) + str(row))
				else:
					res.append(chr(col_n - 1) + str(row))

				if row + 1 < 9:
					pos = newBoard[newBoard['position'] == chr(col_n - 1) + str(row + 1)].values
					if pos[1] != 'x':
						if pos[2] != self.color:
							res.append(chr(col_n - 1) + str(row + 1))
					else:
						res.append(chr(col_n - 1) + str(row + 1))

				if row - 1 > 0:
					pos = newBoard[newBoard['position'] == chr(col_n - 1) + str(row - 1)].values
					if pos[1] != 'x':
						if pos[2] != self.color:
							res.append(chr(col_n - 1) + str(row - 1))
					else:
						res.append(chr(col_n - 1) + str(row - 1))


			if row + 1 < 9:
				pos = boardPos[chr(col_n) + str(row + 1)]
				if pos[1] != 'x':
					if pos[2] != self.color:
						res.append(chr(col_n) + str(row + 1))
				else:
					res.append(chr(col_n) + str(row + 1))

			if row - 1 > 0:
				pos = boardPos[chr(col_n) + str(row - 1)]
				if pos[1] != 'x':
					if pos[2] != self.color:
						res.append(chr(col_n) + str(row - 1))
				else:
					res.append(chr(col_n) + str(row - 1))
			return res

		else:

			if col_n + 1 < 73:
				pos = boardPos[chr(col_n + 1) + str(row)]
				if pos[0]:
					if pos[1].color != self.color:
						res.append(chr(col_n + 1) + str(row))
				else:
					res.append(chr(col_n + 1) + str(row))

				if row + 1 < 9:
					pos = boardPos[chr(col_n + 1) + str(row + 1)]
					if pos[0]:
						if pos[1].color != self.color:
							res.append(chr(col_n + 1) + str(row + 1))
					else:
						res.append(chr(col_n + 1) + str(row + 1))

				if row - 1 > 0:
					pos = boardPos[chr(col_n + 1) + str(row - 1)]
					if pos[0]:
						if pos[1].color != self.color:
							res.append(chr(col_n + 1) + str(row - 1))
					else:
						res.append(chr(col_n + 1) + str(row - 1))


			if col_n - 1 > 64:
				pos = boardPos[chr(col_n - 1) + str(row)]
				if pos[0]:
					if pos[1].color != self.color:
						res.append(chr(col_n - 1) + str(row))
				else:
					res.append(chr(col_n - 1) + str(row))

				if row + 1 < 9:
					pos = boardPos[chr(col_n - 1) + str(row + 1)]
					if pos[0]:
						if pos[1].color != self.color:
							res.append(chr(col_n - 1) + str(row + 1))
					else:
						res.append(chr(col_n - 1) + str(row + 1))

				if row - 1 > 0:
					pos = boardPos[chr(col_n - 1) + str(row - 1)]
					if pos[0]:
						if pos[1].color != self.color:
							res.append(chr(col_n - 1) + str(row - 1))
					else:
						res.append(chr(col_n - 1) + str(row - 1))


			if row + 1 < 9:
				pos = boardPos[chr(col_n) + str(row + 1)]
				if pos[0]:
					if pos[1].color != self.color:
						res.append(chr(col_n) + str(row + 1))
				else:
					res.append(chr(col_n) + str(row + 1))

			if row - 1 > 0:
				pos = boardPos[chr(col_n) + str(row - 1)]
				if pos[0]:
					if pos[1].color != self.color:
						res.append(chr(col_n) + str(row - 1))
				else:
					res.append(chr(col_n) + str(row - 1))

		return res


class Queen(Piece):
	def __init__(self, color, color_side, initPos, type, img):
		super().__init__(color, color_side, initPos, type, img)
		self.value = 9


	def validMoves(self, boardPos, newBoard=None):
		res = []
		currentPos = getPosition(params.board_height, params.board_width, self.color_side, self.pos)
		row = int(currentPos[1])
		col = currentPos[0]

		if newBoard:
			for i in range(row + 1, 9, 1):
				pos = newBoard[newBoard['position'] == col + str(i)].values
				if pos[0]:
					if pos[2] != self.color:
						res.append(col + str(i))
						break
					else:
						break
				else:
					res.append(col + str(i))

			for i in range(row - 1, 0, -1):
				pos = newBoard[newBoard['position'] == col + str(i)].values
				if pos[1] != 'x':
					if pos[2] != self.color:
						res.append(col + str(i))
						break
					else:
						break
				else:
					res.append(col + str(i))

			for i in range(ord(col) + 1, 73, 1):
				pos = newBoard[newBoard['position'] == chr(i) + str(row)].values
				if pos[1] != 'x':
					if pos[2] != self.color:
						res.append(chr(i) + str(row))
						break
					else:
						break
				else:
					res.append(chr(i) + str(row))

			for i in range(ord(col) - 1, 64, -1):
				pos = newBoard[newBoard['position'] == chr(i) + str(row)].values
				if pos[1] != 'x':
					if pos[2] != self.color:
						res.append(chr(i) + str(row))
						break
					else:
						break
				else:
					res.append(chr(i) + str(row))

			count = 1
			for i in range(col_n + 1, 73):
				if row + count < 9:
					pos = newBoard[newBoard['position'] == chr(i) + str(row + count)].values
					if pos[1] != 'x':
						if pos[2] != self.color:
							res.append(chr(i) + str(row + count))
							break
						else:
							break
					else:
						res.append(chr(i) + str(row + count))
					count = count + 1
				else:
					break

			count = 1
			for i in range(col_n - 1, 64, -1):
				if row + count < 9:
					pos = newBoard[newBoard['position'] == chr(i) + str(row + count)].values
					if pos[1] != 'x':
						if pos[2] != self.color:
							res.append(chr(i) + str(row + count))
							break
						else:
							break
					else:
						res.append(chr(i) + str(row + count))
					count = count + 1
				else:
					break

			count = -1
			for i in range(col_n + 1, 73):
				if row + count > 0:
					pos = newBoard[newBoard['position'] == chr(i) + str(row + count)].values
					if pos[1] != 'x':
						if pos[2] != self.color:
							res.append(chr(i) + str(row + count))
							break
						else:
							break
					else:
						res.append(chr(i) + str(row + count))
					count = count - 1
				else:
					break

			count = -1
			for i in range(col_n - 1, 64, -1):
				if row + count > 0:
					pos = newBoard[newBoard['position'] == chr(i) + str(row + count)].values
					if pos[1] != 'x':
						if pos[2] != self.color:
							res.append(chr(i) + str(row + count))
							break
						else:
							break
					else:
						res.append(chr(i) + str(row + count))
					count = count - 1
				else:
					break
			return set(res)

		else:

			for i in range(row + 1, 9, 1):
				pos = boardPos[col + str(i)]
				if pos[0]:
					if pos[1].color != self.color:
						res.append(col + str(i))
						break
					else:
						break
				else:
					res.append(col + str(i))

			for i in range(row - 1, 0, -1):
				pos = boardPos[col + str(i)]
				if pos[0]:
					if pos[1].color != self.color:
						res.append(col + str(i))
						break
					else:
						break
				else:
					res.append(col + str(i))

			for i in range(ord(col) + 1, 73, 1):
				pos = boardPos[chr(i) + str(row)]
				if pos[0]:
					if pos[1].color != self.color:
						res.append(chr(i) + str(row))
						break
					else:
						break
				else:
					res.append(chr(i) + str(row))

			for i in range(ord(col) - 1, 64, -1):
				pos = boardPos[chr(i) + str(row)]
				if pos[0]:
					if pos[1].color != self.color:
						res.append(chr(i) + str(row))
						break
					else:
						break
				else:
					res.append(chr(i) + str(row))

			col_n = ord(col)

			count = 1
			for i in range(col_n + 1, 73):
				if row + count < 9:
					pos = boardPos[chr(i) + str(row + count)]
					if pos[0]:
						if pos[1].color != self.color:
							res.append(chr(i) + str(row + count))
							break
						else:
							break
					else:
						res.append(chr(i) + str(row + count))
					count = count + 1
				else:
					break

			count = 1
			for i in range(col_n - 1, 64, -1):
				if row + count < 9:
					pos = boardPos[chr(i) + str(row + count)]
					if pos[0]:
						if pos[1].color != self.color:
							res.append(chr(i) + str(row + count))
							break
						else:
							break
					else:
						res.append(chr(i) + str(row + count))
					count = count + 1
				else:
					break

			count = -1
			for i in range(col_n + 1, 73):
				if row + count > 0:
					pos = boardPos[chr(i) + str(row + count)]
					if pos[0]:
						if pos[1].color != self.color:
							res.append(chr(i) + str(row + count))
							break
						else:
							break
					else:
						res.append(chr(i) + str(row + count))
					count = count - 1
				else:
					break

			count = -1
			for i in range(col_n - 1, 64, -1):
				if row + count > 0:
					pos = boardPos[chr(i) + str(row + count)]
					if pos[0]:
						if pos[1].color != self.color:
							res.append(chr(i) + str(row + count))
							break
						else:
							break
					else:
						res.append(chr(i) + str(row + count))
					count = count - 1
				else:
					break

		return set(res)

class Rook(Piece):
	def __init__(self, color, color_side, initPos, type, img):
		super().__init__(color, color_side, initPos, type, img)
		self.value = 5


	def validMoves(self, boardPos, newBoard=None):
		res = []
		currentPos = getPosition(params.board_height, params.board_width, self.color_side, self.pos)
		row = int(currentPos[1])
		col = currentPos[0]

		if newBoard:
			for i in range(row + 1, 9, 1):
				pos = newBoard[newBoard['position'] == col + str(i)].values
				if pos[0]:
					if pos[2] != self.color:
						res.append(col + str(i))
						break
					else:
						break
				else:
					res.append(col + str(i))

			for i in range(row - 1, 0, -1):
				pos = newBoard[newBoard['position'] == col + str(i)].values
				if pos[1] != 'x':
					if pos[2] != self.color:
						res.append(col + str(i))
						break
					else:
						break
				else:
					res.append(col + str(i))

			for i in range(ord(col) + 1, 73, 1):
				pos = newBoard[newBoard['position'] == chr(i) + str(row)].values
				if pos[1] != 'x':
					if pos[2] != self.color:
						res.append(chr(i) + str(row))
						break
					else:
						break
				else:
					res.append(chr(i) + str(row))

			for i in range(ord(col) - 1, 64, -1):
				pos = newBoard[newBoard['position'] == chr(i) + str(row)].values
				if pos[1] != 'x':
					if pos[2] != self.color:
						res.append(chr(i) + str(row))
						break
					else:
						break
				else:
					res.append(chr(i) + str(row))
			return res
		else:

			for i in range(row + 1, 9, 1):
				pos = boardPos[col + str(i)]
				if pos[0]:
					if pos[1].color != self.color:
						res.append(col + str(i))
						break
					else:
						break
				else:
					res.append(col + str(i))

			for i in range(row - 1, 0, -1):
				pos = boardPos[col + str(i)]
				if pos[0]:
					if pos[1].color != self.color:
						res.append(col + str(i))
						break
					else:
						break
				else:
					res.append(col + str(i))

			for i in range(ord(col) + 1, 73, 1):
				pos = boardPos[chr(i) + str(row)]
				if pos[0]:
					if pos[1].color != self.color:
						res.append(chr(i) + str(row))
						break
					else:
						break
				else:
					res.append(chr(i) + str(row))

			for i in range(ord(col) - 1, 64, -1):
				pos = boardPos[chr(i) + str(row)]
				if pos[0]:
					if pos[1].color != self.color:
						res.append(chr(i) + str(row))
						break
					else:
						break
				else:
					res.append(chr(i) + str(row))
		return res

class Bishop(Piece):
	def __init__(self, color, color_side, initPos, type, img):
		super().__init__(color, color_side, initPos, type, img)
		self.value = 3.5

	def validMoves(self, boardPos, newBoard=None):
		res = []
		currentPos = getPosition(params.board_height, params.board_width, self.color_side, self.pos)
		row = int(currentPos[1])
		col = currentPos[0]

		col_n = ord(col)

		if newBoard:
			count = 1
			for i in range(col_n + 1, 73):
				if row + count < 9:
					pos = newBoard[newBoard['position'] == chr(i) + str(row + count)].values
					if pos[1] != 'x':
						if pos[2] != self.color:
							res.append(chr(i) + str(row + count))
							break
						else:
							break
					else:
						res.append(chr(i) + str(row + count))
					count = count + 1
				else:
					break

			count = 1
			for i in range(col_n - 1, 64, -1):
				if row + count < 9:
					pos = newBoard[newBoard['position'] == chr(i) + str(row + count)].values
					if pos[1] != 'x':
						if pos[2] != self.color:
							res.append(chr(i) + str(row + count))
							break
						else:
							break
					else:
						res.append(chr(i) + str(row + count))
					count = count + 1
				else:
					break

			count = -1
			for i in range(col_n + 1, 73):
				if row + count > 0:
					pos = newBoard[newBoard['position'] == chr(i) + str(row + count)].values
					if pos[1] != 'x':
						if pos[2] != self.color:
							res.append(chr(i) + str(row + count))
							break
						else:
							break
					else:
						res.append(chr(i) + str(row + count))
					count = count - 1
				else:
					break

			count = -1
			for i in range(col_n - 1, 64, -1):
				if row + count > 0:
					pos = newBoard[newBoard['position'] == chr(i) + str(row + count)].values
					if pos[1] != 'x':
						if pos[2] != self.color:
							res.append(chr(i) + str(row + count))
							break
						else:
							break
					else:
						res.append(chr(i) + str(row + count))
					count = count - 1
				else:
					break
			return res

		else:

			count = 1
			for i in range(col_n + 1, 73):
				if row + count < 9:
					pos = boardPos[chr(i) + str(row + count)]
					if pos[0]:
						if pos[1].color != self.color:
							res.append(chr(i) + str(row + count))
							break
						else:
							break
					else:
						res.append(chr(i) + str(row + count))
					count = count + 1
				else:
					break

			count = 1
			for i in range(col_n - 1, 64, -1):
				if row + count < 9:
					pos = boardPos[chr(i) + str(row + count)]
					if pos[0]:
						if pos[1].color != self.color:
							res.append(chr(i) + str(row + count))
							break
						else:
							break
					else:
						res.append(chr(i) + str(row + count))
					count = count + 1
				else:
					break

			count = -1
			for i in range(col_n + 1, 73):
				if row + count > 0:
					pos = boardPos[chr(i) + str(row + count)]
					if pos[0]:
						if pos[1].color != self.color:
							res.append(chr(i) + str(row + count))
							break
						else:
							break
					else:
						res.append(chr(i) + str(row + count))
					count = count - 1
				else:
					break

			count = -1
			for i in range(col_n - 1, 64, -1):
				if row + count > 0:
					pos = boardPos[chr(i) + str(row + count)]
					if pos[0]:
						if pos[1].color != self.color:
							res.append(chr(i) + str(row + count))
							break
						else:
							break
					else:
						res.append(chr(i) + str(row + count))
					count = count - 1
				else:
					break

		return res

class Knight(Piece):
	def __init__(self, color, color_side, initPos, type, img):
		super().__init__(color, color_side, initPos, type, img)
		self.value = 3

	def validMoves(self, boardPos, newBoard=None):
		res = []
		currentPos = getPosition(params.board_height, params.board_width, self.color_side, self.pos)
		row = int(currentPos[1])
		col = currentPos[0]

		col_n = ord(col)

		if newBoard:
			if row + 1 < 9:
				if col_n + 2 < 73:
					pos = newBoard[newBoard['position'] == chr(col_n + 2) + str(row + 1)].values
					if pos[1] != 'x' and pos[2] != self.color:
						res.append(chr(col_n + 2) + str(row + 1))
					elif pos[1] == 'x':
						res.append(chr(col_n + 2) + str(row + 1))
				if col_n - 2 > 64:
					pos = newBoard[newBoard['position'] == chr(col_n - 2) + str(row + 1)].values
					if pos[1] != 'x' and pos[2] != self.color:
						res.append(chr(col_n - 2) + str(row + 1))
					elif pos[1] == 'x':
						res.append(chr(col_n - 2) + str(row + 1))
			if row + 2 < 9:
				if col_n + 1 < 73:
					pos = newBoard[newBoard['position'] == chr(col_n + 1) + str(row + 2)].values
					if pos[1] != 'x' and pos[2] != self.color:
						res.append(chr(col_n + 1) + str(row + 2))
					elif pos[1] == 'x':
						res.append(chr(col_n + 1) + str(row + 2))
				if col_n - 1 > 64:
					pos = newBoard[newBoard['position'] == chr(col_n - 1) + str(row + 2)].values
					if pos[1] != 'x' and pos[2] != self.color:
						res.append(chr(col_n - 1) + str(row + 2))
					elif pos[1] == 'x':
						res.append(chr(col_n - 1) + str(row + 2))
			if row - 1 > 0:
				if col_n + 2 < 73:
					pos = newBoard[newBoard['position'] == chr(col_n + 2) + str(row - 1)].values
					if pos[1] != 'x' and pos[2] != self.color:
						res.append(chr(col_n + 2) + str(row - 1))
					elif pos[1] == 'x':
						res.append(chr(col_n + 2) + str(row - 1))
				if col_n - 2 > 64:
					pos = newBoard[newBoard['position'] == chr(col_n - 2) + str(row - 1)].values
					if pos[1] != 'x' and pos[2] != self.color:
						res.append(chr(col_n - 2) + str(row - 1))
					elif pos[1] == 'x':
						res.append(chr(col_n - 2) + str(row - 1))
			if row - 2 > 0:
				if col_n + 1 < 73:
					pos = newBoard[newBoard['position'] == chr(col_n + 1) + str(row - 2)].values
					if pos[1] != 'x' and pos[2] != self.color:
						res.append(chr(col_n + 1) + str(row - 2))
					elif pos[1] == 'x':
						res.append(chr(col_n + 1) + str(row - 2))
				if col_n - 1 > 64:
					pos = newBoard[newBoard['position'] == chr(col_n - 1) + str(row - 2)].values
					if pos[1] != 'x' and pos[2] != self.color:
						res.append(chr(col_n - 1) + str(row - 2))
					elif pos[1] == 'x':
						res.append(chr(col_n - 1) + str(row - 2))
			return res

		else:
			if row + 1 < 9:
				if col_n + 2 < 73:
					pos = boardPos[chr(col_n + 2) + str(row + 1)]
					if pos[0] and pos[1].color != self.color:
						res.append(chr(col_n + 2) + str(row + 1))
					elif pos[0] == False:
						res.append(chr(col_n + 2) + str(row + 1))
				if col_n - 2 > 64:
					pos = boardPos[chr(col_n - 2) + str(row + 1)]
					if pos[0] and pos[1].color != self.color:
						res.append(chr(col_n - 2) + str(row + 1))
					elif pos[0] == False:
						res.append(chr(col_n - 2) + str(row + 1))
			if row + 2 < 9:
				if col_n + 1 < 73:
					pos = boardPos[chr(col_n + 1) + str(row + 2)]
					if pos[0] and pos[1].color != self.color:
						res.append(chr(col_n + 1) + str(row + 2))
					elif pos[0] == False:
						res.append(chr(col_n + 1) + str(row + 2))
				if col_n - 1 > 64:
					pos = boardPos[chr(col_n - 1) + str(row + 2)]
					if pos[0] and pos[1].color != self.color:
						res.append(chr(col_n - 1) + str(row + 2))
					elif pos[0] == False:
						res.append(chr(col_n - 1) + str(row + 2))
			if row - 1 > 0:
				if col_n + 2 < 73:
					pos = boardPos[chr(col_n + 2) + str(row - 1)]
					if pos[0] and pos[1].color != self.color:
						res.append(chr(col_n + 2) + str(row - 1))
					elif pos[0] == False:
						res.append(chr(col_n + 2) + str(row - 1))
				if col_n - 2 > 64:
					pos = boardPos[chr(col_n - 2) + str(row - 1)]
					if pos[0] and pos[1].color != self.color:
						res.append(chr(col_n - 2) + str(row - 1))
					elif pos[0] == False:
						res.append(chr(col_n - 2) + str(row - 1))
			if row - 2 > 0:
				if col_n + 1 < 73:
					pos = boardPos[chr(col_n + 1) + str(row - 2)]
					if pos[0] and pos[1].color != self.color:
						res.append(chr(col_n + 1) + str(row - 2))
					elif pos[0] == False:
						res.append(chr(col_n + 1) + str(row - 2))
				if col_n - 1 > 64:
					pos = boardPos[chr(col_n - 1) + str(row - 2)]
					if pos[0] and pos[1].color != self.color:
						res.append(chr(col_n - 1) + str(row - 2))
					elif pos[0] == False:
						res.append(chr(col_n - 1) + str(row - 2))


		return res




class Pawn(Piece):
	def __init__(self, color, color_side, initPos, type, img):
		super().__init__(color, color_side, initPos, type, img)
		self.value = 1


	def validMoves(self, boardPos, newBoard=None):
		if newBoard:
			res = []
			currentPos = getPosition(params.board_height, params.board_width, self.color_side, self.pos)
			row = int(currentPos[1])
			col = currentPos[0]


			if self.color == "white":
				if row + 1 > 8:
					return res

				pos_1 = newBoard[newBoard['position'] == col + str(row + 1)].values
				if pos_1[1] == 'x':
					res.append(col + str(row + 1))

				if row + 2 < 9:
					pos_2 = newBoard[newBoard['position'] == col + str(row + 2)].values
					if pos_1[1] == 'x' and self.moved == False:
						res.append(col + str(row + 2))

				if ord(col) + 1 < 73:
					pos_3 = newBoard[newBoard['position'] == chr(ord(col) + 1) + str(row + 1)].values
					if pos_3[1] != 'x' and pos_3[2] != self.color:
						res.append(chr(ord(col) + 1) + str(row + 1))

				if ord(col) - 1 > 64:
					pos_4 = newBoard[newBoard['position'] == chr(ord(col) - 1) + str(row + 1)].values
					if pos_4[1] != 'x' and pos_4[2] != self.color:
						res.append(chr(ord(col) - 1) + str(row + 1))


			else:
				if row - 1 < 1:
					return res

				pos_1 = newBoard[newBoard['position'] == col + str(row - 1)].values
				if pos_1[1] == 'x':
					res.append(col + str(row - 1))

				if row - 2 > 0:
					pos_2 = newBoard[newBoard['position'] == col + str(row - 2)].values
					if pos_1[1] == 'x' and self.moved == False:
						res.append(col + str(row - 2))

				if ord(col) + 1 < 73:
					pos_3 = newBoard[newBoard['position'] == chr(ord(col) + 1) + str(row - 1)].values
					if pos_3[1] != 'x' and pos_3[2] != self.color:
						res.append(chr(ord(col) + 1) + str(row - 1))

				if ord(col) - 1 > 64:
					pos_4 = newBoard[newBoard['position'] == chr(ord(col) - 1) + str(row - 1)].values
					if pos_4[1] != 'x' and pos_4[2] != self.color:
						res.append(chr(ord(col) - 1) + str(row - 1))
			return res

		else:
			res = []
			currentPos = getPosition(params.board_height, params.board_width, self.color_side, self.pos)
			row = int(currentPos[1])
			col = currentPos[0]


			if self.color == "white":
				if row + 1 > 8:
					return res

				pos_1 = boardPos[col + str(row + 1)]
				if pos_1[0] == False:
					res.append(col + str(row + 1))

				if row + 2 < 9:
					pos_2 = boardPos[col + str(row + 2)]
					if pos_1[0] == False and pos_2[0] == False and self.moved == False:
						res.append(col + str(row + 2))

				if ord(col) + 1 < 73:
					pos_3 = boardPos[chr(ord(col) + 1) + str(row + 1)]
					if pos_3[0] and pos_3[1].color != self.color:
						res.append(chr(ord(col) + 1) + str(row + 1))

				if ord(col) - 1 > 64:
					pos_4 = boardPos[chr(ord(col) - 1) + str(row + 1)]
					if pos_4[0] and pos_4[1].color != self.color:
						res.append(chr(ord(col) - 1) + str(row + 1))


			else:
				if row - 1 < 1:
					return res

				pos_1 = boardPos[col + str(row - 1)]
				if pos_1[0] == False:
					res.append(col + str(row - 1))

				if row - 2 > 0:
					pos_2 = boardPos[col + str(row - 2)]
					if pos_1[0] == False and pos_2[0] == False and self.moved == False:
						res.append(col + str(row - 2))

				if ord(col) + 1 < 73:
					pos_3 = boardPos[chr(ord(col) + 1) + str(row - 1)]
					if pos_3[0] and pos_3[1].color != self.color:
						res.append(chr(ord(col) + 1) + str(row - 1))

				if ord(col) - 1 > 64:
					pos_4 = boardPos[chr(ord(col) - 1) + str(row - 1)]
					if pos_4[0] and pos_4[1].color != self.color:
						res.append(chr(ord(col) - 1) + str(row - 1))



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
		self.static_positions = ['A1', 'A2', 'A3', 'A4', 'A5', 'A6', 'A7', 'A8',
								'B1', 'B2', 'B3', 'B4', 'B5', 'B6', 'B7', 'B8',
								'C1', 'C2', 'C3', 'C4', 'C5', 'C6', 'C7', 'C8',
								'D1', 'D2', 'D3', 'D4', 'D5', 'D6', 'D7', 'D8',
								'E1', 'E2', 'E3', 'E4', 'E5', 'E6', 'E7', 'E8',
								'F1', 'F2', 'F3', 'F4', 'F5', 'F6', 'F7', 'F8',
								'G1', 'G2', 'G3', 'G4', 'G5', 'G6', 'G7', 'G8',
								'H1', 'H2', 'H3', 'H4', 'H5', 'H6', 'H7', 'H8']

		self.board_dataframe = pd.DataFrame(columns=['position', 'piece_type', 'color', 'moved', 'value',
													'A1', 'A2', 'A3', 'A4', 'A5', 'A6', 'A7', 'A8',
													'B1', 'B2', 'B3', 'B4', 'B5', 'B6', 'B7', 'B8',
													'C1', 'C2', 'C3', 'C4', 'C5', 'C6', 'C7', 'C8',
													'D1', 'D2', 'D3', 'D4', 'D5', 'D6', 'D7', 'D8',
													'E1', 'E2', 'E3', 'E4', 'E5', 'E6', 'E7', 'E8',
													'F1', 'F2', 'F3', 'F4', 'F5', 'F6', 'F7', 'F8',
													'G1', 'G2', 'G3', 'G4', 'G5', 'G6', 'G7', 'G8',
													'H1', 'H2', 'H3', 'H4', 'H5', 'H6', 'H7', 'H8'])

		counter = 0
		for i in range(65, 73):
			for j in range(1, 9):
				pos = chr(i) + str(j)
				self.board_dataframe.loc[counter] = self.createRow(pos)
				counter += 1



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
		print(self.board_dataframe)



	def show(self, board):
		for piece in self.pieces:
			piece.show(board)
		if self.prevPiece:
			self.prevPiece.show(board)

	def createRow(self, name):
		data = [name, 'x', 'x', False, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 							 	0, 0, 0, 0, 0, 0, 0, 0,
									 	0, 0, 0, 0, 0, 0, 0, 0,
									 	0, 0, 0, 0, 0, 0, 0, 0,
									 	0, 0, 0, 0, 0, 0, 0, 0,
									 	0, 0, 0, 0, 0, 0, 0, 0,
									 	0, 0, 0, 0, 0, 0, 0, 0,
									 	0, 0, 0, 0, 0, 0, 0, 0]
		return data


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

		self.board_dataframe['piece_type'], self.board_dataframe['color'], self.board_dataframe['moved'], self.board_dataframe['value'] = zip(*self.board_dataframe['position'].apply(self.setData))
		tempdf = self.board_dataframe['position'].apply(self.setValidMoves).dropna()
		tempdf = tempdf[tempdf.apply(lambda x: len(x) > 0)]
		for i in range(64):
			if i in tempdf.index:
				self.board_dataframe.loc[i, tempdf.loc[i]] = 1
			else:
				self.board_dataframe.loc[i, self.static_positions] = 0

	def setData(self, pos):
		for piece in self.pieces:
			if piece.pos == self.positions[pos] and piece.color == 'white':
				return (piece.type, piece.color, piece.moved, piece.value)
			elif piece.pos == self.positions[pos] and piece.color != 'white':
				return (piece.type, piece.color, piece.moved, -piece.value)

		return ('x', 'x', False, 0)

	def setValidMoves(self, pos):
		return self.getValidMoves(pos)

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
		if self.turn == 'white':
			self.turn = 'black'
			self.moveNumber += 1
		else:
			self.turn = 'white'

		#print(self.outcomeGame())
		if castling == 1:
			notation = "O-O-O"
		elif castling == 2:
			notation = "O-O"
		else:
			notation = notation + loc2.lower()
		print(self.moveNumber, notation)
		if self.outcomeGame() == 2:
			print("White Wins!")
		elif self.outcomeGame() == 3:
			print("Black Wins!")



	def getValidMoves(self, loc, board=None, newBoard=None):

		if newBoard:
			ret = None
			for i in range(len(newBoard)):
				value = newBoard.iloc[i].values
				if value[1] != 'x':
					piece = value[2]
					if piece.pos == self.positions[loc]:
						ret = piece.validMoves(board, newBoard)
						ret = self.filterCheck(board, ret, piece)

						# Castling rights
						if piece.type == 'k' and not piece.moved:
							if piece.color == 'white':
								allVMoves = self.getAllValidMoves('black')
								isRookThere = board['A1']
								isCheck = self.isInCheck('white')
								if not isCheck and board['A1'][0] and not board['A1'][1].moved and 'B1' not in allVMoves and 'C1' not in allVMoves and 'D1' not in allVMoves:
									if not board['B1'][0] and not board['C1'][0] and not board['D1'][0]:
										ret.append('A1')
										ret.append('C1')
										ret.append('D1')
								if not isCheck and board['H1'][0] and not board['H1'][1].moved and 'F1' not in allVMoves and 'G1' not in allVMoves :
									if not board['F1'][0] and not board['G1'][0]:
										ret.append('F1')
										ret.append('G1')
										ret.append('H1')
							else:
								allVMoves = self.getAllValidMoves('white')
								isRookThere = board['A1']
								isCheck = self.isInCheck('black')
								if not isCheck and board['A8'][0] and not board['A8'][1].moved and 'B8' not in allVMoves and 'C8' not in allVMoves and 'D8' not in allVMoves:
									if not board['B8'][0] and not board['C8'][0] and not board['D8'][0]:
										ret.append('A8')
										ret.append('C8')
										ret.append('D8')
								if not isCheck and board['H8'][0] and not board['H8'][1].moved and 'F8' not in allVMoves and 'G8' not in allVMoves :
									if not board['F8'][0] and not board['G8'][0]:
										ret.append('F8')
										ret.append('G8')
										ret.append('H8')
						break

		elif not board and not newBoard:
			ret = None
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
									ret.append('A1')
									ret.append('C1')
									ret.append('D1')
							if not isCheck and self.occupied_squares['H1'][0] and not self.occupied_squares['H1'][1].moved and 'F1' not in allVMoves and 'G1' not in allVMoves :
								if not self.occupied_squares['F1'][0] and not self.occupied_squares['G1'][0]:
									ret.append('F1')
									ret.append('G1')
									ret.append('H1')
						else:
							allVMoves = self.getAllValidMoves('white')
							isRookThere = self.occupied_squares['A1']
							isCheck = self.isInCheck('black')
							if not isCheck and self.occupied_squares['A8'][0] and not self.occupied_squares['A8'][1].moved and 'B8' not in allVMoves and 'C8' not in allVMoves and 'D8' not in allVMoves:
								if not self.occupied_squares['B8'][0] and not self.occupied_squares['C8'][0] and not self.occupied_squares['D8'][0]:
									ret.append('A8')
									ret.append('C8')
									ret.append('D8')
							if not isCheck and self.occupied_squares['H8'][0] and not self.occupied_squares['H8'][1].moved and 'F8' not in allVMoves and 'G8' not in allVMoves :
								if not self.occupied_squares['F8'][0] and not self.occupied_squares['G8'][0]:
									ret.append('F8')
									ret.append('G8')
									ret.append('H8')


					break
		else:
			ret = None
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
										ret.append('A1')
										ret.append('C1')
										ret.append('D1')
								if not isCheck and board['H1'][0] and not board['H1'][1].moved and 'F1' not in allVMoves and 'G1' not in allVMoves :
									if not board['F1'][0] and not board['G1'][0]:
										ret.append('F1')
										ret.append('G1')
										ret.append('H1')
							else:
								allVMoves = self.getAllValidMoves('white')
								isRookThere = board['A1']
								isCheck = self.isInCheck('black')
								if not isCheck and board['A8'][0] and not board['A8'][1].moved and 'B8' not in allVMoves and 'C8' not in allVMoves and 'D8' not in allVMoves:
									if not board['B8'][0] and not board['C8'][0] and not board['D8'][0]:
										ret.append('A8')
										ret.append('C8')
										ret.append('D8')
								if not isCheck and board['H8'][0] and not board['H8'][1].moved and 'F8' not in allVMoves and 'G8' not in allVMoves :
									if not board['F8'][0] and not board['G8'][0]:
										ret.append('F8')
										ret.append('G8')
										ret.append('H8')


						break

		return ret

	def getAllValidMoves(self, color, board=None):
		ret = []
		if board == None:
			for piece in self.pieces:
				if piece.color == color:
					ret = ret + list(piece.validMoves(self.occupied_squares))
		else:

			for key, value in board.items():
				if value[0] and value[1].color == color:
					ret = ret + list(value[1].validMoves(board))

		return list(set(ret))

	def isInCheck(self, color, board=None):
		ret = False
		if board == None:
			if color == "white":
				AllValidMoves = self.getAllValidMoves("black")
			else:
				AllValidMoves = self.getAllValidMoves("white")


			for i in range(len(self.pieces)):
				if self.pieces[i].color == color and self.pieces[i].type == 'k' and getPosition(params.board_height, params.board_width, self.color, self.pieces[i].pos) in AllValidMoves:
					ret = True
					break
			self.pieces[i].isInCheck = ret

		else:
			if color == "white":
				AllValidMoves = self.getAllValidMoves("black", board)
			else:
				AllValidMoves = self.getAllValidMoves("white", board)

			for key, value in board.items():
				if value[0] and value[1].color == color and value[1].type == 'k' and getPosition(params.board_height, params.board_width, self.color, value[1].pos) in AllValidMoves:
					ret = True
					break

		return ret

	def filterCheck(self, boardPositions, validMoves, piece):
		boardCopy = boardPositions.copy()
		mrPiecePos = getPosition(params.board_height, params.board_width, self.color, piece.pos)
		boardCopy[mrPiecePos] = (False, None)
		res = []

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
				res.append(pos)

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
			mrMove = self.minimaxR(False, self.board_dataframe, 3)

		if turn == "black" and self.turn == 'white':
			mrMove = self.minimaxR(True, self.board_dataframe, 3)

		#print(mrMove, self.traversedNodes)
		if mrMove and mrMove[0] == m.inf:
			print('Black Resigns')
			return -1
		if mrMove and mrMove[0] == -m.inf:
			print('White Resigns')
			return -1
		if mrMove and mrMove[1] and mrMove[2]:
			self.movePiece(mrMove[1], mrMove[2])

		self.traversedNodes = 0

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

		boardCopy = boardPos.copy()
		board = boardCopy[(boardCopy.T == 1).any()]
		board = board.loc[:, (boardCopy != 0).any(axis=0)]


		if useMax:
			board = board[(board['color'] == 'white')]
			for i in board.index:
				vMoves = board.loc[:, board.loc[i] == 1].columns
				coords = board.loc[i]['position']
				for move in vMoves:
					if move != 'value':
						# Create Board
						tmpBoard = boardPos.copy()

						#Get Previous attributes
						# tp = tmpBoard.loc[tmpBoard['position'] == move][['piece_type', 'color', 'moved', 'value']].copy()
						tp2 = tmpBoard.loc[tmpBoard['position'] == coords, ['piece_type', 'color', 'moved', 'value']].copy()
						tmpBoard.loc[tmpBoard['position'] == move, ['piece_type', 'color', 'moved', 'value']] = tp2[['piece_type', 'color', 'moved', 'value']].values
						tmpBoard.loc[tmpBoard['position'] == coords, ['piece_type', 'color', 'moved', 'value']] = ['x', 'x', False, 0]

						# tmpBoard.loc[tp2.index][['piece_type', 'color', 'moved', 'value']] = ['x', 'x', False, 0]


						#Place piece
						tempdf = tmpBoard['position'].apply(self.setValidMoves).dropna()
						tempdf = tempdf[tempdf.apply(lambda x: len(x) > 0)]

						# tmpBoard.loc[i] = self.setValidMoves(move)
						for i in range(64):
							if i in tempdf.index:
								tmpBoard.loc[i, tempdf.loc[i]] = 1
							else:
								tmpBoard.loc[i, self.static_positions] = 0

						print(tmpBoard)

					#print(tmpBoard)

				ret = self.minimaxR(False, tmpBoard, maxDepth, depth=depth+1, alpha=alpha, beta=beta)[0]

				if ret > res:
					res = ret
					loc = coords
					bestMove = move

				alpha = max(alpha, res)
				if alpha >= beta:
					break

			# for key, value in boardCopy.items():
			# 	if value[0] and value[1].color == 'white':
			# 		coords = getPosition(params.board_height, params.board_width, self.color, value[1].pos)
			# 		vMoves = self.getValidMoves(coords, boardPos)
			# 		if vMoves:
			# 			for move in vMoves:
			#
			# 				ret = self.minimaxR(False, tmpBoard, maxDepth, depth=depth+1, alpha=alpha, beta=beta)[0]
			#
			# 				if ret > res:
			# 					res = ret
			# 					loc = coords
			# 					bestMove = move
			#
			# 				alpha = max(alpha, res)
			# 				if alpha >= beta:
			# 					break

		else:
			for key, value in boardCopy.items():
				if value[0] and value[1].color == 'black':
					coords = getPosition(params.board_height, params.board_width, self.color, value[1].pos)
					vMoves = self.getValidMoves(coords, boardPos)
					if vMoves:
						for move in vMoves:
							tmpBoard = boardPos.copy()
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
		for key, value in boardPos.items():
			if value[0]:
				if value[1].color == 'white':
					#res += len(self.getValidMoves(getPosition(params.board_height, params.board_width, self.color, value[1].pos), board=boardPos)) / 10
					res += value[1].value
				else:
					#res -= len(self.getValidMoves(getPosition(params.board_height, params.board_width, self.color, value[1].pos), board=boardPos)) / 10
					res -= value[1].value


		return res




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
		if boardPieces.turn == "white":
			boardPieces.prevPiece = None
			mm = boardPieces.minimax('black')
			pg.display.flip()
		else:
			boardPieces.prevPiece = None
			mm = boardPieces.minimax('white')
			pg.display.flip()

		if mm == -1:
			os.system("pause")
			crashed = True

		clock.tick(144)


	pg.quit()
	quit()
	return 0


if __name__=='__main__':
    main()
