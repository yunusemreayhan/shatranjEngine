#pragma once

#include <cstdint>
#include <ostream>
namespace shatranj
{
enum class Color : uint8_t
{
    kWhite,
    kBlack
};

enum class GameState : uint8_t
{
    kNormal,
    kCheck,
    kCheckmate,
    kStalemate,
    kDraw
};

inline Color OpponentColor(Color color) {
    return color == Color::kWhite ? Color::kBlack : Color::kWhite;
}

inline std::ostream &operator<<(std::ostream &ostr, const Color &color)
{
    ostr << (color == Color::kWhite ? "w" : "b");
    return ostr;
}

constexpr static inline bool kDebugGlobal = false;
constexpr static inline bool kDebugShatranj = false;
constexpr static inline bool kStressTest = false;
constexpr static inline bool kDebugTablePrint = false;
constexpr static inline bool kCaptureDebug = false;
constexpr static inline bool kPieceGroupDebug = false;
constexpr static inline bool kBoardDebug = false;
constexpr static inline bool kMinMaxDebug = false;

} // namespace shatranj