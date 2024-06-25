#pragma once

#include <memory>
#include <optional>

#include "piece_group.h"
#include "player.h"
#include "shatranc_piece.h"
#include "types.h"

namespace shatranj
{
class PieceGroup;
class Player;
class Piece;
class Position;

class Board : public std::enable_shared_from_this<Board>
{
  public:
    Board(std::shared_ptr<Player> player1, std::shared_ptr<Player> player2)
        : pieces_(std::make_shared<PieceGroup>()), players_({std::move(player1), std::move(player2)}), currentTurn_(GetWhitePlayer())
    {
    }

    std::shared_ptr<Player> GetWhitePlayer();

    // Implement move logic functions here (similar to the provided ShatranjPiece functions)
    void MoveSuccesful(const std::shared_ptr<Piece> &piece, const std::optional<std::shared_ptr<Piece>> &targetPiece,
                       const Position &frompos, const Position &topos);
    bool MovePiece(std::shared_ptr<Piece> &piece, Position pos);
    static std::shared_ptr<Piece> PromotePiyade(std::shared_ptr<Piece> &piyade);
    bool WouldBeInCheck(const std::shared_ptr<Piece> &piece, Position pos);
    bool IsCheck(const std::shared_ptr<Player> &player);
    bool IsCheckmate(const std::shared_ptr<Player>& player);
    bool IsGameOver();
    std::shared_ptr<Player> Winner();
    bool IsDraw();
    bool IsStalemate(const std::shared_ptr<Player>& player);
    std::shared_ptr<Player> Opponent(const std::shared_ptr<Player>& player);
    void SwitchTurn();
    bool IsPathClear(const Position& from, const Position& target);
    bool IsUnderAttack(int posx, int posy, Player *player); // not used yet
    bool Play(std::string from_pos, std::string to_pos);
    std::string BoardToString() const;
    std::shared_ptr<PieceGroup> &GetPieces()
    {
        return pieces_;
    }
    bool AddPiece(const std::shared_ptr<Piece> &piece);

    void RemovePiece(const std::shared_ptr<Piece> &piece);
    std::shared_ptr<Board> GetSharedFromThis()
    {
        return shared_from_this();
    }

    friend std::ostream &operator<<(std::ostream &ostr, const Board &board)
    {
        ostr << board.BoardToString();
        return ostr;
    }

    const std::vector<std::shared_ptr<Player>> &GetPlayers() const
    {
        return players_;
    }

  private:
    std::shared_ptr<PieceGroup> pieces_;
    std::vector<std::shared_ptr<Player>> players_;
    std::weak_ptr<Player> currentTurn_;
    int halfMoveClock_ = 0;
    int fullMoveNumber_ = 1;
};
} // namespace shatranj