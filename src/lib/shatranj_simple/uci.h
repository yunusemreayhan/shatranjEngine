#pragma once

#include "shatranj.h"
#include <string>
#include <vector>
#include <chrono>

namespace shatranj {

class UCI {
public:
    UCI();
    void run();
    
private:
    void handle_uci();
    void handle_isready();
    void handle_ucinewgame();
    void handle_position(const std::vector<std::string>& tokens);
    void handle_go(const std::vector<std::string>& tokens);
    void handle_stop();
    void handle_quit();
    
    std::vector<std::string> split(const std::string& str);
    void send_bestmove(const Movement& move);
    void send_info(int depth, int score, int nodes, int time_ms, const std::string& pv = "");
    
    Shatranj engine_;
    bool searching_;
    std::chrono::milliseconds search_time_;
};

} // namespace shatranj