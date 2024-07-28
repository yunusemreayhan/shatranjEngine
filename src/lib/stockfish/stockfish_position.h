#pragma once

#include <memory>
#include <deque>

#include "types.h"
#include "bitboard.h"

namespace Stockfish {

struct StateInfo {

    // Copied when making a move
    Key   materialKey;
    Key   pawnKey;
    Value nonPawnMaterial[COLOR_NB];
    int   castlingRights;
    int   rule50;
    int   pliesFromNull;
    // Square epSquare;

    // Not copied when making a move (will be recomputed anyhow)
    Key        key;
    Bitboard   checkersBB;
    StateInfo* previous;
    Bitboard   blockersForKing[COLOR_NB];
    Bitboard   pinners[COLOR_NB];
    Bitboard   checkSquares[PIECE_TYPE_NB];
    Piece      capturedPiece;
    int        repetition;

    // Used by NNUE
    //Eval::NNUE::Accumulator<Eval::NNUE::TransformedFeatureDimensionsBig>   accumulatorBig;
    //Eval::NNUE::Accumulator<Eval::NNUE::TransformedFeatureDimensionsSmall> accumulatorSmall;
    DirtyPiece dirtyPiece;
};


// A list to keep track of the position states along the setup moves (from the
// start position to the position just before the search starts). Needed by
// 'draw by repetition' detection. Use a std::deque because pointers to
// elements are not invalidated upon list resizing.
using StateListPtr = std::unique_ptr<std::deque<StateInfo>>;

// Position class stores information regarding the board representation as
// pieces, side to move, hash keys, castling info, etc. Important methods are
// do_move() and undo_move(), used by the search to update node info when
// traversing the search tree.
class Position {
   public:
    static void init();
    // FEN string input/output
    Position&
    set(const std::string& fenStr, /*bool isChess960,*/ StateInfo* si, bool shatranj = false);
    Position&   set(const std::string& code, Color c, StateInfo* si, bool shatranj = false);
    std::string fen(bool shatranj = false) const;

    void do_move(Move m, StateInfo& newSt);
    void do_move(Move m, StateInfo& newSt, bool givesCheck);
    void undo_move(Move m);

    // Static Exchange Evaluation
    bool see_ge(Move m, int threshold = 0) const;

    Piece piece_on(Square s) const;
    void  remove_piece(Square s);
    void  move_piece(Square from, Square to);
    bool  gives_check(Move m) const;
    void  put_piece(Piece pc, Square s);

    // Attacks to/from a given square
    Bitboard attackers_to(Square s) const;
    Bitboard attackers_to(Square s, Bitboard occupied) const;
    void     update_slider_blockers(Color c) const;
    template<PieceType Pt>
    Bitboard attacks_by(Color c) const;

    // Position representation
    Bitboard pieces(PieceType pt = ALL_PIECES) const;
    template<typename... PieceTypes>
    Bitboard pieces(PieceType pt, PieceTypes... pts) const;
    Bitboard pieces(Color c) const;
    template<typename... PieceTypes>
    Bitboard pieces(Color c, PieceTypes... pts) const;
    bool     capture(Move m) const;
    bool     capture_stage(Move m) const;

    Piece    moved_piece(Move m) const;
    Bitboard check_squares(PieceType pt) const;
    Bitboard pinners(Color c) const;

    template<PieceType Pt>
    int count(Color c) const;
    template<PieceType Pt>
    int  count() const;
    bool pos_is_ok() const;
    void set_state() const;
    void set_check_info() const;
    bool empty(Square s) const;

    Bitboard checkers() const;
    bool     legal(Move m) const;
    bool     pseudo_legal(const Move m) const;

    Color side_to_move() const;

    Bitboard blockers_for_king(Color c) const;

    Key key() const;
    Key key_after(Move m) const;
    Key material_key() const;
    Key pawn_key() const;

    template<bool AfterMove>
    Key adjust_key50(Key k) const;

    template<PieceType Pt>
    Square square(Color c) const;

