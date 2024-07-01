#include "piece_group.h"
#include "position.h"
#include "shatranc_piece.h"
#include "types.h"

#include <algorithm>
#include <optional>

namespace shatranj
{

PieceGroup::PieceGroup()
{
    pieces_primitive_ = std::vector<PiecePrimitive>(64, PiecePrimitive(ChessPieceEnum::kNone, Color::kWhite, false));
    pieces_primitive_.reserve(64);
}
bool PieceGroup::AddPiece(const PiecePrimitive &piece, const Position &pos)
{
    auto calccoord = Coord2to1(pos);
    if (pieces_primitive_[calccoord].GetPieceType() != ChessPieceEnum::kNone)
    {
        if constexpr (kPieceGroupDebug)
        {
            std::cout << "Already have " << pos.ToString() << std::endl;
        }
        return false;
    }
    if constexpr (kPieceGroupDebug)
    {
        std::cout << "Adding " << pos.ToString() << " " << static_cast<uint>(calccoord) << std::endl;
    }
    pieces_primitive_[calccoord] = piece;
    if (pieces_primitive_[calccoord].IsShah())
    {
        if (pieces_primitive_[calccoord].GetColor() == Color::kBlack)
        {
            blackShah_ = &pieces_primitive_[calccoord];
            blackShahPos_ = pos;
        }
        else
        {
            whiteShah_ = &pieces_primitive_[calccoord];
            whiteShahPos_ = pos;
        }
    }
    counter_++;
    if (piece.GetColor() == Color::kBlack)
    {
        black_count_++;
    }
    else
    {
        white_count_++;
    }
    return true;
}

void PieceGroup::RemovePiece(const Position &pos)
{
    if constexpr (kPieceGroupDebug)
    {
        std::cout << "Removing have " << pos.ToString() << std::endl;
    }
    if (pieces_primitive_[Coord2to1(pos)].GetColor() == Color::kBlack)
    {
        black_count_--;
    }
    else
    {
        white_count_--;
    }
    pieces_primitive_[Coord2to1(pos)] = PiecePrimitive(ChessPieceEnum::kNone, Color::kWhite, false);
    counter_--;
}

void PieceGroup::RemovePieceNoCounterUpdate(const Position &pos)
{
    if constexpr (kPieceGroupDebug)
    {
        std::cout << "Removing have " << pos.ToString() << std::endl;
    }
    pieces_primitive_[Coord2to1(pos)] = PiecePrimitive(ChessPieceEnum::kNone, Color::kWhite, false);
}

bool PieceGroup::MovePiece(const Position &frompos, const Position &topos)
{
    auto calccoord = Coord2to1(topos);
    auto *target = &pieces_primitive_[calccoord];
    *target = pieces_primitive_[Coord2to1(frompos)];
    RemovePieceNoCounterUpdate(frompos);
    if (target->IsShah())
    {
        if (target->GetColor() == Color::kBlack)
        {
            blackShah_ = target;
            blackShahPos_ = topos;
        }
        else
        {
            whiteShah_ = target;
            whiteShahPos_ = topos;
        }
    }
    return true;
}

std::optional<PiecePrimitive *> PieceGroup::GetPiece(const Position &pos)
{
    PiecePrimitive *ret = &pieces_primitive_[Coord2to1(pos)];
    if (ret->GetPieceType() != ChessPieceEnum::kNone)
        return ret;

    if constexpr (kPieceGroupDebug)
    {
        std::cout << "No piece at " << pos.ToString() << " set says it must exist" << std::endl;
    }
    return std::nullopt;
}

std::optional<Piece> PieceGroup::GetPieceByVal(const Position &pos)
{
    auto coord = Coord2to1(pos);
    PiecePrimitive *ret = &pieces_primitive_[coord];
    if (ret->GetPieceType() != ChessPieceEnum::kNone)
        return Get(coord);
    return std::nullopt;
}

std::vector<Piece> PieceGroup::GetSubPieces(Color color)
{
    std::vector<Piece> ret;
    for (auto pieceitr = pieces_primitive_.begin(); pieceitr != pieces_primitive_.end(); pieceitr++)
    {
        if (pieceitr->GetColor() == color && pieceitr->GetPieceType() != ChessPieceEnum::kNone)
        {
            ret.push_back(FromPiecePrimitive(pieceitr));
        }
    }
    return ret;
}

const std::vector<Movement> &PieceGroup::GetPossibleMoves(Color color, const std::shared_ptr<Board> &board)
{
    auto fen = board->GenerateFEN(false);
    if (possibleMovesMemory_.Have(fen))
    {
        return possibleMovesMemory_.Get(fen);
    }
    std::vector<Movement> ret;
    if constexpr (kPieceGroupDebug)
        std::cout << "Getting possible moves for " << color << std::endl;
    for (auto pieceitr = pieces_primitive_.begin(); pieceitr != pieces_primitive_.end(); pieceitr++)
    {
        const Piece piece = FromPiecePrimitive(pieceitr);
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

bool PieceGroup::IsAllInstanceOf(ChessPieceEnum chessPiece) const
{
    return std::all_of(pieces_primitive_.begin(), pieces_primitive_.end(),
                       [chessPiece](const auto &piece) { return piece.GetPieceType() == chessPiece; });
}
std::optional<Piece> PieceGroup::Get(size_t index)
{
    auto &pieceatindex = pieces_primitive_[index];
    if (pieceatindex.GetPieceType() == ChessPieceEnum::kNone)
        return std::nullopt;
    return FromPiecePrimitive(pieces_primitive_.begin() + index);
}
std::optional<PiecePrimitive *> PieceGroup::GetBlackPtr(size_t index)
{
    auto &pieceatindex = pieces_primitive_[index];
    if (pieceatindex.GetPieceType() == ChessPieceEnum::kNone || pieceatindex.GetColor() == Color::kWhite)
        return std::nullopt;
    return &*(pieces_primitive_.begin() + index);
}
std::optional<PiecePrimitive *> PieceGroup::GetWhitePtr(size_t index)
{
    auto &pieceatindex = pieces_primitive_[index];
    if (pieceatindex.GetPieceType() == ChessPieceEnum::kNone || pieceatindex.GetColor() == Color::kBlack)
        return std::nullopt;
    return &*(pieces_primitive_.begin() + index);
}

void PieceGroup::Clear()
{
    memset((void *)pieces_primitive_.data(), 0, sizeof(PiecePrimitive) * pieces_primitive_.size());
    counter_ = 0;
    black_count_ = 0;
    white_count_ = 0;
}

Piece PieceGroup::FromPiecePrimitive(std::vector<PiecePrimitive>::iterator primitr)
{
    return Piece(*primitr, Position{Coord1to2(std::distance(pieces_primitive_.begin(), primitr))});
}
} // namespace shatranj