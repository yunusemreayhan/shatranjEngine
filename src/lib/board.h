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
    Board(const std::string &name1, const std::string &name2);

    const Player &GetPlayer(Color color) const;

    // Implement move logic functions here (similar to the provided ShatranjPiece functions)
    void MoveSuccesful(const Piece &piece, const Position &frompos, const Position &topos);
    bool MovePiece(Position frompos, Position topos);
    static Piece PromotePiyade(Piece &piyade);
    bool WouldBeInCheck(Position from, Position pos);
    bool OpponnentCanCapturePos(const Position &pos);
    bool IsCheck();
    bool IsCheckmate();
    bool IsGameOver();
    std::optional<Player> Winner();
    bool IsDraw();
    bool IsStalemate();
    Player Opponent(const Color &pColor);
    void SwitchTurn();
    bool IsPathClear(const Position &from, const Position &target);
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
    const Player &GetCurrentPlayer() const
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

    const std::vector<Player> &GetPlayers() const
    {
        return players_;
    }

  private:
    std::shared_ptr<PieceGroup> pieces_;
    std::vector<Player> players_;
    Color currentTurn_;
    int halfMoveClock_ = 0;
    int fullMoveNumber_ = 1;
    constexpr static inline bool kDebug = kDebugGlobal;
};
} // namespace shatranj