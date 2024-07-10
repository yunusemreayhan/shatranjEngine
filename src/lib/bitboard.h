#pragma once

#include <bitset>
#include <cassert>
#include <cstdint>
#include <ctype.h>
#include <iostream>
#include <string_view>

namespace shatranj {

namespace stockfish_bitboard {

using Key      = uint64_t;
using Bitboard = uint64_t;

constexpr int MAX_MOVES = 256;
constexpr int MAX_PLY   = 246;

enum Color {
    WHITE,
    BLACK,
    COLOR_NB = 2
};
/*
enum CastlingRights {
    NO_CASTLING,
    WHITE_OO,
    WHITE_OOO = WHITE_OO << 1,
    BLACK_OO  = WHITE_OO << 2,
    BLACK_OOO = WHITE_OO << 3,

    KING_SIDE      = WHITE_OO | BLACK_OO,
    QUEEN_SIDE     = WHITE_OOO | BLACK_OOO,
    WHITE_CASTLING = WHITE_OO | WHITE_OOO,
    BLACK_CASTLING = BLACK_OO | BLACK_OOO,
    ANY_CASTLING   = WHITE_CASTLING | BLACK_CASTLING,

    CASTLING_RIGHT_NB = 16
};

enum Bound {
    BOUND_NONE,
    BOUND_UPPER,
    BOUND_LOWER,
    BOUND_EXACT = BOUND_UPPER | BOUND_LOWER
};*/

// Value is used as an alias for int16_t, this is done to differentiate between
// a search value and any other integer value. The values used in search are always
// supposed to be in the range (-VALUE_NONE, VALUE_NONE] and should not exceed this range.
using Value = int;

constexpr Value VALUE_ZERO     = 0;
constexpr Value VALUE_NONE     = 32002;
constexpr Value VALUE_INFINITE = 32001;

constexpr Value VALUE_MATE             = 32000;
constexpr Value VALUE_DRAW             = VALUE_MATE;
constexpr Value VALUE_MATE_IN_MAX_PLY  = VALUE_MATE - MAX_PLY;
constexpr Value VALUE_MATED_IN_MAX_PLY = -VALUE_MATE_IN_MAX_PLY;

constexpr Value VALUE_TB                 = VALUE_MATE_IN_MAX_PLY - 1;
constexpr Value VALUE_TB_WIN_IN_MAX_PLY  = VALUE_TB - MAX_PLY;
constexpr Value VALUE_TB_LOSS_IN_MAX_PLY = -VALUE_TB_WIN_IN_MAX_PLY;

// In the code, we make the assumption that these values
// are such that non_pawn_material() can be used to uniquely
// identify the material on the board.
constexpr Value PawnValue   = 208;
constexpr Value KnightValue = 781;
constexpr Value BishopValue = 825;
constexpr Value RookValue   = 1276;
constexpr Value QueenValue  = 2538;

// clang-format off
enum PieceType {
    NO_PIECE_TYPE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
    ALL_PIECES = 0,
    PIECE_TYPE_NB = 8
};

enum Piece {
    NO_PIECE,
    W_PAWN = PAWN,     W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
    B_PAWN = PAWN + 8, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING,
    PIECE_NB = 16
};

const std::string_view piece_type_to_string(PieceType type) {
    switch (type) {
        case PAWN:
            return "pawn";
        case KNIGHT:
            return "knight";
        case BISHOP:
            return "bishop";
        case ROOK:
            return "rook";
        case QUEEN:
            return "queen";
        case KING:
            return "king";
        default:
            return "unknown";
    }
}
// clang-format on

constexpr Value PieceValue[PIECE_NB] = {
  VALUE_ZERO, PawnValue, KnightValue, BishopValue, RookValue, QueenValue, VALUE_ZERO, VALUE_ZERO,
  VALUE_ZERO, PawnValue, KnightValue, BishopValue, RookValue, QueenValue, VALUE_ZERO, VALUE_ZERO};

constexpr Bitboard FileABB = 0x0101010101010101ULL;
constexpr Bitboard FileBBB = FileABB << 1;
constexpr Bitboard FileCBB = FileABB << 2;
constexpr Bitboard FileDBB = FileABB << 3;
constexpr Bitboard FileEBB = FileABB << 4;
constexpr Bitboard FileFBB = FileABB << 5;
constexpr Bitboard FileGBB = FileABB << 6;
constexpr Bitboard FileHBB = FileABB << 7;

constexpr Bitboard Rank1BB = 0xFF;
constexpr Bitboard Rank2BB = Rank1BB << (8 * 1);
constexpr Bitboard Rank3BB = Rank1BB << (8 * 2);
constexpr Bitboard Rank4BB = Rank1BB << (8 * 3);
constexpr Bitboard Rank5BB = Rank1BB << (8 * 4);
constexpr Bitboard Rank6BB = Rank1BB << (8 * 5);
constexpr Bitboard Rank7BB = Rank1BB << (8 * 6);
constexpr Bitboard Rank8BB = Rank1BB << (8 * 7);

enum Square : int {
    SQ_A1,
    SQ_B1,
    SQ_C1,
    SQ_D1,
    SQ_E1,
    SQ_F1,
    SQ_G1,
    SQ_H1,
    SQ_A2,
    SQ_B2,
    SQ_C2,
    SQ_D2,
    SQ_E2,
    SQ_F2,
    SQ_G2,
    SQ_H2,
    SQ_A3,
    SQ_B3,
    SQ_C3,
    SQ_D3,
    SQ_E3,
    SQ_F3,
    SQ_G3,
    SQ_H3,
    SQ_A4,
    SQ_B4,
    SQ_C4,
    SQ_D4,
    SQ_E4,
    SQ_F4,
    SQ_G4,
    SQ_H4,
    SQ_A5,
    SQ_B5,
    SQ_C5,
    SQ_D5,
    SQ_E5,
    SQ_F5,
    SQ_G5,
    SQ_H5,
    SQ_A6,
    SQ_B6,
    SQ_C6,
    SQ_D6,
    SQ_E6,
    SQ_F6,
    SQ_G6,
    SQ_H6,
    SQ_A7,
    SQ_B7,
    SQ_C7,
    SQ_D7,
    SQ_E7,
    SQ_F7,
    SQ_G7,
    SQ_H7,
    SQ_A8,
    SQ_B8,
    SQ_C8,
    SQ_D8,
    SQ_E8,
    SQ_F8,
    SQ_G8,
    SQ_H8,
    SQ_NONE,

