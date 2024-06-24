#pragma once

#include "shatranc_piece.h"
#include "position.h"
#include <iterator>

class PieceGroup
{
  public:
    using iterator = std::vector<std::shared_ptr<ShatranjPiece>>::iterator;
    using cons_iterator = std::vector<std::shared_ptr<ShatranjPiece>>::const_iterator;
    using reverse_iterator = std::vector<std::shared_ptr<ShatranjPiece>>::reverse_iterator;
    using const_reverse_iterator = std::vector<std::shared_ptr<ShatranjPiece>>::const_reverse_iterator;

    bool AddPiece(const std::shared_ptr<ShatranjPiece> &piece);
    void RemovePiece(const Position &pos);
    void RemovePiece(const std::shared_ptr<ShatranjPiece> &piece);
    bool HasPiece(const Position &pos);
    std::optional<std::shared_ptr<ShatranjPiece>> GetPiece(const Position &pos);

    auto begin() -> iterator { return pieces_.begin(); }
    auto end() -> iterator { return pieces_.end(); }

    auto begin() const -> cons_iterator { return pieces_.begin(); }
    auto end() const -> cons_iterator { return pieces_.end(); }

    auto rbegin() -> reverse_iterator { return pieces_.rbegin(); }
    auto rend() -> reverse_iterator { return pieces_.rend(); }

    auto rbegin() const -> const_reverse_iterator { return pieces_.rbegin(); }
    auto rend() const -> const_reverse_iterator { return pieces_.rend(); }
  private:
    std::vector<std::shared_ptr<ShatranjPiece>> pieces_;
};