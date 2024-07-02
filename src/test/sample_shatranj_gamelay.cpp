#include <algorithm>
#include <cassert>
#include <chrono>
#include <limits.h>
#include <optional>
#include <sstream>
#include <utility>

#include "position.h"
#include "shatranc_piece.h"
#include "shatranj.h"
#include "types.h"
#include "gtest/gtest.h"
#include "helper.h"

namespace
{

void DumpPossibleMoves(std::shared_ptr<shatranj::Board> &board, size_t expected_moves = 0)
{
    auto posmoves = board->GetPossibleMoves(board->GetCurrentTurn());
    std::cout << *(board) << std::endl;
    for (const auto &move : posmoves)
    {
        std::cout << move.ToString() << " ";
    }
    std::cout << std::endl;
    EXPECT_EQ(posmoves.size(), expected_moves);
}

void CheckPossibleMoves(shatranj::Shatranj &shatranj, std::string piece_coordinates,
                        const std::vector<std::string> &expectedmoves)
{
    auto lastcapturedplace =
        shatranj.GetBoard()->GetPieces()->GetPieceByVal(shatranj::Position(std::move(piece_coordinates)));
    ASSERT_TRUE(lastcapturedplace.has_value());
    if (!lastcapturedplace.has_value())
    {
        return;
    }
    auto lastcapturedpiece = *lastcapturedplace;
    auto possiblemovesoflastcapturedpiece = shatranj.GetBoard()->GetPossibleMoves(
        lastcapturedpiece.GetPos(), lastcapturedpiece.GetPieceType(), lastcapturedpiece.GetColor());
    for (const auto &move : possiblemovesoflastcapturedpiece)
    {
        auto found = std::find(expectedmoves.begin(), expectedmoves.end(), move.ToString()) != expectedmoves.end();
        if (!found)
        {
            std::cout << "not expected move found " << move.ToString() << std::endl;
        }
        EXPECT_TRUE(found);
    }
    for (const auto &move : expectedmoves)
    {
        auto found = std::find_if(possiblemovesoflastcapturedpiece.begin(), possiblemovesoflastcapturedpiece.end(),
                                  [&move](const shatranj::Movement &movefromcaptured) -> bool {
                                      return movefromcaptured.ToString() == move;
                                  }) != possiblemovesoflastcapturedpiece.end();
        EXPECT_TRUE(found);
        if (!found)
        {
            std::cout << *(shatranj.GetBoard()) << std::endl;
            std::cout << "expected move not found " << move << std::endl;
        }
    }
}
TEST(SampleCaptureTest_Piyade, Positive)
{
    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));

        EXPECT_EQ(shatranj.GetBoard()->GetPieces()->Size(), 32);
        EXPECT_EQ(shatranj.PlaySeq({"a2a3"}), true);
        EXPECT_EQ(shatranj.GetBoard()->Revert(1), true);
        for (auto citr : std::vector<char>{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'})
        {
            for (int intitr : std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8})
            {
                std::stringstream tempstrcomb;
                tempstrcomb << citr << intitr;
                auto piece =
                    shatranj.GetBoard()->GetPieces()->GetPieceByVal(shatranj::Position(std::string(tempstrcomb.str())));

                if (intitr == 8 || intitr == 1 || intitr == 2 || intitr == 7)
                {
                    std::cout << "checking existance " << tempstrcomb.str() << " piece pos "
                              << piece->GetPos().ToString() << std::endl;
                    EXPECT_EQ(piece.has_value(), true);
                    EXPECT_EQ(tempstrcomb.str(), piece->GetPos().ToString());
                }
                else
                {
                    std::cout << "checking existance " << tempstrcomb.str() << std::endl;
                    EXPECT_EQ(piece.has_value(), false);
                }
            }
        }
        EXPECT_EQ(shatranj.PlaySeq({"a2a3", "b7b6", "a3a4", "b6b5"}), true);
        EXPECT_EQ(shatranj.GetBoard()->GetPieces()->Size(), 32);
        EXPECT_EQ(shatranj.PlaySeq({"a4b5"}), true);
        EXPECT_EQ(shatranj.GetBoard()->GetPieces()->Size(), 31);
    }
    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));

        EXPECT_EQ(shatranj.PlaySeq({"a2a3", "b7b6", "a3a4", "b6b5"}), true);
        EXPECT_EQ(shatranj.PlaySeq({"a4a5"}), true);
    }
    {
        {
            // piyade can not capture in front check get possible moves only 2 crosses possible front filled
            shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));

            CheckPossibleMoves(shatranj, "a1", {});
            EXPECT_EQ(shatranj.PlaySeq({"a2a3", "b7b6", "b2b3", "b6b5", "c2c3", "b5b4", "d2d3"}), true);
            CheckPossibleMoves(shatranj, "b4", {"b4a3", "b4c3"});
            EXPECT_EQ(shatranj.PlaySeq({"b4a3"}), true);
        }
        {
            // piyade can not capture in front check get possible moves all 3 possible, front empty
            shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));

            EXPECT_EQ(shatranj.PlaySeq({"a2a3", "b7b6", "e2e3", "b6b5", "c2c3", "b5b4", "d2d3"}), true);
            std::cout << *(shatranj.GetBoard()) << std::endl;
            CheckPossibleMoves(shatranj, "b4", {"b4a3", "b4c3", "b4b3"});
            EXPECT_EQ(shatranj.PlaySeq({"b4b3"}), true);
        }
        {
            // piyade can not capture in front
            shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));

            EXPECT_EQ(shatranj.PlaySeq({"a2a3", "b7b6", "b2b3", "b6b5", "c2c3", "b5b4", "d2d3"}), true);
            EXPECT_EQ(shatranj.PlaySeq({"b4c3"}), true);
        }
        {
            // piyade can not capture in front
            shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));

            EXPECT_EQ(shatranj.PlaySeq({"a2a3", "b7b6", "b2b3", "b6b5", "c2c3", "b5b4", "d2d3"}), true);
            EXPECT_EQ(shatranj.PlaySeq({"b4b3"}), false);
        }
    }
    {
        // piyade can promote to vizier
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));

        EXPECT_EQ(shatranj.PlaySeq({"b2b3", "g7g6", "b3b4", "g6g5", "b4b5", "g5g4", "b5b6", "g4g3", "b6c7"}), true);
        EXPECT_EQ(shatranj.PlaySeq({"g3h2"}), true);
        EXPECT_EQ(shatranj.PlaySeq({"c7b8"}), true);
        const auto b8p = shatranj.GetBoard()->GetPieces()->GetPieceByVal(shatranj::Position("b8"));
        EXPECT_TRUE((*b8p).IsVizier());
        EXPECT_EQ(shatranj.PlaySeq({"h2g1"}), true);
        const auto g1p = shatranj.GetBoard()->GetPieces()->GetPieceByVal(shatranj::Position("g1"));
        EXPECT_TRUE((*g1p).IsVizier());

        std::cout << *(shatranj.GetBoard()) << std::endl;
    }
}

