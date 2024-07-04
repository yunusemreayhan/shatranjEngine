#include <algorithm>
#include <cmath>
#include <memory>
#include <utility>

#include "board.h"
#include "player.h"
#include "position.h"
#include "shatranc_piece.h"
namespace shatranj
{

std::array<
    std::array<std::array<PreComputeTableInternalContainer, static_cast<size_t>(ChessPieceEnum::kCountpiecetypes)>, 8>,
    8>
    PiecePrimitive::move_per_square_table;
std::array<
    std::array<std::array<PreComputeTableInternalContainer, static_cast<size_t>(ChessPieceEnum::kCountpiecetypes)>, 8>,
    8>
    PiecePrimitive::capture_per_square_table;

PiecePrimitive::PiecePrimitive(ChessPieceEnum pieceType, Color color, bool moved)
{
    SetPieceType(pieceType);
    SetWhite(color == Color::kWhite);
    SetMoved(moved);
}

Piece::Piece(ChessPieceEnum pieceType, Position pos, const Color color, bool moved)
    : PiecePrimitive(pieceType, color, moved), pos_(pos)
{
    if (!pos.IsValid())
        throw std::invalid_argument("Position is invalid " + pos.ToString());
}

bool Piece::CanMove(Position frompos, Position topos, ChessPieceEnum pieceType)
{
    if (frompos == topos)
    {
        return false;
    }
    if (!topos.IsValid())
    {
        return false;
    }

    return CanMoveWithMem(pieceType, frompos, topos);
}

bool Piece::CanPawnCapture(Position frompos, Position topos, ChessPieceEnum pieceType)
{
    if (pieceType != ChessPieceEnum::kPiyadeBlack && pieceType != ChessPieceEnum::kPiyadeWhite)
    {
        return false;
    }
    if (frompos == topos)
    {
        return false;
    }
    if (!topos.IsValid())
    {
        return false;
    }
    return CanCaptureWithMem(pieceType, frompos, topos);
}

bool Piece::Move(Position pos)
{
    if (!pos.IsValid())
    {
        return false;
    }
    pos_ = pos;
    SetMoved(true);
    return true;
}

Shah::Shah(Position pos, Color color) : Piece(ChessPieceEnum::kShah, pos, color, false)
{
}
Rook::Rook(Position pos, Color color) : Piece(ChessPieceEnum::kRook, pos, color, false)
{
}

Piyade::Piyade(Position pos, Color color)
    : Piece(color == Color::kWhite ? ChessPieceEnum::kPiyadeWhite : ChessPieceEnum::kPiyadeBlack, pos, color, false)
{
}

Horse::Horse(Position pos, Color color) : Piece(ChessPieceEnum::kHorse, pos, color, false)
{
}

Fil::Fil(Position pos, Color color) : Piece(ChessPieceEnum::kFil, pos, color, false)
{
}

Vizier::Vizier(Position pos, Color color) : Piece(ChessPieceEnum::kVizier, pos, color, false)
{
}

} // namespace shatranj
