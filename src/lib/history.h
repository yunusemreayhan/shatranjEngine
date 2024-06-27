#pragma once

#include <memory>
#include <stack>

#include "shatranc_piece.h"

namespace shatranj
{
class Position;
class Piece;
struct HistoryPoint
{
    Position from;
    Position to;
    std::unique_ptr<Piece> captured;
    bool promoted;
    Color color;

    HistoryPoint(Position frompos, Position topos, std::unique_ptr<Piece> captured = nullptr, bool promoted = false, Color color = Color::kWhite)
        : from(frompos), to(topos), captured(std::move(captured)), promoted(promoted), color(color)
    {
    }
};

struct MoveHistory
{
    void AddMove(const Position &frompos, const Position &topos, std::unique_ptr<Piece> captured = nullptr,
                 bool promoted = false, Color color = Color::kWhite)
    {
        auto toinsert = std::make_unique<HistoryPoint>(frompos, topos, std::move(captured), promoted, color);
        history_.emplace(std::move(toinsert));
    }

    const std::unique_ptr<HistoryPoint> &GetLastMove() const
    {
        return history_.top();
    }

    void PopLastMove()
    {
        history_.pop();
    }

    const std::stack<std::unique_ptr<HistoryPoint>> &GetHistory() const
    {
        return history_;
    }

    void Clear()
    {
        while (!history_.empty())
        {
            history_.pop();
        }
    }

  private:
    std::stack<std::unique_ptr<HistoryPoint>> history_;
};
} // namespace shatranj