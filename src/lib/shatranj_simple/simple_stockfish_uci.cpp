#include "simple_stockfish_uci.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace shatranj {

SimpleStockfishUCI::SimpleStockfishUCI() 
    : searching_(false), hash_size_mb_(16) {}

void SimpleStockfishUCI::run() {
    std::string line;
    while (std::getline(std::cin, line)) {
        auto tokens = split(line);
        if (tokens.empty()) continue;
        
        const std::string& command = tokens[0];
        
        if (command == "uci") {
            handle_uci();
        } else if (command == "isready") {
            handle_isready();
        } else if (command == "ucinewgame") {
            handle_ucinewgame();
        } else if (command == "position") {
            handle_position(tokens);
        } else if (command == "go") {
            handle_go(tokens);
        } else if (command == "stop") {
            handle_stop();
        } else if (command == "setoption") {
            handle_setoption(tokens);
        } else if (command == "quit") {
            handle_quit();
            break;
        }
    }
}

void SimpleStockfishUCI::handle_uci() {
    std::cout << "id name ShatranjEngine-Simple 1.0" << std::endl;
    std::cout << "id author ShatranjEngine Team" << std::endl;
    std::cout << "option name Hash type spin default 16 min 1 max 1024" << std::endl;
    std::cout << "option name Threads type spin default 1 min 1 max 1" << std::endl;
    std::cout << "uciok" << std::endl;
}

void SimpleStockfishUCI::handle_isready() {
    std::cout << "readyok" << std::endl;
}

void SimpleStockfishUCI::handle_ucinewgame() {
    engine_ = Shatranj();
}

void SimpleStockfishUCI::handle_setoption(const std::vector<std::string>& tokens) {
    if (tokens.size() >= 5 && tokens[1] == "name" && tokens[3] == "value") {
        const std::string& name = tokens[2];
        const std::string& value = tokens[4];
        
        if (name == "Hash") {
            hash_size_mb_ = std::stoi(value);
        }
    }
}

void SimpleStockfishUCI::handle_position(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) return;
    
    if (tokens[1] == "startpos") {
        engine_ = Shatranj();
        
        // Handle moves if present
        auto moves_it = std::find(tokens.begin(), tokens.end(), "moves");
        if (moves_it != tokens.end()) {
            for (auto it = moves_it + 1; it != tokens.end(); ++it) {
                if (it->length() == 4) {
                    engine_.Play(*it);
                }
            }
        }
    }
}

void SimpleStockfishUCI::handle_go(const std::vector<std::string>& tokens) {
    searching_ = true;
    
    // Parse time controls
    int depth = 6;
    std::chrono::milliseconds movetime(5000);
    
    for (size_t i = 1; i < tokens.size(); i++) {
        if (tokens[i] == "depth" && i + 1 < tokens.size()) {
            depth = std::stoi(tokens[i + 1]);
        } else if (tokens[i] == "movetime" && i + 1 < tokens.size()) {
            movetime = std::chrono::milliseconds(std::stoi(tokens[i + 1]));
        }
    }
    
    // Use simple Shatranj search
    auto start_time = std::chrono::steady_clock::now();
    
    int nodes_visited = 0;
    std::chrono::microseconds duration;
    
    auto best_move = engine_.PickMoveInBoard(depth, &nodes_visited, &duration);
    auto end_time = std::chrono::steady_clock::now();
    int elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    
    if (best_move) {
        send_info(depth, 0, nodes_visited, elapsed_ms, best_move->ToString());
        send_bestmove(*best_move);
    } else {
        send_bestmove(Movement::GetEmpty());
    }
    
    searching_ = false;
}

void SimpleStockfishUCI::handle_stop() {
    searching_ = false;
}

void SimpleStockfishUCI::handle_quit() {
    searching_ = false;
}

std::vector<std::string> SimpleStockfishUCI::split(const std::string& str) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

void SimpleStockfishUCI::send_bestmove(const Movement& move) {
    if (move.from == Position(0, 0) && move.to == Position(0, 0)) {
        std::cout << "bestmove (none)" << std::endl;
    } else {
        std::cout << "bestmove " << move.ToString() << std::endl;
    }
}

void SimpleStockfishUCI::send_info(int depth, int score, int nodes, int time_ms, const std::string& pv) {
    std::cout << "info depth " << depth 
              << " score cp " << score
              << " nodes " << nodes
              << " time " << time_ms;
    if (!pv.empty()) {
        std::cout << " pv " << pv;
    }
    std::cout << std::endl;
}

} // namespace shatranj