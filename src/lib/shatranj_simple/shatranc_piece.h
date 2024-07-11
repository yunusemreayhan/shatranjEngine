#pragma once

#include <algorithm>
#include <array>
#include <memory>
#include <set>
#include <stdexcept>
#include <string_view>
#include <variant>
#include <vector>

#include "board.h"
#include "player.h"
#include "position.h"
#include "types.h"
namespace shatranj {
class Board;
class Player;

using PreComputeTableInternalContainer = std::vector<Position>;
enum class ChessPieceEnum : std::uint8_t {
    kShah,
    kVizier,
    kFil,
    kHorse,
    kRook,
    kPiyadeWhite,
    kPiyadeBlack,
    kCountpiecetypes,
    kNone
};
class PiecePrimitive  // positionless piece
{
   public:
    PiecePrimitive(ChessPieceEnum pieceType, Color color, bool moved);

    const static inline std::vector<Step> kRookSteps        = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    const static inline std::vector<Step> kPiyadeWhiteSteps = {{0, +1}};
    const static inline std::vector<Step> kPiyadeBlackSteps = {{0, -1}};
    const static inline std::vector<Step> kVizierSteps      = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    const static inline std::vector<Step> kShahSteps        = {{0, 1}, {1, 0},  {0, -1}, {-1, 0},
                                                               {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    const static inline std::vector<Step> kHorseSteps = {{+1, +2}, {+2, +1}, {-1, +2}, {-2, +1},
                                                         {-1, -2}, {-2, -1}, {+1, -2}, {+2, -1}};
    const static inline std::vector<Step> kFilSteps   = {{2, 2}, {2, -2}, {-2, 2}, {-2, -2}};
    const static inline std::vector<Step> kPiyadeWhiteCaptureSteps = {{1, +1}, {-1, +1}};
    const static inline std::vector<Step> kPiyadeBlackCaptureSteps = {{1, -1}, {-1, -1}};
    const static inline std::vector<Step> kEmptySteps              = {};

    inline std::string GetSymbol() const {
        if (GetColor() == Color::kWhite)
            switch (GetPieceType())
            {
            case ChessPieceEnum::kShah :
                return "\u2654";
            case ChessPieceEnum::kVizier :
                return "\u2655";
            case ChessPieceEnum::kRook :
                return "\u2656";
            case ChessPieceEnum::kFil :
                return "\u2657";
            case ChessPieceEnum::kPiyadeWhite :
                return "\u2659";
            case ChessPieceEnum::kHorse :
                return "\u2658";
            case ChessPieceEnum::kNone :
                return " ";
            case ChessPieceEnum::kPiyadeBlack :
                throw std::runtime_error("PiyadeBlack should not be here");
            case ChessPieceEnum::kCountpiecetypes :
                throw std::runtime_error("kCountpiecetypes should not be here");
            }
        else
            switch (GetPieceType())
            {
            case ChessPieceEnum::kShah :
                return "\u265A";
            case ChessPieceEnum::kVizier :
                return "\u265B";
            case ChessPieceEnum::kRook :
                return "\u265C";
            case ChessPieceEnum::kFil :
                return "\u265D";
            case ChessPieceEnum::kHorse :
                return "\u265E";
            case ChessPieceEnum::kPiyadeBlack :
                return "\u265F";
            case ChessPieceEnum::kNone :
                return " ";
            case ChessPieceEnum::kPiyadeWhite :
                throw std::runtime_error("PiyadeWhite should not be here");
            case ChessPieceEnum::kCountpiecetypes :
                throw std::runtime_error("kCountpiecetypes should not be here");
            }

        return " ";
    }
    inline std::string GetSymbolOld() const {
        if (GetColor() == Color::kWhite)
            switch (GetPieceType())
            {
            case ChessPieceEnum::kPiyadeWhite :
                return "P";
            case ChessPieceEnum::kVizier :
                return "V";
            case ChessPieceEnum::kShah :
                return "S";
            case ChessPieceEnum::kHorse :
                return "H";
            case ChessPieceEnum::kFil :
                return "F";
            case ChessPieceEnum::kRook :
                return "R";
            case ChessPieceEnum::kNone :
                return " ";
            case ChessPieceEnum::kPiyadeBlack :
                throw std::runtime_error("PiyadeBlack should not be here");
            case ChessPieceEnum::kCountpiecetypes :
                throw std::runtime_error("kCountpiecetypes should not be here");
            }
        else
            switch (GetPieceType())
            {
            case ChessPieceEnum::kPiyadeBlack :
                return "p";
            case ChessPieceEnum::kVizier :
                return "v";
            case ChessPieceEnum::kShah :
                return "s";
            case ChessPieceEnum::kFil :
                return "f";
            case ChessPieceEnum::kHorse :
                return "h";
            case ChessPieceEnum::kRook :
                return "r";
            case ChessPieceEnum::kNone :
                return " ";
            case ChessPieceEnum::kPiyadeWhite :
                throw std::runtime_error("PiyadeWhite should not be here");
            case ChessPieceEnum::kCountpiecetypes :
                throw std::runtime_error("kCountpiecetypes should not be here");
            }

        return " ";
    }
    inline constexpr std::string_view GetName() const {
        switch (GetPieceType())
        {
        case ChessPieceEnum::kPiyadeWhite :
            return "WPiyade";
        case ChessPieceEnum::kPiyadeBlack :
            return "BPiyade";
        case ChessPieceEnum::kVizier :
            return "Vizier";
        case ChessPieceEnum::kShah :
            return "Shah";
        case ChessPieceEnum::kHorse :
            return "Horse";
        case ChessPieceEnum::kFil :
            return "Fil";
        case ChessPieceEnum::kRook :
            return "Rook";
        case ChessPieceEnum::kNone :
            return "None";
        case ChessPieceEnum::kCountpiecetypes :
            throw std::runtime_error("kCountpiecetypes should not be here");
        }

        return " ";
    }

    static constexpr inline const std::vector<Step>&
    GetPossibleRegularMoves(ChessPieceEnum pieceType) {
        switch (pieceType)
        {
        case ChessPieceEnum::kPiyadeWhite :
            return kPiyadeWhiteSteps;
        case ChessPieceEnum::kPiyadeBlack :
            return kPiyadeBlackSteps;
        case ChessPieceEnum::kVizier :
            return kVizierSteps;
        case ChessPieceEnum::kShah :
            return kShahSteps;
        case ChessPieceEnum::kHorse :
            return kHorseSteps;
        case ChessPieceEnum::kFil :
            return kFilSteps;
        case ChessPieceEnum::kRook :
            return kRookSteps;
        case ChessPieceEnum::kNone :
            return kEmptySteps;
        case ChessPieceEnum::kCountpiecetypes :
            throw std::runtime_error("kCountpiecetypes should not be here");
        }

        return kEmptySteps;
    }
    static std::array<std::array<std::array<PreComputeTableInternalContainer,
                                            static_cast<size_t>(ChessPieceEnum::kCountpiecetypes)>,
                                 8>,
                      8>
      move_per_square_table;

    static void InitMovePerSquareTable() {
        for (size_t i = 0; i < 8; i++)
        {
            for (size_t j = 0; j < 8; j++)
            {
                for (ChessPieceEnum piece_type = ChessPieceEnum::kShah;
                     piece_type < ChessPieceEnum::kCountpiecetypes;
                     piece_type = static_cast<ChessPieceEnum>(static_cast<int>(piece_type) + 1))
                {
                    GetPreComputedMoveTable(piece_type, Position(i, j));
                }
            }
        }
    }

    static PreComputeTableInternalContainer& GetPreComputedMoveTable(ChessPieceEnum  pieceType,
                                                                     const Position& frompos) {
        PreComputeTableInternalContainer& ret =
          move_per_square_table[frompos.Getx()][frompos.Gety()][static_cast<size_t>(pieceType)];

        if (!ret.empty())
            return ret;

        auto tocalc = GetPossibleRegularMoves(pieceType);

        for (const auto& step : tocalc)
        {
            if (CanMultipleMove(pieceType))
            {
                for (int8_t i = 0; i < 8; i++)
                {
                    Position newpos = frompos;
                    auto     mstep  = step;
                    newpos.Move(mstep.Times(i));
                    if (newpos.IsValid())
                        ret.push_back(newpos);
                }
            }
            else
            {
                Position newpos = frompos;
                newpos.Move(step);
                if (newpos.IsValid())
                    ret.push_back(newpos);
            }
        }

        return ret;
    }

    inline static constexpr const std::vector<Step>&
    GetPossibleCaptureMoves(ChessPieceEnum pieceType) {
        if (pieceType != ChessPieceEnum::kPiyadeBlack && pieceType != ChessPieceEnum::kPiyadeWhite)
            return kEmptySteps;
        switch (pieceType)
        {
        case ChessPieceEnum::kPiyadeBlack :
            return kPiyadeBlackCaptureSteps;
        case ChessPieceEnum::kPiyadeWhite :
            return kPiyadeWhiteCaptureSteps;
        default :
            return kEmptySteps;
        }

        return kEmptySteps;
    }

    static std::array<std::array<std::array<PreComputeTableInternalContainer,
                                            static_cast<size_t>(ChessPieceEnum::kCountpiecetypes)>,
                                 8>,
                      8>
      capture_per_square_table;

    static void InitCapturePerSquareTable() {
        for (size_t i = 0; i < 8; i++)
        {
            for (size_t j = 0; j < 8; j++)
            {
                for (ChessPieceEnum piece_type = ChessPieceEnum::kShah;
                     piece_type < ChessPieceEnum::kCountpiecetypes;
                     piece_type = static_cast<ChessPieceEnum>(static_cast<int>(piece_type) + 1))
                {
                    GetPreComputedCaptureTable(piece_type, Position(i, j));
                }
            }
        }
    }
    static PreComputeTableInternalContainer& GetPreComputedCaptureTable(ChessPieceEnum  pieceType,
                                                                        const Position& frompos) {
        PreComputeTableInternalContainer& ret =
          capture_per_square_table[frompos.Getx()][frompos.Gety()][static_cast<size_t>(pieceType)];

        if (!ret.empty())
            return ret;

        auto tocalc = GetPossibleCaptureMoves(pieceType);

        for (const auto& step : tocalc)
        {
            Position newpos = frompos;
            newpos.Move(step);
            if (newpos.IsValid())
                ret.push_back(newpos);
        }

        return ret;
    }

    static bool
    CanMoveWithMem(ChessPieceEnum pieceType, const Position& frompos, const Position& topos) {
        auto check = GetPreComputedMoveTable(pieceType, frompos);
        return std::find(check.begin(), check.end(), topos) != check.end();
    }

    static bool
    CanCaptureWithMem(ChessPieceEnum pieceType, const Position& frompos, const Position& topos) {
        auto check = GetPreComputedCaptureTable(pieceType, frompos);
        return std::find(check.begin(), check.end(), topos) != check.end();
    }

    inline static constexpr bool CanMultipleMove(ChessPieceEnum pieceType) {
        switch (pieceType)
        {
        case ChessPieceEnum::kPiyadeBlack :
        case ChessPieceEnum::kPiyadeWhite :
        case ChessPieceEnum::kVizier :
        case ChessPieceEnum::kShah :
        case ChessPieceEnum::kHorse :
        case ChessPieceEnum::kFil :
            return false;
        case ChessPieceEnum::kRook :
            return true;
        case ChessPieceEnum::kNone :
        case ChessPieceEnum::kCountpiecetypes :
            throw std::runtime_error("this should not be called here CanMultipleMove");
        }

        return false;
    }

    inline static constexpr bool CanJumpOverOthers(ChessPieceEnum pieceType) {
        switch (pieceType)
        {
        case ChessPieceEnum::kPiyadeBlack :
        case ChessPieceEnum::kPiyadeWhite :
        case ChessPieceEnum::kVizier :
        case ChessPieceEnum::kShah :
        case ChessPieceEnum::kRook :
            return false;
        case ChessPieceEnum::kFil :
        case ChessPieceEnum::kHorse :
            return true;
        case ChessPieceEnum::kNone :
        case ChessPieceEnum::kCountpiecetypes :
            throw std::runtime_error("this should not be called here CanJumpOverOthers");
        }

        return false;
    }

    inline constexpr bool IsPiyade() const {
        return GetPieceType() == ChessPieceEnum::kPiyadeBlack
            || GetPieceType() == ChessPieceEnum::kPiyadeWhite;
    }

    inline constexpr bool IsVizier() const { return GetPieceType() == ChessPieceEnum::kVizier; }

    inline constexpr bool IsShah() const { return GetPieceType() == ChessPieceEnum::kShah; }

    inline constexpr bool IsHorse() const { return GetPieceType() == ChessPieceEnum::kHorse; }

    constexpr bool IsFil() const { return GetPieceType() == ChessPieceEnum::kFil; }

    inline constexpr bool IsRook() const { return GetPieceType() == ChessPieceEnum::kRook; }

    inline Color GetColor() const { return IsWhite() ? Color::kWhite : Color::kBlack; }

    inline static double GetCenterDistance(Position pos) {
        const static Position kCenter(4, 4);
        auto                  res = kCenter.Diff(pos);
        return res.OklideanDistance();
    }

    inline double GetVizierDistanceForPiyade(Position pos) const {
        if (GetColor() == Color::kBlack)
        {
            return std::abs(0 - pos.Gety());
        }

        return std::abs(7 - pos.Gety());
    }

    inline double GetPiecePoint() const {
        switch (GetPieceType())
        {
        case ChessPieceEnum::kPiyadeBlack :
        case ChessPieceEnum::kPiyadeWhite :
            return 1.0;
        case ChessPieceEnum::kShah :
            return 200.0;
        case ChessPieceEnum::kVizier :
        case ChessPieceEnum::kHorse :
        case ChessPieceEnum::kFil :
            return 3.0;
        case ChessPieceEnum::kRook :
            return 5.0;
        case ChessPieceEnum::kNone :
        case ChessPieceEnum::kCountpiecetypes :
            throw std::runtime_error("this should not be called here GetPiecePoint");
        }

        return 0;
    }

    bool IsWhite() const { return static_cast<bool>(data_ & 0b10000000); }

    constexpr ChessPieceEnum GetPieceType() const {
        return static_cast<ChessPieceEnum>(data_ & 0b00111111);
    }

    bool IsMoved() const { return static_cast<bool>(data_ & 0b01000000); }

   protected:
    void SetWhite(bool isWhite) {
        data_ = (data_ & 0b01111111) | (static_cast<uint8_t>(isWhite) << 7);
    }

    void SetMoved(bool moved) { data_ = (data_ & 0b10111111) | (static_cast<uint8_t>(moved) << 6); }

    void SetPieceType(ChessPieceEnum pieceType) {
        data_ = (data_ & 0b11000000) | static_cast<uint8_t>(pieceType);
    }
    uint8_t data_;
};

class Piece: public PiecePrimitive {
   public:
    explicit Piece(ChessPieceEnum pieceType, Position pos, const Color color, bool moved);
    explicit Piece(PiecePrimitive primitive, Position pos) :
        PiecePrimitive(primitive),
        pos_(pos) {}

    const Position& GetPos() const { return pos_; }

    PiecePrimitive GetPrimitive() const {
        return PiecePrimitive(GetPieceType(), GetColor(), IsMoved());
    }

    static bool CanMove(Position frompos, Position topos, ChessPieceEnum pieceType);
    static bool CanPawnCapture(Position frompos, Position pos, ChessPieceEnum pieceType);
    bool        Move(Position pos);

    bool operator==(const Piece& other) const {
        return GetPieceType() == other.GetPieceType() && pos_ == other.pos_
            && GetColor() == other.GetColor();
    }

   protected:
    Position pos_;
};

class Rook: public Piece {
   public:
    Rook(Position pos, Color color);
};

class Piyade: public Piece {

   public:
    Piyade(Position pos, Color color);

   private:
};

class Horse: public Piece {
   public:
    Horse(Position pos, Color color);
};
class Fil: public Piece {
   public:
    Fil(Position pos, Color color);
};
class Vizier: public Piece {
   public:
    Vizier(Position pos, Color color);
};
class Shah: public Piece {
   public:
    Shah(Position pos, Color color);
};

using ChessPiece = std::variant<Rook, Piyade, Vizier, Shah, Horse, Fil>;

static inline PiecePrimitive FromChar(char piecechar) {
    switch (piecechar)
    {
    case 'P' :
        return PiecePrimitive(ChessPieceEnum::kPiyadeWhite, Color::kWhite, false);
    case 'p' :
        return PiecePrimitive(ChessPieceEnum::kPiyadeBlack, Color::kBlack, false);
    case 'V' :
        return PiecePrimitive(ChessPieceEnum::kVizier, Color::kWhite, false);
    case 'v' :
        return PiecePrimitive(ChessPieceEnum::kVizier, Color::kBlack, false);
    case 'S' :
        return PiecePrimitive(ChessPieceEnum::kShah, Color::kWhite, false);
    case 's' :
        return PiecePrimitive(ChessPieceEnum::kShah, Color::kBlack, false);
    case 'H' :
        return PiecePrimitive(ChessPieceEnum::kHorse, Color::kWhite, false);
    case 'h' :
        return PiecePrimitive(ChessPieceEnum::kHorse, Color::kBlack, false);
    case 'F' :
        return PiecePrimitive(ChessPieceEnum::kFil, Color::kWhite, false);
    case 'f' :
        return PiecePrimitive(ChessPieceEnum::kFil, Color::kBlack, false);
    case 'R' :
        return PiecePrimitive(ChessPieceEnum::kRook, Color::kWhite, false);
    case 'r' :
        return PiecePrimitive(ChessPieceEnum::kRook, Color::kBlack, false);
    }

    throw std::runtime_error("Invalid piece char: " + std::to_string(piecechar));
}
}  // namespace shatranj
