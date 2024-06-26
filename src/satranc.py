def XY2POS(x, y):
    # Convert the x coordinate to a letter (e.g., 0 -> 'a', 1 -> 'b', etc.)
    # 'a' is the starting point, and the x value is added to it.
    letter = chr(ord("a") + x)

    # Convert the y coordinate to a 1-based system (e.g., 0 -> 1, 1 -> 2, etc.)
    number = y + 1

    # Combine the letter and number into a string (e.g., 'a1', 'b3', etc.)
    return f'{letter}{number}'

def POS2XY(pos):
    # Extract the letter part and convert it to a lowercase character
    # Then find its position in the alphabet (0-based)
    x = ord(pos[0].lower()) - ord('a')

    # Extract the number part, convert it to an integer, and adjust to a 0-based system
    y = int(pos[1]) - 1

    # Return the numeric x and y coordinates as a tuple
    return x, y
class Player:
    def __init__(self, name, color):
        self.name = name
        self.color = color  # 'white' or 'black'
        self.pieces = []  # Pieces that belong to the player

    def addPiece(self, piece):
        self.pieces.append(piece)

    def removePiece(self, piece):
        assert piece in self.pieces
        self.pieces.remove(piece)
class ShatranjPiece:
    def __init__(self, player, pos):
        self.x, self.y = POS2XY( pos )
        self.player = player
        self.possibleRegularMoves = None
        self.possibleCaptureMoves = None
        self.canJumpOverOthers = None
        self.multipleMove = None
        self.moved = False

    def canMove(self, x: int, y: int, board, ctrlCheck=True):
        if x == self.x and y == self.y:
            return False
        if x < 0 or x > 7 or y < 0 or y > 7:
            return False
        diff = (x - self.x, y - self.y)
        if not self.multipleMove:
            if diff not in self.possibleRegularMoves:
                return False
        else:
            ok = False
            for i in range(1, 8):
                if (diff[0] % i == 0 and diff[1] % i == 0 and
                    (diff[0] // i, diff[1] // i) in self.possibleRegularMoves):
                    ok = True
                    break
            if not ok:
                return False
        piece = board.getCell(x, y)
        if piece and piece.player == self.player:
            return False
        if ctrlCheck and board.wouldBeInCheck( self, x, y ):
            return False
        return self.canJumpOverOthers or board.isPathClear(self.x, self.y, x, y)

    def canThreat(self, x: int, y: int, board, ctrlCheck=True):
        if x == self.x and y == self.y:
            return False
        if x < 0 or x > 7 or y < 0 or y > 7:
            return False
        diff = (x - self.x, y - self.y)
        if not self.multipleMove:
            if diff not in self.possibleCaptureMoves:
                return False
        else:
            ok = False
            for i in range(1, 8):
                if (diff[0] % i == 0 and diff[1] % i == 0 and
                    (diff[0] // i, diff[1] // i)  in self.possibleCaptureMoves):
                    ok = True
                    break
            if not ok:
                return False
        if ctrlCheck and board.wouldBeInCheck( self, x, y ):
            return False
        return self.canJumpOverOthers or board.isPathClear(self.x, self.y, x, y)

    def canCapture(self, x: int, y: int, board, ctrlCheck=True):
        if not self.canThreat(x, y, board, ctrlCheck=ctrlCheck):
            return False
        piece = board.getCell(x, y)
        if not piece or piece.player == self.player:
            return False
        return self.canJumpOverOthers or board.isPathClear(self.x, self.y, x, y)

    def move( self, x, y ):
        self.x = x
        self.y = y
        self.moved = True

    def __str__( self ):
        return f'{"W" if self.player.color=="white" else "B"}{self.name}({XY2POS(self.x, self.y)})'
class Rook(ShatranjPiece):
    def __init__(self, player, pos):
        super().__init__(player, pos)
        self.multipleMove = True
        self.canJumpOverOthers = False
        self.possibleRegularMoves = self.possibleCaptureMoves = [(0, 1), (1, 0), (0, -1), (-1, 0)]
        self.name = 'Rook'
        self.symbol = 'R'
class Fil(ShatranjPiece):
    def __init__(self, player, pos):
        super().__init__(player, pos)
        self.canJumpOverOthers = True
        self.possibleRegularMoves = self.possibleCaptureMoves = [(2, 2), (2, -2), (-2, 2), (-2, -2)]
        self.name = 'Fil'
        self.symbol = 'F'
class Vizier(ShatranjPiece):
    def __init__(self, player, pos):
        super().__init__(player, pos)
        self.multipleMove = False
        self.canJumpOverOthers = False
        self.possibleRegularMoves = self.possibleCaptureMoves = [ (1, 1), (1, -1), (-1, 1), (-1, -1) ]
        self.name = 'Vizier'
        self.symbol = 'V'
class Horse( ShatranjPiece ):
    def __init__(self, player, pos):
        super().__init__(player, pos)
        self.possibleRegularMoves = self.possibleCaptureMoves = [
            ( +1, +2 ), ( +2, +1 ),
            ( -1, +2 ), ( -2, +1 ),
            ( -1, -2 ), ( -2, -1 ),
            ( +1, -2 ), ( +2, -1 ),
        ]
        self.canJumpOverOthers = True
        self.multipleMove = False
        self.name = 'Horse'
        self.symbol = 'H'
class Shah(ShatranjPiece):
    def __init__(self, player, pos):
        super().__init__(player, pos)
        self.multipleMove = False
        self.canJumpOverOthers = False
        self.possibleRegularMoves = self.possibleCaptureMoves = [
            (0, 1), (1, 0), (0, -1), (-1, 0), (1, 1), (1, -1), (-1, 1), (-1, -1)
        ]
        self.name = 'Shah'
        self.symbol = 'S'
class Piyade(ShatranjPiece):
    def __init__(self, player, pos):
        super().__init__(player, pos)
        self.multipleMove = False
        self.canJumpOverOthers = False
        self.direction = +1 if player.color == 'white' else -1
        self.possibleRegularMoves = [(0, self.direction)]
        self.possibleCaptureMoves = [(1, self.direction), (-1, self.direction)]
        self.name = 'Piyade'
        self.symbol = 'P'
class Board:
    def __init__(self, player1, player2):
        self.pieces = []
        self.currentTurn = player1
        self.players = [player1, player2]
        self.lastMove = None
        self.halfMoveClock = 0
        self.fullMoveNumber = 1

    def addPiece(self, piece):
        self.pieces.append(piece)
        piece.player.addPiece(piece)

    def removePiece(self, piece):
        self.pieces.remove(piece)
        piece.player.removePiece(piece)

    def getCell(self, x, y):
        for piece in self.pieces:
            if piece.x == x and piece.y == y:
                return piece
        return None

    def moveSuccesful( self, piece, targetPiece, fromX, fromY, toX, toY ):
        self.lastMove = {
            "moved_piece": piece,
            "from_pos": (fromX, fromY),
            "to_pos": (toX, toY),
            "captured_piece": targetPiece,
            "prev_half_move_clock": self.halfMoveClock,
            "prev_full_move_number": self.fullMoveNumber,
        }
        if isinstance(piece, Piyade) or targetPiece is not None:
            self.halfMoveClock = 0
        else:
            self.halfMoveClock += 1

        if self.currentTurn.color == 'black':
            self.fullMoveNumber += 1

        self.switchTurn()

    def movePiece(self, piece, x, y):
        if piece.player != self.currentTurn:
            return False

        fromX = piece.x
        fromY = piece.y
        toX = x
        toY = y
        targetPiece = self.getCell( x, y )

        # Normal Move
        canMove = piece.canMove(x, y, self)
        canCapture = piece.canCapture(x, y, self)
        if not canMove and not canCapture:
            return False

        # Check if the move puts or leaves the player's Shah in check
        if self.wouldBeInCheck(piece, x, y):
            return False  # Cannot make a move that puts/keeps the Shah in check

        # Move the piece and handle captures
        captured_piece = self.getCell(x, y)
        if captured_piece and canCapture:
            self.pieces.remove(captured_piece)
            captured_piece.player.removePiece(captured_piece)
        piece.move( x, y )

        # Piyade Promotion Logic
        if isinstance(piece, Piyade) and (piece.y == 0 or piece.y == 7):
            promoted_piece = self.promotePiyade(piece)
            self.removePiece(piece)
            self.addPiece(promoted_piece)

        self.moveSuccesful( piece, targetPiece, fromX, fromY, toX, toY )
        return True

    def promotePiyade(self, piyade):
        x, y = piyade.x, piyade.y
        return Vizier(piyade.player, XY2POS(x, y))

    def wouldBeInCheck(self, piece, x, y):
        original_x, original_y = piece.x, piece.y
        capturedPiece = self.getCell( x, y )
        if capturedPiece:
            capturedPiece.player.pieces.remove( capturedPiece )
            self.pieces.remove( capturedPiece )

        piece.x, piece.y = x, y
        inCheck = self.isCheck(piece.player)

        piece.x, piece.y = original_x, original_y
        if capturedPiece:
            capturedPiece.player.pieces.append( capturedPiece )
            self.pieces.append( capturedPiece )

        return inCheck

    def isCheck(self, player):
        shah = next((p for p in player.pieces if isinstance(p, Shah)), None)
        if not shah:
            return False

        opponent = self.players[0] if player == self.players[1] else self.players[1]
        for p in opponent.pieces:
            if p.canCapture(shah.x, shah.y, self, ctrlCheck=False):
                return True
        return False

    def isCheckmate(self, player):
        if not self.isCheck(player):
            return False
        for piece in player.pieces:
            for x in range(8):
                for y in range(8):
                    if ( piece.canMove(x, y, self, ctrlCheck=False) or piece.canCapture(x, y, self, ctrlCheck=False) ) and not self.wouldBeInCheck(piece, x, y):
                        return False
        return True

    def isGameOver(self):
        if self.isStalemate(self.currentTurn):
            return True
        if self.isCheckmate(self.currentTurn):
            return True
        if self.isDraw():
            return True
        return False

    def winner(self):
        if self.isDraw():
            return None
        if self.isStalemate(self.currentTurn):
            return self.opponent(self.currentTurn)
        if self.isCheckmate(self.currentTurn):
            return self.opponent(self.currentTurn)
        self.currentTurn

    def isDraw(self):
        # Check shah for two shahs condition
        if len(self.pieces) == 2 and all(isinstance(piece, Shah) for piece in self.pieces):
            return True

        piecesCurrent = self.currentTurn.pieces
        piecesOpponent = self.opponent(self.currentTurn).pieces
        if len( piecesCurrent ) == 1 and len( piecesOpponent ) == 2 and (
            piecesCurrent[ 0 ].canCapture( piecesOpponent[0].x, piecesOpponent[0].y, self ) or
            piecesCurrent[ 0 ].canCapture( piecesOpponent[1].x, piecesOpponent[1].y, self )
        ):
            return True

        # Check shah for shah and fil/horse against shah condition
        # if len(self.pieces) == 3:
        #    shahs = [piece for piece in self.pieces if isinstance(piece, Shah)]
        #    others = [piece for piece in self.pieces if not isinstance(piece, Shah)]
        #    if len(shahs) == 2 and (isinstance(others[0], Fil) or isinstance(others[0], Horse)):
        #        return True

        return False

    def isStalemate(self, player):
        if self.isCheck(player):
            return False  # Not a stalemate if the player is in check

        for piece in player.pieces:
            for x in range(8):
                for y in range(8):
                    if piece.canMove(x, y, self) and not self.wouldBeInCheck(piece, x, y):
                        return False  # Found a legal move, so not a stalemate
        return True

    def opponent(self, player):
        if player == self.players[1]:
            return self.players[0]
        else:
          return self.players[1]

    def switchTurn(self):
        print(self.currentTurn.name, self.opponent(self.currentTurn).name, self.players[0].name, self.players[1].name)
        self.currentTurn = self.opponent(self.currentTurn)

    def isPathClear(self, x1: int, y1: int, x2: int, y2: int):
        if x1 == x2:
            stepX = 0
            stepY = 1 if y1 < y2 else -1
        elif y1 == y2:
            stepY = 0
            stepX = 1 if x1 < x2 else -1
        else:
            stepX = 1 if x1 < x2 else -1
            stepY = 1 if y1 < y2 else -1
        curX, curY = x1 + stepX, y1 + stepY
        while curX != x2 or curY != y2:
            if self.getCell(curX, curY):
                return False
            curX += stepX
            curY += stepY
        return True

    def isUnderAttack(self, x, y, player):
        opponent = self.players[0] if player == self.players[1] else self.players[1]
        for p in opponent.pieces:
            if p.canThreat(x, y, self):
                return True
        return False

    def play( self, from_pos, to_pos ):
        from_pos = POS2XY( from_pos )
        to_pos = POS2XY( to_pos )
        if not from_pos or not to_pos:
            return False

        piece = self.getCell(*from_pos)
        if not piece or piece.player != self.currentTurn:
            return False

        return self.movePiece(piece, *to_pos)

    def boardToString(self):
        # Initialize an 8x8 matrix with dots
        board = [['.' for _ in range(8)] for _ in range(8)]

        # Place pieces on the board
        for piece in self.pieces:
            symbol = piece.symbol.upper() if piece.player.color == 'white' else piece.symbol.lower()
            board[7 - piece.y][piece.x] = symbol  # Adjust for 0-indexing and flip y-axis

        # Add row and column labels
        board_with_labels = ['  a b c d e f g h']
        for i, row in enumerate(board):
            row_label = f'{8 - i} ' + ' '.join(row)  # Adjust for 0-indexing and flip y-axis
            board_with_labels.append(row_label)

        return '\n'.join(board_with_labels)

class Shatranj:
    def __init__( self, player1, player2 ):
        self.player1 = Player(player1, 'white')
        self.player2 = Player(player2, 'black')
        self.board = Board( self.player1, self.player2 )
        self.initializeBoard()

    def initializeBoard(self):
      self.board.addPiece( Rook( self.player1, 'a1' ) )
      self.board.addPiece( Rook( self.player1, 'h1' ) )
      self.board.addPiece( Horse( self.player1, 'b1' ) )
      self.board.addPiece( Horse( self.player1, 'g1' ) )
      self.board.addPiece( Fil( self.player1, 'c1' ) )
      self.board.addPiece( Fil( self.player1, 'f1' ) )
      self.board.addPiece( Vizier( self.player1, 'd1' ) )
      self.board.addPiece( Shah( self.player1, 'e1' ) )
      for c in [ 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' ]:
          self.board.addPiece( Piyade( self.player1, f'{c}2' ) )
      self.board.addPiece( Rook( self.player2, 'a8' ) )
      self.board.addPiece( Rook( self.player2, 'h8' ) )
      self.board.addPiece( Horse( self.player2, 'b8' ) )
      self.board.addPiece( Horse( self.player2, 'g8' ) )
      self.board.addPiece( Fil( self.player2, 'c8' ) )
      self.board.addPiece( Fil( self.player2, 'f8' ) )
      self.board.addPiece( Vizier( self.player2, 'd8' ) )
      self.board.addPiece( Shah( self.player2, 'e8' ) )
      for c in [ 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' ]:
          self.board.addPiece( Piyade( self.player2, f'{c}7' ) )

    def parseInput( self, moveInput ):
        if len(moveInput) == 4:
            return moveInput[ :2 ], moveInput[ 2:4 ]
            # g1f3
        else:
            return None

    def getInput(self):
        moveInput = None
        while moveInput is None:
            moveInput = self.parseInput( input( 'Please Enter Your Move: ' ) )
        return moveInput

    def play( self, input ):
        print(input)
        input = self.parseInput(input)
        status = self.board.play(*input)
        assert( status )

    def run(self):
        while not self.board.isGameOver():
            print( self.board.boardToString() )
            moveInput = self.getInput()
            status = self.board.play(*moveInput)
            if status == True:
                print("Move successful.")
            else:
                print("Invalid move. Try again.")
        if self.board.isDraw():
            print( 'Draw!' )
        elif self.board.winner() == self.player1:
            print( f'{self.player1.name} win!' )
        else:
            print( f'{self.player2.name} win!' )

a = Shatranj('a', 'b')
a.run()