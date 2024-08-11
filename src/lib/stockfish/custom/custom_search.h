#pragma once

#include "../types.h"
#include "../tt.h"
#include "../stockfish_position.h"
#include "custommovepicker.h"
#include "evaluate.h"
#include "../movegen.h"
#include <cstddef>
#include <deque>
#include <limits>
#include <string>
#include "pv_manager.h"
#include "../stockfish_helper.h"
#include <atomic>

namespace Stockfish {

struct Stack {
    int   moveCount;
    int   ply;
    Move  move;
    Value eval;
    Move* pv;
    Value staticEval;
};

enum SearchRunType {
    Root,
    PV,
    NonPV
};

struct RootMove {

    explicit RootMove(Move m) :
        pv(1, m) {}
    bool extract_ponder_from_tt(const TranspositionTable& tt, Position& pos);
    bool operator==(const Move& m) const { return pv[0] == m; }
    // Sort in descending order
    bool operator<(const RootMove& m) const {
        return m.score != score ? m.score < score : m.previousScore < previousScore;
    }

    uint64_t          effort          = 0;
    Value             score           = -VALUE_INFINITE;
    Value             previousScore   = -VALUE_INFINITE;
    Value             averageScore    = -VALUE_INFINITE;
    Value             uciScore        = -VALUE_INFINITE;
    bool              scoreLowerbound = false;
    bool              scoreUpperbound = false;
    int               selDepth        = 0;
    int               tbRank          = 0;
    Value             tbScore;
    std::vector<Move> pv;
};

using RootMoves = std::vector<RootMove>;
class search {
    // Sort moves in descending order up to and including
    // a given limit. The order of moves smaller than the limit is left unspecified.
    inline void partial_insertion_sort(ExtMove* begin, ExtMove* end, int limit) {

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

    Value value_draw(size_t nodes) { return VALUE_DRAW - 1 + Value(nodes & 0x2); }

    template<SearchRunType nodeType>
    Value negmax(Stack* ss, int depth, Value alpha, Value beta, bool cutNode = true);

    template<SearchRunType nodeType>
    Value qnegmax(Stack* ss, Value alpha, Value beta);

    TTData GetFromTT() {
        auto [ttHit, ttData, ttWriter] = m_tt->probe(m_pos.key());

        return ttData;
    }

   public:
    void dump_root_moves() {
        for (auto& rm : rootMoves)
        {
            std::cout << rm.pv[0] << " " << rm.score << " -> ";
            for (auto& pvm : rm.pv)
            {
                std::cout << pvm << ", ";
            }
            std::cout << std::endl;
        }
    }
    Move iterative_deepening(int d = 20);

    search(TranspositionTable* tt, Position& pos) :
        m_tt(tt),
        m_pos(pos) {}

    void update_pv(Move* pv, Move move, const Move* childPv) {
        for (*pv++ = move; childPv && *childPv != Move::none();)
            *pv++ = *childPv++;
        *pv = Move::none();
    }

   private:
    const static int    MAX_PLY = 500;
    TranspositionTable* m_tt;
    Position&           m_pos;
    PVManager2          pv_manager2;
    RootMoves           rootMoves;
    size_t              multiPV = 1;
    Value               rootDelta;
    Depth               rootDepth;

    long pvrun    = 0;
    long nonpvrun = 0;
    long rootrun  = 0;
    long qrun     = 0;

    std::atomic<uint64_t> nodes, tbHits, bestMoveChanges;
    int                   delta;
    size_t                pvIdx, pvLast;
};
}
