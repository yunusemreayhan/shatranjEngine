#pragma once

#include <memory>
#include <vector>

#include "player.h"
#include "position.h"
#include "board.h"
#include "types.h"

class Board;
class Player;

class ShatranjPiece : public std::enable_shared_from_this<ShatranjPiece>
{
  public:
    explicit ShatranjPiece(Position pos, const std::weak_ptr<Player> &player);

    Position GetPos()
    {
        return pos_;
    }

    std::weak_ptr<Player> GetPlayer()
    {
        return player_;
    }

    bool CanMove(Position pos, std::shared_ptr<Board> &board, bool ctrlCheck = true);

 
    std::shared_ptr<ShatranjPiece> GetSharedFromThis()
    {
        return shared_from_this();
    }
  private:
    Position pos_;
    std::vector<std::pair<int, int>> possibleRegularMoves_;
    std::vector<std::pair<int, int>> possibleCaptureMoves_;
    bool multipleMove_ = false;
    bool canJumpOverOthers_ = false;
    bool moved_ = false;
    std::weak_ptr<Player> player_;
};

class Rook : public ShatranjPiece
{
};
class King : public ShatranjPiece
{
};
class Piyade : public ShatranjPiece
{
};
class Knight : public ShatranjPiece
{
};
class Fil : public ShatranjPiece
{
};
class Vizier : public ShatranjPiece
{
};
class Shah : public ShatranjPiece
{
};