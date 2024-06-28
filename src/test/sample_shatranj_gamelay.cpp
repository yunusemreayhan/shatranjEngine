#include <algorithm>
#include <limits.h>
#include <utility>

#include "position.h"
#include "shatranc_piece.h"
#include "shatranj.h"
#include "types.h"
#include "gtest/gtest.h"

namespace
{

void CheckPossibleMoves(shatranj::Shatranj &shatranj, std::string piece_coordinates,
                        const std::vector<std::string> &expectedmoves)
{
    auto lastcapturedplace =
        shatranj.GetBoard()->GetPieces()->GetPiece(shatranj::Position(std::move(piece_coordinates)));
    ASSERT_TRUE(lastcapturedplace.has_value());
    if (!lastcapturedplace.has_value())
    {
        return;
    }
    auto lastcapturedpiece = **lastcapturedplace;
    auto possiblemovesoflastcapturedpiece = lastcapturedpiece.GetPossibleMoves(shatranj.GetBoard());
    for (const auto &move : possiblemovesoflastcapturedpiece)
    {
        const std::string movestring = move.first.ToString() + move.second.ToString();
        auto found = std::find(expectedmoves.begin(), expectedmoves.end(), movestring) != expectedmoves.end();
        if (!found)
        {
            std::cout << "not expected move found " << movestring << std::endl;
        }
        EXPECT_TRUE(found);
    }
    for (const auto &move : expectedmoves)
    {
        auto found =
            std::find_if(possiblemovesoflastcapturedpiece.begin(), possiblemovesoflastcapturedpiece.end(),
                         [&move](const std::pair<shatranj::Position, shatranj::Position> &movefromcaptured) -> bool {
                             return movefromcaptured.first.ToString() + movefromcaptured.second.ToString() == move;
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
        EXPECT_EQ(shatranj.GetBoard()->GetPieces()->size(), 32);
        EXPECT_EQ(shatranj.PlaySeq({"a2a3", "b7b6", "a3a4", "b6b5"}), true);
        EXPECT_EQ(shatranj.GetBoard()->GetPieces()->size(), 32);
        EXPECT_EQ(shatranj.PlaySeq({"a4b5"}), true);
        EXPECT_EQ(shatranj.GetBoard()->GetPieces()->size(), 31);
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
        const auto b8p = shatranj.GetBoard()->GetPieces()->GetPiece(shatranj::Position("b8"));
        EXPECT_TRUE((**b8p).IsVizier());
        EXPECT_EQ(shatranj.PlaySeq({"h2g1"}), true);
        const auto g1p = shatranj.GetBoard()->GetPieces()->GetPiece(shatranj::Position("g1"));
        EXPECT_TRUE((**g1p).IsVizier());

        std::cout << *(shatranj.GetBoard()) << std::endl;
    }
}
TEST(SampleCaptureTest_Rook, Positive)
{
    shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));

    EXPECT_EQ(shatranj.PlaySeq({"a2a3", "b7b6", "a3a4", "b6b5", "a4b5", "f7f6", "a1a7", "d7d6"}), true);
    std::cout << *(shatranj.GetBoard()) << std::endl;
    EXPECT_EQ(shatranj.GetBoard()->GetCurrentPlayer().GetColor(), shatranj::Color::kWhite);
    auto lastcapturedplace = shatranj.GetBoard()->GetPieces()->GetPiece(shatranj::Position("a7"));
    EXPECT_EQ(lastcapturedplace.has_value(), true);
    auto lastcapturedpiece = **lastcapturedplace;
    EXPECT_EQ(lastcapturedpiece.GetSymbol() == 'R', true);
    EXPECT_TRUE(lastcapturedpiece.GetColor() == shatranj::Color::kWhite);
    CheckPossibleMoves(shatranj, "a7", {"a7a8", "a7a6", "a7a5", "a7a4", "a7a3", "a7a2", "a7a1", "a7b7", "a7c7"});
}

TEST(SampleCaptureTest_Horse, Positive)
{
    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));

        CheckPossibleMoves(shatranj, "b1", {"b1a3", "b1c3"});
        EXPECT_EQ(shatranj.PlaySeq({"b1c3"}), true);
        CheckPossibleMoves(shatranj, "c3", {"c3b5", "c3a4", "c3d5", "c3e4", "c3b1"});
        EXPECT_EQ(shatranj.PlaySeq({"b8c5"}), false);
        EXPECT_EQ(shatranj.PlaySeq({"b8c6"}), true);
        CheckPossibleMoves(shatranj, "c6", {"c6b4", "c6a5", "c6e5", "c6d4", "c6b8"});
    }
    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));

        CheckPossibleMoves(shatranj, "b1", {"b1a3", "b1c3"});
        EXPECT_EQ(shatranj.PlaySeq({"b1c3", "g8f6", "c3d5"}), true);
        CheckPossibleMoves(shatranj, "f6", {"f6d5", "f6e4", "f6g4", "f6h5", "f6g8"});
        EXPECT_EQ(shatranj.PlaySeq({"f6h3"}), false);
        EXPECT_EQ(shatranj.PlaySeq({"f6d5"}), true);
        CheckPossibleMoves(shatranj, "d5", {"d5b6", "d5b4", "d5c3", "d5e3", "d5f4", "d5f6"});
    }
}

