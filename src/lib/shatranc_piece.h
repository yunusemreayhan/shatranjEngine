#pragma once

#include <memory>
#include <stdexcept>
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
    kNone,
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
    PiecePrimitive(ChessPieceEnum pieceType, Color color, bool moved);

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
        case ChessPieceEnum::kNone:
            return ' ';
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
        case ChessPieceEnum::kNone:
            return "None";
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

    static constexpr inline const std::vector<Step> &GetPossibleRegularMoves(ChessPieceEnum pieceType, Color color)
    {
        switch (pieceType)
        {
        case ChessPieceEnum::kPiyade:
            switch (color)
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
        case ChessPieceEnum::kNone:
            return kEmptySteps;
        }

        return kEmptySteps;
    }
    const static std::vector<Step> kPiyadeWhiteCaptureSteps;

    const static std::vector<Step> kPiyadeBlackCaptureSteps;

    const static std::vector<Step> kEmptySteps;

    static constexpr inline const std::vector<Step> &GetPossibleCaptureMoves(ChessPieceEnum pieceType, Color color)
    {
        switch (pieceType)
        {
        case ChessPieceEnum::kPiyade:
            switch (color)
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
        case ChessPieceEnum::kNone:
            return kEmptySteps;
        }

        return kEmptySteps;
    }

    static constexpr bool CanMultipleMove(ChessPieceEnum pieceType)
    {
        switch (pieceType)
        {
        case ChessPieceEnum::kPiyade:
        case ChessPieceEnum::kVizier:
        case ChessPieceEnum::kShah:
        case ChessPieceEnum::kHorse:
        case ChessPieceEnum::kFil:
        case ChessPieceEnum::kNone:
            return false;
        case ChessPieceEnum::kRook:
            return true;
        }

        return false;
    }

    static constexpr bool CanJumpOverOthers(ChessPieceEnum pieceType)
    {
        switch (pieceType)
        {
        case ChessPieceEnum::kPiyade:
        case ChessPieceEnum::kVizier:
        case ChessPieceEnum::kShah:
        case ChessPieceEnum::kRook:
        case ChessPieceEnum::kNone:
            return false;
        case ChessPieceEnum::kFil:
        case ChessPieceEnum::kHorse:
            return true;
        }

        return false;
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

    constexpr bool IsMoved() const
    {
        return moved_;
    }

    Color GetColor() const
    {
        return isWhite_ ? Color::kWhite : Color::kBlack;
    }

  protected:
    ChessPieceEnum pieceType_;

    int8_t isWhite_ : 1;
    int8_t moved_ : 1;
};

class Piece : public PiecePrimitive
{
  public:
    explicit Piece(ChessPieceEnum pieceType, Position pos, const Color color, bool moved);

    const Position &GetPos() const
    {
        return pos_;
    }

    static bool CanMove(Position frompos, Position pos, const std::shared_ptr<Board> &board, ChessPieceEnum pieceType,
                        Color color);
    static bool CanThreat(Position frompos, Position pos, const std::shared_ptr<Board> &board, ChessPieceEnum pieceType,
                          Color color);
    static bool CanCapture(Position frompos, Position pos, const std::shared_ptr<Board> &board,
                           ChessPieceEnum pieceType, Color color);
    static bool CanGo(Position frompos, Position pos, const std::shared_ptr<Board> &board, ChessPieceEnum pieceType,
                      Color color);
    bool Move(Position pos);
    static std::vector<Movement> GetPossibleMoves(Position frompos, const std::shared_ptr<Board> &board,
                                                  ChessPieceEnum pieceType, Color color);

    bool operator==(const Piece &other) const
    {
        return pieceType_ == other.pieceType_ && pos_ == other.pos_ && GetColor() == other.GetColor();
    }

    double GetCenterDistance() const
    {
        const static Position kCenter(4, 4);
        auto res =  kCenter.Diff(GetPos());
        return res.OklideanDistance();
    }

    double GetVizierDistanceForPiyade() const
    {
        if (GetColor() == Color::kBlack)
        {
            return std::abs(0 - GetPos().Gety());
        }

        return std::abs(7 - GetPos().Gety());
    }

    constexpr double GetPiecePoint() const
    {
        switch (pieceType_)
        {
        case ChessPieceEnum::kPiyade:
            return 1.0 / (1 + GetVizierDistanceForPiyade()) / GetCenterDistance();
        case ChessPieceEnum::kVizier:
            return 2.0 / GetCenterDistance();
        case ChessPieceEnum::kShah:
            return 1.0 / GetCenterDistance();
        case ChessPieceEnum::kHorse:
            return 3.0 / GetCenterDistance();
        case ChessPieceEnum::kFil:
            return 2.0 / GetCenterDistance();
        case ChessPieceEnum::kRook:
            return 4.0 / GetCenterDistance();
        case ChessPieceEnum::kNone:
            return 0 * GetCenterDistance();
        }

        return 0;
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

static Piece FromChar(char piecechar, Position pos)
{
    switch (piecechar)
    {
    case 'P':
        return Piyade(pos, Color::kWhite);
    case 'p':
        return Piyade(pos, Color::kBlack);
    case 'V':
        return Vizier(pos, Color::kWhite);
    case 'v':
        return Vizier(pos, Color::kBlack);
    case 'S':
        return Shah(pos, Color::kWhite);
    case 's':
        return Shah(pos, Color::kBlack);
    case 'H':
        return Horse(pos, Color::kWhite);
    case 'h':
        return Horse(pos, Color::kBlack);
    case 'F':
        return Fil(pos, Color::kWhite);
    case 'f':
        return Fil(pos, Color::kBlack);
    case 'R':
        return Rook(pos, Color::kWhite);
    case 'r':
        return Rook(pos, Color::kBlack);
    }

    throw std::runtime_error("Invalid piece char: " + std::to_string(piecechar));
}
} // namespace shatranj