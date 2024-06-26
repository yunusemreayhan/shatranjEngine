#pragma once

#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
namespace shatranj
{

class Step
{
  public:
    Step() = default;
    Step(int8_t posx, int8_t posy) : x_(posx), y_(posy)
    {
    }

    constexpr int8_t Diffx() const
    {
        return x_;
    }

    constexpr int8_t Diffy() const
    {
        return y_;
    }
    constexpr double Diffxd() const
    {
        return x_;
    }

    constexpr double Diffyd() const
    {
        return y_;
    }

    bool operator==(const Step &other) const
    {
        return x_ == other.x_ && y_ == other.y_;
    }

    void operator*=(int8_t factor)
    {
        int tempx = x_ * factor;
        int tempy = y_ * factor;

        if (tempy > std::numeric_limits<int8_t>::max())
            std::cerr << "y overflow" << std::endl;

        if (tempx > std::numeric_limits<int8_t>::max())
            std::cerr << "x overflow" << std::endl;

        x_ = static_cast<int8_t>(tempx);
        y_ = static_cast<int8_t>(tempy);
    }
    static Step StepFromDouble(double posx, double posy)
    {
        return {static_cast<int8_t>(std::floor(posx)), static_cast<int8_t>(std::floor(posy))};
    }

    int8_t x_;
    int8_t y_;
};

class Position
{
  public:
    explicit Position(std::string fromString)
    {
        x_ = fromString[0] - 'a';
        y_ = fromString[1] - '1';
    }
    explicit Position(std::pair<int, int> posvalues) : x_(posvalues.first), y_(posvalues.second)
    {
    }

    [[nodiscard]] auto IsValid() const -> bool
    {
        return x_ >= 0 && x_ < 8 && y_ >= 0 && y_ < 8;
    }

    [[nodiscard]] auto ToString() const -> std::string
    {
        return {static_cast<char>(x_ + 'a'), static_cast<char>(y_ + '1')};
    }

    [[nodiscard]] int Getx() const
    {
        return x_;
    }
    [[nodiscard]] int Gety() const
    {
        return y_;
    }

    bool operator==(const Position &other) const
    {
        return x_ == other.x_ && y_ == other.y_;
    }

    bool operator!=(const Position &other) const
    {
        return !(*this == other);
    }

    Step Diff(const Position &other) const
    {
        return {static_cast<int8_t>(x_ - other.x_), static_cast<int8_t>(y_ - other.y_)};
    }

    void Move(const std::pair<int, int> &step)
    {
        x_ = x_ + step.first;
        y_ = y_ + step.second;
    }

    void Move(const Step &step)
    {
        x_ = x_ + step.Diffx();
        y_ = y_ + step.Diffy();
    }

  private:
    uint8_t x_ : 4;
    uint8_t y_ : 4;
};

} // namespace shatranj