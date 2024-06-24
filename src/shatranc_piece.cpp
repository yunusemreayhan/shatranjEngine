#include <algorithm>
#include <cmath>
#include <memory>
#include <utility>

#include "board.h"
#include "player.h"
#include "shatranc_piece.h"
namespace Shatranj
{
Piece::Piece(Position pos, const std::weak_ptr<Player> &player, std::string name, char symbol,
                             bool multipleMove, bool canJumpOverOthers, bool moved,
                             std::vector<std::pair<int, int>> possibleRegularMoves,
                             std::vector<std::pair<int, int>> possibleCaptureMoves)
    : pos_(pos), player_(player), name_(std::move(name)), symbol_(symbol), multipleMove_(multipleMove),
      canJumpOverOthers_(canJumpOverOthers), moved_(moved), possibleRegularMoves_(std::move(possibleRegularMoves)),
      possibleCaptureMoves_(std::move(possibleCaptureMoves))
{
}

bool Piece::CanMove(Position pos, std::shared_ptr<Board> &board, bool ctrlCheck)
{
    if (pos_ == pos)
    {
        return false;
    }
    if (pos.IsValid())
    {
        return false;
    }

    auto diff = pos_.Diff(pos);
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
                if (findres == possibleRegularMoves_.end())
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
        if (auto shp_pl_sp = (*piece)->GetPlayer().lock())
        {
            if (auto selfpl_sp = player_.lock())
            {
                if (*shp_pl_sp == *selfpl_sp)
                {
                    return false;
                }
            }
        }
    }
    if (ctrlCheck && board->WouldBeInCheck(GetSharedFromThis(), pos))
    {
        return false;
    }

    return true;
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
} // namespace Shatranj