    SQUARE_ZERO = 0,
    SQUARE_NB   = 64
};

const std::string_view square_to_string(Square sq) {
    switch (sq)
    {
    case SQ_A1 :
        return "a1";
    case SQ_B1 :
        return "b1";
    case SQ_C1 :
        return "c1";
    case SQ_D1 :
        return "d1";
    case SQ_E1 :
        return "e1";
    case SQ_F1 :
        return "f1";
    case SQ_G1 :
        return "g1";
    case SQ_H1 :
        return "h1";
    case SQ_A2 :
        return "a2";
    case SQ_B2 :
        return "b2";
    case SQ_C2 :
        return "c2";
    case SQ_D2 :
        return "d2";
    case SQ_E2 :
        return "e2";
    case SQ_F2 :
        return "f2";
    case SQ_G2 :
        return "g2";
    case SQ_H2 :
        return "h2";
    case SQ_A3 :
        return "a3";
    case SQ_B3 :
        return "b3";
    case SQ_C3 :
        return "c3";
    case SQ_D3 :
        return "d3";
    case SQ_E3 :
        return "e3";
    case SQ_F3 :
        return "f3";
    case SQ_G3 :
        return "g3";
    case SQ_H3 :
        return "h3";
    case SQ_A4 :
        return "a4";
    case SQ_B4 :
        return "b4";
    case SQ_C4 :
        return "c4";
    case SQ_D4 :
        return "d4";
    case SQ_E4 :
        return "e4";
    case SQ_F4 :
        return "f4";
    case SQ_G4 :
        return "g4";
    case SQ_H4 :
        return "h4";
    case SQ_A5 :
        return "a5";
    case SQ_B5 :
        return "b5";
    case SQ_C5 :
        return "c5";
    case SQ_D5 :
        return "d5";
    case SQ_E5 :
        return "e5";
    case SQ_F5 :
        return "f5";
    case SQ_G5 :
        return "g5";
    case SQ_H5 :
        return "h5";
    case SQ_A6 :
        return "a6";
    case SQ_B6 :
        return "b6";
    case SQ_C6 :
        return "c6";
    case SQ_D6 :
        return "d6";
    case SQ_E6 :
        return "e6";
    case SQ_F6 :
        return "f6";
    case SQ_G6 :
        return "g6";
    case SQ_H6 :
        return "h6";
    case SQ_A7 :
        return "a7";
    case SQ_B7 :
        return "b7";
    case SQ_C7 :
        return "c7";
    case SQ_D7 :
        return "d7";
    case SQ_E7 :
        return "e7";
    case SQ_F7 :
        return "f7";
    case SQ_G7 :
        return "g7";
    case SQ_H7 :
        return "h7";
    case SQ_A8 :
        return "a8";
    case SQ_B8 :
        return "b8";
    case SQ_C8 :
        return "c8";
    case SQ_D8 :
        return "d8";
    case SQ_E8 :
        return "e8";
    case SQ_F8 :
        return "f8";
    case SQ_G8 :
        return "g8";
    case SQ_H8 :
        return "h8";
    case SQ_NONE :
        return "none";
    default :
        return "unknown";
    }

    return "unknown";
}
// clang-format on

enum Direction : int {
    NORTH = 8,
    EAST  = 1,
    SOUTH = -NORTH,
    WEST  = -EAST,

