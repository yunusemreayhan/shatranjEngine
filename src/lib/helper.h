#pragma once

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
} // namespace shatranj