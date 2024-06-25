#pragma once

#include "position.h"
#include "shatranc_piece.h"
#include <cstddef>
#include <iterator>
namespace shatranj
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

    size_t size() const
    {
        return pieces_.size();
    }

    bool empty() const
    {
        return pieces_.empty();
    }

    template<typename T>
    bool is_all_instance_of() const {
        for (const auto &piece : pieces_) {
            if (!std::dynamic_pointer_cast<T>(piece)) {
                return false;
            }
        }
        return true;
    }

    std::shared_ptr<Piece> get(size_t index) const
    {
        return pieces_[index];
    }

  private:
    std::vector<std::shared_ptr<Piece>> pieces_;
};
} // namespace shatranj