    // Data members
    Piece    board[SQUARE_NB];
    Bitboard byTypeBB[PIECE_TYPE_NB];
    Bitboard byColorBB[COLOR_NB];
    int      pieceCount[PIECE_NB];
    // int        castlingRightsMask[SQUARE_NB];
    // Square     castlingRookSquare[CASTLING_RIGHT_NB];
    // Bitboard   castlingPath[CASTLING_RIGHT_NB];
    StateInfo* st;
    int        gamePly;
    Color      sideToMove;
};

inline bool Position::capture(Move m) const {
    assert(m.is_ok());
    return (!empty(m.to_sq()) /* && m.type_of() != CASTLING */) /* || m.type_of() == EN_PASSANT */;
}

// Returns true if a move is generated from the capture stage, having also
// queen promotions covered, i.e. consistency with the capture stage move generation
// is needed to avoid the generation of duplicate moves.
inline bool Position::capture_stage(Move m) const {
    assert(m.is_ok());
    return capture(m) || m.promotion_type() == QUEEN;
}

inline void Position::do_move(Move m, StateInfo& newSt) { do_move(m, newSt, gives_check(m)); }

std::ostream& operator<<(std::ostream& os, const Position& pos);

inline Key Position::key() const { return adjust_key50<false>(st->key); }

inline Key Position::pawn_key() const { return st->pawnKey; }

inline Key Position::material_key() const { return st->materialKey; }

template<bool AfterMove>
inline Key Position::adjust_key50(Key k) const {
    return st->rule50 < 14 - AfterMove ? k : k ^ make_key((st->rule50 - (14 - AfterMove)) / 8);
}

inline Color    Position::side_to_move() const { return sideToMove; }
inline Bitboard Position::checkers() const { return st->checkersBB; }

inline Piece Position::piece_on(Square s) const {
    assert(is_ok(s));
    return board[s];
}
inline void Position::remove_piece(Square s) {

    Piece pc = board[s];
    byTypeBB[ALL_PIECES] ^= s;
    byTypeBB[type_of(pc)] ^= s;
    byColorBB[color_of(pc)] ^= s;
    board[s] = NO_PIECE;
    pieceCount[pc]--;
    pieceCount[make_piece(color_of(pc), ALL_PIECES)]--;
}

inline void Position::move_piece(Square from, Square to) {

    Piece    pc     = board[from];
    Bitboard fromTo = from | to;
    byTypeBB[ALL_PIECES] ^= fromTo;
    byTypeBB[type_of(pc)] ^= fromTo;
    byColorBB[color_of(pc)] ^= fromTo;
    board[from] = NO_PIECE;
    board[to]   = pc;
}

inline void Position::put_piece(Piece pc, Square s) {

    board[s] = pc;
    byTypeBB[ALL_PIECES] |= byTypeBB[type_of(pc)] |= s;
    byColorBB[color_of(pc)] |= s;
    pieceCount[pc]++;
    pieceCount[make_piece(color_of(pc), ALL_PIECES)]++;
}

inline Piece Position::moved_piece(Move m) const { return piece_on(m.from_sq()); }

inline Bitboard Position::pieces(PieceType pt) const { return byTypeBB[pt]; }

template<typename... PieceTypes>
inline Bitboard Position::pieces(PieceType pt, PieceTypes... pts) const {
    return pieces(pt) | pieces(pts...);
}

inline Bitboard Position::pieces(Color c) const { return byColorBB[c]; }

template<typename... PieceTypes>
inline Bitboard Position::pieces(Color c, PieceTypes... pts) const {
    return pieces(c) & pieces(pts...);
}

template<PieceType Pt>
inline int Position::count() const {
    return count<Pt>(WHITE) + count<Pt>(BLACK);
}

template<PieceType Pt>
inline int Position::count(Color c) const {
    return pieceCount[make_piece(c, Pt)];
}

template<PieceType Pt>
inline Square Position::square(Color c) const {
    assert(count<Pt>(c) == 1);
    return lsb(pieces(c, Pt));
}

inline Bitboard Position::attackers_to(Square s) const { return attackers_to(s, pieces()); }

template<PieceType Pt>
inline Bitboard Position::attacks_by(Color c) const {

    if constexpr (Pt == PAWN)
        return c == WHITE ? pawn_attacks_bb<WHITE>(pieces(WHITE, PAWN))
                          : pawn_attacks_bb<BLACK>(pieces(BLACK, PAWN));
    else
    {
        Bitboard threats   = 0;
        Bitboard attackers = pieces(c, Pt);
        while (attackers)
            threats |= attacks_bb<Pt>(pop_lsb(attackers), pieces());
        return threats;
    }
}

inline Bitboard Position::check_squares(PieceType pt) const { return st->checkSquares[pt]; }

inline Bitboard Position::blockers_for_king(Color c) const { return st->blockersForKing[c]; }

inline Bitboard Position::pinners(Color c) const { return st->pinners[c]; }
}
