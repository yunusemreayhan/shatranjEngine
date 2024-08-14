
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

#include "custom_search.h"
#include "testhelper.h"
#include "tt.h"
#include "types.h"
#include "pesto_evaluate.h"
#include "evaluate.h"

#include <fstream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

using namespace Stockfish;

enum MoveTypeFromJson {
    INITIAL,
    NORMAL,
    ANY
};


struct gamepoint {
    FenWithType      fenwithtype;
    Move             expectedMove;
    MoveTypeFromJson expectedMoveType = ANY;
};

struct problem {
    std::string            problem_fpath;
    std::vector<gamepoint> games;
};

bool operator<(const problem& a, const problem& b) { return a.games.size() < b.games.size(); }

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
            gamepoint toinsert;
            toinsert.fenwithtype.fen      = move.at("FEN");
            toinsert.fenwithtype.shatranj = false;
            auto movestr                  = move.at("move");
            toinsert.expectedMove         = Move::none();
            if (movestr == "...")
                toinsert.expectedMoveType = MoveTypeFromJson::ANY;
            else if (movestr == "initial")
                toinsert.expectedMoveType = MoveTypeFromJson::INITIAL;
            else
            {
                toinsert.expectedMoveType = MoveTypeFromJson::NORMAL;
                toinsert.expectedMove     = strToMove(move.at("move"));
            }
            current_problem.games.push_back(toinsert);
        }
    }

    std::cout << problems.size() << std::endl;
    std::sort(problems.begin(), problems.end());
    size_t             successes = 0;
    size_t             fails     = 0;
    TranspositionTable tt;
    tt.resize(2048);
    std::vector<std::string> solvedproblemnames;
    std::vector<std::string> failedproblem;
    std::vector<std::string> skippedproblems;
    for (auto& problem : problems)
    {
        if (problem.games.size() > 10)
        {
            skippedproblems.push_back(problem.problem_fpath);
            continue;
        }
        bool solved = true;
        tt.clear();
        auto prev = problem.games.begin();
        for (auto current = problem.games.begin() + 1; current != problem.games.end();
             prev++, current++)
        {
            std::stringstream ss;
            if (current->expectedMoveType == MoveTypeFromJson::NORMAL)
                ss << current->expectedMove;
            else
                ss << "none";
            std::cout << "game.fen : " << current->fenwithtype.fen
                      << " game.shatranj : " << current->fenwithtype.shatranj
                      << " move : " << ss.str() << std::endl;
            Position  pos;
            StateInfo st;
            pos.set(prev->fenwithtype.fen, &st, current->fenwithtype.shatranj);

            std::cout << "pos : " << pos << std::endl;
            search s(&tt, pos);
            auto   move = s.iterative_deepening(9);

            if (current->expectedMoveType == MoveTypeFromJson::NORMAL
                && move != current->expectedMove)
            {
                EXPECT_EQ(move, current->expectedMove);
                fails++;
                solved = false;
            }

            if (move == current->expectedMove && pos.checkers() == 0)
            {
                successes++;
            }
            std::cout << "successes: " << successes << " fails: " << fails << std::endl;
        }
        if (solved)
        {
            solvedproblemnames.push_back(problem.problem_fpath);
        }
        else
        {
            failedproblem.push_back(problem.problem_fpath);
        }
    }

    std::cout << "successes: " << successes << " fails: " << fails << std::endl;
    for (auto p : solvedproblemnames)
    {
        std::cout << "solved problem : " << p << " solved " << std::endl;
    }
    for (auto p : failedproblem)
    {
        std::cout << "failed problem : " << p << " solved " << std::endl;
    }
    for (auto p : skippedproblems)
    {
        std::cout << "skipped problem : " << p << " solved " << std::endl;
    }
}
