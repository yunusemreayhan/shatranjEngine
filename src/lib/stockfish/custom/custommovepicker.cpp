#include "custommovepicker.h"

namespace Stockfish {

int MoveSorter::DetermineEvasionType(Position& pos, Move& m, const bool debug) {
    if (debug)
        std::cout << "determining evasion type " << m << std::endl;
    Piece     pc = pos.moved_piece(m);
    PieceType pt = type_of(pc);
    // Square    from = m.from_sq();
    Square to = m.to_sq();

    Piece targetPosPiece = pos.piece_on(to);

    int retscore = EVASION_COEFFICIENT;
    // evasion capturing
    if (targetPosPiece != NO_PIECE)
    {
        retscore += 5 * move_arround;
        if (pt == KING)
        {
            retscore += -move_arround;
        }
        else
        {
            retscore += +move_arround + capturing_offset(pt, type_of(targetPosPiece));
        }
    }
    // evasion moving king
    else if (pt == KING)
    {
        // todo reveal threading move away
        retscore += -2 * move_arround;
    }
    else  // evasion blocking by other piece
    {
        auto [p_protected, protectingPt, p_capturable, capturingPt] =
          pos.IsMoveToAProtectedPosition(m);
        if (p_protected && p_capturable)
        {
            retscore += 3 * move_arround + capturing_offset(capturingPt, protectingPt);
        }
        else if (!p_protected && p_capturable)
        {
            retscore += -3 * move_arround;
        }
        else if (p_protected && !p_capturable)  // not possible most probably
        {
            retscore += +3 * move_arround;
        }
        else
        {
            // not possible most probably
            retscore += +2 * move_arround;
        }
    }
    if (pos.gives_check(m))
        retscore += CHECK_COEFFICIENT;
    return retscore;
}

int MoveSorter::DetermineCaptureType(Position& pos, Move& m, const bool debug) {
    if (debug)
        std::cout << "determining capture type " << m << std::endl;
    Piece     capturingPiece     = pos.moved_piece(m);
    PieceType capturingPieceType = type_of(capturingPiece);
    // Square    from               = m.from_sq();
    // Square    to                = m.to_sq();
    Piece     capturedPiece     = pos.piece_on(m.to_sq());
    PieceType capturedPieceType = type_of(capturedPiece);

    auto [p_protected, protectingPt, pos_counterCapturable, counterCapturingPieceType] =
      pos.IsMoveToAProtectedPosition(m);

    int retscore = CAPTURE_COEFFICIENT;
    if (!p_protected)
    {
        if (!pos_counterCapturable)
        {
            if (debug)
                std::cout << "not protected and not counter capturable " << m << std::endl;
            retscore = +CAPTURE_COEFFICIENT;
            if (PieceValue[capturingPieceType] > PieceValue[capturedPieceType])
            {
                if (capturingPieceType == ROOK)
                    retscore -= 14 * move_arround;
                else if (capturingPieceType == BISHOP || capturingPieceType == QUEEN
                         || capturingPieceType == KNIGHT)
                    retscore -= 8 * move_arround;
                else
                    retscore -= 3 * move_arround;
            }
            else
            {
                if (capturedPieceType == ROOK)
                    retscore += 14 * move_arround;
                else if (capturedPieceType == BISHOP || capturedPieceType == QUEEN
                         || capturedPieceType == KNIGHT)
                    retscore += 8 * move_arround;
                else
                    retscore += 3 * move_arround;
            }
        }
        else if (pos_counterCapturable)
        {
            if (debug)
                std::cout << "not protected and counter capturable, losing capture" << m
                          << std::endl;
            if (PieceValue[capturingPieceType] > PieceValue[capturedPieceType])
            {
                retscore = -CAPTURE_COEFFICIENT;

                if (PieceValue[counterCapturingPieceType] > PieceValue[capturingPieceType])
                {
                    retscore += move_arround;
                }
                else
                {
                    retscore -= move_arround;
                }
            }
            else if (PieceValue[capturingPieceType] == PieceValue[capturedPieceType])
            {
                retscore = +CAPTURE_COEFFICIENT;
            }
            else
            {
                retscore = +CAPTURE_COEFFICIENT + 13 * move_arround;
            }
        }
    }
    else if (p_protected)
    {
        if (!pos_counterCapturable)
        {
            if (debug)
                std::cout << "protected and not counter capturable " << m << std::endl;
            retscore = +CAPTURE_COEFFICIENT + 13 * move_arround;
            if (capturedPieceType == ROOK)
                retscore += 14 * move_arround;
            else if (capturedPieceType == BISHOP || capturedPieceType == QUEEN
                     || capturedPieceType == KNIGHT)
                retscore += 8 * move_arround;
            else
                retscore += 3 * move_arround;
        }
        else if (pos_counterCapturable)
        {
            if (debug)
                std::cout << "protected and counter capturable " << m << std::endl;
            if (PieceValue[capturingPieceType] > PieceValue[capturedPieceType])
            {
                retscore = -CAPTURE_COEFFICIENT;
                if (PieceValue[counterCapturingPieceType] > PieceValue[capturingPieceType])
                {
                    retscore += move_arround;
                }
                else
                {
                    retscore -= move_arround;
                }
            }
            else if (PieceValue[capturingPieceType] == PieceValue[capturedPieceType])
            {
                retscore = +CAPTURE_COEFFICIENT + 3 * move_arround;
            }
            else
            {
                retscore = +CAPTURE_COEFFICIENT + 13 * move_arround;
            }
        }
    }

    if (pos.gives_check(m))
        retscore += CHECK_COEFFICIENT;

    return retscore;
}

int MoveSorter::DetermineQuiteType(Position& pos, Move& m, const bool debug) {
    if (debug)
        std::cout << "determining quite type " << m << std::endl;
    Piece     movingPiece     = pos.moved_piece(m);
    PieceType movingPieceType = type_of(movingPiece);
    // Square    from            = m.from_sq();
    // Square to = m.to_sq();

    Bitboard oppAttacksToTo =
      pos.attackers_to(m.to_sq(), pos.pieces()) & pos.pieces(~pos.side_to_move());
    Bitboard oppAttackToFrom =
      pos.attackers_to(m.from_sq(), pos.pieces()) & pos.pieces(~pos.side_to_move());

    auto [p_protected, protectingPt, pos_counterCapturable, counterCapturingPieceType] =
      pos.IsMoveToAProtectedPosition(m);

    int retscore = QUIET_COEFFICIENT;

    if (p_protected)
    {
        if (pos_counterCapturable)
        {
            if (PieceValue[counterCapturingPieceType] < PieceValue[movingPieceType])
            {
                retscore = -QUIET_COEFFICIENT;
            }
            else
            {
                retscore = +QUIET_COEFFICIENT;
            }
        }
        else
        {
            retscore = +QUIET_COEFFICIENT + 2 * move_arround;
        }
    }
    else
    {
        if (pos_counterCapturable)
        {
            if (PieceValue[counterCapturingPieceType] < PieceValue[movingPieceType])
            {
                retscore = -QUIET_COEFFICIENT - 10 * move_arround;
            }
            else
            {
                retscore = -QUIET_COEFFICIENT - 8 * move_arround;
            }
        }
        else
        {
            retscore = QUIET_COEFFICIENT;
        }
    }

    if (oppAttackToFrom != 0)
    {
        if (oppAttacksToTo != 0)
        {
            if (movingPieceType == ROOK)
                retscore -= 14 * move_arround;
            else if (movingPieceType == BISHOP || movingPieceType == QUEEN
                     || movingPieceType == KNIGHT)
                retscore -= 8 * move_arround;
            else
                retscore -= 3 * move_arround;
        }
        else
        {
            if (movingPieceType == ROOK)
                retscore += 14 * move_arround;
            else if (movingPieceType == BISHOP || movingPieceType == QUEEN
                     || movingPieceType == KNIGHT)
                retscore += 8 * move_arround;
            else
                retscore += 3 * move_arround;
        }
    }
    else
    {
        if (oppAttacksToTo != 0)
        {
            if (movingPieceType == ROOK)
                retscore -= 14 * move_arround;
            else if (movingPieceType == BISHOP || movingPieceType == QUEEN
                     || movingPieceType == KNIGHT)
                retscore -= 8 * move_arround;
            else
                retscore -= 3 * move_arround;
        }
        else
        {
            if (movingPieceType == ROOK)
                retscore += 14 * move_arround;
            else if (movingPieceType == BISHOP || movingPieceType == QUEEN
                     || movingPieceType == KNIGHT)
                retscore += 8 * move_arround;
            else
                retscore += 3 * move_arround;
        }
    }

    if (pos.gives_check(m))
        retscore += CHECK_COEFFICIENT;

    return retscore;
}

int MoveSorter::DetermineScore(Position& pos, Move& m, Move& ttm) {
    if (m == ttm)
    {
        return HASH_COEFFICIENT;
    }
    if (pos.checkers())
    {
        return DetermineEvasionType(pos, m);
    }
    else if (pos.capture_stage(m))
    {
        return DetermineCaptureType(pos, m);
    }
    else
    {
        return DetermineQuiteType(pos, m);
    }
}

}
