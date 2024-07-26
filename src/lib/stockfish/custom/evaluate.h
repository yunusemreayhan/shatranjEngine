#pragma once

#include "../stockfish_position.h"

namespace Stockfish {
int evaluate(Stockfish::Position pos, Color currentColor, Color maximColor, size_t movecount);
}
