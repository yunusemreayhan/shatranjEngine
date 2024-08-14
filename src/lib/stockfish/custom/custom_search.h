#pragma once

#include "../types.h"
#include "../tt.h"
#include "../stockfish_position.h"
#include "custommovepicker.h"
#include "evaluate.h"
#include "../movegen.h"
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <future>
#include <limits>
#include <mutex>
#include <string>
#include "pv_manager.h"
#include "../stockfish_helper.h"
#include <atomic>
#include <thread>

namespace Stockfish {

struct Stack {
    int   moveCount;
    int   ply;
    Move  move;
    Value eval;
    Move* pv;
    Value staticEval;
    bool  improving;
    bool  ttHit;
    Move  ttMove;
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

template<bool enabled = false>
class TimeChecker {
   public:
    TimeChecker(std::atomic<bool>&        stopper_flag_inp,
                std::chrono::milliseconds t = std::chrono::milliseconds(0)) :
        stopper_flag(stopper_flag_inp) {
        if constexpr (enabled)
        {
            std::async(std::launch::async, [&]() {
                // Use sleep_for to wait specified time (or sleep_until).
                std::this_thread::sleep_for(t);
                // Do whatever you want.
                stopper_flag.store(true);
            });

            deadline = std::chrono::system_clock::now() + t;
        }
    }

    inline bool IsTimeUp() {
        if constexpr (enabled)
        {
            if (deadline < std::chrono::system_clock::now())
                return true;
        }
        return false;
    }

   private:
    std::chrono::time_point<std::chrono::system_clock> deadline;
    std::atomic<bool>&                                 stopper_flag;
};

using RootMoves = std::vector<RootMove>;

template<bool HaveTimeOut = false>
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

    void dump_root_moves() {
        for (auto& rm : rootMoves)
        {
            /* if (rm.score == -VALUE_INFINITE)
                continue; */

            std::cout << rm.pv[0] << " " << rm.score << " -> ";
            for (auto& pvm : rm.pv)
            {
                std::cout << pvm << ", ";
            }
            std::cout << std::endl;
        }
    }
    Move picked_move() { return rootMoves[0].pv[0]; }
    Move iterative_deepening(int d = 20);


    void update_pv(Move* pv, Move move, const Move* childPv) {
        for (*pv++ = move; childPv && *childPv != Move::none();)
            *pv++ = *childPv++;
        *pv = Move::none();
    }

   public:
    search(TranspositionTable*       tt,
           Position&                 pos,
           std::chrono::milliseconds t = std::chrono::milliseconds(0)) :
        m_tt(tt),
        m_pos(pos),
        stopflag(false),
        timeChecker(stopflag, t) {}

    void start_parallel_root(int d = 20) {
        cv.wait(mutex, [&]() { return busy.load() == 0; });
        rootThread = std::thread([&]() {
            std::lock_guard<std::mutex> lock(mutex);
            stopflag = false;
            busy     = true;
            this->iterative_deepening(d);
            busy = false;
            cv.notify_all();
        });
    }

    void stop() { stopflag = true; }

   private:
    const static int    MAX_PLY = 500;
    TranspositionTable* m_tt;
    Position&           m_pos;
    PVManager2          pv_manager2;
    RootMoves           rootMoves;
    size_t              multiPV = 1;
    Value               rootDelta;
    Depth               rootDepth;
    std::thread         rootThread;

    long pvrun    = 0;
    long nonpvrun = 0;
    long rootrun  = 0;
    long qrun     = 0;

    std::atomic<uint64_t>    nodes, tbHits, bestMoveChanges;
    int                      delta;
    size_t                   pvIdx, pvLast;
    std::atomic<bool>        stopflag = false, busy = false;
    TimeChecker<HaveTimeOut> timeChecker;
    std::condition_variable  cv;
    std::mutex               mutex;
};
}
