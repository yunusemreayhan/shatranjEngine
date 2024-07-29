
#pragma once
#include <chrono>

#include "../stockfish_position.h"
#include "../movegen.h"

long long                        perft(Stockfish::Position pos, int depth);
std::tuple<long long, long long> perft_time(Stockfish::Position pos, int depth);
