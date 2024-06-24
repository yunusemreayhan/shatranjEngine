#pragma once

#include "position.h"
#include "shatranc_piece.h"
#include <iterator>
namespace Shatranj
{
class PieceGroup
{
  public:
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

  private:
    std::vector<std::shared_ptr<Piece>> pieces_;
};
} // namespace Shatranj