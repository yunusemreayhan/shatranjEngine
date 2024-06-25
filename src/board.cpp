#include "board.h"
#include "position.h"
#include "shatranc_piece.h"
#include "types.h"
#include <algorithm>
#include <cctype>
#include <memory>
#include <optional>
#include <stdexcept>
#include <utility>

namespace shatranj
{
// would be in check if I move piece to pos
bool Board::WouldBeInCheck(const std::shared_ptr<Piece> &piece, Position pos)
{
    /*
        def wouldBeInCheck(self, piece, x, y):
            original_x, original_y = piece.Getx(), piece.Gety()
            capturedPiece = self.getCell( x, y )
            if capturedPiece:
                capturedPiece.player.pieces.remove( capturedPiece )
                self.pieces.remove( capturedPiece )

            piece.Getx(), piece.Gety() = x, y
            inCheck = self.isCheck(piece.player)

            piece.Getx(), piece.Gety() = original_x, original_y
            if capturedPiece:
                capturedPiece.player.pieces.append( capturedPiece )
                self.pieces.append( capturedPiece )

            return inCheck
     */
    auto player_sp = piece->GetPlayer().lock();
    if (!player_sp)
    {
        return false;
    }
    const auto captured_piece = GetPieces()->GetPiece(pos);
    if (captured_piece)
    {
        const auto &captured_piece_sp = (*captured_piece);
        if (auto captured_player_sp = captured_piece_sp->GetPlayer().lock())
        {
            captured_player_sp->GetPieces()->RemovePiece(pos);
        }
        // if target location have a piece we will replace it with our piece
        // remove from user's pieces
        // remove from board's pieces
        GetPieces()->RemovePiece(pos);
    }

    bool ret_is_check = IsCheck(player_sp);

    if (captured_piece)
    {
        const auto &captured_piece_sp = (*captured_piece);
        // TODO(yunus): put back the captured piece after check

        if (auto captured_player_sp = captured_piece_sp->GetPlayer().lock())
        {
            // put back to user's pieces
            const auto &captured_piece_sp = (*captured_piece);
            if (auto captured_player_sp = captured_piece_sp->GetPlayer().lock())
            {
                captured_player_sp->GetPieces()->AddPiece(captured_piece_sp);
            }

            // put back to board's pieces
            GetPieces()->AddPiece(captured_piece_sp);
        }
    }

    return ret_is_check;
}

bool Board::IsCheck(const std::shared_ptr<Player> &player)
{
    std::shared_ptr<Piece> shah = nullptr;
    for (const auto &piece : *player->GetPieces())
    {
        if (dynamic_cast<Shah *>(piece.get()) != nullptr)
        {
            shah = piece;
        }
    }

    if (!shah)
    {
        throw std::runtime_error("Shah piece not found");
    }

    // get opponent
    const std::shared_ptr<Player> &opponent = players_[0] == player ? players_[1] : players_[0];

    return std::any_of(opponent->GetPieces()->begin(), opponent->GetPieces()->end(), [&](const auto &op_piece) {
        return op_piece->CanCapture(shah->GetPos(), GetSharedFromThis(), false);
    });
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

bool Board::MovePiece(std::shared_ptr<Piece> &piece, Position pos)
{
    auto current_turn_player_sp = currentTurn_.lock();
    auto piece_player_sp = piece->GetPlayer().lock();
    if (current_turn_player_sp != piece_player_sp)
    {
        return false;
    }
    const auto from = piece->GetPos();
    const bool can_move = piece->CanMove(pos, GetSharedFromThis());
    const bool can_capture = piece->CanCapture(pos, GetSharedFromThis());
    if (!can_move && !can_capture)
    {
        return false;
    }
    if (WouldBeInCheck(piece, pos))
    {
        return false;
    }
    const auto captured_piece = GetPieces()->GetPiece(pos);
    if (captured_piece && can_capture)
    {
        GetPieces()->RemovePiece(pos);
        if (auto captured_player_sp = (*captured_piece)->GetPlayer().lock())
        {
            captured_player_sp->GetPieces()->RemovePiece(pos);
        }
    }
    piece->Move(pos);
    if (nullptr != dynamic_cast<Piyade *>(piece.get()))
    {
        if (pos.Gety() == 0 || pos.Gety() == 7)
        {
            auto promoted_piece = PromotePiyade(piece);
            GetPieces()->RemovePiece(pos);
            GetPieces()->AddPiece(promoted_piece);
            piece_player_sp->GetPieces()->RemovePiece(pos);
            piece_player_sp->GetPieces()->AddPiece(promoted_piece);
        }
    }

    MoveSuccesful(piece, captured_piece, from, pos);
    return true;
}

void Board::MoveSuccesful(const std::shared_ptr<Piece> &piece,
                          const std::optional<std::shared_ptr<Piece>> & /*targetPiece*/, const Position & /*fromPos*/,
                          const Position & /*toPos*/)
{
    // TODO(yunus) :  not sure if needed but last move textual iformation could be saved later
    if (nullptr != dynamic_cast<Piyade *>(piece.get()))
    {
        halfMoveClock_ = 0;
    }
    else
    {
        ++halfMoveClock_;
    }

    auto current_turn_sp = currentTurn_.lock();
    if (current_turn_sp && current_turn_sp->GetColor() == Color::kBlack)
    {
        fullMoveNumber_++;
    }
    SwitchTurn();
}

void Board::SwitchTurn()
{
    auto current_turn_sp = currentTurn_.lock();
    currentTurn_ = players_[0] == current_turn_sp ? players_[1] : players_[0];
}

std::shared_ptr<Piece> Board::PromotePiyade(std::shared_ptr<Piece> &piyade)
{
    const auto pos = piyade->GetPos();
    auto ret = std::make_shared<Vizier>(pos, piyade->GetPlayer());
    return ret;
}

bool Board::IsCheckmate(const std::shared_ptr<Player> &player)
{
    if (!IsCheck(player))
    {
        return false;
    }
    for (const auto &piece : *player->GetPieces())
    {
        for (int xitr = 0; xitr < 8; xitr++)
        {
            for (int yitr = 0; yitr < 8; yitr++)
            {
                const Position pos(std::make_pair(xitr, yitr));
                if ((piece->CanMove(pos, GetSharedFromThis(), false) ||
                     piece->CanCapture(pos, GetSharedFromThis(), false)) &&
                    !WouldBeInCheck(piece, pos))
                {
                    return false;
                }
            }
        }
    }
    return false;
}

bool Board::IsStalemate(const std::shared_ptr<Player> &player)
{
    if (IsCheck(player))
    {
        return false;
    }

    for (const auto &piece : *player->GetPieces())
    {
        for (int xitr = 0; xitr < 8; xitr++)
        {
            for (int yitr = 0; yitr < 8; yitr++)
            {
                const Position pos(std::make_pair(xitr, yitr));
                // TODO(yunus) : this could be optimized, can move already calls wouldBeInCheck
                if (piece->CanMove(pos, GetSharedFromThis(), false) && !WouldBeInCheck(piece, pos))
                {
                    return false;
                }
            }
        }
    }
    return true;
}

bool Board::IsGameOver()
{
    if (IsStalemate(currentTurn_.lock()))
    {
        return true;
    }
    if (IsCheckmate(currentTurn_.lock()))
    {
        return true;
    }
    if (IsDraw())
    {
        return true;
    }
    return false;
}

std::shared_ptr<Player> Board::Winner()
{
    if (IsDraw())
    {
        return nullptr;
    }
    if (IsStalemate(currentTurn_.lock()))
    {
        return Opponent(currentTurn_.lock());
    }
    if (IsCheckmate(currentTurn_.lock()))
    {
        return Opponent(currentTurn_.lock());
    }
    return currentTurn_.lock();
}

bool Board::IsDraw()
{
    if (pieces_->size() == 2 && GetPieces()->is_all_instance_of<Shah>())
    {
        return true;
    }
    const auto &current_turn_sp = currentTurn_.lock();
    const auto &current_turn_pieces = current_turn_sp->GetPieces();
    const auto &opponent_pieces = Opponent(current_turn_sp)->GetPieces();
    if (current_turn_pieces->size() == 1 && opponent_pieces->size() == 2)
    {
        if (current_turn_sp->GetPieces()->get(0)->CanCapture(opponent_pieces->get(0)->GetPos(), GetSharedFromThis()) ||
            current_turn_sp->GetPieces()->get(0)->CanCapture(opponent_pieces->get(1)->GetPos(), GetSharedFromThis()))
        {
            return true;
        }
    }
    return false;
}

std::shared_ptr<Player> Board::Opponent(const std::shared_ptr<Player> &player)
{
    return players_[0] == player ? players_[1] : players_[0];
}

bool Board::Play(std::string from_pos, std::string to_pos)
{
    const Position from_pos_local(std::move(from_pos));
    const Position to_pos_local(std::move(to_pos));
    if (!from_pos_local.IsValid() || !to_pos_local.IsValid())
    {
        return false;
    }

    std::optional<std::shared_ptr<Piece>> piece = pieces_->GetPiece(from_pos_local);
    if (!piece || (*piece)->GetPlayer().lock() != currentTurn_.lock())
    {
        return false;
    }

    return MovePiece(*piece, to_pos_local);
}

std::string Board::BoardToString() const
{
    std::vector<std::vector<char>> board(8, std::vector<char>(8, '.'));
    for (const auto &piece : *pieces_)
    {
        const auto pos = piece->GetPos();
        board[pos.Getx()][pos.Gety()] = piece->GetPlayer().lock()->GetColor() == Color::kWhite
                                            ? std::toupper(piece->GetSymbol())
                                            : std::tolower(piece->GetSymbol());
    }
    std::string ret;
    for (int yitr = 0; yitr < 8; yitr++)
    {
        if (yitr == 0)
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

    ret += "  current turn : " + currentTurn_.lock()->GetName() + " color " +
           std::string(currentTurn_.lock()->GetColor() == Color::kWhite ? "White which is uppercase"
                                                                        : "Black which is lowercase") +
           '\n';
    ret += "  current move count : " + std::to_string(fullMoveNumber_) + '\n';
    ret += "  half move count : " + std::to_string(halfMoveClock_) + '\n';

    return ret;
}

std::shared_ptr<Player> Board::GetWhitePlayer()
{
    for (const auto &player : players_)
    {
        if (player->GetColor() == Color::kWhite)
        {
            return player;
        }
    }
    return nullptr;
}

bool Board::AddPiece(const std::shared_ptr<Piece> &piece)
{
    auto res = pieces_->AddPiece(piece);
    if (!res)
    {
        return false;
    }

    res = piece->GetPlayer().lock()->GetPieces()->AddPiece(piece);
    return res;
}

void Board::RemovePiece(const std::shared_ptr<Piece> &piece)
{
    pieces_->RemovePiece(piece);
    piece->GetPlayer().lock()->GetPieces()->RemovePiece(piece);
}

} // namespace shatranj