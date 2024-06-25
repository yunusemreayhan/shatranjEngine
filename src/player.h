#pragma once

#include <memory>
#include <string>

#include "piece_group.h"
#include "types.h"
namespace shatranj
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
    std::string name_;
    Color color_;
    std::shared_ptr<PieceGroup> pieces_;
};
} // namespace shatranj