TEST(SampleCaptureTest_Rook, Positive)
{
    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));

        EXPECT_EQ(shatranj.PlaySeq({"a2a3", "b7b6", "a3a4", "b6b5", "a4b5", "f7f6", "a1a7", "d7d6"}), true);
        std::cout << *(shatranj.GetBoard()) << std::endl;
        EXPECT_EQ(shatranj.GetBoard()->GetCurrentPlayer().GetColor(), shatranj::Color::kWhite);
        auto lastcapturedplace = shatranj.GetBoard()->GetPieces()->GetPieceByVal(shatranj::Position("a7"));
        EXPECT_EQ(lastcapturedplace.has_value(), true);
        auto lastcapturedpiece = *lastcapturedplace;
        EXPECT_EQ(lastcapturedpiece.GetSymbolOld() == "R", true);
        EXPECT_TRUE(lastcapturedpiece.GetColor() == shatranj::Color::kWhite);
        CheckPossibleMoves(shatranj, "a7", {"a7a8", "a7a6", "a7a5", "a7a4", "a7a3", "a7a2", "a7a1", "a7b7", "a7c7"});
    }
    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));

        EXPECT_EQ(shatranj.PlaySeq({"b1c3"}), true);
        CheckPossibleMoves(shatranj, "a1", {});
        EXPECT_EQ(shatranj.PlaySeq({"a1b1"}), false);
        EXPECT_EQ(shatranj.PlaySeq({"b7b6"}), true);
        CheckPossibleMoves(shatranj, "a1", {"a1b1"});
        EXPECT_EQ(shatranj.PlaySeq({"a1b1"}), true);
    }
    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));

        EXPECT_NO_THROW(shatranj.GetBoard()->ApplyFEN("r1fvsf1r/pppppppp/h4h2/3H4/8/8/PPPPPPPP/R1FVSFHR w 0 3"));

        CheckPossibleMoves(shatranj, "a2", {"a2a3"});
    }
}

