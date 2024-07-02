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

    inline std::string GetSymbol() const
    {
        if (GetColor() == Color::kWhite)
            switch (GetPieceType())
            {
            case ChessPieceEnum::kShah:
                return "\u2654";
            case ChessPieceEnum::kVizier:
                return "\u2655";
            case ChessPieceEnum::kRook:
                return "\u2656";
            case ChessPieceEnum::kFil:
                return "\u2657";
            case ChessPieceEnum::kPiyade:
                return "\u2659";
            case ChessPieceEnum::kHorse:
                return "\u2658";
            case ChessPieceEnum::kNone:
                return " ";
            }
        else
            switch (GetPieceType())
            {
            case ChessPieceEnum::kShah:
                return "\u265A";
            case ChessPieceEnum::kVizier:
                return "\u265B";
            case ChessPieceEnum::kRook:
                return "\u265C";
            case ChessPieceEnum::kFil:
                return "\u265D";
            case ChessPieceEnum::kHorse:
                return "\u265E";
            case ChessPieceEnum::kPiyade:
                return "\u265F";
            case ChessPieceEnum::kNone:
                return " ";
            }

        return " ";
    }
    inline std::string GetSymbolOld() const
    {
        if (GetColor() == Color::kWhite)
            switch (GetPieceType())
            {
            case ChessPieceEnum::kPiyade:
                return "P";
            case ChessPieceEnum::kVizier:
                return "V";
            case ChessPieceEnum::kShah:
                return "S";
            case ChessPieceEnum::kHorse:
                return "H";
            case ChessPieceEnum::kFil:
                return "F";
            case ChessPieceEnum::kRook:
                return "R";
            case ChessPieceEnum::kNone:
                return " ";
            }
        else
            switch (GetPieceType())
            {
            case ChessPieceEnum::kPiyade:
                return "p";
            case ChessPieceEnum::kVizier:
                return "v";
            case ChessPieceEnum::kShah:
                return "s";
            case ChessPieceEnum::kHorse:
                return "h";
            case ChessPieceEnum::kFil:
                return "f";
            case ChessPieceEnum::kRook:
                return "r";
            case ChessPieceEnum::kNone:
                return " ";
            }

        return " ";
    }
    inline constexpr std::string_view GetName() const
    {
        switch (GetPieceType())
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

    inline static constexpr const std::vector<Step> &GetPossibleCaptureMoves(ChessPieceEnum pieceType, Color color)
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

    inline static constexpr bool CanMultipleMove(ChessPieceEnum pieceType)
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

    inline static constexpr bool CanJumpOverOthers(ChessPieceEnum pieceType)
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

    inline constexpr bool IsPiyade() const
    {
        return GetPieceType() == ChessPieceEnum::kPiyade;
    }

    inline constexpr bool IsVizier() const
    {
        return GetPieceType() == ChessPieceEnum::kVizier;
    }

    inline constexpr bool IsShah() const
    {
        return GetPieceType() == ChessPieceEnum::kShah;
    }

    inline constexpr bool IsHorse() const
    {
        return GetPieceType() == ChessPieceEnum::kHorse;
    }

    constexpr bool IsFil() const
    {
        return GetPieceType() == ChessPieceEnum::kFil;
    }

    inline constexpr bool IsRook() const
    {
        return GetPieceType() == ChessPieceEnum::kRook;
    }

    inline Color GetColor() const
    {
        return IsWhite() ? Color::kWhite : Color::kBlack;
    }

    inline static double GetCenterDistance(Position pos)
    {
        const static Position kCenter(4, 4);
        auto res = kCenter.Diff(pos);
        return res.OklideanDistance();
    }

    inline double GetVizierDistanceForPiyade(Position pos) const
    {
        if (GetColor() == Color::kBlack)
        {
            return std::abs(0 - pos.Gety());
        }

        return std::abs(7 - pos.Gety());
    }

    inline double GetPiecePoint(Position pos) const
    {
        switch (GetPieceType())
        {
        case ChessPieceEnum::kPiyade:
            return 1.0;
        case ChessPieceEnum::kShah:
            return 200.0;
        case ChessPieceEnum::kVizier:
        case ChessPieceEnum::kHorse:
        case ChessPieceEnum::kFil:
            return 3.0;
        case ChessPieceEnum::kRook:
            return 5.0;
        case ChessPieceEnum::kNone:
            return 0;
        }

        return 0;
    }

    bool IsWhite() const
    {
        return static_cast<bool>(data_ & 0b10000000);
    }

    constexpr ChessPieceEnum GetPieceType() const
    {
        return static_cast<ChessPieceEnum>(data_ & 0b00111111);
    }

    bool IsMoved() const
    {
        return static_cast<bool>(data_ & 0b01000000);
    }

  protected:
    void SetWhite(bool isWhite)
    {
        data_ = (data_ & 0b01111111) | (static_cast<uint8_t>(isWhite) << 7);
    }

    void SetMoved(bool moved)
    {
        data_ = (data_ & 0b10111111) | (static_cast<uint8_t>(moved) << 6);
    }

    void SetPieceType(ChessPieceEnum pieceType)
    {
        data_ = (data_ & 0b11000000) | static_cast<uint8_t>(pieceType);
    }
    uint8_t data_;
};

class Piece : public PiecePrimitive
{
  public:
    explicit Piece(ChessPieceEnum pieceType, Position pos, const Color color, bool moved);
    explicit Piece(PiecePrimitive primitive, Position pos) : PiecePrimitive(primitive), pos_(pos)
    {
    }

    const Position &GetPos() const
    {
        return pos_;
    }

    PiecePrimitive GetPrimitive() const
    {
        return PiecePrimitive(GetPieceType(), GetColor(), IsMoved());
    }

    static bool CanMove(Position frompos, Position pos, ChessPieceEnum pieceType, Color color);
    static bool CanPawnCapture(Position frompos, Position pos, ChessPieceEnum pieceType, Color color);
    bool Move(Position pos);

    bool operator==(const Piece &other) const
    {
        return GetPieceType() == other.GetPieceType() && pos_ == other.pos_ && GetColor() == other.GetColor();
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

static inline PiecePrimitive FromChar(char piecechar)
{
    switch (piecechar)
    {
    case 'P':
        return PiecePrimitive(ChessPieceEnum::kPiyade, Color::kWhite, false);
    case 'p':
        return PiecePrimitive(ChessPieceEnum::kPiyade, Color::kBlack, false);
    case 'V':
        return PiecePrimitive(ChessPieceEnum::kVizier, Color::kWhite, false);
    case 'v':
        return PiecePrimitive(ChessPieceEnum::kVizier, Color::kBlack, false);
    case 'S':
        return PiecePrimitive(ChessPieceEnum::kShah, Color::kWhite, false);
    case 's':
        return PiecePrimitive(ChessPieceEnum::kShah, Color::kBlack, false);
    case 'H':
        return PiecePrimitive(ChessPieceEnum::kHorse, Color::kWhite, false);
    case 'h':
        return PiecePrimitive(ChessPieceEnum::kHorse, Color::kBlack, false);
    case 'F':
        return PiecePrimitive(ChessPieceEnum::kFil, Color::kWhite, false);
    case 'f':
        return PiecePrimitive(ChessPieceEnum::kFil, Color::kBlack, false);
    case 'R':
        return PiecePrimitive(ChessPieceEnum::kRook, Color::kWhite, false);
    case 'r':
        return PiecePrimitive(ChessPieceEnum::kRook, Color::kBlack, false);
    }

    throw std::runtime_error("Invalid piece char: " + std::to_string(piecechar));
}
} // namespace shatranj