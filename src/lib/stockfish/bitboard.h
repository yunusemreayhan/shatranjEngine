#pragma once

#include <bitset>
#include <cassert>
#include <cstdint>
#include <ctype.h>
#include <iostream>
#include <string_view>

#include "types.h"
#include "helper.h"

namespace Stockfish {

namespace Bitboards {

void        init();
std::string pretty(Bitboard b);

}  // namespace Stockfish::Bitboards

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

extern uint8_t PopCnt16[1 << 16];
extern uint8_t SquareDistance[SQUARE_NB][SQUARE_NB];

extern Bitboard BetweenBB[SQUARE_NB][SQUARE_NB];
extern Bitboard LineBB[SQUARE_NB][SQUARE_NB];
extern Bitboard PseudoAttacks[PIECE_TYPE_NB][SQUARE_NB];
extern Bitboard PawnAttacks[COLOR_NB][SQUARE_NB];


// Magic holds all magic bitboards relevant data for a single square
struct Magic {
    Bitboard  mask;
    Bitboard  magic;
    Bitboard* attacks;
    unsigned  shift;

    // Compute the attack's index using the 'magic bitboards' approach
    unsigned index(Bitboard occupied) const {

        if (HasPext)
            return unsigned(pext(occupied, mask));

        if (Is64Bit)
            return unsigned(((occupied & mask) * magic) >> shift);

        unsigned lo = unsigned(occupied) & unsigned(mask);
        unsigned hi = unsigned(occupied >> 32) & unsigned(mask >> 32);
        return (lo * unsigned(magic) ^ hi * unsigned(magic >> 32)) >> shift;
    }
};

extern Magic RookMagics[SQUARE_NB];
extern Magic BishopMagics[SQUARE_NB];

constexpr Bitboard square_bb(Square s) {
    assert(is_ok(s));
    return (1ULL << s);
}


// Overloads of bitwise operators between a Bitboard and a Square for testing
// whether a given bit is set in a bitboard, and for setting and clearing bits.

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


// Moves a bitboard one or two steps as specified by the direction D
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

class StockfishPrecomputationTables {

   public:
    // Returns the squares attacked by pawns of the given color
    // from the squares in the given bitboard.
    template<Color C>
    static constexpr Bitboard pawn_attacks_bb(Bitboard b) {
        return C == WHITE ? shift<NORTH_WEST>(b) | shift<NORTH_EAST>(b)
                          : shift<SOUTH_WEST>(b) | shift<SOUTH_EAST>(b);
    }

    static inline Bitboard pawn_attacks_bb(Color c, Square s) {

        assert(is_ok(s));
        return PawnAttacks[c][s];
    }
    static inline int popcount(Bitboard b) {

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


    static inline int distance_file(Square x, Square y) {
        return std::abs(file_of(x) - file_of(y));
    }

    static inline int distance_rank(Square x, Square y) {
        return std::abs(rank_of(x) - rank_of(y));
    }

    static inline int distance_square(Square x, Square y) { return SquareDistance[x][y]; }

    inline int edge_distance(File f) { return std::min(f, File(FILE_H - f)); }

    // Returns the pseudo attacks of the given piece type
    // assuming an empty board.
    template<PieceType Pt>
    static inline Bitboard attacks_bb(Square s) {

        assert((Pt != PAWN) && (is_ok(s)));
        return PseudoAttacks[Pt][s];
    }

    // Returns the attacks by the given piece
    // assuming the board is occupied according to the passed Bitboard.
    // Sliding piece attacks do not continue passed an occupied square.
    template<PieceType Pt>
    static inline Bitboard attacks_bb(Square s, Bitboard occupied) {

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
    static inline Bitboard attacks_bb(PieceType pt, Square s, Bitboard occupied) {

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
    static Bitboard safe_destination(Square s, int step) {
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
    static Bitboard sliding_attack(Square sq, Bitboard occupied) {

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
    static void init_magics(Bitboard table[], Magic magics[]) {

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

    static inline bool initialized = false;
    static void        init() {
        if (initialized)
            return;

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
        initialized = true;
    }

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

    static void dump_pseudo_attacks() {

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


    static Bitboard attacks_bb(PieceType pt, Square sq, Color color = WHITE, Bitboard occ = 0) {
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
    static inline uint8_t PopCnt16[1 << 16]                    = {};
    static inline uint8_t SquareDistance[SQUARE_NB][SQUARE_NB] = {};

    static inline Bitboard LineBB[SQUARE_NB][SQUARE_NB]            = {};
    static inline Bitboard BetweenBB[SQUARE_NB][SQUARE_NB]         = {};
    static inline Bitboard PseudoAttacks[PIECE_TYPE_NB][SQUARE_NB] = {};
    static inline Bitboard PawnAttacks[COLOR_NB][SQUARE_NB]        = {};

    static inline Magic    RookMagics[SQUARE_NB] = {};
    static inline Bitboard RookTable[0x19000];  // To store rook attacks
};

}  // namespace Stockfish