TEST(SampleCaptureTest_Horse, Positive)
{
    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));

        CheckPossibleMoves(shatranj, "b1", {"b1a3", "b1c3"});
        CheckPossibleMoves(shatranj, "b1", {"b1a3", "b1c3"});
        EXPECT_EQ(shatranj.PlaySeq({"b1c3", "g8f6"}), true);
        CheckPossibleMoves(shatranj, "c3", {"c3b5", "c3a4", "c3d5", "c3e4", "c3b1"});
        EXPECT_EQ(shatranj.PlaySeq({"b8c5"}), false);

        std::cout << *(shatranj.GetBoard()) << std::endl;
        EXPECT_EQ(shatranj.PlaySeq({"e2e3"}), true);
        EXPECT_EQ(shatranj.PlaySeq({"b8c6", "d2d3"}), true);
        std::cout << *(shatranj.GetBoard()) << std::endl;
        CheckPossibleMoves(shatranj, "c6", {"c6b4", "c6a5", "c6e5", "c6d4", "c6b8"});
    }
    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));

        CheckPossibleMoves(shatranj, "b1", {"b1a3", "b1c3"});
        EXPECT_EQ(shatranj.PlaySeq({"b1c3", "g8f6", "c3d5"}), true);
        CheckPossibleMoves(shatranj, "f6", {"f6d5", "f6e4", "f6g4", "f6h5", "f6g8"});
        EXPECT_EQ(shatranj.PlaySeq({"f6h3"}), false);
        EXPECT_EQ(shatranj.PlaySeq({"f6d5", "h2h3"}), true);
        CheckPossibleMoves(shatranj, "d5", {"d5b6", "d5b4", "d5c3", "d5e3", "d5f4", "d5f6"});
    }
}

TEST(SampleCaptureTest_Fil, Positive)
{
    shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));

    CheckPossibleMoves(shatranj, "c1", {"c1a3", "c1e3"});
}

TEST(SampleCaptureTest_LeakTest, Positive)
{
    EXPECT_EQ(sizeof(shatranj::Piece), 2);
    EXPECT_EQ(sizeof(shatranj::Piyade), 2);
    EXPECT_EQ(sizeof(shatranj::Shah), 2);
    EXPECT_EQ(sizeof(shatranj::PiecePrimitive), 1);

    if constexpr (shatranj::kStressTest)
        for (int i = 0; i < 1000; i++)
        {
            shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));
            EXPECT_EQ(shatranj.PlaySeq({"a2a3", "a7a6", "b2b3", "b7b6", "c2c3", "c7c6", "d2d3", "d7d6", "e2e3", "e7e6",
                                        "f2f3", "f7f6", "g2g3", "g7g6", "h2h3", "h7h6"}),
                      true);
            EXPECT_EQ(shatranj.PlaySeq({"a3a4", "a6a5", "b3b4", "b6b5", "c3c4", "c6c5", "d3d4", "d6d5", "e3e4", "e6e5",
                                        "f3f4", "f6f5", "g3g4", "g6g5", "h3h4", "h6h5"}),
                      true);
            EXPECT_EQ(shatranj.PlaySeq({"a4b5", "a5b4", "c4d5", "c5d4", "e4f5", "e5f4", "g4h5", "g5h4"}), true);
            EXPECT_EQ(shatranj.PlaySeq({"a1a8", "h8h5", "a8b8", "h5f5", "b8c8", "f5d5", "c8d8"}), true);
            EXPECT_EQ(shatranj.PlaySeq({"d5b5"}),
                      false); // is check should return false and seq should fail, since rook is next to king
            EXPECT_EQ(shatranj.PlaySeq({"e8d8", "h1h4", "d5b5", "f1d3", "f8d6", "h4f4", "d6f4", "d3b5", "g8f6", "b1c3",
                                        "f6d5", "c3e2", "d4d3", "g1f3", "d3d2"}),
                      true);
            EXPECT_EQ(shatranj.PlaySeq({"c1c3"}),
                      false); // is check should return false and seq should fail, since piyade is next to king
            EXPECT_EQ(shatranj.PlaySeq({"e1d2"}),
                      false); // shah can not take the piyade because it is being protected by fill
            EXPECT_EQ(shatranj.PlaySeq({"e1f1", "d2c1", "e2c1"}), true);
        }
}

