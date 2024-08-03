#pragma once

#include "../stockfish_position.h"

namespace Stockfish {
int16_t evaluate(Stockfish::Position pos);
namespace Testing {
int get_table_value_mg(Stockfish::Piece pc, Stockfish::Square sq);
}
}
