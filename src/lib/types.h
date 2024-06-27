#pragma once

namespace shatranj
{
enum class Color : bool
{
    kWhite,
    kBlack
};

inline Color OpponentColor(Color color) {
    return color == Color::kWhite ? Color::kBlack : Color::kWhite;
}

constexpr static inline bool kDebugGlobal = true;
} // namespace shatranj