TEST(SampleCaptureTest_DrawTest, Positive)
{

    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));

        EXPECT_EQ(shatranj.PlaySeq({"a2a3", "a7a6", "b2b3", "b7b6", "c2c3", "c7c6", "d2d3", "d7d6", "e2e3", "e7e6",
                                    "f2f3", "f7f6", "g2g3", "g7g6", "h2h3", "h7h6"}),
                  true);
        EXPECT_EQ(shatranj.PlaySeq({"a3a4", "a6a5", "b3b4", "b6b5", "c3c4", "c6c5", "d3d4", "d6d5", "e3e4", "e6e5",
                                    "f3f4", "f6f5", "g3g4", "g6g5", "h3h4", "h6h5"}),
                  true);
        EXPECT_EQ(shatranj.PlaySeq({"a4b5", "a5b4", "c4d5", "c5d4", "e4f5", "e5f4", "g4h5", "g5h4"}), true);
        EXPECT_EQ(shatranj.PlaySeq({"a1a8", "h8h5", "a8b8", "h5f5", "b8c8", "f5d5"}), true);
        EXPECT_EQ(shatranj.PlaySeq({"c8d8"}), true);
        EXPECT_EQ(shatranj.PlaySeq({"d5b5"}),
                  false); // is check should return false and seq should fail, since rook is next to king
        EXPECT_EQ(shatranj.PlaySeq({"e8d8", "h1h4", "d5b5", "f1d3", "f8d6", "h4f4", "d6f4", "d3b5", "g8f6", "b1c3",
                                    "f6d5", "c3e2", "d4d3", "g1f3", "d3d2"}),
                  true);
        EXPECT_EQ(shatranj.PlaySeq({"c1c3"}),
                  false); // is check should return false and seq should fail, since piyade is next to king
        EXPECT_EQ(shatranj.PlaySeq({"e1d2"}),
                  false); // shah can not take the piyade because it is being protected by fill
        EXPECT_EQ(shatranj.PlaySeq({"e1f1", "d2c1", "e2c1", "d5c3", "c1d3", "c3b5", "d3b4", "b5c3", "b4d3", "c3d1",
                                    "d3f4", "d1f2", "f4h3", "f2h3", "f3e5", "h3g1", "f1g1"},
                                   true),
                  true);
        EXPECT_EQ(shatranj.PlaySeq({"d8d7"}, true), false);
        EXPECT_EQ(shatranj.GetBoard()->GetBoardState(), shatranj::GameState::kCheckmate);
    }
    {

        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));

        EXPECT_EQ(shatranj.PlaySeq({"a2a3", "a7a6", "b2b3", "b7b6", "c2c3", "c7c6", "d2d3", "d7d6", "e2e3", "e7e6",
                                    "f2f3", "f7f6", "g2g3", "g7g6", "h2h3", "h7h6"}),
                  true);
        EXPECT_EQ(shatranj.PlaySeq({"a3a4", "a6a5", "b3b4", "b6b5", "c3c4", "c6c5", "d3d4", "d6d5", "e3e4", "e6e5",
                                    "f3f4", "f6f5", "g3g4", "g6g5", "h3h4", "h6h5"}),
                  true);
        EXPECT_EQ(shatranj.PlaySeq({"a4b5", "a5b4", "c4d5", "c5d4", "e4f5", "e5f4", "g4h5", "g5h4"}), true);
        EXPECT_EQ(shatranj.PlaySeq({"a1a8", "h8h5", "a8b8", "h5f5", "b8c8", "f5d5"}), true);
        EXPECT_EQ(shatranj.PlaySeq({"c8d8"}), true);
        EXPECT_EQ(shatranj.PlaySeq({"d5b5"}),
                  false); // is check should return false and seq should fail, since rook is next to king
        EXPECT_EQ(shatranj.PlaySeq({"e8d8", "h1h4", "d5b5", "f1d3", "f8d6", "h4f4", "d6f4", "d3b5", "g8f6", "b1c3",
                                    "f6d5", "c3e2", "d4d3", "g1f3", "d3d2"}),
                  true);
        EXPECT_EQ(shatranj.PlaySeq({"c1c3"}),
                  false); // is check should return false and seq should fail, since piyade is next to king
        EXPECT_EQ(shatranj.PlaySeq({"e1d2"}),
                  false); // shah can not take the piyade because it is being protected by fill
        EXPECT_EQ(shatranj.PlaySeq({"e1f1", "d2c1", "e2c1", "d5c3", "c1d3", "c3b5", "d3b4", "b5c3", "b4d3", "c3d1",
                                    "d3f4", "d1f2", "f4h3", "f2h3", "f3e5", "h3g1", "e5d7"},
                                   true),
                  true);
        EXPECT_EQ(shatranj.PlaySeq({"d8d7"}, true), true);
        EXPECT_EQ(shatranj.GetBoard()->GetBoardState(), shatranj::GameState::kDraw);
        EXPECT_EQ(shatranj.PlaySeq({"f1g1"}, true), false);
        EXPECT_EQ(shatranj.PlaySeq({"f1g2"}, true), false);
    }
}

