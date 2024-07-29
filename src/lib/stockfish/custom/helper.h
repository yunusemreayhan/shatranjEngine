#pragma once

#include <cstddef>
#include <map>
#include <queue>
#include <chrono>
#include <ratio>

template<typename ftype>
long long timeit_us(ftype func) {
    auto start = std::chrono::steady_clock::now();
    func();
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

template<typename TRollbackFunc>
struct RollbackerRAII {

    TRollbackFunc t;

    RollbackerRAII(TRollbackFunc t) :
        t(t) {}

    ~RollbackerRAII() { t(); }
};

template<typename TypeOfKey, typename TypeOfHold>
class KeyBasedMemory {
   public:
    bool Have(const TypeOfKey& key) { return moves_.find(key) != moves_.end(); }

    void Add(const TypeOfKey& key, const TypeOfHold& moves) {
        moves_[key] = moves;
        if (moves_.size() > maxkeyCount_)
        {
            moves_.erase(keys_.front());
            keys_.pop();
        }
        keys_.push(key);
    }

    const TypeOfHold& Get(const TypeOfKey& key) { return moves_.at(key); }

   private:
    const size_t                    maxkeyCount_ = 10000000;
    std::queue<TypeOfKey>           keys_;
    std::map<TypeOfKey, TypeOfHold> moves_;
};
