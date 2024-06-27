#pragma once

#include "board.h"
#include "position.h"
#include "shatranc_piece.h"
#include <algorithm>
#include <cstddef>
#include <memory>
namespace shatranj
{

class Piece;
enum class ChessPieceEnum : uint8_t;
class Board;
enum class Color : bool;
class PieceGroup
{
  public:
    PieceGroup()
    {
        pieces_ = std::vector<Piece>();
    }
    using iterator = std::vector<Piece>::iterator;
    using cons_iterator = std::vector<Piece>::const_iterator;
    using reverse_iterator = std::vector<Piece>::reverse_iterator;
    using const_reverse_iterator = std::vector<Piece>::const_reverse_iterator;

    bool AddPiece(const Piece &piece);
    void RemovePiece(const Position &pos);
    void RemovePiece(const Piece &piece);
    bool HasPiece(const Position &pos);
    std::optional<Piece*> GetPiece(const Position &pos);
    std::optional<Piece> GetPieceByVal(const Position &pos);
    std::vector<Piece> GetSubPieces(Color color);

    auto begin() -> iterator
    {
        return pieces_.begin();
    }
    auto end() -> iterator
    {
        return pieces_.end();
    }

    auto begin() const -> cons_iterator
    {
        return pieces_.begin();
    }
    auto end() const -> cons_iterator
    {
        return pieces_.end();
    }

    auto rbegin() -> reverse_iterator
    {
        return pieces_.rbegin();
    }
    auto rend() -> reverse_iterator
    {
        return pieces_.rend();
    }

    auto rbegin() const -> const_reverse_iterator
    {
        return pieces_.rbegin();
    }
    auto rend() const -> const_reverse_iterator
    {
        return pieces_.rend();
    }

    size_t size() const
    {
        return pieces_.size();
    }

    bool empty() const
    {
        return pieces_.empty();
    }

    bool is_all_instance_of(ChessPieceEnum chessPiece) const;

    Piece &get(size_t index);

    std::vector<std::pair<Position, Position>> GetPossibleMoves(Color color, const std::shared_ptr<Board> &board);

  private:
    std::vector<Piece> pieces_;
    constexpr static bool inline kDebug = false;
};
} // namespace shatranj