TEST(SampleCaptureTest_GenerateFEN, Negative)
{
    shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));

    EXPECT_EQ(shatranj.GetBoard()->GenerateFEN(), std::string("rhfvsfhr/pppppppp/8/8/8/8/PPPPPPPP/RHFVSFHR w 0 1"));
    EXPECT_EQ(shatranj.PlaySeq({"e2e3"}), true);
    EXPECT_EQ(shatranj.GetBoard()->GenerateFEN(), std::string("rhfvsfhr/pppppppp/8/8/8/4P3/PPPP1PPP/RHFVSFHR b 0 1"));
    EXPECT_EQ(shatranj.PlaySeq({"e7e6"}), true);
    EXPECT_EQ(shatranj.GetBoard()->GenerateFEN(), std::string("rhfvsfhr/pppp1ppp/4p3/8/8/4P3/PPPP1PPP/RHFVSFHR w 0 2"));
}

TEST(SampleCaptureTest_ApplyFEN, Negative)
{
    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));

        shatranj.GetBoard()->ApplyFEN(std::string("rhfvsfhr/pppp1ppp/4p3/8/8/4P3/PPPP1PPP/RHFVSFHR w 0 2"));
        auto e3opt = shatranj.GetBoard()->GetPieces()->GetPieceByVal(shatranj::Position{"e3"});
        auto e6opt = shatranj.GetBoard()->GetPieces()->GetPieceByVal(shatranj::Position{"e6"});
        EXPECT_EQ(e3opt.has_value(), true);
        EXPECT_EQ(e6opt.has_value(), true);
        auto e3val = *e3opt;
        auto e6val = *e6opt;
        EXPECT_EQ(e3val.GetPieceType(), shatranj::ChessPieceEnum::kPiyade);
        EXPECT_EQ(e6val.GetPieceType(), shatranj::ChessPieceEnum::kPiyade);
        EXPECT_EQ(shatranj.GetBoard()->GetFullMoveNumber(), 2);
        EXPECT_EQ(shatranj.GetBoard()->GetHalfMoveClock(), 0);
    }
    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));

        shatranj.GetBoard()->ApplyFEN(std::string("s7/8/4p3/8/8/4P3/8/7S w 0 2"));
        auto a8opt = shatranj.GetBoard()->GetPieces()->GetPieceByVal(shatranj::Position{"a8"});
        auto h1opt = shatranj.GetBoard()->GetPieces()->GetPieceByVal(shatranj::Position{"h1"});
        auto e3opt = shatranj.GetBoard()->GetPieces()->GetPieceByVal(shatranj::Position{"e3"});
        auto e6opt = shatranj.GetBoard()->GetPieces()->GetPieceByVal(shatranj::Position{"e6"});
        EXPECT_EQ(a8opt.has_value(), true);
        EXPECT_EQ(h1opt.has_value(), true);
        EXPECT_EQ(e3opt.has_value(), true);
        EXPECT_EQ(e6opt.has_value(), true);
        auto e3val = *e3opt;
        auto e6val = *e6opt;
        EXPECT_EQ(e3val.GetPieceType(), shatranj::ChessPieceEnum::kPiyade);
        EXPECT_EQ(e6val.GetPieceType(), shatranj::ChessPieceEnum::kPiyade);
        auto a8val = *a8opt;
        auto h1val = *h1opt;
        EXPECT_EQ(a8val.GetPieceType(), shatranj::ChessPieceEnum::kShah);
        EXPECT_EQ(h1val.GetPieceType(), shatranj::ChessPieceEnum::kShah);
    }
}