    NORTH_EAST = NORTH + EAST,
    SOUTH_EAST = SOUTH + EAST,
    SOUTH_WEST = SOUTH + WEST,
    NORTH_WEST = NORTH + WEST
};

enum File : int {
    FILE_A,
    FILE_B,
    FILE_C,
    FILE_D,
    FILE_E,
    FILE_F,
    FILE_G,
    FILE_H,
    FILE_NB
};

enum Rank : int {
    RANK_1,
    RANK_2,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8,
    RANK_NB
};
// Keep track of what a move changes on the board (used by NNUE)
struct DirtyPiece {

    // Number of changed pieces
    int dirty_num;

    // Max 3 pieces can change in one move. A promotion with capture moves
    // both the pawn and the captured piece to SQ_NONE and the piece promoted
    // to from SQ_NONE to the capture square.
    Piece piece[3];

    // From and to squares, which may be SQ_NONE
    Square from[3];
    Square to[3];
};

#define ENABLE_INCR_OPERATORS_ON(T) \
    inline T& operator++(T& d) { return d = T(int(d) + 1); } \
    inline T& operator--(T& d) { return d = T(int(d) - 1); }

ENABLE_INCR_OPERATORS_ON(PieceType)
ENABLE_INCR_OPERATORS_ON(Square)
ENABLE_INCR_OPERATORS_ON(File)
ENABLE_INCR_OPERATORS_ON(Rank)

#undef ENABLE_INCR_OPERATORS_ON

constexpr Direction operator+(Direction d1, Direction d2) { return Direction(int(d1) + int(d2)); }
constexpr Direction operator*(int i, Direction d) { return Direction(i * int(d)); }

// Additional operators to add a Direction to a Square
constexpr Square operator+(Square s, Direction d) { return Square(int(s) + int(d)); }
constexpr Square operator-(Square s, Direction d) { return Square(int(s) - int(d)); }
inline Square&   operator+=(Square& s, Direction d) { return s = s + d; }
inline Square&   operator-=(Square& s, Direction d) { return s = s - d; }

// Toggle color
constexpr Color operator~(Color c) { return Color(c ^ BLACK); }

// Swap A1 <-> A8
constexpr Square flip_rank(Square s) { return Square(s ^ SQ_A8); }

// Swap A1 <-> H1
constexpr Square flip_file(Square s) { return Square(s ^ SQ_H1); }

// Swap color of piece B_KNIGHT <-> W_KNIGHT
constexpr Piece operator~(Piece pc) { return Piece(pc ^ 8); }

/*constexpr CastlingRights operator&(Color c, CastlingRights cr) {
    return CastlingRights((c == WHITE ? WHITE_CASTLING : BLACK_CASTLING) & cr);
}*/

constexpr Value mate_in(int ply) { return VALUE_MATE - ply; }

constexpr Value mated_in(int ply) { return -VALUE_MATE + ply; }

constexpr Square make_square(File f, Rank r) { return Square((r << 3) + f); }

constexpr Piece make_piece(Color c, PieceType pt) { return Piece((c << 3) + pt); }

constexpr PieceType type_of(Piece pc) { return PieceType(pc & 7); }

inline Color color_of(Piece pc) {
    assert(pc != NO_PIECE);
    return Color(pc >> 3);
}

constexpr bool is_ok(Square s) { return s >= SQ_A1 && s <= SQ_H8; }

constexpr File file_of(Square s) { return File(s & 7); }

constexpr Rank rank_of(Square s) { return Rank(s >> 3); }

constexpr Square relative_square(Color c, Square s) { return Square(s ^ (c * 56)); }

constexpr Rank relative_rank(Color c, Rank r) { return Rank(r ^ (c * 7)); }

constexpr Rank relative_rank(Color c, Square s) { return relative_rank(c, rank_of(s)); }

constexpr Direction pawn_push(Color c) { return c == WHITE ? NORTH : SOUTH; }


// Based on a congruential pseudo-random number generator
constexpr Key make_key(uint64_t seed) {
    return seed * 6364136223846793005ULL + 1442695040888963407ULL;
}


enum MoveType {
    NORMAL,
    PROMOTION  = 1 << 14,
    EN_PASSANT = 2 << 14,
    CASTLING   = 3 << 14
};

// A move needs 16 bits to be stored
//
// bit  0- 5: destination square (from 0 to 63)
// bit  6-11: origin square (from 0 to 63)
// bit 12-13: promotion piece type - 2 (from KNIGHT-2 to QUEEN-2)
// bit 14-15: special move flag: promotion (1), en passant (2), castling (3)
// NOTE: en passant bit is set only when a pawn can be captured
//
// Special cases are Move::none() and Move::null(). We can sneak these in because in
// any normal move destination square is always different from origin square
// while Move::none() and Move::null() have the same origin and destination square.
class Move {
   public:
    Move() = default;
    constexpr explicit Move(std::uint16_t d) :
        data(d) {}

