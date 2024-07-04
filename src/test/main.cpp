#include <gtest/gtest.h>

#include "position.h"
#include "shatranc_piece.h"

int main(int argc, char **argv)
{
    shatranj::Piece::InitCapturePerSquareTable();
    shatranj::Piece::InitMovePerSquareTable();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}