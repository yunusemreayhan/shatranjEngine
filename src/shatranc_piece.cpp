#include <algorithm>
#include <cmath>
#include <memory>

#include "board.h"
#include "player.h"
#include "shatranc_piece.h"

ShatranjPiece::ShatranjPiece(Position pos, const std::weak_ptr<Player> &player) : pos_(pos), player_(player)
{
}

bool ShatranjPiece::CanMove(Position pos, std::shared_ptr<Board> &board, bool ctrlCheck)
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