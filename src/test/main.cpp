#include <gtest/gtest.h>
#include <thread>

#include "position.h"
#include "stockfish_position.h"
#include "shatranc_piece.h"
#include "bitboard.h"

using namespace Stockfish;

unsigned int hash3(unsigned int h1, unsigned int h2, unsigned int h3) {
    return (((h1 * 2654435789U) + h2) * 2654435789U) + h3;
}
int main(int argc, char** argv) {
    shatranj::Piece::InitCapturePerSquareTable();
    shatranj::Piece::InitMovePerSquareTable();
    Bitboards::init();
    Position::init();
    ::testing::InitGoogleTest(&argc, argv);
    struct timeval time;
    gettimeofday(&time, NULL);
    srand(hash3(time.tv_sec, time.tv_usec, getpid()));

    return RUN_ALL_TESTS();
}
