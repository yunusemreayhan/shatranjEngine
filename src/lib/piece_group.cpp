#include "piece_group.h"
#include "position.h"
#include "shatranc_piece.h"

#include <algorithm>

namespace shatranj
{
bool PieceGroup::AddPiece(const Piece &piece)
{
    if (positions_.find(piece.GetPos()) != positions_.end())
    {
        if constexpr (kPieceGroupDebug)
        {
            std::cout << "Already have " << piece.GetPos().ToString() << std::endl;
        }
        return false;
    }
    if constexpr (kPieceGroupDebug)
    {
        std::cout << "Adding " << piece.GetPos().ToString() << std::endl;
    }
    positions_.insert(piece.GetPos());
    pieces_.push_back(piece);
    return true;
}

void PieceGroup::RemovePiece(const Position &pos)
{
    if constexpr (kPieceGroupDebug)
    {
        std::cout << "Removing have " << pos.ToString() << std::endl;
    }
    positions_.erase(pos);
    pieces_.erase(std::remove_if(pieces_.begin(), pieces_.end(),
                                 [&pos](const Piece &pieceitr) { return pos == pieceitr.GetPos(); }),
                  pieces_.end());
}

bool PieceGroup::MovePiece(const Position &frompos, const Position &topos)
{
    if (positions_.find(frompos) == positions_.end())
    {
        if constexpr (kPieceGroupDebug)
        {
            std::cout << "No piece at " << frompos.ToString() << std::endl;
        }
        return false;
    }
    positions_.erase(frompos);
    positions_.insert(topos);
    (*GetPiece(frompos, false))->Move(topos);
    return true;
}

std::optional<Piece *> PieceGroup::GetPiece(const Position &pos, bool check)
{
    if (check && positions_.find(pos) == positions_.end())
    {
        if constexpr (kPieceGroupDebug)
        {
            std::cout << "No piece at " << pos.ToString() << std::endl;
        }
        return std::nullopt;
    }
    for (auto it = pieces_.begin(); it != pieces_.end(); it++)
    {
        if ((it)->GetPos() == pos)
        {
            return &(*it);
        }
    }
    if constexpr (kPieceGroupDebug)
    {
        std::cout << "No piece at " << pos.ToString() << " set says it must exist" << std::endl;
    }
    return std::nullopt;
}

std::optional<Piece> PieceGroup::GetPieceByVal(const Position &pos)
{
    if (positions_.find(pos) == positions_.end())
    {
        return std::nullopt;
    }
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

const std::vector<Movement> &PieceGroup::GetPossibleMoves(Color color, const std::shared_ptr<Board> &board)
{
    auto fen = board->GenerateFEN();
    if (possibleMovesMemory_.Have(fen))
    {
        return possibleMovesMemory_.Get(fen);
    }
    std::vector<Movement> ret;
    if constexpr (kPieceGroupDebug)
        std::cout << "Getting possible moves for " << color << std::endl;
    auto positions = GetPositions();
    for (const auto &pos : positions)
    {
        const auto piece = *board->GetPieces()->GetPieceByVal(pos);
        if (piece.GetColor() != color)
        {
            continue;
        }
        auto insertable = Piece::GetPossibleMoves(piece.GetPos(), board, piece.GetPieceType(), piece.GetColor());
        for (auto it = insertable.begin(); it != insertable.end(); it++)
        {
            if (it->from == it->to)
            {
                throw std::runtime_error("from == to in GetPossibleMoves " + it->from.ToString());
            }
            ret.push_back(*it);
        }
    }
    if constexpr (kPieceGroupDebug)
        std::cout << "Got " << ret.size() << " possible moves" << std::endl;

    possibleMovesMemory_.Add(fen, std::move(ret));
    return possibleMovesMemory_.Get(fen);
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