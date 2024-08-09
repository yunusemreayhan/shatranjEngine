#pragma once

#include "types.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <iomanip>

using json = nlohmann::json;

struct fen_move_couple {
    std::string fen;
    std::string move;
    double      calculationtime_s = 0.0;
};
class JsonExporter {
    std::string path;
    std::string filename;
    json        j;

   public:
    JsonExporter(std::string title, std::string path, size_t p1level, size_t p2level) :
        path(path) {
        // filename {p1level}level_{p2level}level_{day}_{month}_{year}__{hour}_{minute}.json
        filename = title + "_" + std::to_string(p1level) + "level_" + std::to_string(p2level)
                 + "level_" + gettimtestamp() + ".json";
        j["created"]     = gettimtestamp();
        j["filename"]    = filename;
        j["p1level"]     = p1level;
        j["p2level"]     = p2level;
        j["gameName"]    = title;
        j["result"]      = "0-1";
        j["game"]        = "chess";
        j["gameDesc"]    = "";
        j["gameVariant"] = "shatranj";
        j["questionNo"]  = "1.1.6.4 Problem 5";
        j["book"]        = "Libro del acedrex";
        j["pageCell"]    = "187";
        j["event"]       = "Problem 5";
        j["title"]       = title;
        j["site"]        = "";
        j["date"]        = gettimtestamp();
        j["white"]       = "";
        j["black"]       = "";
        j["eco"]         = "";
        j["round"]       = "";
        j["timeControl"] = "";
        j["whiteClock"]  = "";
        j["blackClock"]  = "";
        j["clock"]       = "";
        j["moveList"]    = json::array();
    }

    std::string gettimtestamp() {
        std::time_t t   = std::time(0);  // get time now
        std::tm*    now = std::localtime(&t);
        return std::to_string(now->tm_mday) + "_" + std::to_string(now->tm_mon) + "_"
             + std::to_string(now->tm_year + 1900) + "__" + std::to_string(now->tm_hour) + "_"
             + std::to_string(now->tm_min);
    }

    void set_winner(Stockfish::Color winner) {
        if (winner == Stockfish::Color::WHITE)
            j["result"] = "1-0";
        else if (winner == Stockfish::Color::BLACK)
            j["result"] = "0-1";
        else
            j["result"] = "1/2-1/2";
    }

    void add_move(fen_move_couple              fenmove,
                  std::vector<fen_move_couple> children    = {},
                  std::string                  description = "") {
        j["moveList"].push_back({{"FEN", fenmove.fen},
                                 {"move", fenmove.move},
                                 {"calculationtime_s", fenmove.calculationtime_s},
                                 {"description", description}});
        for (auto c : children)
        {
            j["moveList"].back()["children"].push_back({{"FEN", c.fen},
                                                        {"move", c.move},
                                                        {"calculationtime_s", c.calculationtime_s},
                                                        {"description", description}});
        }
    }

    void write() {
        // write prettified JSON to another file
        j["lastUpdated"] = gettimtestamp();
        std::ofstream o(path + "/" + filename);
        o << std::setw(4) << j << std::endl;
    }
    ~JsonExporter() { write(); }
};