TEST(SampleCaptureTest_LeakTest, Positive)
{
    EXPECT_EQ(sizeof(shatranj::Piece), 3);
    EXPECT_EQ(sizeof(shatranj::Piyade), 3);
    EXPECT_EQ(sizeof(shatranj::Shah), 3);
    EXPECT_EQ(sizeof(shatranj::PiecePrimitive), 2);

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
        EXPECT_EQ(shatranj.PlaySeq({"e1f1", "d2c1", "e2c1", "d5c3", "c1d3", "c3b5", "d3b4", "b5c3", "b4d3", "c3d1",
                                    "d3f4", "d1f2", "f4h3", "f2h3", "f3e5", "h3g1", "f1g1"}),
                  true);
        EXPECT_EQ(shatranj.PlaySeq({"d8d7"}), false);

        EXPECT_EQ(shatranj.PlaySeq({"d8c7", "e5d7"}), true);
        EXPECT_EQ(shatranj.GetBoard()->IsDraw(), true);
        std::cout << *(shatranj.GetBoard()) << std::endl;
        EXPECT_EQ(shatranj.PlaySeq({"c7d7"}), false); // because of draw
        EXPECT_EQ(shatranj.GetBoard()->IsDraw(), true);
        std::cout << *(shatranj.GetBoard()) << std::endl;
        shatranj.GetBoard()->Revert(1000);
        std::cout << *(shatranj.GetBoard()) << std::endl;
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
        auto e3opt = shatranj.GetBoard()->GetPieces()->GetPiece(shatranj::Position{"e3"});
        auto e6opt = shatranj.GetBoard()->GetPieces()->GetPiece(shatranj::Position{"e6"});
        EXPECT_EQ(e3opt.has_value(), true);
        EXPECT_EQ(e6opt.has_value(), true);
        auto *e3val = *e3opt;
        auto *e6val = *e6opt;
        EXPECT_EQ(e3val->GetPieceType(), shatranj::ChessPieceEnum::kPiyade);
        EXPECT_EQ(e6val->GetPieceType(), shatranj::ChessPieceEnum::kPiyade);
        EXPECT_EQ(shatranj.GetBoard()->GetFullMoveNumber(), 2);
        EXPECT_EQ(shatranj.GetBoard()->GetHalfMoveClock(), 0);
    }
    {
        shatranj::Shatranj shatranj(std::string("player1"), std::string("player2"));

        shatranj.GetBoard()->ApplyFEN(std::string("s7/8/4p3/8/8/4P3/8/7S w 0 2"));
        auto a8opt = shatranj.GetBoard()->GetPieces()->GetPiece(shatranj::Position{"a8"});
        auto h1opt = shatranj.GetBoard()->GetPieces()->GetPiece(shatranj::Position{"h1"});
        auto e3opt = shatranj.GetBoard()->GetPieces()->GetPiece(shatranj::Position{"e3"});
        auto e6opt = shatranj.GetBoard()->GetPieces()->GetPiece(shatranj::Position{"e6"});
        EXPECT_EQ(a8opt.has_value(), true);
        EXPECT_EQ(h1opt.has_value(), true);
        EXPECT_EQ(e3opt.has_value(), true);
        EXPECT_EQ(e6opt.has_value(), true);
        auto *e3val = *e3opt;
        auto *e6val = *e6opt;
        EXPECT_EQ(e3val->GetPieceType(), shatranj::ChessPieceEnum::kPiyade);
        EXPECT_EQ(e6val->GetPieceType(), shatranj::ChessPieceEnum::kPiyade);
        auto *a8val = *a8opt;
        auto *h1val = *h1opt;
        EXPECT_EQ(a8val->GetPieceType(), shatranj::ChessPieceEnum::kShah);
        EXPECT_EQ(h1val->GetPieceType(), shatranj::ChessPieceEnum::kShah);
    }
}

} // namespace
