#include "evaluate.h"
#include <limits>

namespace Stockfish {

int evaluate(Stockfish::Position pos, Color maximColor, size_t movecount) {
    int ret = pos.count<PAWN>(maximColor) * PawnValue;
    ret += pos.count<QUEEN>(maximColor) * QueenValue;
    ret += pos.count<KNIGHT>(maximColor) * KnightValue;
    ret += pos.count<BISHOP>(maximColor) * BishopValue;
    ret += pos.count<ROOK>(maximColor) * RookValue;

    ret -= pos.count<PAWN>(~maximColor) * PawnValue;
    ret -= pos.count<QUEEN>(~maximColor) * QueenValue;
    ret -= pos.count<KNIGHT>(~maximColor) * KnightValue;
    ret -= pos.count<BISHOP>(~maximColor) * BishopValue;
    ret -= pos.count<ROOK>(~maximColor) * RookValue;

    if (pos.side_to_move() == maximColor)
    {
        if (movecount == 0)
            ret = VALUE_INFINITE;
    }
    else
    {
        if (movecount == 0)
            ret = -VALUE_INFINITE;
    }

    return ret;
}
}
