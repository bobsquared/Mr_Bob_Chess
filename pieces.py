class Rook(Piece):
	def __init__(self, color, color_side, initPos, type, img):
		super().__init__(color, color_side, initPos, type, img)



	def validMoves(self, boardPos):
		res = []
		currentPos = getPosition(params.board_height, params.board_width, self.color_side, self.pos)
		row = int(currentPos[1])
		col = currentPos[0]

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

class Pawn(Piece):
	def __init__(self, color, color_side, initPos, type, img):
		super().__init__(color, color_side, initPos, type, img)


	def validMoves(self, boardPos):
		res = []
		currentPos = getPosition(params.board_height, params.board_width, self.color_side, self.pos)
		row = int(currentPos[1])
		col = currentPos[0]

		if self.color_side == "white":
			#Pawn moves up the board
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
		else:
			if self.color != "white":
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

			else:
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


		return res
