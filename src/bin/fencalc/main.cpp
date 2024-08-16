#include <chrono>
#include <iostream>
#include <sys/select.h>

#include "stockfish_position.h"
#include "custom_search.h"
#include "time.h"
unsigned int hash3(unsigned int h1, unsigned int h2, unsigned int h3) {
    return (((h1 * 2654435789U) + h2) * 2654435789U) + h3;
}
int main(int argc, char** argv) {
    if (argc < 3)
    {
        std::cout << "usage: fencalc <depth> <ttsize_mb> <timeout_s> \"<fen>\"" << std::endl;
        std::cout << "example: fencalc 4 2048 \"8/8/8/1k6/8/1KQ5/8/q7 w - - 0 1\"" << std::endl;
        return 1;
    }

    shatranj::Piece::InitCapturePerSquareTable();
    shatranj::Piece::InitMovePerSquareTable();
    Bitboards::init();
    Position::init();

    Stockfish::TranspositionTable tt;
    size_t                        i              = 0;
    std::string                   depth          = argv[++i];
    std::string                   ttsize         = argv[++i];
    std::string                   timeout_s      = argv[++i];
    std::string                   fen            = argv[++i];
    long                          depth_int      = std::stol(depth);
    long                          ttsize_int     = std::stol(ttsize);
    long                          timeout_s_long = std::stol(timeout_s);
    std::chrono::seconds          timeout(timeout_s_long);
    tt.resize(ttsize_int);
    Stockfish::StateInfo st;
    Stockfish::Position  pos;
    pos.set(fen, &st, false);
    search s1(&tt, pos, timeout);
    s1.iterative_deepening(depth_int);
    std::cout << s1.picked_move() << std::endl;
    return 0;
}
