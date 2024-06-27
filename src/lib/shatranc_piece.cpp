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

const std::vector<Step> Piece::kRookSteps = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
const std::vector<Step> Piece::kPiyadeWhiteSteps = {{0, +1}};
const std::vector<Step> Piece::kPiyadeBlackSteps = {{0, -1}};

const std::vector<Step> Piece::kVizierSteps = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

const std::vector<Step> Piece::kShahSteps = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

const std::vector<Step> Piece::kHorseSteps = {{+1, +2}, {+2, +1}, {-1, +2}, {-2, +1},
                                              {-1, -2}, {-2, -1}, {+1, -2}, {+2, -1}};

const std::vector<Step> Piece::kFilSteps = {{2, 2}, {2, -2}, {-2, 2}, {-2, -2}};

const std::vector<Step> Piece::kPiyadeWhiteCaptureSteps = {{1, +1}, {-1, +1}};

const std::vector<Step> Piece::kPiyadeBlackCaptureSteps = {{1, -1}, {-1, -1}};

const std::vector<Step> Piece::kEmptySteps = {};

Piece::Piece(ChessPieceEnum pieceType, Position pos, const Color color, bool multipleMove, bool canJumpOverOthers,
             bool moved)
    : pieceType_(pieceType), pos_(pos), color_(color), multipleMove_(multipleMove),
      canJumpOverOthers_(canJumpOverOthers), moved_(moved)
{
}

bool Piece::CanMove(Position pos, const std::shared_ptr<Board> &board, bool ctrlCheck)
{
    if (pos_ == pos)
    {
        return false;
    }
    if (!pos.IsValid())
    {
        return false;
    }

    auto diff = pos.Diff(pos_);
    if (!multipleMove_)
    {
        if (std::find(GetPossibleRegularMoves().begin(), GetPossibleRegularMoves().end(), diff) ==
            GetPossibleRegularMoves().end())
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
                const auto findres =
                    std::find(GetPossibleRegularMoves().begin(), GetPossibleRegularMoves().end(), multiplemoveinstance);
                if (findres != GetPossibleRegularMoves().end())
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
    const auto piece = board->GetPieces()->GetPiece(pos);
    if (!IsPiyade() && piece)
    {
        if ((**piece).GetColor() == GetColor())
        {
            return false;
        }
    }
    else if (IsPiyade() && piece)
    {
        // piyade can not move over another piece
        return false;
    }
    if (ctrlCheck && board->WouldBeInCheck(this, pos))
    {
        return false;
    }

    return canJumpOverOthers_ || board->IsPathClear(pos_, pos);
}

bool Piece::CanThreat(Position pos, const std::shared_ptr<Board> &board, bool ctrlCheck)
{
    if (!IsPiyade())
    {
        return false;
    }
    if (pos_ == pos)
    {
        return false;
    }
    if (!pos.IsValid())
    {
        return false;
    }
    auto diff = pos.Diff(pos_);
    if (!multipleMove_)
    {
        if (std::find(GetPossibleCaptureMoves().begin(), GetPossibleCaptureMoves().end(), diff) ==
            GetPossibleCaptureMoves().end())
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
                const auto findres =
                    std::find(GetPossibleCaptureMoves().begin(), GetPossibleCaptureMoves().end(), multiplemoveinstance);
                if (findres != GetPossibleCaptureMoves().end())
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

    if (ctrlCheck && board->WouldBeInCheck(this, pos))
    {
        return false;
    }
    return canJumpOverOthers_ || board->IsPathClear(pos_, pos);
}

bool Piece::CanCapture(Position pos, const std::shared_ptr<Board> &board, bool ctrlCheck)
{
    /*    def canCapture(self, x: int, y: int, board, ctrlCheck=True):
        if not self.canThreat(x, y, board, ctrlCheck=ctrlCheck):
            return False
        piece = board.getCell(x, y)
        if not piece or piece.player == self.player:
            return False
        return self.canJumpOverOthers or board.isPathClear(self.x, self.y, x, y)*/

    if (!CanThreat(pos, board, ctrlCheck))
    {
        return false;
    }
    auto piece = board->GetPieces()->GetPiece(pos);
    if (!piece || (**piece).GetColor() == GetColor())
    {
        // if the piece is not on the board or it belongs to same player
        return false;
    }
    // TODO(yunus): check if this works below || could be && IMHO
    return canJumpOverOthers_ || board->IsPathClear(pos_, pos);
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

std::vector<std::pair<Position, Position>> Piece::GetPossibleMoves(const std::shared_ptr<Board> &board)
{
    std::vector<std::pair<Position, Position>> possible_moves;
    for (const auto &diff : GetPossibleRegularMoves())
    {
        if (!multipleMove_)
        {
            Position pos = pos_;
            pos.Move(diff);
            if (!pos.IsValid())
            {
                continue;
            }
            if (CanMove(pos, board))
            {
                possible_moves.push_back(std::make_pair(pos_, pos));
            }
        }
        else
        {
            for (int i = 1; i < 8; i++)
            {
                Position pos = pos_;
                pos.Move(std::make_pair(static_cast<int>(std::floor(diff.Diffx() * i)),
                                        static_cast<int>(std::floor(diff.Diffy() * i))));
                if (!pos.IsValid())
                    continue;

                if (CanMove(pos, board))
                {
                    possible_moves.push_back(std::make_pair(pos_, pos));
                }
            }
        }
    }

    if (IsPiyade())
    {
        for (const auto &diff : GetPossibleCaptureMoves())
        {
            Position pos = pos_;
            pos.Move(diff);
            if (!pos_.IsValid())
            {
                continue;
            }
            if (CanCapture(pos, board))
            {
                possible_moves.push_back(std::make_pair(pos_, pos));
            }
        }
    }
    return possible_moves;
}

Shah::Shah(Position pos, Color color) : Piece(ChessPieceEnum::kShah, pos, color, false, false, false)
{
}
Rook::Rook(Position pos, Color color) : Piece(ChessPieceEnum::kRook, pos, color, true, false, false)
{
}

Piyade::Piyade(Position pos, Color color) : Piece(ChessPieceEnum::kPiyade, pos, color, false, false, false)
{
}

Horse::Horse(Position pos, Color color) : Piece(ChessPieceEnum::kHorse, pos, color, false, true, false)
{
}

Fil::Fil(Position pos, Color color) : Piece(ChessPieceEnum::kFil, pos, color, false, true, false)
{
}

Vizier::Vizier(Position pos, Color color) : Piece(ChessPieceEnum::kVizier, pos, color, false, false, false)
{
}

} // namespace shatranj