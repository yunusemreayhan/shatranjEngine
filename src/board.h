#pragma once

#include <memory>
#include <optional>

#include "piece_group.h"
#include "player.h"
#include "shatranc_piece.h"

class PieceGroup;
class Player;
class ShatranjPiece;

class Board
{
  public:
    Board(std::shared_ptr<Player> &player1, std::shared_ptr<Player> &player2)
        : players_({player1, player2})
    {
    }

    inline std::optional<std::weak_ptr<ShatranjPiece>> GetCell(const Position &pos) const;

    // Implement move logic functions here (similar to the provided ShatranjPiece functions)
    bool MoveSuccesful(std::shared_ptr<ShatranjPiece> &piece, std::shared_ptr<ShatranjPiece> &targetPiece, int fromX,
                       int fromY, int toX, int toY);
    bool MovePiece(std::shared_ptr<ShatranjPiece> &piece, int posx, int posy);
    bool PromotePiyade(std::shared_ptr<ShatranjPiece> &piyade);
    bool WouldBeInCheck(const std::shared_ptr<ShatranjPiece>& piece, Position pos);
    bool IsCheck(std::shared_ptr<Player>& player) const;
    bool IsCheckmate(Player *player);
    bool IsGameOver();
    Player *Winner();
    bool IsDraw();
    bool IsStalemate(Player *player);
    Player *Opponent(Player *player);
    void SwitchTurn();
    bool IsPathClear(int fromX, int fromY, int toX, int toY);
    bool IsUnderAttack(int posx, int posy, Player *player);
    bool Play(std::string from_pos, std::string to_pos);
    std::string BoardToString();
    std::shared_ptr<PieceGroup> &GetPieces()
    {
        return pieces_;
    }

  private:
    std::shared_ptr<PieceGroup> pieces_;
    std::weak_ptr<Player> currentTurn_;
    std::vector<std::weak_ptr<Player>> players_;
};