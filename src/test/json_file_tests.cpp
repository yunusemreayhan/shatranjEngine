
#include "custommovepicker.h"
#include "minimax.h"
#include "movegen.h"

#include "gtest/gtest.h"
#include "stockfish_helper.h"
#include "stockfish_position.h"
#include <cstdint>
#include <limits>
#include <sstream>
#include <string>
#include <sys/types.h>

#include "evaluate.h"

#include "search_prototypes.h"
#include "testhelper.h"
#include "tt.h"
#include "types.h"
#include "pesto_evaluate.h"
#include "evaluate.h"

#include <fstream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

using namespace Stockfish;

struct fenwithtype {
    std::string fen;
    bool        shatranj;
};
struct gamepoint {
    fenwithtype fen;
    Move        move;
};
struct problem {
    std::string            problem_fpath;
    std::vector<gamepoint> games;
};

TEST(JsonFileTests, IteratingThroughJsonFiles) {
    std::vector<problem> problems;
    // for file in path
    std::string path = "./src/test/testmetadata/";
    for (auto file : std::filesystem::directory_iterator(path))
    {
        auto& current_problem         = problems.emplace_back();
        current_problem.problem_fpath = file.path();
        std::cout << file.path() << std::endl;
        std::ifstream f(file.path());
        json          data = json::parse(f);
        for (auto& move : data.at("moveList"))
        {
            current_problem.games.push_back({{move.at("FEN"), false}, strToMove(move.at("move"))});
        }
    }
    std::vector<std::string> skipped = {"./src/test/testmetadata/2.json"};
    std::cout << problems.size() << std::endl;
    size_t             successes = 0;
    size_t             fails     = 0;
    TranspositionTable tt;
    tt.resize(2048);
    for (auto& problem : problems)
    {
        if (std::find(skipped.begin(), skipped.end(), problem.problem_fpath) == skipped.end())
        {
            tt.clear();
            auto prev    = *problem.games.begin();
            bool success = true;
            for (auto game = problem.games.begin() + 1; game != problem.games.end(); game++)
            {
                std::stringstream ss;
                if (game->move.is_ok())
                    ss << game->move;
                else
                    ss << "none";
                std::cout << "game.fen : " << game->fen.fen
                          << " game.shatranj : " << game->fen.shatranj << " move : " << ss.str()
                          << std::endl;
                Position  pos;
                StateInfo st;
                pos.set(prev.fen.fen, &st, game->fen.shatranj);

                std::cout << "pos : " << pos << std::endl;
                search s(&tt, pos);
                auto   move = s.iterative_deepening(0);

                if (move == Move::none() || move != game->move)
                {
                    EXPECT_EQ(move, game->move);
                    success = false;
                    break;
                }
                prev = *game;
            }
            if (!success)
            {
                fails++;
            }
            else
            {
                successes++;
            }
        }
    }

    std::cout << "successes: " << successes << " fails: " << fails << std::endl;
}
