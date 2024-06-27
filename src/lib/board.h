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
        : pieces_(std::make_shared<PieceGroup>()), players_({std::move(player1), std::move(player2)}), currentTurn_(Color::kWhite)
    {
    }

    const std::shared_ptr<Player>& GetPlayer(Color color) const;

    // Implement move logic functions here (similar to the provided ShatranjPiece functions)
    void MoveSuccesful(const Piece &piece, const std::optional<Piece> &targetPiece,
                       const Position &frompos, const Position &topos);
    bool MovePiece(Position frompos, Position topos);
    static Piece PromotePiyade(Piece &piyade);
    bool WouldBeInCheck(Piece* piece, Position pos);
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
    void PrintValidMoves();
    bool AddPiece(Piece piece);

    void RemovePiece(Piece &piece);
    std::shared_ptr<Player> GetCurrentPlayer() const
    {
        return GetPlayer(currentTurn_);
    }
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
    Color currentTurn_;
    int halfMoveClock_ = 0;
    int fullMoveNumber_ = 1;
    constexpr static inline bool kDebug = false;
};
} // namespace shatranj