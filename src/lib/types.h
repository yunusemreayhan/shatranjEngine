#pragma once

#include <cstdint>
namespace shatranj
{
enum class Color : uint8_t
{
    kWhite,
    kBlack
};

inline Color OpponentColor(Color color) {
    return color == Color::kWhite ? Color::kBlack : Color::kWhite;
}

constexpr static inline bool kDebugGlobal = false;
constexpr static inline bool kStressTest = false;
} // namespace shatranj