TEST(RevertTest, Positive)
{

    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));

        for (int i = 0; i < 10; i++)
        {
            EXPECT_EQ(shatranj.PlaySeq({"a2a3", "b7b6", "a3a4", "b6b5", "a4b5", "f7f6", "a1a7", "d7d6"}), true);
            std::cout << "before revert:" << std::endl << *(shatranj.GetBoard()) << std::endl;
            EXPECT_EQ(shatranj.GetBoard()->Revert(100), false);
            std::cout << "after revert:" << std::endl << *(shatranj.GetBoard()) << std::endl;
        }
    }
}

TEST(RevertTest2, Positive)
{
    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));
        EXPECT_EQ(shatranj.PlaySeq({"b1c3", "b8a6", "c3d5", "g8f6", "g1h3"}), true);
        EXPECT_EQ(shatranj.GetBoard()->Revert(1), true);
        auto oneplayandrevert = [&](const std::vector<shatranj::Movement> &moves) {
            for (const auto &move : moves)
            {
                // std::cout << "checking " << move.first.ToString() << " " << move.second.ToString() << std::endl;
                // std::cout << "============================before play:" << std::endl << *(shatranj.GetBoard()) <<
                // std::endl;
                ASSERT_EQ(shatranj.PlaySeq2({shatranj::Movement(move)}), true);
                // std::cout << "after play:" << std::endl << *(shatranj.GetBoard()) << std::endl;
                EXPECT_EQ(shatranj.GetBoard()->Revert(1), true);
                // std::cout << "after revert:" << std::endl << *(shatranj.GetBoard()) << std::endl;
            }
        };

        oneplayandrevert(shatranj.GetBoard()->GetPossibleMoves(shatranj.GetBoard()->GetCurrentTurn()));
        CheckPossibleMoves(shatranj, "a2", {"a2a3"});
    }
}

TEST(SampleGameEndTests, PosNeg)
{
    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));
        shatranj.GetBoard()->ApplyFEN("1r1vr3/1p2sp2/p1p5/3pp2p/2f3p1/2H2P2/PPPf1PPP/1RFVSF1R w 1 43");
        EXPECT_EQ(shatranj.GetBoard()->GetBoardState(), shatranj::GameState::kNormal);
        DumpPossibleMoves(shatranj.GetBoard(), 19);
    }
    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));
        shatranj.GetBoard()->ApplyFEN("2rr2f1/2v1p1p1/p2p1p1p/1p2h3/2p1s1h1/2H5/PPPfPPPP/R1FVSFHR b 7 37");
        EXPECT_EQ(shatranj.GetBoard()->GetBoardState(), shatranj::GameState::kCheck);
        DumpPossibleMoves(shatranj.GetBoard(), 3);
    }
    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));
        shatranj.GetBoard()->ApplyFEN("2rvs2r/pp1p1pp1/3fp2p/2p1h2h/2f5/2H5/PPPP1PPP/1RFVSFHR w 0 21");
        EXPECT_EQ(shatranj.GetBoard()->GetBoardState(), shatranj::GameState::kNormal);
        DumpPossibleMoves(shatranj.GetBoard(), 20);
    }
    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));
        shatranj.GetBoard()->ApplyFEN("r1fv3r/2ppp1sp/pp3ppf/4h3/8/2H5/PPPPPPhP/R1FVSFHR w 6 20");
        EXPECT_EQ(shatranj.GetBoard()->GetBoardState(), shatranj::GameState::kCheckmate);
        DumpPossibleMoves(shatranj.GetBoard(), 0);
    }
    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));
        shatranj.GetBoard()->ApplyFEN("1h1vsf1r/r1ppp1p1/pp2fp1h/7p/8/1P1FPS1P/P1PP1PPR/RHFV2H1 b 1 9");
        EXPECT_EQ(shatranj.GetBoard()->GetBoardState(), shatranj::GameState::kNormal);
        DumpPossibleMoves(shatranj.GetBoard(), 22);
    }
    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));
        shatranj.GetBoard()->ApplyFEN("rhfvsfhr/2Hppppp/8/1p6/p7/P3F2H/1PPPPPPP/R2VSF1R b 6 6");
        EXPECT_EQ(shatranj.GetBoard()->GetBoardState(), shatranj::GameState::kCheck);
        DumpPossibleMoves(shatranj.GetBoard(), 1);
    }
    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));
        shatranj.GetBoard()->ApplyFEN("rhfvsf1r/ppppp1pp/8/5pH1/5h2/2P3P1/PP1PPP1P/RH1VSFFR b 0 6");
        EXPECT_EQ(shatranj.GetBoard()->GetBoardState(), shatranj::GameState::kNormal);
        DumpPossibleMoves(shatranj.GetBoard(), 22);
    }
    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));
        shatranj.GetBoard()->ApplyFEN("1s6/pV1R2pp/2H1p2f/1pF2F1P/8/2P5/PPSPPPP1/7R b 6 45");
        EXPECT_EQ(shatranj.GetBoard()->GetBoardState(), shatranj::GameState::kCheckmate);
        DumpPossibleMoves(shatranj.GetBoard(), 0);
    }
    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));
        shatranj.GetBoard()->ApplyFEN("4s3/1pppvpp1/4f3/1p1p2p1/r4f2/4hP1r/4P3/3h2S1 w 18 37");
        EXPECT_EQ(shatranj.GetBoard()->GetBoardState(), shatranj::GameState::kStalemate);
        DumpPossibleMoves(shatranj.GetBoard(), 0);
    }
    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));
        shatranj.GetBoard()->ApplyFEN("r3sf1r/p1vppppp/1p6/8/2fh4/F2PPhP1/PPP2P1P/R2VSFR1 w 3 11");
        EXPECT_EQ(shatranj.GetBoard()->GetBoardState(), shatranj::GameState::kCheckmate);
        DumpPossibleMoves(shatranj.GetBoard(), 0);
    }
}

