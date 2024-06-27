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
class PiecePrimitive // positionless piece
{
  public:
    PiecePrimitive(ChessPieceEnum pieceType, const Color color, bool multipleMove, bool canJumpOverOthers, bool moved);

  protected:
    ChessPieceEnum pieceType_;

    int8_t isWhite_ : 1;
    int8_t multipleMove_ : 1;
    int8_t canJumpOverOthers_ : 1;
    int8_t moved_ : 1;
};

class Piece : public PiecePrimitive
{
  public:
    explicit Piece(ChessPieceEnum pieceType, Position pos, const Color color, bool multipleMove, bool canJumpOverOthers,
                   bool moved);

    const Position &GetPos() const
    {
        return pos_;
    }

    Color GetColor() const
    {
        return isWhite_ ? Color::kWhite : Color::kBlack;
    }

    bool CanMove(Position pos, const std::shared_ptr<Board> &board, bool ctrlCheck = true);
    bool CanThreat(Position pos, const std::shared_ptr<Board> &board, bool ctrlCheck = true);
    bool CanCapture(Position pos, const std::shared_ptr<Board> &board, bool ctrlCheck = true);
    bool Move(Position pos);
    std::vector<std::pair<Position, Position>> GetPossibleMoves(const std::shared_ptr<Board> &board);

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

        return ' ';
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

        return " ";
    }

    constexpr ChessPieceEnum GetPieceType() const
    {
        return pieceType_;
    }

    const static std::vector<Step> kRookSteps;
    const static std::vector<Step> kPiyadeWhiteSteps;
    const static std::vector<Step> kPiyadeBlackSteps;

    const static std::vector<Step> kVizierSteps;

    const static std::vector<Step> kShahSteps;

    const static std::vector<Step> kHorseSteps;

    const static std::vector<Step> kFilSteps;

    constexpr inline const std::vector<Step> &GetPossibleRegularMoves() const
    {
        switch (pieceType_)
        {
        case ChessPieceEnum::kPiyade:
            switch (GetColor())
            {
            case Color::kWhite:
                return kPiyadeWhiteSteps;
            case Color::kBlack:
                return kPiyadeBlackSteps;
            default:
                return kEmptySteps;
            }
        case ChessPieceEnum::kVizier:
            return kVizierSteps;
        case ChessPieceEnum::kShah:
            return kShahSteps;
        case ChessPieceEnum::kHorse:
            return kHorseSteps;
        case ChessPieceEnum::kFil:
            return kFilSteps;
        case ChessPieceEnum::kRook:
            return kRookSteps;
        }

        return kEmptySteps;
    }
    const static std::vector<Step> kPiyadeWhiteCaptureSteps;

    const static std::vector<Step> kPiyadeBlackCaptureSteps;

    const static std::vector<Step> kEmptySteps;

    constexpr inline const std::vector<Step> &GetPossibleCaptureMoves() const
    {
        switch (pieceType_)
        {
        case ChessPieceEnum::kPiyade:
            switch (GetColor())
            {
            case Color::kWhite:
                return kPiyadeWhiteCaptureSteps;
            case Color::kBlack:
                return kPiyadeBlackCaptureSteps;
            default:
                return kEmptySteps;
            }
        case ChessPieceEnum::kVizier:
        case ChessPieceEnum::kShah:
        case ChessPieceEnum::kHorse:
        case ChessPieceEnum::kFil:
        case ChessPieceEnum::kRook:
            return kEmptySteps;
        }

        return kEmptySteps;
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

    bool operator==(const Piece &other) const
    {
        return pieceType_ == other.pieceType_ && pos_ == other.pos_ && GetColor() == other.GetColor();
    }

  protected:
    Position pos_;
};

class Rook : public Piece
{
  public:
    Rook(Position pos, Color color);
};

class Piyade : public Piece
{

  public:
    Piyade(Position pos, Color color);

  private:
};

class Horse : public Piece
{
  public:
    Horse(Position pos, Color color);
};
class Fil : public Piece
{
  public:
    Fil(Position pos, Color color);
};
class Vizier : public Piece
{
  public:
    Vizier(Position pos, Color color);
};
class Shah : public Piece
{
  public:
    Shah(Position pos, Color color);
};

using ChessPiece = std::variant<Rook, Piyade, Vizier, Shah, Horse, Fil>;

} // namespace shatranj