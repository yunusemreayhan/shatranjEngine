#include "board.h"
#include "position.h"
#include "shatranc_piece.h"
#include "types.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <utility>

namespace shatranj
{

Board::Board(const std::string &name1, const std::string &name2)
    : pieces_(std::make_shared<PieceGroup>()), players_{{name1, Color::kWhite}, {name2, Color::kBlack}},
      currentTurn_(Color::kWhite)
{
}
// would be in check if I move piece to pos
bool Board::WouldBeInCheck(Position from, Position pos)
{
    auto captured_piece = GetPieces()->GetPiece(pos);
    std::unique_ptr<Piece> temp = nullptr;
    if (captured_piece)
    {
        temp = std::make_unique<Piece>(**captured_piece);
        GetPieces()->RemovePiece(pos);
    }
    auto piece_updated = GetPieces()->GetPiece(from);
    if (!piece_updated)
        return false;
    auto temppos = (*piece_updated)->GetPos();
    (*piece_updated)->Move(pos);

    bool ret_is_check = IsCheck();

    (*piece_updated)->Move(temppos);

    if (captured_piece)
    {
        GetPieces()->AddPiece(*temp);
    }

    return ret_is_check;
}

bool Board::OpponnentCanCapturePos(const Position &pos)
{
    return std::any_of(GetPieces()->begin(), GetPieces()->end(), [&](auto &pitr) {
        if (pitr.GetColor() != OpponentColor(currentTurn_))
            return false;
        auto calc = pitr.CanMove(pos, GetSharedFromThis(), false) || pitr.CanCapture(pos, GetSharedFromThis(), false);
        if constexpr (kDebugGlobal)
        {
            if (calc)
            {
                std::cout << pitr.GetName() << " " << pitr.GetPos().ToString() << " Can capture " << pos.ToString()
                          << std::endl;
            }
            else
            {
                std::cout << pitr.GetName() << " " << pitr.GetPos().ToString() << " Cannot capture " << pos.ToString()
                          << std::endl;
            }
        }
        return calc;
    });
}
bool Board::IsCheck()
{
    Piece *shah = nullptr;
    for (auto &piece : *GetPieces())
    {
        if (currentTurn_ == piece.GetColor() && piece.IsShah())
        {
            shah = &piece;
            break;
        }
    }

    if (shah == nullptr)
    {
        throw std::runtime_error("Shah piece not found");
    }

    // get opponent
    const Player &opponent = Opponent(currentTurn_);

    return OpponnentCanCapturePos(shah->GetPos());
}

bool Board::IsPathClear(const Position &from, const Position &target)
{
    int step_x = 0;
    int step_y = 0;
    if (from.Getx() < target.Getx())
    {
        step_x = 1;
    }
    else if (from.Getx() > target.Getx())
    {
        step_x = -1;
    }
    if (from.Gety() < target.Gety())
    {
        step_y = 1;
    }
    else if (from.Gety() > target.Gety())
    {
        step_y = -1;
    }
    const int cur_x = from.Getx() + step_x;
    const int cur_y = from.Gety() + step_y;
    Position cur(std::make_pair(cur_x, cur_y));
    while (cur != target)
    {
        if (GetPieces()->GetPiece(cur))
        {
            return false;
        }
        cur.Move(std::make_pair(step_x, step_y));
    }
    return true;
}

bool Board::MovePiece(Position frompos, Position topos)
{
    auto piece = GetPieces()->GetPieceByVal(frompos);
    if (currentTurn_ != (*piece).GetColor())
    {
        return false;
    }
    const auto from = (*piece).GetPos();
    const bool can_move = (*piece).CanMove(topos, GetSharedFromThis());
    const bool can_capture = (*piece).CanCapture(topos, GetSharedFromThis());
    if (!can_move && !can_capture)
    {
        return false;
    }
    if (WouldBeInCheck(frompos, topos))
    {
        return false;
    }
    std::unique_ptr<Piece> captured_piece_uptr = nullptr;
    auto captured_piece = GetPieces()->GetPiece(topos);
    if (captured_piece)
    {
        captured_piece_uptr = std::make_unique<Piece>(**captured_piece);
        if (((*piece).IsPiyade() && can_capture) || (!(*piece).IsPiyade() && can_move))
            RemovePiece(**captured_piece);
    }
    {
        bool promoted = false;
        Piece &piece = **GetPieces()->GetPiece(frompos);
        piece.Move(topos);
        if (piece.IsPiyade())
        {
            if (topos.Gety() == 0 || topos.Gety() == 7)
            {
                auto promoted_piece = PromotePiyade(piece);
                RemovePiece(piece);
                AddPiece(promoted_piece);
                promoted = true;
            }
        }
        history_.AddMove(frompos, topos, std::move(captured_piece_uptr), promoted, piece.GetColor());
        MoveSuccesful(piece, from, topos);
    }

    return true;
}

bool Board::Revert(int move_count)
{
    if constexpr (kDebug)
    {
        std::cout << "Reverting " << move_count << " moves" << std::endl;
    }
    if (move_count <= 0)
    {
        return true;
    }
    while (move_count > 0)
    {
        if (history_.GetHistory().size() == 0)
        {
            if constexpr (kDebug)
            {
                std::cout << "No more moves to revert" << std::endl;
            }
            return false;
        }
        const std::unique_ptr<HistoryPoint> &last_move = history_.GetLastMove();

        if constexpr (kDebug)
        {
            std::cout << "Reverting from " << last_move->to.ToString() << " to " << last_move->from.ToString()
                      << std::endl;
        }

        auto piece_opt = GetPieces()->GetPiece(last_move->to);
        if (!piece_opt)
        {
            if constexpr (kDebug)
            {
                std::cout << *this << std::endl;
            }
            throw std::runtime_error("Piece not found, illogical board state at " + last_move->to.ToString());
        }
        auto *piece = *piece_opt;
        piece->Move(last_move->from);
        if (last_move->promoted)

        {
            auto demoted_piece = DemotePromoted(*piece);
            RemovePiece(*piece);
            AddPiece(demoted_piece);
        }

        if (last_move->captured != nullptr)
        {
            AddPiece(*last_move->captured);
        }

        if constexpr (kDebug)
        {
            std::cout << "Revert done from " << last_move->to.ToString() << " to " << last_move->from.ToString()
                      << std::endl;
        }
        move_count--;
        if (last_move->color == Color::kBlack)
        {
            fullMoveNumber_--;
        }
        if constexpr (kDebug)
        {
            std::cout << "Reverting " << move_count << " moves" << std::endl;
        }
        history_.PopLastMove();
    }
    return true;
}

void Board::MoveSuccesful(const Piece &piece, const Position & /*fromPos*/, const Position & /*toPos*/)
{
    // TODO(yunus) :  not sure if needed but last move textual iformation could be saved later
    if (piece.IsPiyade())
    {
        halfMoveClock_ = 0;
    }
    else
    {
        ++halfMoveClock_;
    }

    if (currentTurn_ == Color::kBlack)
    {
        fullMoveNumber_++;
    }
    SwitchTurn();
}

void Board::SwitchTurn()
{
    currentTurn_ = Color::kWhite == currentTurn_ ? Color::kBlack : Color::kWhite;
}

Piece Board::PromotePiyade(Piece &piyade)
{
    return Vizier(piyade.GetPos(), piyade.GetColor());
}
Piece Board::DemotePromoted(Piece &promoted)
{
    return Piyade(promoted.GetPos(), promoted.GetColor());
}

bool Board::IsCheckmate()
{
    if (!IsCheck())
    {
        return false;
    }
    for (auto &piece : *GetPieces())
    {
        if (piece.GetColor() == currentTurn_)
        {
            for (int xitr = 0; xitr < 8; xitr++)
            {
                for (int yitr = 0; yitr < 8; yitr++)
                {
                    const Position pos(std::make_pair(xitr, yitr));
                    if ((piece.CanMove(pos, GetSharedFromThis(), false) ||
                         piece.CanCapture(pos, GetSharedFromThis(), false)) &&
                        !WouldBeInCheck(piece.GetPos(), pos))
                    {
                        return false;
                    }
                }
            }
        }
    }
    return false;
}

bool Board::IsStalemate()
{
    if (IsCheck())
    {
        return false;
    }

    for (auto &piece : *GetPieces())
    {
        if (piece.GetColor() == currentTurn_)
        {
            for (int xitr = 0; xitr < 8; xitr++)
            {
                for (int yitr = 0; yitr < 8; yitr++)
                {
                    const Position pos(std::make_pair(xitr, yitr));
                    // TODO(yunus) : this could be optimized, can move already calls wouldBeInCheck
                    if (piece.CanMove(pos, GetSharedFromThis(), false) && !WouldBeInCheck(piece.GetPos(), pos))
                    {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

bool Board::IsGameOver()
{
    if (IsStalemate())
    {
        return true;
    }
    if (IsCheckmate())
    {
        return true;
    }
    if (IsDraw())
    {
        return true;
    }
    return false;
}

std::optional<Player> Board::Winner()
{
    if (IsDraw())
    {
        return std::nullopt;
    }
    if (IsStalemate())
    {
        return Opponent(currentTurn_);
    }
    if (IsCheckmate())
    {
        return Opponent(currentTurn_);
    }
    return std::nullopt;
}

bool Board::IsDraw()
{
    if (pieces_->size() == 2 && GetPieces()->is_all_instance_of(ChessPieceEnum::kShah))
    {
        return true;
    }
    auto current_turn_pieces = GetPieces()->GetSubPieces(currentTurn_);
    auto opponent_pieces = GetPieces()->GetSubPieces(OpponentColor(currentTurn_));
    if (current_turn_pieces.size() == 1 && opponent_pieces.size() == 2)
    {
        if (current_turn_pieces[0].CanCapture(opponent_pieces[0].GetPos(), GetSharedFromThis()) ||
            current_turn_pieces[0].CanCapture(opponent_pieces[1].GetPos(), GetSharedFromThis()))
        {
            return true;
        }
    }
    return false;
}

Player Board::Opponent(const Color &color)
{
    return color == players_[1].GetColor() ? players_[0] : players_[1];
}

bool Board::Play(std::string from_pos, std::string to_pos)
{
    const Position from_pos_local(std::move(from_pos));
    const Position to_pos_local(std::move(to_pos));
    if (!from_pos_local.IsValid() || !to_pos_local.IsValid())
    {
        return false;
    }

    auto piece = pieces_->GetPiece(from_pos_local);
    if (!piece || (*piece)->GetColor() != currentTurn_)
    {
        return false;
    }

    return MovePiece(from_pos_local, to_pos_local);
}

std::string Board::BoardToString() const
{
    std::vector<std::vector<char>> board(8, std::vector<char>(8, '.'));
    for (auto &piece : *pieces_)
    {
        const auto pos = piece.GetPos();
        board[pos.Getx()][pos.Gety()] =
            piece.GetColor() == Color::kWhite ? std::toupper(piece.GetSymbol()) : std::tolower(piece.GetSymbol());
    }
    std::string ret;
    for (int yitr = 7; yitr >= 0; yitr--)
    {
        if (yitr == 7)
        {
            ret += "  ";
            for (int xitr = 0; xitr < 8; xitr++)
            {
                ret += static_cast<char>(xitr + 'a');
                ret += ' ';
            }
            ret += '\n';
        }

        ret += std::to_string(1 + yitr);
        ret += ' ';

        for (int xitr = 0; xitr < 8; xitr++)
        {
            ret += board[xitr][yitr];
            ret += ' ';
        }
        ret += '\n';
    }

    ret += "  ";
    for (int xitr = 0; xitr < 8; xitr++)
    {
        ret += static_cast<char>(xitr + 'a');
        ret += ' ';
    }
    ret += '\n';

    ret += "  current turn : " + GetCurrentPlayer().GetName() + " color " +
           std::string(currentTurn_ == Color::kWhite ? "White which is uppercase" : "Black which is lowercase") + '\n';
    ret += "  current move count : " + std::to_string(fullMoveNumber_) + '\n';
    ret += "  half move count : " + std::to_string(halfMoveClock_) + '\n';

    return ret;
}

const Player &Board::GetPlayer(Color color) const
{
    for (const auto &player : players_)
    {
        if (player.GetColor() == color)
        {
            return player;
        }
    }
    return players_[0];
}

bool Board::AddPiece(Piece piece)
{
    auto res = pieces_->AddPiece(piece);
    if (!res)
    {
        return false;
    }

    return res;
}

void Board::RemovePiece(Piece &piece)
{
    if constexpr (kDebug)
    {
        std::cout << "Removing " << piece.GetPos().ToString() << std::endl;
        std::cout << *this << std::endl;
    }
    pieces_->RemovePiece(piece);
    // piece.GetPlayer().lock()->GetPieces()->RemovePiece(piece);
    if constexpr (kDebug)
    {
        std::cout << "Removed " << piece.GetPos().ToString() << std::endl;
        std::cout << *this << std::endl;
    }
}

void Board::PrintValidMoves()
{
    const auto &moves = GetPieces()->GetPossibleMoves(currentTurn_, GetSharedFromThis());

    for (const auto &move : moves)
    {
        std::cout << move.first.ToString() << "" << move.second.ToString() << " ";
    }

    std::cout << std::endl;
}

} // namespace shatranj