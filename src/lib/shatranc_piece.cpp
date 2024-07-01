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

const std::vector<Step> PiecePrimitive::kRookSteps = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
const std::vector<Step> PiecePrimitive::kPiyadeWhiteSteps = {{0, +1}};
const std::vector<Step> PiecePrimitive::kPiyadeBlackSteps = {{0, -1}};

const std::vector<Step> PiecePrimitive::kVizierSteps = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

const std::vector<Step> PiecePrimitive::kShahSteps = {{0, 1}, {1, 0},  {0, -1}, {-1, 0},
                                                      {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

const std::vector<Step> PiecePrimitive::kHorseSteps = {{+1, +2}, {+2, +1}, {-1, +2}, {-2, +1},
                                                       {-1, -2}, {-2, -1}, {+1, -2}, {+2, -1}};

const std::vector<Step> PiecePrimitive::kFilSteps = {{2, 2}, {2, -2}, {-2, 2}, {-2, -2}};

const std::vector<Step> PiecePrimitive::kPiyadeWhiteCaptureSteps = {{1, +1}, {-1, +1}};

const std::vector<Step> PiecePrimitive::kPiyadeBlackCaptureSteps = {{1, -1}, {-1, -1}};

const std::vector<Step> PiecePrimitive::kEmptySteps = {};

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

bool Piece::CanMove(Position frompos, Position topos, ChessPieceEnum pieceType, Color color)
{
    if (frompos == topos)
    {
        return false;
    }
    if (!topos.IsValid())
    {
        return false;
    }

    auto diff = topos.Diff(frompos);
    auto regmoves = GetPossibleRegularMoves(pieceType, color);
    if (!CanMultipleMove(pieceType))
    {
        if (std::find(regmoves.begin(), regmoves.end(), diff) == regmoves.end())
        {
            return false;
        }
    }
    else
    {
        bool check = false;
        for (int i = 1; i < 8; i++)
        {
            if (diff.Diffx() % i == 0 && diff.Diffy() % i == 0)
            {
                Step multiplemoveinstance = Step::StepFromDouble(diff.Diffxd() / i, diff.Diffyd() / i);
                const auto findres = std::find(regmoves.begin(), regmoves.end(), multiplemoveinstance);
                if (findres != regmoves.end())
                {
                    check = true;
                    break;
                }
            }
        }
        if (!check)
        {
            return false;
        }
    }
    return true;
}

bool Piece::CanPawnCapture(Position frompos, Position pos, ChessPieceEnum pieceType, Color color)
{
    if (pieceType != ChessPieceEnum::kPiyade)
    {
        return false;
    }
    if (frompos == pos)
    {
        return false;
    }
    if (!pos.IsValid())
    {
        return false;
    }
    auto capmoves = GetPossibleCaptureMoves(pieceType, color);
    auto diff = pos.Diff(frompos);
    if (!CanMultipleMove(pieceType))
    {
        if (std::find(capmoves.begin(), capmoves.end(), diff) == capmoves.end())
        {
            return false;
        }
    }
    else
    {
        bool check = false;
        for (int i = 1; i < 8; i++)
        {
            if (diff.Diffx() % i == 0 && diff.Diffy() % i == 0)
            {
                const auto multiplemoveinstance = Step::StepFromDouble(diff.Diffxd() / i, diff.Diffyd() / i);
                const auto findres = std::find(capmoves.begin(), capmoves.end(), multiplemoveinstance);
                if (findres != capmoves.end())
                {
                    check = true;
                    break;
                }
            }
        }
        if (!check)
        {
            return false;
        }
    }
    return true;
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

Piyade::Piyade(Position pos, Color color) : Piece(ChessPieceEnum::kPiyade, pos, color, false)
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