#include "position.h"
#include <cstring>

namespace Stockfish {

namespace Zobrist {

Key psq[PIECE_NB][SQUARE_NB];
//Key enpassant[FILE_NB];
//Key castling[CASTLING_RIGHT_NB];
Key side, noPawns;
}

namespace {

constexpr std::string_view PieceToChar(" PNBRQK  pnbrqk");

constexpr Piece Pieces[] = {W_PAWN, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
                            B_PAWN, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING};
}  // namespace

// Makes a move, and saves all information necessary
// to a StateInfo object. The move is assumed to be legal. Pseudo-legal
// moves should be filtered out before this function is called.
void Position::do_move(Move m, StateInfo& newSt, bool givesCheck) {

    assert(m.is_ok());
    assert(&newSt != st);

    Key k = st->key ^ Zobrist::side;

    // Copy some fields of the old state to our new StateInfo object except the
    // ones which are going to be recalculated from scratch anyway and then switch
    // our state pointer to point to the new (ready to be updated) state.
    std::memcpy(&newSt, st, offsetof(StateInfo, key));
    newSt.previous = st;
    st             = &newSt;

    // Increment ply counters. In particular, rule50 will be reset to zero later on
    // in case of a capture or a pawn move.
    ++gamePly;
    ++st->rule50;
    ++st->pliesFromNull;

    // Used by NNUE
    /*st->accumulatorBig.computed[WHITE]     = st->accumulatorBig.computed[BLACK] =
      st->accumulatorSmall.computed[WHITE] = st->accumulatorSmall.computed[BLACK] = false;*/

    auto& dp     = st->dirtyPiece;
    dp.dirty_num = 1;

    Color  us       = sideToMove;
    Color  them     = ~us;
    Square from     = m.from_sq();
    Square to       = m.to_sq();
    Piece  pc       = piece_on(from);
    Piece  captured = /*m.type_of() == EN_PASSANT ? make_piece(them, PAWN) :*/ piece_on(to);

    assert(color_of(pc) == us);
    assert(captured == NO_PIECE
           || color_of(captured)
                == (/*m.type_of() != CASTLING ?*/ them
                    /*:  us*/));
    assert(type_of(captured) != KING);

    /*if (m.type_of() == CASTLING)
    {
        assert(pc == make_piece(us, KING));
        assert(captured == make_piece(us, ROOK));

        Square rfrom, rto;
        do_castling<true>(us, from, to, rfrom, rto);

        k ^= Zobrist::psq[captured][rfrom] ^ Zobrist::psq[captured][rto];
        captured = NO_PIECE;
    }*/

    if (captured)
    {
        Square capsq = to;

        // If the captured piece is a pawn, update pawn hash key, otherwise
        // update non-pawn material.
        if (type_of(captured) == PAWN)
        {
            /*if (m.type_of() == EN_PASSANT)
            {
                capsq -= pawn_push(us);

                assert(pc == make_piece(us, PAWN));
                assert(to == st->epSquare);
                assert(relative_rank(us, to) == RANK_6);
                assert(piece_on(to) == NO_PIECE);
                assert(piece_on(capsq) == make_piece(them, PAWN));
            }*/

            st->pawnKey ^= Zobrist::psq[captured][capsq];
        }
        else
            st->nonPawnMaterial[them] -= PieceValue[captured];

        dp.dirty_num = 2;  // 1 piece moved, 1 piece captured
        dp.piece[1]  = captured;
        dp.from[1]   = capsq;
        dp.to[1]     = SQ_NONE;

        // Update board and piece lists
        remove_piece(capsq);

        k ^= Zobrist::psq[captured][capsq];
        st->materialKey ^= Zobrist::psq[captured][pieceCount[captured]];

        // Reset rule 50 counter
        st->rule50 = 0;
    }

    // Update hash key
    k ^= Zobrist::psq[pc][from] ^ Zobrist::psq[pc][to];

    // Reset en passant square
    /*if (st->epSquare != SQ_NONE)
    {
        k ^= Zobrist::enpassant[file_of(st->epSquare)];
        st->epSquare = SQ_NONE;
    }

    // Update castling rights if needed
    if (st->castlingRights && (castlingRightsMask[from] | castlingRightsMask[to]))
    {
        k ^= Zobrist::castling[st->castlingRights];
        st->castlingRights &= ~(castlingRightsMask[from] | castlingRightsMask[to]);
        k ^= Zobrist::castling[st->castlingRights];
    }*/

    // Move the piece. The tricky Chess960 castling is handled earlier
    /*if (m.type_of() != CASTLING)
    {
        dp.piece[0] = pc;
        dp.from[0]  = from;
        dp.to[0]    = to;

        move_piece(from, to);
    }*/

    // If the moving piece is a pawn do some special extra work
    if (type_of(pc) == PAWN)
    {
        // Set en passant square if the moved pawn can be captured
        /*if ((int(to) ^ int(from)) == 16
            && (pawn_attacks_bb(us, to - pawn_push(us)) & pieces(them, PAWN)))
        {
            st->epSquare = to - pawn_push(us);
            k ^= Zobrist::enpassant[file_of(st->epSquare)];
        }

        else*/
        if (m.type_of() == PROMOTION)
        {
            Piece promotion = make_piece(us, m.promotion_type());

            assert(relative_rank(us, to) == RANK_8);
            assert(type_of(promotion) >= KNIGHT && type_of(promotion) <= QUEEN);

            remove_piece(to);
            put_piece(promotion, to);

            // Promoting pawn to SQ_NONE, promoted piece from SQ_NONE
            dp.to[0]               = SQ_NONE;
            dp.piece[dp.dirty_num] = promotion;
            dp.from[dp.dirty_num]  = SQ_NONE;
            dp.to[dp.dirty_num]    = to;
            dp.dirty_num++;

            // Update hash keys
            k ^= Zobrist::psq[pc][to] ^ Zobrist::psq[promotion][to];
            st->pawnKey ^= Zobrist::psq[pc][to];
            st->materialKey ^=
              Zobrist::psq[promotion][pieceCount[promotion] - 1] ^ Zobrist::psq[pc][pieceCount[pc]];

            // Update material
            st->nonPawnMaterial[us] += PieceValue[promotion];
        }

        // Update pawn hash key
        st->pawnKey ^= Zobrist::psq[pc][from] ^ Zobrist::psq[pc][to];

        // Reset rule 50 draw counter
        st->rule50 = 0;
    }

    // Set capture piece
    st->capturedPiece = captured;

    // Update the key with the final value
    st->key = k;

    // Calculate checkers bitboard (if move gives check)
    st->checkersBB = givesCheck ? attackers_to(square<KING>(them)) & pieces(us) : 0;

    sideToMove = ~sideToMove;

    // Update king attacks used for fast check detection
    set_check_info();

    // Calculate the repetition info. It is the ply distance from the previous
    // occurrence of the same position, negative in the 3-fold case, or zero
    // if the position was not repeated.
    st->repetition = 0;
    int end        = std::min(st->rule50, st->pliesFromNull);
    if (end >= 4)
    {
        StateInfo* stp = st->previous->previous;
        for (int i = 4; i <= end; i += 2)
        {
            stp = stp->previous->previous;
            if (stp->key == st->key)
            {
                st->repetition = stp->repetition ? -i : i;
                break;
            }
        }
    }

    assert(pos_is_ok());
}


// Unmakes a move. When it returns, the position should
// be restored to exactly the same state as before the move was made.
void Position::undo_move(Move m) {

    assert(m.is_ok());

    sideToMove = ~sideToMove;

    Color  us   = sideToMove;
    Square from = m.from_sq();
    Square to   = m.to_sq();
    Piece  pc   = piece_on(to);

    assert(empty(from) /*|| m.type_of() == CASTLING*/);
    assert(type_of(st->capturedPiece) != KING);

    if (m.type_of() == PROMOTION)
    {
        assert(relative_rank(us, to) == RANK_8);
        assert(type_of(pc) == m.promotion_type());
        assert(type_of(pc) >= KNIGHT && type_of(pc) <= QUEEN);

        remove_piece(to);
        pc = make_piece(us, PAWN);
        put_piece(pc, to);
    }

    /*if (m.type_of() == CASTLING)
    {
        Square rfrom, rto;
        do_castling<false>(us, from, to, rfrom, rto);
    }
    else
    {*/
    move_piece(to, from);  // Put the piece back at the source square

    if (st->capturedPiece)
    {
        Square capsq = to;

        /*if (m.type_of() == EN_PASSANT)
            {
                capsq -= pawn_push(us);

                assert(type_of(pc) == PAWN);
                assert(to == st->previous->epSquare);
                assert(relative_rank(us, to) == RANK_6);
                assert(piece_on(capsq) == NO_PIECE);
                assert(st->capturedPiece == make_piece(~us, PAWN));
            }*/

        put_piece(st->capturedPiece, capsq);  // Restore the captured piece
    }
    /*}*/

    // Finally point our state pointer back to the previous state
    st = st->previous;
    --gamePly;

    assert(pos_is_ok());
}


template<PieceType Pt>
inline int Position::count(Color c) const {
    return pieceCount[make_piece(c, Pt)];
}

template<PieceType Pt>
inline int Position::count() const {
    return count<Pt>(WHITE) + count<Pt>(BLACK);
}

template<PieceType Pt>
inline Square Position::square(Color c) const {
    assert(count<Pt>(c) == 1);
    return lsb(pieces(c, Pt));
}

inline void Position::do_move(Move m, StateInfo& newSt) { do_move(m, newSt, gives_check(m)); }

// Performs some consistency checks for the position object
// and raise an assert if something wrong is detected.
// This is meant to be helpful when debugging.
bool Position::pos_is_ok() const {

    constexpr bool Fast = true;  // Quick (default) or full check?

    if ((sideToMove != WHITE && sideToMove != BLACK) || piece_on(square<KING>(WHITE)) != W_KING
        || piece_on(square<KING>(BLACK)) != B_KING
        /*|| (ep_square() != SQ_NONE && relative_rank(sideToMove, ep_square()) != RANK_6) */)
        assert(0 && "pos_is_ok: Default");

    if (Fast)
        return true;

    if (pieceCount[W_KING] != 1 || pieceCount[B_KING] != 1
        || attackers_to(square<KING>(~sideToMove)) & pieces(sideToMove))
        assert(0 && "pos_is_ok: Kings");

    if ((pieces(PAWN) & (Rank1BB | Rank8BB)) || pieceCount[W_PAWN] > 8 || pieceCount[B_PAWN] > 8)
        assert(0 && "pos_is_ok: Pawns");

    if ((pieces(WHITE) & pieces(BLACK)) || (pieces(WHITE) | pieces(BLACK)) != pieces()
        || popcount(pieces(WHITE)) > 16 || popcount(pieces(BLACK)) > 16)
        assert(0 && "pos_is_ok: Bitboards");

    for (PieceType p1 = PAWN; p1 <= KING; ++p1)
        for (PieceType p2 = PAWN; p2 <= KING; ++p2)
            if (p1 != p2 && (pieces(p1) & pieces(p2)))
                assert(0 && "pos_is_ok: Bitboards");


    for (Piece pc : Pieces)
        if (pieceCount[pc] != popcount(pieces(color_of(pc), type_of(pc)))
            || pieceCount[pc] != std::count(board, board + SQUARE_NB, pc))
            assert(0 && "pos_is_ok: Pieces");

    /*for (Color c : {WHITE, BLACK})
        for (CastlingRights cr : {c & KING_SIDE, c & QUEEN_SIDE})
        {
            if (!can_castle(cr))
                continue;

            if (piece_on(castlingRookSquare[cr]) != make_piece(c, ROOK)
                || castlingRightsMask[castlingRookSquare[cr]] != cr
                || (castlingRightsMask[square<KING>(c)] & cr) != cr)
                assert(0 && "pos_is_ok: Castling");
        }*/

    return true;
}

// Tests whether a pseudo-legal move gives a check
bool Position::gives_check(Move m) const {

    assert(m.is_ok());
    assert(color_of(moved_piece(m)) == sideToMove);

    Square from = m.from_sq();
    Square to   = m.to_sq();

    // Is there a direct check?
    if (check_squares(type_of(piece_on(from))) & to)
        return true;

    // Is there a discovered check?
    if (blockers_for_king(~sideToMove) & from)
        return !aligned(from, to, square<KING>(~sideToMove)) /*|| m.type_of() == CASTLING*/;

    switch (m.type_of())
    {
    case NORMAL :
        return false;

    case PROMOTION :
        return attacks_bb(m.promotion_type(), to, pieces() ^ from) & square<KING>(~sideToMove);

    // En passant capture with check? We have already handled the case of direct
    // checks and ordinary discovered check, so the only case we need to handle
    // is the unusual case of a discovered check through the captured pawn.
    /*case EN_PASSANT : {
        Square   capsq = make_square(file_of(to), rank_of(from));
        Bitboard b     = (pieces() ^ from ^ capsq) | to;

        return (attacks_bb<ROOK>(square<KING>(~sideToMove), b) & pieces(sideToMove, QUEEN, ROOK))
             | (attacks_bb<BISHOP>(square<KING>(~sideToMove), b)
                & pieces(sideToMove, QUEEN, BISHOP));
    }*/
    default :  //CASTLING
    {          /*
        // Castling is encoded as 'king captures the rook'
        Square rto = relative_square(sideToMove, to > from ? SQ_F1 : SQ_D1);

        return check_squares(ROOK) & rto;
    */
        return false;
    }
    }
}

// Computes a bitboard of all pieces which attack a given square.
// Slider attacks use the occupied bitboard to indicate occupancy.
Bitboard Position::attackers_to(Square s, Bitboard occupied) const {

    return (pawn_attacks_bb(BLACK, s) & pieces(WHITE, PAWN))
         | (pawn_attacks_bb(WHITE, s) & pieces(BLACK, PAWN))
         | (attacks_bb<KNIGHT>(s) & pieces(KNIGHT))
         | (attacks_bb<ROOK>(s, occupied) & pieces(ROOK, QUEEN))
         | (attacks_bb<BISHOP>(s, occupied) & pieces(BISHOP, QUEEN))
         | (attacks_bb<KING>(s) & pieces(KING));
}

// Sets king attacks to detect if a move gives check
void Position::set_check_info() const {

    update_slider_blockers(WHITE);
    update_slider_blockers(BLACK);

    Square ksq = square<KING>(~sideToMove);

    st->checkSquares[PAWN]   = pawn_attacks_bb(~sideToMove, ksq);
    st->checkSquares[KNIGHT] = attacks_bb<KNIGHT>(ksq);
    st->checkSquares[BISHOP] = attacks_bb<BISHOP>(ksq, pieces());
    st->checkSquares[ROOK]   = attacks_bb<ROOK>(ksq, pieces());
    st->checkSquares[QUEEN]  = st->checkSquares[BISHOP] | st->checkSquares[ROOK];
    st->checkSquares[KING]   = 0;
}

// Calculates st->blockersForKing[c] and st->pinners[~c],
// which store respectively the pieces preventing king of color c from being in check
// and the slider pieces of color ~c pinning pieces of color c to the king.
void Position::update_slider_blockers(Color c) const {

    Square ksq = square<KING>(c);

    st->blockersForKing[c] = 0;
    st->pinners[~c]        = 0;

    // Snipers are sliders that attack 's' when a piece and other snipers are removed
    Bitboard snipers = ((attacks_bb<ROOK>(ksq) & pieces(QUEEN, ROOK))
                        | (attacks_bb<BISHOP>(ksq) & pieces(QUEEN, BISHOP)))
                     & pieces(~c);
    Bitboard occupancy = pieces() ^ snipers;

    while (snipers)
    {
        Square   sniperSq = pop_lsb(snipers);
        Bitboard b        = between_bb(ksq, sniperSq) & occupancy;

        if (b && !more_than_one(b))
        {
            st->blockersForKing[c] |= b;
            if (b & pieces(c))
                st->pinners[~c] |= sniperSq;
        }
    }
}
inline bool Position::empty(Square s) const { return piece_on(s) == NO_PIECE; }
}
