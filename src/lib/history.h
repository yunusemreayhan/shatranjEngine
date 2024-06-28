#pragma once

#include <memory>
#include <stack>

#include "shatranc_piece.h"
#include "types.h"

namespace shatranj
{
class Position;
class Piece;
enum class ChessPieceEnum : std::uint8_t;
struct HistoryPoint
{
    Position from;
    Position to;
    std::unique_ptr<Piece> captured;
    bool promoted;
    Color color;
    ChessPieceEnum lastMovedPieceType;

    HistoryPoint(Position frompos, Position topos, ChessPieceEnum lastMovedPieceType, std::unique_ptr<Piece> captured = nullptr, bool promoted = false, Color color = Color::kWhite);
};

struct MoveHistory
{
    void AddMove(const Position &frompos, const Position &topos, ChessPieceEnum lastMovedPieceType,
                 std::unique_ptr<Piece> captured = nullptr, bool promoted = false, Color color = Color::kWhite)
    {
        auto toinsert =
            std::make_unique<HistoryPoint>(frompos, topos, lastMovedPieceType, std::move(captured), promoted, color);
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