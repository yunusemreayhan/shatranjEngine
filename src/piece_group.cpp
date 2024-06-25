#include "piece_group.h"

#include <algorithm>

namespace shatranj
{
bool PieceGroup::AddPiece(const std::shared_ptr<Piece> &piece)
{
    if (HasPiece(piece->GetPos()))
    {
        return false;
    }
    pieces_.push_back(piece);
    return true;
}

void PieceGroup::RemovePiece(const Position &pos)
{
    for (auto it = pieces_.begin(); it != pieces_.end(); it++)
    {
        if ((*it)->GetPos() == pos)
        {
            pieces_.erase(it);
            break;
        }
    }
}

void PieceGroup::RemovePiece(const std::shared_ptr<Piece> &piece)
{
    pieces_.erase(std::remove_if(pieces_.begin(), pieces_.end(),
                                 [&piece](const std::shared_ptr<Piece> &current) -> bool {
                                     return current->GetPos() == piece->GetPos();
                                 }),
                  pieces_.end());
}

bool PieceGroup::HasPiece(const Position &pos)
{
    for (auto it = pieces_.begin(); it != pieces_.end(); it++)
    {
        if ((*it)->GetPos() == pos)
        {
            return true;
        }
    }
    return false;
}

std::optional<std::shared_ptr<Piece>> PieceGroup::GetPiece(const Position &pos)
{
    for (auto it = pieces_.begin(); it != pieces_.end(); it++)
    {
        if ((*it)->GetPos() == pos)
        {
            return *it;
        }
    }
    return std::nullopt;
}
} // namespace shatranj