    constexpr Move(Square from, Square to) :
        data((from << 6) + to) {}

    template<MoveType T>
    static constexpr Move make(Square from, Square to, PieceType pt = KNIGHT) {
        return Move(T + ((pt - KNIGHT) << 12) + (from << 6) + to);
    }

    constexpr Square from_sq() const {
        assert(is_ok());
        return Square((data >> 6) & 0x3F);
    }

    constexpr Square to_sq() const {
        assert(is_ok());
        return Square(data & 0x3F);
    }

    constexpr int from_to() const { return data & 0xFFF; }

    constexpr MoveType type_of() const { return MoveType(data & (3 << 14)); }

    constexpr PieceType promotion_type() const { return PieceType(((data >> 12) & 3) + KNIGHT); }

    constexpr bool is_ok() const { return none().data != data && null().data != data; }

    static constexpr Move null() { return Move(65); }
    static constexpr Move none() { return Move(0); }

    constexpr bool operator==(const Move& m) const { return data == m.data; }
    constexpr bool operator!=(const Move& m) const { return data != m.data; }

    constexpr explicit operator bool() const { return data != 0; }

    constexpr std::uint16_t raw() const { return data; }

    struct MoveHash {
        std::size_t operator()(const Move& m) const { return make_key(m.data); }
    };

   protected:
    std::uint16_t data;
};

constexpr Bitboard square_bb(Square s) {
    assert(is_ok(s));
    return (1ULL << s);
}
inline Bitboard  operator&(Bitboard b, Square s) { return b & square_bb(s); }
inline Bitboard  operator|(Bitboard b, Square s) { return b | square_bb(s); }
inline Bitboard  operator^(Bitboard b, Square s) { return b ^ square_bb(s); }
inline Bitboard& operator|=(Bitboard& b, Square s) { return b |= square_bb(s); }
inline Bitboard& operator^=(Bitboard& b, Square s) { return b ^= square_bb(s); }

inline Bitboard operator&(Square s, Bitboard b) { return b & s; }
inline Bitboard operator|(Square s, Bitboard b) { return b | s; }
inline Bitboard operator^(Square s, Bitboard b) { return b ^ s; }

inline Bitboard operator|(Square s1, Square s2) { return square_bb(s1) | s2; }

constexpr bool more_than_one(Bitboard b) { return b & (b - 1); }

// rank_bb() and file_bb() return a bitboard representing all the squares on
// the given file or rank.

constexpr Bitboard rank_bb(Rank r) { return Rank1BB << (8 * r); }

constexpr Bitboard rank_bb(Square s) { return rank_bb(rank_of(s)); }

constexpr Bitboard file_bb(File f) { return FileABB << f; }

constexpr Bitboard file_bb(Square s) { return file_bb(file_of(s)); }

template<Direction D>
constexpr Bitboard shift(Bitboard b) {
    return D == NORTH         ? b << 8
         : D == SOUTH         ? b >> 8
         : D == NORTH + NORTH ? b << 16
         : D == SOUTH + SOUTH ? b >> 16
         : D == EAST          ? (b & ~FileHBB) << 1
         : D == WEST          ? (b & ~FileABB) >> 1
         : D == NORTH_EAST    ? (b & ~FileHBB) << 9
         : D == NORTH_WEST    ? (b & ~FileABB) << 7
         : D == SOUTH_EAST    ? (b & ~FileHBB) >> 7
         : D == SOUTH_WEST    ? (b & ~FileABB) >> 9
                              : 0;
}

struct Magic;
struct Magic {
    Bitboard  mask;
    Bitboard  magic;
    Bitboard* attacks;
    unsigned  shift;

    // Compute the attack's index using the 'magic bitboards' approach
    unsigned index(Bitboard occupied) const {
        return unsigned(((occupied & mask) * magic) >> shift);
    }
};

class BoardWithBitboardRepresentation {

