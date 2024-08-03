#pragma once

#include "../stockfish_position.h"
#include <ostream>

using namespace Stockfish;

struct pv_tree {

    std::vector<ExtMove> moves = {};

    bool operator<(const pv_tree& other) const {
        for (size_t i = 0; i < moves.size(); i++)
        {
            if (moves[i].value != other.moves[i].value)
                return moves[i].value < other.moves[i].value;
        }
        return false;
    }

    bool operator==(const pv_tree& other) const {
        if (moves.size() != other.moves.size())
            return false;
        else
        {
            for (size_t i = 0; i < moves.size(); i++)
            {
                if (moves[i].from_sq() != other.moves[i].from_sq()
                    && moves[i].to_sq() != other.moves[i].to_sq())
                {
                    std::cout << "moves[" << i << "] : " << moves[i] << " != " << other.moves[i]
                              << std::endl;
                    return false;
                }
            }
        }
        return true;
    }

    void clear() { moves.clear(); }

    void dump(std::string title, size_t till) {
        std::cout << title << " : ";
        for (size_t i = 0; i <= till; i++)
        {
            if (moves[i] != Move::none())
            {
                std::cout << moves[i] << " ";
            }
            else
            {
                std::cout << "none" << " ";
            }
        }
    }

    void set(size_t index, ExtMove move) {
        if (moves.size() > index)
        {
            moves[index] = move;
        }
        else
        {
            moves.push_back(move);
        }
    }
};

class PVManager {
   public:
    PVManager(const size_t max_pv_tree_item = 4) :
        MAX_PV_TREE_ITEM(max_pv_tree_item) {}
    bool insert(pv_tree& tree) {
        insert_or_replace(tree);
        if (pvs.size() > MAX_PV_TREE_ITEM)
            pvs.erase(pvs.rbegin()->first);
        return true;
    }

    std::map<ExtMove, pv_tree>::const_iterator find_move(Move move) const {
        for (auto itr = pvs.begin(); itr != pvs.end(); ++itr)
        {
            if (static_cast<Move>(itr->first) == move)
                return itr;
        }
        return pvs.end();
    }

    void insert_or_replace(pv_tree& tree) {
        auto oldone = find_move(tree.moves[0]);

        if (oldone != pvs.end())
        {
            if (oldone->second.moves.size() > tree.moves.size())
                return;
            else if (oldone->second.moves.size() == tree.moves.size())
            {
                if (oldone->second.moves[0].value >= tree.moves[0].value)
                    return;
            }
            pvs.erase(oldone);
        }
        pvs.insert({tree.moves[0], tree});
        dump();
    }

    int index(Move move, size_t depth) {
        int index = 0;
        for (auto it = pvs.begin(); it != pvs.end(); ++it)
        {
            if (it->second.moves.size() > depth)
                if (it->second.moves[depth] == move)
                    return index;
            index++;
        }
        return -1;
    }

    void dump() {
        std::cout << "pvs : " << pvs.size() << " ";
        int i = 0;
        for (auto it = pvs.begin(); it != pvs.end(); ++it)
        {
            std::cout << std::endl << ++i << ". ";
            for (auto& it2 : it->second.moves)
            {
                if (it2 == Move::none())
                    std::cout << " none " << it2.value << ", ";
                else
                    std::cout << it2 << " " << it2.value << ", ";
            }
            std::cout << std::endl;
        }
    }

   private:
    std::map<ExtMove, pv_tree> pvs;
    PVManager(){};
    const size_t MAX_PV_TREE_ITEM = 4;
};
