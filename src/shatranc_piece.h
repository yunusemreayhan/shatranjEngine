#pragma once

#include <memory>
#include <vector>

#include "board.h"
#include "player.h"
#include "position.h"
#include "types.h"
namespace shatranj
{
class Board;
class Player;


class Piece : public std::enable_shared_from_this<Piece>
{
  public:
    explicit Piece(Position pos, const std::weak_ptr<Player> &player, std::string name, char symbol, bool multipleMove,
                   bool canJumpOverOthers, bool moved, std::vector<std::pair<int, int>> possibleRegularMoves,
                   std::vector<std::pair<int, int>> possibleCaptureMoves);

    Position GetPos()
    {
        return pos_;
    }

    std::weak_ptr<Player> GetPlayer()
    {
        return player_;
    }

    bool CanMove(Position pos, const std::shared_ptr<Board> &board, bool ctrlCheck = true);
    bool CanThreat(Position pos, const std::shared_ptr<Board> &board, bool ctrlCheck = true);
    bool CanCapture(Position pos, const std::shared_ptr<Board> &board, bool ctrlCheck = true);
    bool Move(Position pos);

    std::shared_ptr<Piece> GetSharedFromThis()
    {
        return shared_from_this();
    }

    virtual ~Piece() = default;

  private:
    Position pos_;
    std::weak_ptr<Player> player_;

  protected:
    bool canJumpOverOthers_ = false;
    bool moved_ = false;
    bool multipleMove_ = false;
    char symbol_;
    std::string name_;
    std::vector<std::pair<int, int>> possibleRegularMoves_;
    std::vector<std::pair<int, int>> possibleCaptureMoves_;
};

class Rook : public Piece
{
    /*
    class Rook(ShatranjPiece):
      def __init__(self, player, pos):
          super().__init__(player, pos)
          self.multipleMove = True
          self.canJumpOverOthers = False
          self.possibleRegularMoves = self.possibleCaptureMoves = [(0, 1), (1, 0), (0, -1), (-1, 0)]
          self.name = 'Rook'
          self.symbol = 'R'*/

    Rook(Position pos, const std::weak_ptr<Player> &player)
        : Piece(pos, player, "Rook", 'R', true, false, false, {{0, 1}, {1, 0}, {0, -1}, {-1, 0}}, {})
    {
    }
};

class Piyade : public Piece
{
    /*class Piyade(ShatranjPiece):
      def __init__(self, player, pos):
          super().__init__(player, pos)
          self.multipleMove = False
          self.canJumpOverOthers = False
          self.direction = +1 if player.color == 'white' else -1
          self.possibleRegularMoves = [(0, self.direction)]
          self.possibleCaptureMoves = [(1, self.direction), (-1, self.direction)]
          self.name = 'Piyade'
          self.symbol = 'P'*/

  public:
    Piyade(Position pos, const std::weak_ptr<Player> &player);

  private:
    int direction_;
};
class Horse : public Piece
{
    /*        super().__init__(player, pos)
          self.possibleRegularMoves = self.possibleCaptureMoves = [
              ( +1, +2 ), ( +2, +1 ),
              ( -1, +2 ), ( -2, +1 ),
              ( -1, -2 ), ( -2, -1 ),
              ( +1, -2 ), ( +2, -1 ),
          ]
          self.canJumpOverOthers = True
          self.multipleMove = False
          self.name = 'Horse'
          self.symbol = 'H'*/
  public:
    Horse(Position pos, const std::weak_ptr<Player> &player)
        : Piece(pos, player, "Horse", 'H', false, true, false,
                {{+1, +2}, {+2, +1}, {-1, +2}, {-2, +1}, {-1, -2}, {-2, -1}, {+1, -2}, {+2, -1}}, {})
    {
    }
};
class Fil : public Piece
{
    /**class Fil(ShatranjPiece):
      def __init__(self, player, pos):
          super().__init__(player, pos)
          self.canJumpOverOthers = True
          self.possibleRegularMoves = self.possibleCaptureMoves = [(2, 2), (2, -2), (-2, 2), (-2, -2)]
          self.name = 'Fil'
          self.symbol = 'F' */
  public:
    Fil(Position pos, const std::weak_ptr<Player> &player)
        : Piece(pos, player, "Fil", 'F', false, true, false, {{2, 2}, {2, -2}, {-2, 2}, {-2, -2}}, {})
    {
    }
};
class Vizier : public Piece
{
    /**class Vizier(ShatranjPiece):
      def __init__(self, player, pos):
          super().__init__(player, pos)
          self.multipleMove = False
          self.canJumpOverOthers = False
          self.possibleRegularMoves = self.possibleCaptureMoves = [ (1, 1), (1, -1), (-1, 1), (-1, -1) ]
          self.name = 'Vizier'
          self.symbol = 'V' */

  public:
    Vizier(Position pos, const std::weak_ptr<Player> &player)
        : Piece(pos, player, "Vizier", 'V', false, false, false, {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}, {})
    {
    }
};
class Shah : public Piece
{
    /*class Shah(ShatranjPiece):
      def __init__(self, player, pos):
          super().__init__(player, pos)
          self.multipleMove = False
          self.canJumpOverOthers = False
          self.possibleRegularMoves = self.possibleCaptureMoves = [
              (0, 1), (1, 0), (0, -1), (-1, 0), (1, 1), (1, -1), (-1, 1), (-1, -1)
          ]
          self.name = 'Shah'
          self.symbol = 'S'*/

  public:
    Shah(Position pos, const std::weak_ptr<Player> &player)
        : Piece(pos, player, "Shah", 'S', false, false, false,
                {{0, 1}, {1, 0}, {0, -1}, {-1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}}, {})
    {
    }
};
} // namespace shatranj