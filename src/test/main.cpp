#include <gtest/gtest.h>
#include <thread>

#include "position.h"
#include "stockfish_position.h"
#include "shatranc_piece.h"
#include "bitboard.h"

using namespace Stockfish;

int main(int argc, char** argv) {
    shatranj::Piece::InitCapturePerSquareTable();
    shatranj::Piece::InitMovePerSquareTable();
    Bitboards::init();
    Position::init();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
