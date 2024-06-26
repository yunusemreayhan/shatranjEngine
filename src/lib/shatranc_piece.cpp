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
Piece::Piece(Position pos, const std::weak_ptr<Player> &player, std::string name, char symbol, bool multipleMove,
             bool canJumpOverOthers, bool moved, std::vector<std::pair<int, int>> possibleRegularMoves,
             std::vector<std::pair<int, int>> possibleCaptureMoves)
    : pos_(pos), player_(player), name_(std::move(name)), symbol_(symbol), multipleMove_(multipleMove),
      canJumpOverOthers_(canJumpOverOthers), moved_(moved), possibleRegularMoves_(std::move(possibleRegularMoves)),
      possibleCaptureMoves_(std::move(possibleCaptureMoves))
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
        if (std::find(possibleRegularMoves_.begin(), possibleRegularMoves_.end(), diff) == possibleRegularMoves_.end())
        {
            return false;
        }
    }
    else
    {
        bool check = false;
        for (int i = 1; i < 8; i++)
        {
            if (diff.first % i == 0 && diff.second % i == 0)
            {
                const auto multiplemoveinstance = std::make_pair(static_cast<int>(std::floor(diff.first / i)),
                                                                 static_cast<int>(std::floor(diff.second / i)));
                const auto findres =
                    std::find(possibleRegularMoves_.begin(), possibleRegularMoves_.end(), multiplemoveinstance);
                if (findres != possibleRegularMoves_.end())
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
    if (piece)
    {
        auto shp_pl_sp = (*piece)->GetPlayer().lock();
        auto selfpl_sp = player_.lock();
        if (selfpl_sp && shp_pl_sp)
        {
            if (*shp_pl_sp == *selfpl_sp)
            {
                return false;
            }
        }
    }
    if (ctrlCheck && board->WouldBeInCheck(GetSharedFromThis(), pos))
    {
        return false;
    }

    return canJumpOverOthers_ || board->IsPathClear(pos_, pos);
}

bool Piece::CanThreat(Position pos, const std::shared_ptr<Board> &board, bool ctrlCheck)
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
        if (std::find(possibleCaptureMoves_.begin(), possibleCaptureMoves_.end(), diff) == possibleCaptureMoves_.end())
        {
            return false;
        }
    }
    else
    {
        bool check = false;
        for (int i = 1; i < 8; i++)
        {
            if (diff.first % i == 0 && diff.second % i == 0)
            {
                const auto multiplemoveinstance = std::make_pair(static_cast<int>(std::floor(diff.first / i)),
                                                                 static_cast<int>(std::floor(diff.second / i)));
                const auto findres =
                    std::find(possibleCaptureMoves_.begin(), possibleCaptureMoves_.end(), multiplemoveinstance);
                if (findres != possibleCaptureMoves_.end())
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

    if (ctrlCheck && board->WouldBeInCheck(GetSharedFromThis(), pos))
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
    if (!piece || *(*piece)->GetPlayer().lock() == *player_.lock())
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
    for (const auto &diff : possibleRegularMoves_)
    {
        if (!multipleMove_)
        {
            Position pos = pos_;
            pos.Move(diff);
            if (pos.IsValid())
            {
                if (CanMove(pos, board))
                {
                    possible_moves.push_back(std::make_pair(pos_, pos));
                }
            }
        }
        else
        {
            for (int i = 1; i < 8; i++)
            {
                Position pos = pos_;
                pos.Move(std::make_pair(static_cast<int>(std::floor(diff.first * i)),
                                        static_cast<int>(std::floor(diff.second * i))));
                if (pos.IsValid())
                {
                    if (CanMove(pos, board))
                    {
                        possible_moves.push_back(std::make_pair(pos_, pos));
                    }
                }
            }
        }
    }

    if (IsPiyade())
    {
        for (const auto &diff : possibleCaptureMoves_)
        {
            Position pos = pos_;
            pos.Move(diff);
            if (pos_.IsValid())
            {
                if (CanCapture(pos, board))
                {
                    possible_moves.push_back(std::make_pair(pos_, pos));
                }
            }
        }
    }
    return possible_moves;
}

Piyade::Piyade(Position pos, const std::weak_ptr<Player> &player)
    : Piece(pos, player, "Piyade", 'P', false, false, false, {}, {}), direction_{+1}
{
    if (auto p_sp = player.lock())
    {
        direction_ = p_sp->GetColor() == Color::kWhite ? +1 : -1;
    }
    possibleRegularMoves_ = {{0, direction_}};
    possibleCaptureMoves_ = {{1, direction_}, {-1, direction_}};
}
} // namespace shatranj