   public:
    // Returns the squares attacked by pawns of the given color
    // from the squares in the given bitboard.
    template<Color C>
    constexpr Bitboard pawn_attacks_bb(Bitboard b) {
        return C == WHITE ? shift<NORTH_WEST>(b) | shift<NORTH_EAST>(b)
                          : shift<SOUTH_WEST>(b) | shift<SOUTH_EAST>(b);
    }

    inline Bitboard pawn_attacks_bb(Color c, Square s) {

        assert(is_ok(s));
        return PawnAttacks[c][s];
    }
    inline int popcount(Bitboard b) {

#ifndef USE_POPCNT

        union {
            Bitboard bb;
            uint16_t u[4];
        } v = {b};
        return PopCnt16[v.u[0]] + PopCnt16[v.u[1]] + PopCnt16[v.u[2]] + PopCnt16[v.u[3]];

#elif defined(_MSC_VER)

        return int(_mm_popcnt_u64(b));

#else  // Assumed gcc or compatible compiler

        return __builtin_popcountll(b);

#endif
    }

    // Returns the least significant bit in a non-zero bitboard.
    inline Square lsb(Bitboard b) {
        assert(b);

#if defined(__GNUC__)  // GCC, Clang, ICX

        return Square(__builtin_ctzll(b));

#elif defined(_MSC_VER)
    #ifdef _WIN64  // MSVC, WIN64

        unsigned long idx;
        _BitScanForward64(&idx, b);
        return Square(idx);

    #else  // MSVC, WIN32
        unsigned long idx;

        if (b & 0xffffffff)
        {
            _BitScanForward(&idx, int32_t(b));
            return Square(idx);
        }
        else
        {
            _BitScanForward(&idx, int32_t(b >> 32));
            return Square(idx + 32);
        }
    #endif
#else  // Compiler is neither GCC nor MSVC compatible
    #error "Compiler not supported."
#endif
    }

    // Returns the most significant bit in a non-zero bitboard.
    inline Square msb(Bitboard b) {
        assert(b);

#if defined(__GNUC__)  // GCC, Clang, ICX

        return Square(63 ^ __builtin_clzll(b));

#elif defined(_MSC_VER)
    #ifdef _WIN64  // MSVC, WIN64

        unsigned long idx;
        _BitScanReverse64(&idx, b);
        return Square(idx);

    #else  // MSVC, WIN32

        unsigned long idx;

        if (b >> 32)
        {
            _BitScanReverse(&idx, int32_t(b >> 32));
            return Square(idx + 32);
        }
        else
        {
            _BitScanReverse(&idx, int32_t(b));
            return Square(idx);
        }
    #endif
#else  // Compiler is neither GCC nor MSVC compatible
    #error "Compiler not supported."
#endif
    }

    // Returns the bitboard of the least significant
    // square of a non-zero bitboard. It is equivalent to square_bb(lsb(bb)).
    inline Bitboard least_significant_square_bb(Bitboard b) {
        assert(b);
        return b & -b;
    }

    // Finds and clears the least significant bit in a non-zero bitboard.
    inline Square pop_lsb(Bitboard& b) {
        assert(b);
        const Square s = lsb(b);
        b &= b - 1;
        return s;
    }

    // Returns a bitboard representing an entire line (from board edge
    // to board edge) that intersects the two given squares. If the given squares
    // are not on a same file/rank/diagonal, the function returns 0. For instance,
    // line_bb(SQ_C4, SQ_F7) will return a bitboard with the A2-G8 diagonal.
    inline Bitboard line_bb(Square s1, Square s2) {

        assert(is_ok(s1) && is_ok(s2));
        return LineBB[s1][s2];
    }

    // Returns a bitboard representing the squares in the semi-open
    // segment between the squares s1 and s2 (excluding s1 but including s2). If the
    // given squares are not on a same file/rank/diagonal, it returns s2. For instance,
    // between_bb(SQ_C4, SQ_F7) will return a bitboard with squares D5, E6 and F7, but
    // between_bb(SQ_E6, SQ_F8) will return a bitboard with the square F8. This trick
    // allows to generate non-king evasion moves faster: the defending piece must either
    // interpose itself to cover the check or capture the checking piece.
    inline Bitboard between_bb(Square s1, Square s2) {

        assert(is_ok(s1) && is_ok(s2));
        return BetweenBB[s1][s2];
    }

    // Returns true if the squares s1, s2 and s3 are aligned either on a
    // straight or on a diagonal line.
    inline bool aligned(Square s1, Square s2, Square s3) { return line_bb(s1, s2) & s3; }

    // distance() functions return the distance between x and y, defined as the
    // number of steps for a king in x to reach y.


