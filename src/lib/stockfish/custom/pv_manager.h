#pragma once

#include "../stockfish_position.h"
#include <ostream>

using namespace Stockfish;

struct MoveWithValue {
    Move  move;
    Value value;

    bool operator<(const MoveWithValue& other) const { return move < other.move; }
};

inline std::ostream& operator<<(std::ostream& os, const MoveWithValue& move) {
    os << square_to_string(move.move.from_sq()) << "-" << square_to_string(move.move.to_sq());
    return os;
}

class PVManager2 {
   public:
    PVManager2() {}

    void insert_or_replace(MoveWithValue move) {
        auto itr = rootMovesSet.find(move);
        if (itr != rootMovesSet.end())
        {
            rootMovesSet2.erase(*itr);
            rootMovesSet.erase(*itr);
        }
        rootMovesSet2.insert(move);
        rootMovesSet.insert(move);
    }

    void dump() {
        for (auto it = rootMovesSet2.begin(); it != rootMovesSet2.end(); ++it)
        {
            std::cout << "value : " << it->value << " move : " << *it
                      << ", item count : " << rootMoves[it->move].size() << " : ";
            for (auto& move : rootMoves[it->move])
            {
                std::cout << move << " ";
            }
            std::cout << std::endl;
        }
    }

    std::set<MoveWithValue>::const_iterator         begin() const { return rootMovesSet2.begin(); }
    std::set<MoveWithValue>::const_reverse_iterator rbegin() const {
        return rootMovesSet2.rbegin();
    }
    std::set<MoveWithValue>::const_iterator         end() const { return rootMovesSet2.end(); }
    std::set<MoveWithValue>::const_reverse_iterator rend() const { return rootMovesSet2.rend(); }

    size_t size() const { return rootMovesSet.size(); }

    std::vector<Move>& rootPvMoves(Move key) {
        if (rootMoves.count(key) == 0)
            rootMoves[key] = {key};
        return rootMoves[key];
    }

   private:
    std::map<Move, std::vector<Move>> rootMoves;

    struct MoveWithValueCompareByMove {
        bool operator()(const MoveWithValue& lhs, const MoveWithValue& rhs) const {
            return lhs.move < rhs.move;
        }
    };
    std::set<MoveWithValue, MoveWithValueCompareByMove> rootMovesSet;
    struct MoveWithValueCompareByValue {
        bool operator()(const MoveWithValue& lhs, const MoveWithValue& rhs) const {
            return lhs.value < rhs.value || (lhs.value == rhs.value && lhs.move < rhs.move);
        }
    };
    std::multiset<MoveWithValue, MoveWithValueCompareByValue> rootMovesSet2;
};
