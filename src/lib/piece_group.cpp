#include "piece_group.h"

#include <algorithm>

namespace shatranj
{
bool PieceGroup::AddPiece(const Piece &piece)
{
    if (HasPiece(piece.GetPos()))
    {
        return false;
    }
    if constexpr (kDebug)
    {
        std::cout << "Adding " << piece.GetPos().ToString() << std::endl;
    }
    pieces_.push_back(piece);
    return true;
}

void PieceGroup::RemovePiece(const Position &pos)
{
    for (auto it = pieces_.begin(); it != pieces_.end(); it++)
    {
        if ((*it).GetPos() == pos)
        {
            pieces_.erase(it);
            break;
        }
    }
}

void PieceGroup::RemovePiece(const Piece &piece)
{
    pieces_.erase(
        std::remove_if(pieces_.begin(), pieces_.end(), [&piece](const Piece &pieceitr) { return piece == pieceitr; }),
        pieces_.end());
}

bool PieceGroup::HasPiece(const Position &pos)
{
    if (empty())
    {
        return false;
    }
    for (auto it = pieces_.begin(); it != pieces_.end(); it++)
    {
        if ((it)->GetPos() == pos)
        {
            return true;
        }
    }
    return false;
}

std::optional<Piece*> PieceGroup::GetPiece(const Position &pos)
{
    for (auto it = pieces_.begin(); it != pieces_.end(); it++)
    {
        if ((it)->GetPos() == pos)
        {
            return &(*it);
        }
    }
    return std::nullopt;
}

std::optional<Piece> PieceGroup::GetPieceByVal(const Position &pos)
{
    for (auto it = pieces_.begin(); it != pieces_.end(); it++)
    {
        if ((it)->GetPos() == pos)
        {
            return (*it);
        }
    }
    return std::nullopt;
}
std::vector<Piece> PieceGroup::GetSubPieces(Color color)
{
    std::vector<Piece> ret;
    for (auto &piece : pieces_)
    {
        if (piece.GetColor() == color)
        {
            ret.push_back(piece);
        }
    }
    return ret;
}

std::vector<std::pair<Position, Position>> PieceGroup::GetPossibleMoves(Color color, const std::shared_ptr<Board> &board)
{
    std::vector<std::pair<Position, Position>> ret;
    for (auto &piece : pieces_)
    {
        if (piece.GetColor() != color)
        {
            continue;
        }
        auto insertable = piece.GetPossibleMoves(board);
        for (auto it = insertable.begin(); it != insertable.end(); it++)
        {
            ret.push_back(*it);
        }
    }
    return ret;
}

bool PieceGroup::is_all_instance_of(ChessPieceEnum chessPiece) const
{
    return std::all_of(pieces_.begin(), pieces_.end(),
                       [chessPiece](const auto &piece) { return piece.GetPieceType() == chessPiece; });
}
Piece &PieceGroup::get(size_t index)
{
    return pieces_[index];
}
} // namespace shatranj