    inline int distance_file(Square x, Square y) { return std::abs(file_of(x) - file_of(y)); }

    inline int distance_rank(Square x, Square y) { return std::abs(rank_of(x) - rank_of(y)); }

    inline int distance_square(Square x, Square y) { return SquareDistance[x][y]; }

    Bitboard RookTable[0x19000];   // To store rook attacks
    Bitboard BishopTable[0x1480];  // To store bishop attacks

    inline int edge_distance(File f) { return std::min(f, File(FILE_H - f)); }

    // Returns the pseudo attacks of the given piece type
    // assuming an empty board.
    template<PieceType Pt>
    inline Bitboard attacks_bb(Square s) {

        assert((Pt != PAWN) && (is_ok(s)));
        return PseudoAttacks[Pt][s];
    }

    // Returns the attacks by the given piece
    // assuming the board is occupied according to the passed Bitboard.
    // Sliding piece attacks do not continue passed an occupied square.
    template<PieceType Pt>
    inline Bitboard attacks_bb(Square s, Bitboard occupied) {

        assert((Pt != PAWN) && (is_ok(s)));

        switch (Pt)
        {
        case ROOK :
            return RookMagics[s].attacks[RookMagics[s].index(occupied)];
        default :
            return PseudoAttacks[Pt][s];
        }
    }

    // Returns the attacks by the given piece
    // assuming the board is occupied according to the passed Bitboard.
    // Sliding piece attacks do not continue passed an occupied square.
    inline Bitboard attacks_bb(PieceType pt, Square s, Bitboard occupied) {

        assert((pt != PAWN) && (is_ok(s)));

        switch (pt)
        {
        case ROOK :
            return attacks_bb<ROOK>(s, occupied);
        default :
            return PseudoAttacks[pt][s];
        }
    }

    // Returns the bitboard of target square for the given step
    // from the given square. If the step is off the board, returns empty bitboard.
    Bitboard safe_destination(Square s, int step) {
        Square to = Square(s + step);
        return is_ok(to) && distance_square(s, to) <= 2 ? square_bb(to) : Bitboard(0);
    }
    constexpr static bool Is64Bit = true;
    constexpr static bool HasPext = false;

    class PRNG {

        uint64_t s;

        uint64_t rand64() {

            s ^= s >> 12, s ^= s << 25, s ^= s >> 27;
            return s * 2685821657736338717LL;
        }

       public:
        PRNG(uint64_t seed) :
            s(seed) {
            assert(seed);
        }

        template<typename T>
        T rand() {
            return T(rand64());
        }

