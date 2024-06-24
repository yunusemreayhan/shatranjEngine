#pragma once

#include <algorithm>
#include <memory>
#include <string>

#include "piece_group.h"
#include "types.h"
namespace Shatranj
{
class Piece;
class Position;
class PieceGroup;

class Player
{
  public:
    Player(std::string &name, Color color) : name_(name), color_(color)
    {
    }

    std::shared_ptr<PieceGroup> &GetPieces();

    bool operator==(const Player &other) const
    {
        return color_ == other.color_ && name_ == other.name_;
    }

    Color GetColor()
    {
        return color_;
    }

  private:
    Color color_;
    std::string name_;
    std::shared_ptr<PieceGroup> pieces_;
};
} // namespace Shatranj