#pragma once

#include "../stockfish_position.h"

namespace Stockfish {
int evaluate(Stockfish::Position pos, Color maximColor, size_t movecount);
}