        // Special generator used to fast init magic numbers.
        // Output values only have 1/8th of their bits set on average.
        template<typename T>
        T sparse_rand() {
            return T(rand64() & rand64() & rand64());
        }
    };
#if defined(USE_PEXT)
    #include <immintrin.h>  // Header for _pext_u64() intrinsic
    #define pext(b, m) _pext_u64(b, m)
#else
    #define pext(b, m) 0
#endif
    Bitboard sliding_attack(Square sq, Bitboard occupied) {

        Bitboard  attacks           = 0;
        Direction RookDirections[4] = {NORTH, SOUTH, EAST, WEST};

        for (Direction d : RookDirections)
        {
            Square s = sq;
            while (safe_destination(s, d))
            {
                attacks |= (s += d);
                if (occupied & s)
                {
                    break;
                }
            }
        }

        return attacks;
    }
    // Computes all rook and bishop attacks at startup. Magic
    // bitboards are used to look up attacks of sliding pieces. As a reference see
    // www.chessprogramming.org/Magic_Bitboards. In particular, here we use the so
    // called "fancy" approach.
    void init_magics(Bitboard table[], Magic magics[]) {

        // Optimal PRNG seeds to pick the correct magics in the shortest time
        int seeds[][RANK_NB] = {{8977, 44560, 54343, 38998, 5731, 95205, 104912, 17020},
                                {728, 10316, 55013, 32803, 12281, 15100, 16645, 255}};

        Bitboard occupancy[4096], reference[4096], edges, b;
        int      epoch[4096] = {}, cnt = 0, size = 0;

        for (Square s = SQ_A1; s <= SQ_H8; ++s)
        {
            // Board edges are not considered in the relevant occupancies
            edges = ((Rank1BB | Rank8BB) & ~rank_bb(s)) | ((FileABB | FileHBB) & ~file_bb(s));

            // Given a square 's', the mask is the bitboard of sliding attacks from
            // 's' computed on an empty board. The index must be big enough to contain
            // all the attacks for each possible subset of the mask and so is 2 power
            // the number of 1s of the mask. Hence we deduce the size of the shift to
            // apply to the 64 or 32 bits word to get the index.
            Magic& m = magics[s];
            m.mask   = sliding_attack(s, 0) & ~edges;
            m.shift  = (Is64Bit ? 64 : 32) - popcount(m.mask);

            // Set the offset for the attacks table of the square. We have individual
            // table sizes for each square with "Fancy Magic Bitboards".
            m.attacks = s == SQ_A1 ? table : magics[s - 1].attacks + size;

            // Use Carry-Rippler trick to enumerate all subsets of masks[s] and
            // store the corresponding sliding attack bitboard in reference[].
            b = size = 0;
            do
            {
                occupancy[size] = b;
                reference[size] = sliding_attack(s, b);

                if (HasPext)
                    m.attacks[pext(b, m.mask)] = reference[size];

                size++;
                b = (b - m.mask) & m.mask;
            } while (b);

            if (HasPext)
                continue;

            PRNG rng(seeds[Is64Bit][rank_of(s)]);

            // Find a magic for square 's' picking up an (almost) random number
            // until we find the one that passes the verification test.
            for (int i = 0; i < size;)
            {
                for (m.magic = 0; popcount((m.magic * m.mask) >> 56) < 6;)
                    m.magic = rng.sparse_rand<Bitboard>();

                // A good magic must map every possible occupancy to an index that
                // looks up the correct sliding attack in the attacks[s] database.
                // Note that we build up the database for square 's' as a side
                // effect of verifying the magic. Keep track of the attempt count
                // and save it in epoch[], little speed-up trick to avoid resetting
                // m.attacks[] after every failed attempt.
                for (++cnt, i = 0; i < size; ++i)
                {
                    unsigned idx = m.index(occupancy[i]);

                    if (epoch[idx] < cnt)
                    {
                        epoch[idx]     = cnt;
                        m.attacks[idx] = reference[i];
                    }
                    else if (m.attacks[idx] != reference[i])
                        break;
                }
            }
        }
    }
    void init() {
        for (unsigned i = 0; i < (1 << 16); ++i)
            PopCnt16[i] = uint8_t(std::bitset<16>(i).count());

        for (Square s1 = SQ_A1; s1 <= SQ_H8; ++s1)
            for (Square s2 = SQ_A1; s2 <= SQ_H8; ++s2)
                SquareDistance[s1][s2] = std::max(distance_file(s1, s2), distance_rank(s1, s2));

        init_magics(RookTable, RookMagics);

        for (Square s1 = SQ_A1; s1 <= SQ_H8; ++s1)
        {
            PawnAttacks[WHITE][s1] = pawn_attacks_bb<WHITE>(square_bb(s1));
            PawnAttacks[BLACK][s1] = pawn_attacks_bb<BLACK>(square_bb(s1));

            for (int step : {-9, -8, -7, -1, 1, 7, 8, 9})
                PseudoAttacks[KING][s1] |= safe_destination(s1, step);

            for (int step : {-17, -15, -10, -6, 6, 10, 15, 17})
                PseudoAttacks[KNIGHT][s1] |= safe_destination(s1, step);

            for (int step : {-18, -14, 14, 18})
                PseudoAttacks[BISHOP][s1] |= safe_destination(s1, step);

            for (int step : {-9, -7, 7, 9})
                PseudoAttacks[QUEEN][s1] |= safe_destination(s1, step);

            for (PieceType pt : {BISHOP, ROOK})
                for (Square s2 = SQ_A1; s2 <= SQ_H8; ++s2)
                {
                    if (PseudoAttacks[pt][s1] & s2)
                    {
                        LineBB[s1][s2] = (attacks_bb(pt, s1, 0) & attacks_bb(pt, s2, 0)) | s1 | s2;
                        BetweenBB[s1][s2] =
                          (attacks_bb(pt, s1, square_bb(s2)) & attacks_bb(pt, s2, square_bb(s1)));
                    }
                    BetweenBB[s1][s2] |= s2;
                }
        }
    }

    void put(Color color, Piece p, Square sq) {
        if (!is_ok(sq))
        {
            std::cerr << "Error: trying to put a piece to an invalid square" << square_to_string(sq)
                      << std::endl;
            return;
        }
        // check if the place is empty
        if (pieces[color][p] & (1ULL << sq))
        {
            std::cerr << "Error: trying to put a piece to an occupied square"
                      << square_to_string(sq) << std::endl;
            return;
        }
        pieces[color][p] |= 1ULL << sq;
    }

