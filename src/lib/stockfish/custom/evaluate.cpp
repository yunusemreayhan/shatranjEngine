#include "evaluate.h"
#include <limits>

namespace Stockfish {

double evaluate(Stockfish::Position pos, Color currentColor, Color maximColor, size_t movecount) {
    double ret = pos.count<PAWN>(maximColor) * PawnValue;
    ret += pos.count<QUEEN>(maximColor) * QueenValue;
    ret += pos.count<KNIGHT>(maximColor) * KnightValue;
    ret += pos.count<BISHOP>(maximColor) * BishopValue;
    ret += pos.count<ROOK>(maximColor) * RookValue;

    ret -= pos.count<PAWN>(~maximColor) * PawnValue;
    ret -= pos.count<QUEEN>(~maximColor) * QueenValue;
    ret -= pos.count<KNIGHT>(~maximColor) * KnightValue;
    ret -= pos.count<BISHOP>(~maximColor) * BishopValue;
    ret -= pos.count<ROOK>(~maximColor) * RookValue;

    if (currentColor == maximColor)
    {
        ret += movecount * 0.1;
        if (movecount == 0)
            ret = std::numeric_limits<double>::min();
    }
    else
    {
        ret -= movecount * 0.1;
        if (movecount == 0)
            ret = std::numeric_limits<double>::max();
    }

    return ret;
}

}
