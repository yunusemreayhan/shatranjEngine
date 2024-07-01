#pragma once

#include <map>
#include <queue>
#include <string>
#include <vector>

namespace shatranj
{
inline std::vector<std::string> SplitStringByChar(const std::string &str, char forsplit)
{
    std::vector<std::string> ret;
    std::string::size_type start = 0;
    std::string::size_type end = 0;

    while ((end = str.find(forsplit, start)) != std::string::npos)
    {
        ret.push_back(str.substr(start, end - start));
        start = end + 1;
    }
    ret.push_back(str.substr(start));
    return ret;
}

template <typename FunctionType> struct DeferedCall
{

    DeferedCall(FunctionType function) : function_(function)
    {
    }

    ~DeferedCall()
    {
        function_();
    }

    FunctionType function_;
};

template <typename TypeOfKey, typename TypeOfHold> class KeyBasedMemory
{
  public:
    bool Have(const TypeOfKey &key)
    {
        return moves_.find(key) != moves_.end();
    }

    void Add(const TypeOfKey &key, const TypeOfHold &&moves)
    {
        moves_[key] = moves;
        if (moves_.size() > maxkeyCount_)
        {
            moves_.erase(keys_.front());
            keys_.pop();
        }
        keys_.push(key);
    }
    void Add(const TypeOfKey &key, const TypeOfHold &moves)
    {
        moves_[key] = moves;
        if (moves_.size() > maxkeyCount_)
        {
            moves_.erase(keys_.front());
            keys_.pop();
        }
        keys_.push(key);
    }

    const TypeOfHold &Get(const TypeOfKey &key)
    {
        return moves_.at(key);
    }

  private:
    const size_t maxkeyCount_ = 1000;
    std::queue<TypeOfKey> keys_;
    std::map<TypeOfKey, TypeOfHold> moves_;
};

} // namespace shatranj