    void remove(Color color, Piece p, Square sq) {
        if (!is_ok(sq))
        {
            std::cerr << "Error: trying to remove a piece from an invalid square"
                      << square_to_string(sq) << std::endl;
            return;
        }
        // check if the piece is actually there
        if (!(pieces[color][p] & (1ULL << sq)))
        {
            std::cerr << "Error: trying to remove a piece that is not there" << square_to_string(sq)
                      << std::endl;
            return;
        }
        pieces[color][p] &= ~(1ULL << sq);
    }

    void move(Color color, Piece p, Square from, Square to) {
        remove(color, p, from);
        put(color, p, to);
    }

    bool have_piece_in_square(Color color, Square sq) {
        bool ret = false;
        for (int piecetypeitr = 0; piecetypeitr < PIECE_NB; piecetypeitr++)
        {
            if (pieces[color][piecetypeitr] & (1ULL << sq))
            {
                ret = true;
                break;
            }
        }
        return ret;
    }

    bool have_piece_in_square(Square sq) {
        bool ret = false;
        for (int coloritr = 0; coloritr < COLOR_NB; coloritr++)
        {
            for (int piecetypeitr = 0; piecetypeitr < PIECE_NB; piecetypeitr++)
            {
                if (pieces[coloritr][piecetypeitr] & (1ULL << sq))
                {
                    ret = true;
                    break;
                }
            }
            if (ret)
            {
                break;
            }
        }
        return ret;
    }

    BoardWithBitboardRepresentation() { init(); }

    static void dump_bitboard_as_one_zero(const std::string& title, const Bitboard& bb) {
        std::cout << title << std::endl;

        std::cout << bb << std::endl;

        // print column numbers
        std::cout << "    ";
        for (int x = 0; x < 8; x++)
            std::cout << char(x + 'a') << " ";
        std::cout << std::endl;
        for (int y = 7; y >= 0; y--)
        {
            std::cout << " " << y + 1 << "  ";
            for (int x = 0; x < 8; x++)
            {
                if (bb & (1ULL << (y * 8 + x)))
                    std::cout << 1 << " ";
                else
                    std::cout << 0 << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    void dump_pseudo_attacks() {

        for (Square s2 = SQ_A1; s2 <= SQ_H8; ++s2)
        {
            for (PieceType pt : {PieceType::PAWN, PieceType::ROOK, PieceType::BISHOP,
                                 PieceType::KNIGHT, PieceType::QUEEN, PieceType::KING})
            {
                if (pt != PieceType::PAWN)
                    dump_bitboard_as_one_zero("PseudoAttacks "
                                                + std::string(piece_type_to_string(pt)) + " "
                                                + std::string(square_to_string(s2)) + " "
                                                + std::to_string(attacks_bb(pt, s2)),
                                              attacks_bb(pt, s2));
                else
                {
                    dump_bitboard_as_one_zero("PseudoAttacks WHITE "
                                                + std::string(piece_type_to_string(pt)) + " "
                                                + std::string(square_to_string(s2)) + " "
                                                + std::to_string(attacks_bb(pt, s2, WHITE)),
                                              attacks_bb(pt, s2, WHITE));
                    dump_bitboard_as_one_zero("PseudoAttacks BLACK "
                                                + std::string(piece_type_to_string(pt)) + " "
                                                + std::string(square_to_string(s2)) + " "
                                                + std::to_string(attacks_bb(pt, s2, BLACK)),
                                              attacks_bb(pt, s2, BLACK));
                }
            }
            std::cout << std::endl;
        }
    }


    Bitboard attacks_bb(PieceType pt, Square sq, Color color = WHITE, Bitboard occ = 0) {
        switch (pt)
        {
        case PAWN :
            return PawnAttacks[color][sq];
        case KNIGHT :
        case BISHOP :
        case QUEEN :
        case KING :
            return PseudoAttacks[pt][sq];
        case ROOK :
            return attacks_bb<ROOK>(sq, occ);
        case PIECE_TYPE_NB :
        case NO_PIECE_TYPE :
        default :
            return 0;
        }
    }


   private:
    uint8_t PopCnt16[1 << 16]                    = {};
    uint8_t SquareDistance[SQUARE_NB][SQUARE_NB] = {};

    Bitboard LineBB[SQUARE_NB][SQUARE_NB]            = {};
    Bitboard BetweenBB[SQUARE_NB][SQUARE_NB]         = {};
    Bitboard PseudoAttacks[PIECE_TYPE_NB][SQUARE_NB] = {};
    Bitboard PawnAttacks[COLOR_NB][SQUARE_NB]        = {};

    Magic RookMagics[SQUARE_NB] = {};


    Bitboard pieces[COLOR_NB][PIECE_NB] = {};
};

}  // namespace stockfish_bitboard
}  // namespace shatranj
