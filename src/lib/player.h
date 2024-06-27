#pragma once

#include <cstdint>
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
    Player(const std::string &name, Color color) : name_(name), isWhite_(color == Color::kWhite)
    {
    }


    bool operator==(const Player &other) const
    {
        return GetColor() == other.GetColor() && name_ == other.name_;
    }

    Color GetColor() const
    {
        return isWhite_ ? Color::kWhite : Color::kBlack;
    }

    std::string GetName() const
    {
        return name_;
    }

    friend std::ostream &operator<<(std::ostream &ostr, const Player &player)
    {
        ostr << player.GetName() << " " << (player.GetColor() == Color::kWhite ? "White" : "Black");
        return ostr;
    }

  private:
    std::string name_;
    uint8_t isWhite_:1;
};


} // namespace shatranj