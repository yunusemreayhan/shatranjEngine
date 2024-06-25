#pragma once

#include <memory>
#include <ostream>
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
    Player(const std::string &name, Color color) : name_(name), color_(color)
    {
    }

    std::shared_ptr<PieceGroup> &GetPieces();

    bool operator==(const Player &other) const
    {
        return color_ == other.color_ && name_ == other.name_;
    }

    Color GetColor() const
    {
        return color_;
    }

    std::string GetName() const
    {
        return name_;
    }

    friend std::ostream &operator<<(std::ostream &ostr, const Player &player)
    {
        ostr << player.GetName();
        return ostr;
    }

  private:
    std::string name_;
    Color color_;
    std::shared_ptr<PieceGroup> pieces_;
};


} // namespace shatranj