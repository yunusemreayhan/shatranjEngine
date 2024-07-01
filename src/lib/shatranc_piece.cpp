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
    : pieceType_(pieceType), isWhite_(color == Color::kWhite), moved_(moved)
{
}

Piece::Piece(ChessPieceEnum pieceType, Position pos, const Color color, bool moved)
    : PiecePrimitive(pieceType, color, moved), pos_(pos)
{
    if (!pos.IsValid())
        throw std::invalid_argument("Position is invalid " + pos.ToString());
}

bool Piece::CanMove(Position frompos, Position pos, const std::shared_ptr<Board> &board, ChessPieceEnum pieceType,
                    Color color)
{
    if (frompos == pos)
    {
        return false;
    }
    if (!pos.IsValid())
    {
        return false;
    }

    auto diff = pos.Diff(frompos);
    if (!CanMultipleMove(pieceType))
    {
        if (std::find(GetPossibleRegularMoves(pieceType, color).begin(),
                      GetPossibleRegularMoves(pieceType, color).end(),
                      diff) == GetPossibleRegularMoves(pieceType, color).end())
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
                const auto findres = std::find(GetPossibleRegularMoves(pieceType, color).begin(),
                                               GetPossibleRegularMoves(pieceType, color).end(), multiplemoveinstance);
                if (findres != GetPossibleRegularMoves(pieceType, color).end())
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
    const auto piece = board->GetPieces()->GetPieceByVal(pos);
    if (pieceType != ChessPieceEnum::kPiyade && piece)
    {
        if ((*piece).GetColor() == color)
        {
            return false;
        }
    }
    else if (pieceType == ChessPieceEnum::kPiyade && piece)
    {
        // piyade can not move over another piece
        return false;
    }

    if (!(CanJumpOverOthers(pieceType) || board->IsPathClear(frompos, pos)))
    {
        return false;
    }

    return true;
}

bool Piece::CanThreat(Position frompos, Position pos, const std::shared_ptr<Board> &board, ChessPieceEnum pieceType,
                      Color color)
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
    // target pos should have opponent piece to pawn can make a thread
    auto target_piece = board->GetPieces()->GetPieceByVal(pos);
    if (!target_piece)
    {
        return false;
    }
    if ((*target_piece).GetColor() == color)
    {
        return false;
    }
    auto diff = pos.Diff(frompos);
    if (!CanMultipleMove(pieceType))
    {
        if (std::find(GetPossibleCaptureMoves(pieceType, color).begin(),
                      GetPossibleCaptureMoves(pieceType, color).end(),
                      diff) == GetPossibleCaptureMoves(pieceType, color).end())
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
                const auto findres = std::find(GetPossibleCaptureMoves(pieceType, color).begin(),
                                               GetPossibleCaptureMoves(pieceType, color).end(), multiplemoveinstance);
                if (findres != GetPossibleCaptureMoves(pieceType, color).end())
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

    if (!(CanJumpOverOthers(pieceType) || board->IsPathClear(frompos, pos)))
    {
        return false;
    }
    return true;
}

bool Piece::CanCapture(Position frompos, Position pos, const std::shared_ptr<Board> &board, ChessPieceEnum pieceType,
                       Color color)
{
    if (!CanThreat(frompos, pos, board, pieceType, color))
    {
        return false;
    }
    auto piece = board->GetPieces()->GetPieceByVal(pos);
    if (!piece || (*piece).GetColor() == color)
    {
        // if the piece is not on the board or it belongs to same player
        return false;
    }
    return CanJumpOverOthers(pieceType) || board->IsPathClear(frompos, pos);
}

bool Piece::CanGo(Position frompos, Position pos, const std::shared_ptr<Board> &board, ChessPieceEnum pieceType,
                  Color color)
{
    return CanCapture(frompos, pos, board, pieceType, color) || CanMove(frompos, pos, board, pieceType, color);
}

bool Piece::Move(Position pos)
{
    if (!pos.IsValid())
    {
        return false;
    }
    pos_ = pos;
    moved_ = true;
    return true;
}

std::vector<Movement> Piece::GetPossibleMoves(Position frompos, const std::shared_ptr<Board> &board,
                                              ChessPieceEnum pieceType, Color color)
{
    if (board->GetCurrentTurn() != color)
    {
        return {};
    }
    std::vector<Movement> possible_moves;
    for (const auto &diff : GetPossibleRegularMoves(pieceType, color))
    {
        if (!CanMultipleMove(pieceType))
        {
            Position pos = frompos;
            pos.Move(diff);
            if (!pos.IsValid())
            {
                continue;
            }
            if (CanMove(frompos, pos, board, pieceType, color))
            {
                if (!board->WouldBeInCheck(Movement{frompos, pos}))
                    possible_moves.push_back(Movement(frompos, pos));
            }
        }
        else
        {
            for (int i = 1; i < 8; i++)
            {
                Position pos = frompos;
                pos.Move(std::make_pair(static_cast<int>(std::floor(diff.Diffx() * i)),
                                        static_cast<int>(std::floor(diff.Diffy() * i))));
                if (!pos.IsValid())
                    continue;

                if (CanMove(frompos, pos, board, pieceType, color))
                {
                    if (!board->WouldBeInCheck(Movement{frompos, pos}))
                        possible_moves.push_back(Movement(frompos, pos));
                }
            }
        }
    }

    if (pieceType == ChessPieceEnum::kPiyade)
    {
        for (const auto &diff : GetPossibleCaptureMoves(pieceType, color))
        {
            Position pos = frompos;
            pos.Move(diff);
            if (!pos.IsValid())
            {
                continue;
            }
            if (CanCapture(frompos, pos, board, pieceType, color))
            {
                if (!board->WouldBeInCheck(Movement{frompos, pos}))
                    possible_moves.push_back(Movement(frompos, pos));
            }
        }
    }
    return possible_moves;
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