TEST(SampleCaptureTest_MinMax, Negative)
{
    shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));
    std::cout << *(shatranj.GetBoard()) << std::endl;
    std::vector<shatranj::Movement> moves;
    shatranj::DeferedCall callback([&]() {
        std::cout << "so far played moves:  " << std::endl << "\t\t\t";
        int count = 0;
        for (const auto &move : moves)
        {
            std::cout << move.ToString() << " ";
            if (++count % 10 == 0)
            {
                std::cout << std::endl << "\t\t\t";
            }
        }
        std::cout << std::endl;
    });
    for (int i = 0; i < 100; i++)
    {
        int countofnodesvisited = 0;
        std::variant<double, shatranj::Movement> pickedmove;
        auto res = shatranj::RunWithTiming("minmax search ", [&]() -> bool {
            try
            {
                auto alpha = -std::numeric_limits<double>::max();
                auto beta = std::numeric_limits<double>::max();
                pickedmove = shatranj.GetBoard()->MinimaxSearch(std::nullopt, countofnodesvisited, alpha, beta, 1,
                                                                shatranj.GetBoard()->GetCurrentTurn(), true);
            }
            catch (...)
            {
                auto state = shatranj.GetBoard()->GetBoardState();
                std::cout << "exception in search, state : " << static_cast<int>(state) << std::endl;
                return state == shatranj::GameState::kNormal;
            }
            return true;
        });
        std::cout << "nodes visited: " << countofnodesvisited << std::endl;
        std::cout << *(shatranj.GetBoard()) << std::endl;
        if (res)
        {
            std::cout << std::get<shatranj::Movement>(pickedmove).ToString() << std::endl;
            moves.push_back(std::get<shatranj::Movement>(pickedmove));
            ASSERT_TRUE(shatranj.Play(std::get<shatranj::Movement>(pickedmove)));
        }
        else
        {
            break;
        }
    }
    if (shatranj.GetBoard()->Winner())
    {
        std::cout << "winner is " << *(shatranj.GetBoard()->Winner()) << std::endl;
    }
    else
    {
        if (shatranj.GetBoard()->GetBoardState() == shatranj::GameState::kDraw)
            std::cout << "draw" << std::endl;
        if (shatranj.GetBoard()->GetBoardState() == shatranj::GameState::kStalemate)
            std::cout << "stalemate" << std::endl;
        if (shatranj.GetBoard()->GetBoardState() == shatranj::GameState::kCheckmate)
            std::cout << "checkmate" << std::endl;
    }
    std::cout << *(shatranj.GetBoard()) << std::endl;
}

} // namespace
