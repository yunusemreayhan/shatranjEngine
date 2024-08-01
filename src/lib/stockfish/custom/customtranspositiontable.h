#pragma once

#include <cstdint>
#include "../stockfish_position.h"
#include "../types.h"

namespace Stockfish {


struct TtEntry {
    int8_t depth;
    int8_t ndepth;
    int    value;
    Move   hashmove;
    int    power() const { return depth - ndepth; }

    bool operator<(const TtEntry& other) const {
        if (power() != other.power())
            return power() < other.power();
        else
            return value < other.value;
    }
};

class TT {
   public:
    void insert(
      uint64_t key, int8_t depth, int8_t ndepth, int value, Move hmove, Color maximizingColor) {
        tt[key].emplace_back(
          TtEntry{depth, ndepth, maximizingColor == WHITE ? value : -value, hmove});
        auto& tosort = tt[key];
        sort(tosort.begin(), tosort.end());
        if (tt[key].size() > 3)
            tt[key].pop_front();
    }

    std::optional<TtEntry> lookup(uint64_t key, int depth, int ndepth, Color maximizingColor) {
        auto it = tt.find(key);
        if (it == tt.end())
            return std::nullopt;
        std::optional<TtEntry> ret = std::nullopt;

        if (it->second.back().power() >= (depth - ndepth))
        {
            ret = it->second.back();
        }
        else
        {
            return std::nullopt;
        }

        if (
          maximizingColor != WHITE
          && ret
               .has_value())  // returning negative for black, since we store white maximization values
            ret->value *= -1;
        return ret;
    }

    std::optional<TtEntry> lookup(uint64_t key, Color maximizingColor) {
        auto it = tt.find(key);
        if (it == tt.end())
            return std::nullopt;
        std::optional<TtEntry> ret = it->second.back();

        if (
          maximizingColor != WHITE
          && ret
               .has_value())  // returning negative for black, since we store white maximization values
            ret->value *= -1;
        return ret;
    }

    template<typename movelisttype>
    void ttordermoves(Position& pos, movelisttype& movelist, Color maximizingColor);
    template<typename movelisttype>
    void static_ordering_moves(Position& pos, movelisttype& movelist);

    Move hash_move(Position& pos) {
        auto it = tt.find(pos.key());
        if (it == tt.end())
            return Move::none();
        return it->second.back().hashmove;
    }

    bool is_hash_move(Position& pos, Move& move, int& index) {
        auto it = tt.find(pos.key());
        if (it == tt.end())
            return false;
        index = 0;
        for (auto ritr = it->second.rbegin(); ritr != it->second.rend(); ++ritr, ++index)
        {
            if (ritr->hashmove == move)
                return true;
        }
        return false;
    }

    int static_eval(Position& pos, Move& m) {
        auto hmove = hash_move(pos);
        if (hmove == m)
        {
            return 4000;
        }

        int eval = 0;
        if (pos.capture_stage(m))  // captures
        {
            eval += 3000;
            eval = int(PieceValue[pos.piece_on(m.to_sq())]) / 2;
        }
        else if (pos.checkers() == 0)  // quiets
        {
            eval += 2000;
            Piece     pc   = pos.moved_piece(m);
            PieceType pt   = type_of(pc);
            Square    from = m.from_sq();
            Square    to   = m.to_sq();

            Color                     us = pos.side_to_move();
            [[maybe_unused]] Bitboard threatenedByPawn, threatenedByMinor, threatenedByRook,
              threatenedPieces;
            threatenedByPawn  = pos.attacks_by<PAWN>(~us);
            threatenedByMinor = pos.attacks_by<KNIGHT>(~us) | pos.attacks_by<BISHOP>(~us)
                              | pos.attacks_by<QUEEN>(~us) | threatenedByPawn;
            threatenedByRook = pos.attacks_by<ROOK>(~us) | threatenedByMinor;

            // Pieces threatened by pieces of lesser material value
            threatenedPieces = (pos.pieces(us, ROOK) & threatenedByMinor)
                             | (pos.pieces(us, KNIGHT, BISHOP, QUEEN) & threatenedByPawn);


            // bonus for checks
            eval += bool(pos.check_squares(pt) & to) * 2250;

            // bonus for escaping from capture
            eval += threatenedPieces & from ? (pt == ROOK && !(to & threatenedByMinor) ? 300
                                               : !(to & threatenedByPawn)              ? 200
                                                                                       : 0)
                                            : 0;

            // malus for putting piece en prise
            eval -=
              (pt == ROOK ? bool(to & threatenedByMinor) * 300 : bool(to & threatenedByPawn) * 200);
        }
        else if (pos.checkers() != 0)  //evasions
        {
            eval = PieceValue[pos.piece_on(m.to_sq())] - type_of(pos.moved_piece(m)) + 5000;
        }
        else
        {
            // we should not reach here
            assert(false);
        }
        return eval;
    }

    void dump(TtEntry& entry, std::string title) {
        std::cout << title << " d:" << int(entry.depth) << " nd:" << int(entry.ndepth)
                  << " v:" << entry.value << std::endl;
    }

    void dump(std::string title, std::optional<uint64_t> key = std::nullopt) {
        std::cout << "=====================" << std::endl;
        std::cout << title << std::endl;
        TtEntry maxdepthe  = {};
        TtEntry maxndepthe = {};
        long    ecount     = 0;
        for (auto& entries : tt)
        {
            for (auto& entry : entries.second)
            {
                ++ecount;
                if (maxdepthe.depth < entry.depth)
                    maxdepthe = entry;
                if (maxndepthe.ndepth < entry.ndepth)
                    maxndepthe = entry;
            }
        }
        dump(maxdepthe, "maxdepth");
        dump(maxndepthe, "maxndepth");
        std::cout << "entries: " << ecount << std::endl;
        std::cout << "=====================" << std::endl;
        if (key && tt.find(*key) != tt.end())
        {
            for (auto& entries : tt.find(*key)->second)
            {
                dump(entries, title + " entry");
            }
        }
        std::cout << "=====================" << std::endl;
    }

    void partial_insertion_sort(ExtMove* begin, ExtMove* end, int limit) {

        for (ExtMove *sortedEnd = begin, *p = begin + 1; p < end; ++p)
            if (p->value >= limit)
            {
                ExtMove tmp = *p, *q;
                *p          = *++sortedEnd;
                for (q = sortedEnd; q != begin && *(q - 1) < tmp; --q)
                    *q = *(q - 1);
                *q = tmp;
            }
    }

   private:
    std::map<uint64_t, std::deque<TtEntry>> tt;
};

template<typename movelisttype>
inline void TT::ttordermoves(Position& pos, movelisttype& movelist, Color maximizingColor) {
    for (auto& m : movelist)
    {
        auto      key          = pos.key_after(m);
        auto      res          = lookup(key, maximizingColor);
        long long tthits       = 0;
        long long visitednodes = 0;
        if (!res)
        {
            minmax(this, pos, m, 2, 0, maximizingColor, std::numeric_limits<int>::min(),
                   std::numeric_limits<int>::max(), tthits, visitednodes, false, true, false);
            res = lookup(key, maximizingColor);
        }

        m.value = res->value;
    }
    partial_insertion_sort(movelist.begin(), movelist.end(), std::numeric_limits<int>::min());
}


template<typename movelisttype>
inline void TT::static_ordering_moves(Position& pos, movelisttype& movelist) {
    for (auto& m : movelist)
    {
        m.value = static_eval(pos, m);
    }
    partial_insertion_sort(movelist.begin(), movelist.end(), std::numeric_limits<int>::min());
}
}
