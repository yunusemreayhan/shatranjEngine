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
class PieceGroup
{
  public:
    PieceGroup() {pieces_ = std::vector<std::shared_ptr<Piece>>();}
    using iterator = std::vector<std::shared_ptr<Piece>>::iterator;
    using cons_iterator = std::vector<std::shared_ptr<Piece>>::const_iterator;
    using reverse_iterator = std::vector<std::shared_ptr<Piece>>::reverse_iterator;
    using const_reverse_iterator = std::vector<std::shared_ptr<Piece>>::const_reverse_iterator;

    bool AddPiece(const std::shared_ptr<Piece> &piece);
    void RemovePiece(const Position &pos);
    void RemovePiece(const std::shared_ptr<Piece> &piece);
    bool HasPiece(const Position &pos);
    std::optional<std::shared_ptr<Piece>> GetPiece(const Position &pos);

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

    std::shared_ptr<Piece> get(size_t index) const
    {
        return pieces_[index];
    }
    
    std::vector<std::pair<Position, Position>> GetPossibleMoves(const std::shared_ptr<Board> &board);

  private:
    std::vector<std::shared_ptr<Piece>> pieces_;
};
} // namespace shatranj