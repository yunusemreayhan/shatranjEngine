#pragma once

#include "../types.h"
#include "../stockfish_position.h"
#include "customtranspositiontable.h"

#include "pv_manager.h"
#include "../tt.h"
namespace Stockfish {

class MoveSorterInterface {
   public:
    Value DetermineScore(Position& pos, ExtMove& m) {

        bool evasions = pos.checkers() != 0;
        bool capture  = pos.capture_stage(m);

        if (evasions)
        {
            if (capture)
                m.value =
                  PieceValue[pos.piece_on(m.to_sq())] - PieceValue[type_of(pos.moved_piece(m))];
        }
        else if (capture)
        {
            m.value =
              int(PieceValue[pos.piece_on(m.to_sq())] - PieceValue[type_of(pos.moved_piece(m))]);
        }
        else
        {

            [[maybe_unused]] Bitboard threatenedByPawn, threatenedByMinor, threatenedByRook,
              threatenedPieces;
            Color us = pos.side_to_move();

            threatenedByPawn  = pos.attacks_by<PAWN>(~us);
            threatenedByMinor = pos.attacks_by<QUEEN>(~us) | pos.attacks_by<KNIGHT>(~us)
                              | pos.attacks_by<BISHOP>(~us) | threatenedByPawn;
            threatenedByRook = pos.attacks_by<ROOK>(~us) | threatenedByMinor;

            // Pieces threatened by pieces of lesser material value
            threatenedPieces = (pos.pieces(us, ROOK) & threatenedByMinor)
                             | (pos.pieces(us, KNIGHT, BISHOP, QUEEN) & threatenedByPawn);

            Piece     pc   = pos.moved_piece(m);
            PieceType pt   = type_of(pc);
            Square    from = m.from_sq();
            Square    to   = m.to_sq();

            // bonus for checks
            m.value += bool(pos.check_squares(pt) & to) * 7000;

            // bonus for escaping from capture
            m.value += threatenedPieces & from ? (pt == ROOK && !(to & threatenedByMinor) ? 256
                                                  : !(to & threatenedByPawn)              ? 144
                                                                                          : 0)
                                               : 0;

            // malus for putting piece en prise
            m.value -=
              (pt == ROOK ? bool(to & threatenedByMinor) * 243 : bool(to & threatenedByPawn) * 149);
        }
        return m.value;
    }
};

class MoveSorter {
   protected:
    using offset = int;
    offset capturing_offset(PieceType capturing, PieceType beingcaptured) {
        return PieceValue[beingcaptured] - PieceValue[capturing];
    }

    inline Bitboard GetThreatenPositionsByPawn(Position& pos) {
        return pos.attacks_by<PAWN>(~pos.side_to_move());
    }

    inline Bitboard GetThreatenPositionsByMinor(Position& pos, Bitboard threatenedByPawn = 0) {
        return pos.attacks_by<KNIGHT>(~pos.side_to_move())
             | pos.attacks_by<BISHOP>(~pos.side_to_move())
             | pos.attacks_by<QUEEN>(~pos.side_to_move()) | threatenedByPawn;
    }

    inline Bitboard GetThreatenPositionsByRook(Position& pos, Bitboard threatenedByMinor = 0) {
        return pos.attacks_by<ROOK>(~pos.side_to_move()) | threatenedByMinor;
    }

    inline Bitboard
    ThreatenedPieces(Position& pos, Bitboard threatenedByMinor, Bitboard threatenedByPawn) {
        return (pos.pieces(pos.side_to_move(), ROOK) & threatenedByMinor)
             | (pos.pieces(pos.side_to_move(), KNIGHT, BISHOP, QUEEN) & threatenedByPawn);
    }

    static constexpr int HASH_COEFFICIENT    = 30000;
    static constexpr int CHECK_COEFFICIENT   = 20000;
    static constexpr int EVASION_COEFFICIENT = 8000;
    static constexpr int CAPTURE_COEFFICIENT = 6000;
    static constexpr int QUIET_COEFFICIENT   = 5000;
    static constexpr int move_arround        = 50;

    int DetermineEvasionType(Position& pos, Move& m, const bool debug = false);

    int DetermineCaptureType(Position& pos, Move& m, const bool debug = false);

    int DetermineQuiteType(Position& pos, Move& m, const bool debug = false);

    int DetermineScore(Position& pos, Move& m, Move& ttm);
};

template<GenType genType = LEGAL>
class CustomMovePicker: public MoveSorter {

   public:
    CustomMovePicker(Position& pos, TranspositionTable* tt) :
        m_pos(pos),
        m_tt(tt),
        list(MoveList<genType>(pos)) {
        for (auto& move : list)
        {
            auto [ttHit, ttData, ttWriter] = m_tt->probe(pos.key());
            move.value                     = DetermineScore(pos, move, ttData.move);
        }
        partial_insertion_sort(list.begin(), list.end(), std::numeric_limits<int>::min());
    }

    ExtMove* begin() { return list.begin(); }

    ExtMove* end() { return list.end(); }

    size_t size() { return list.size(); }

    ExtMove* pickfirst() { return list.pickfirst(); }
    ExtMove* picklast() { return list.pickfirst() + (size() - 1); }

    ExtMove* pick() { return list.pick(); }


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
    Position&           m_pos;
    TranspositionTable* m_tt;
    MoveList<genType>   list;
    ExtMove*            m_cur;
};

class CustomMovePickerForQSearch: public MoveSorter {

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

   public:
    CustomMovePickerForQSearch(Position& pos, TranspositionTable* tt) :
        m_tt(tt) {
        if (pos.checkers())
        {
            for (auto& move : MoveList<EVASIONS>(pos))
            {
                if (move.is_ok())
                    moves[i++] = move;
            }
        }
        else
        {
            /* for (auto& move : MoveList<QUIET_CHECKS>(pos))
            {
                if (move.is_ok())
                    moves[i++] = move;
            } */
            for (auto& move : MoveList<CAPTURES>(pos))
            {
                if (move.is_ok())
                    moves[i++] = move;
            }
        }
        for (auto& move : *this)
        {
            auto [ttHit, ttData, ttWriter] = m_tt->probe(pos.key());

            move.value = DetermineScore(pos, move, ttData.move);
        }
        partial_insertion_sort(this->begin(), this->end(), std::numeric_limits<int>::min());
    }

    ExtMove* begin() { return moves; }

    ExtMove* end() { return moves + size(); }

    ExtMove moves[MAX_MOVES];

    int size() { return i; }

   private:
    int                 i             = 0;
    size_t              current_depth = 0;
    TranspositionTable* m_tt;
};
}
