#pragma once

#include <memory>
#include <string_view>
#include <variant>
#include <vector>

#include "board.h"
#include "player.h"
#include "position.h"
#include "types.h"
namespace shatranj
{
class Board;
class Player;

enum class ChessPieceEnum : std::uint8_t
{
    kPiyade,
    kVizier,
    kShah,
    kHorse,
    kFil,
    kRook
};

class Piece : public std::enable_shared_from_this<Piece>
{
  public:
    explicit Piece(const std::vector<Step> &possibleRegularMoves, const std::vector<Step> &possibleCaptureMoves,
                   const std::weak_ptr<Player> &player, Position pos, ChessPieceEnum pieceType, int8_t direction,
                   bool multipleMove, bool canJumpOverOthers, bool moved);

    Position GetPos()
    {
        return pos_;
    }

    std::weak_ptr<Player> GetPlayer()
    {
        return player_;
    }

    bool CanMove(Position pos, const std::shared_ptr<Board> &board, bool ctrlCheck = true);
    bool CanThreat(Position pos, const std::shared_ptr<Board> &board, bool ctrlCheck = true);
    bool CanCapture(Position pos, const std::shared_ptr<Board> &board, bool ctrlCheck = true);
    bool Move(Position pos);
    std::vector<std::pair<Position, Position>> GetPossibleMoves(const std::shared_ptr<Board> &board);

    std::shared_ptr<Piece> GetSharedFromThis()
    {
        return shared_from_this();
    }

    virtual ~Piece() = default;

    constexpr char GetSymbol() const
    {
        switch (pieceType_)
        {
        case ChessPieceEnum::kPiyade:
            return 'P';
        case ChessPieceEnum::kVizier:
            return 'V';
        case ChessPieceEnum::kShah:
            return 'S';
        case ChessPieceEnum::kHorse:
            return 'H';
        case ChessPieceEnum::kFil:
            return 'F';
        case ChessPieceEnum::kRook:
            return 'R';
        }
    }

    constexpr std::string_view GetName() const
    {
        switch (pieceType_)
        {
        case ChessPieceEnum::kPiyade:
            return "Piyade";
        case ChessPieceEnum::kVizier:
            return "Vizier";
        case ChessPieceEnum::kShah:
            return "Shah";
        case ChessPieceEnum::kHorse:
            return "Horse";
        case ChessPieceEnum::kFil:
            return "Fil";
        case ChessPieceEnum::kRook:
            return "Rook";
        }
    }

    constexpr bool IsPiyade() const
    {
        return pieceType_ == ChessPieceEnum::kPiyade;
    }

    constexpr bool IsVizier() const
    {
        return pieceType_ == ChessPieceEnum::kVizier;
    }

    constexpr bool IsShah() const
    {
        return pieceType_ == ChessPieceEnum::kShah;
    }

    constexpr bool IsHorse() const
    {
        return pieceType_ == ChessPieceEnum::kHorse;
    }

    constexpr bool IsFil() const
    {
        return pieceType_ == ChessPieceEnum::kFil;
    }

    constexpr bool IsRook() const
    {
        return pieceType_ == ChessPieceEnum::kRook;
    }

    constexpr bool IsMultipleMove() const
    {
        return multipleMove_;
    }

    constexpr bool CanJumpOverOthers() const
    {
        return canJumpOverOthers_;
    }

    constexpr bool IsMoved() const
    {
        return moved_;
    }

  protected:
    std::vector<Step> possibleRegularMoves_;
    std::vector<Step> possibleCaptureMoves_;
    std::weak_ptr<Player> player_;
    Position pos_;
    ChessPieceEnum pieceType_;
    int8_t direction_ : 2;
    bool multipleMove_ = false;
    bool canJumpOverOthers_ = false;
    bool moved_ = false;
};

class Rook : public Piece
{
  public:
    Rook(Position pos, const std::weak_ptr<Player> &player)
        : Piece({{0, 1}, {1, 0}, {0, -1}, {-1, 0}}, {}, player, pos, ChessPieceEnum::kRook, 0, true, false, false)
    {
    }
};

class Piyade : public Piece
{

  public:
    Piyade(Position pos, const std::weak_ptr<Player> &player);

  private:
};
class Horse : public Piece
{
  public:
    Horse(Position pos, const std::weak_ptr<Player> &player)
        : Piece({{+1, +2}, {+2, +1}, {-1, +2}, {-2, +1}, {-1, -2}, {-2, -1}, {+1, -2}, {+2, -1}}, {}, player, pos,
                ChessPieceEnum::kHorse, 0, false, true, false)
    {
    }
};
class Fil : public Piece
{
  public:
    Fil(Position pos, const std::weak_ptr<Player> &player)
        : Piece({{2, 2}, {2, -2}, {-2, 2}, {-2, -2}}, {}, player, pos, ChessPieceEnum::kFil, 0, false, true, false)
    {
    }
};
class Vizier : public Piece
{
  public:
    Vizier(Position pos, const std::weak_ptr<Player> &player)
        : Piece({{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}, {}, player, pos, ChessPieceEnum::kVizier, 0, false, false, false)
    {
    }
};
class Shah : public Piece
{
  public:
    Shah(Position pos, const std::weak_ptr<Player> &player)
        : Piece({{0, 1}, {1, 0}, {0, -1}, {-1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}}, {}, player, pos,
                ChessPieceEnum::kShah, 0, false, false, false)
    {
    }
};

using ChessPiece = std::variant<Rook, Piyade, Vizier, Shah, Horse, Fil>;

} // namespace shatranj