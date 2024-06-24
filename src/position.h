#pragma once

#include <string>

class Position {
public:
    explicit Position(std::string fromString) {
        x_ = fromString[0] - 'a';
        y_ = fromString[1] - '1';
    }
    explicit Position (std::pair<int, int> posvalues) : x_(posvalues.first), y_(posvalues.second) {}

    [[nodiscard]] auto IsValid() const -> bool
    {
        return x_ >= 0 && x_ < 8 && y_ >= 0 && y_ < 8;
    }

    [[nodiscard]] auto ToString() const -> std::string
    {
        return {static_cast<char>(x_ + 'a'), static_cast<char>(y_ + '1')};
    }

    [[nodiscard]] int Getx() const { return x_; }
    [[nodiscard]] int Gety() const { return y_; }

    bool operator==(const Position &other) const {
        return x_ == other.x_ && y_ == other.y_;
    }

    std::pair<int, int> Diff(const Position &other) const {
        return {x_ - other.x_, y_ - other.y_};
    }

private:
    int x_, y_;
};