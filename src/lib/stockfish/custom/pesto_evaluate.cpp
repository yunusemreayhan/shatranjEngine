#include "pesto_evaluate.h"

namespace Stockfish {
int get_table_value_mg(Stockfish::Piece pc, Stockfish::Square sq) { return mg_table[pc][sq]; }
}
