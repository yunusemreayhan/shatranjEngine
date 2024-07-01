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

    double OklideanDistance() const {
        return std::sqrt(pow(x_, 2) + pow(y_, 2));
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
    explicit Position(std::pair<uint8_t, uint8_t> posvalues) : x_(posvalues.first), y_(posvalues.second)
    {
    }

    explicit Position(uint8_t x, uint8_t y) : x_(x), y_(y)
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

    bool operator<(const Position &other) const
    {
        return x_ < other.x_ || (x_ == other.x_ && y_ < other.y_);
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

struct Movement
{
    Position from;
    Position to;

    explicit Movement(const Position &from, const Position &to) : from(from), to(to)
    {
    }

    explicit Movement(std::pair<Position, Position> movement) : from(movement.first), to(movement.second)
    {
    }

    explicit Movement(const std::string &from, const std::string &to) : from(from), to(to)
    {
    }

    std::string ToString() const
    {
        return from.ToString() + to.ToString();
    }

    static Movement GetEmpty()
    {
        return Movement(Position(0, 0), Position(0, 0));
    }
};

} // namespace shatranj