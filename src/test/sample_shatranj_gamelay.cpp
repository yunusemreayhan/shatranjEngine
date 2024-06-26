#include <algorithm>
#include <limits.h>
#include <utility>

#include "position.h"
#include "shatranj.h"
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
    auto lastcapturedpiece = *lastcapturedplace;
    auto possiblemovesoflastcapturedpiece = lastcapturedpiece->GetPossibleMoves(shatranj.GetBoard());
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
        shatranj::Shatranj shatranj(
            std::make_shared<shatranj::Player>(std::string("player1"), shatranj::Color::kWhite),
            std::make_shared<shatranj::Player>(std::string("player2"), shatranj::Color::kBlack));
        EXPECT_EQ(shatranj.PlaySeq({"a2a3", "b7b6", "a3a4", "b6b5"}), true);
        EXPECT_EQ(shatranj.PlaySeq({"a4b5"}), true);
    }
    {
        shatranj::Shatranj shatranj(
            std::make_shared<shatranj::Player>(std::string("player1"), shatranj::Color::kWhite),
            std::make_shared<shatranj::Player>(std::string("player2"), shatranj::Color::kBlack));
        EXPECT_EQ(shatranj.PlaySeq({"a2a3", "b7b6", "a3a4", "b6b5"}), true);
        EXPECT_EQ(shatranj.PlaySeq({"a4a5"}), true);
    }
    {
        {
            // piyade can not capture in front check get possible moves only 2 crosses possible front filled
            shatranj::Shatranj shatranj(
                std::make_shared<shatranj::Player>(std::string("player1"), shatranj::Color::kWhite),
                std::make_shared<shatranj::Player>(std::string("player2"), shatranj::Color::kBlack));
            EXPECT_EQ(shatranj.PlaySeq({"a2a3", "b7b6", "b2b3", "b6b5", "c2c3", "b5b4", "d2d3"}), true);
            CheckPossibleMoves(shatranj, "b4", {"b4a3", "b4c3"});
            EXPECT_EQ(shatranj.PlaySeq({"b4a3"}), true);
        }
        {
            // piyade can not capture in front check get possible moves all 3 possible, front empty
            shatranj::Shatranj shatranj(
                std::make_shared<shatranj::Player>(std::string("player1"), shatranj::Color::kWhite),
                std::make_shared<shatranj::Player>(std::string("player2"), shatranj::Color::kBlack));
            EXPECT_EQ(shatranj.PlaySeq({"a2a3", "b7b6", "e2e3", "b6b5", "c2c3", "b5b4", "d2d3"}), true);
            std::cout << *(shatranj.GetBoard()) << std::endl;
            CheckPossibleMoves(shatranj, "b4", {"b4a3", "b4c3", "b4b3"});
            EXPECT_EQ(shatranj.PlaySeq({"b4b3"}), true);
        }
        {
            // piyade can not capture in front
            shatranj::Shatranj shatranj(
                std::make_shared<shatranj::Player>(std::string("player1"), shatranj::Color::kWhite),
                std::make_shared<shatranj::Player>(std::string("player2"), shatranj::Color::kBlack));
            EXPECT_EQ(shatranj.PlaySeq({"a2a3", "b7b6", "b2b3", "b6b5", "c2c3", "b5b4", "d2d3"}), true);
            EXPECT_EQ(shatranj.PlaySeq({"b4c3"}), true);
        }
        {
            // piyade can not capture in front
            shatranj::Shatranj shatranj(
                std::make_shared<shatranj::Player>(std::string("player1"), shatranj::Color::kWhite),
                std::make_shared<shatranj::Player>(std::string("player2"), shatranj::Color::kBlack));
            EXPECT_EQ(shatranj.PlaySeq({"a2a3", "b7b6", "b2b3", "b6b5", "c2c3", "b5b4", "d2d3"}), true);
            EXPECT_EQ(shatranj.PlaySeq({"b4b3"}), false);
        }
    }
    {
        // piyade can promote to vizier
        shatranj::Shatranj shatranj(
            std::make_shared<shatranj::Player>(std::string("player1"), shatranj::Color::kWhite),
            std::make_shared<shatranj::Player>(std::string("player2"), shatranj::Color::kBlack));
        EXPECT_EQ(shatranj.PlaySeq({"b2b3", "g7g6", "b3b4", "g6g5", "b4b5", "g5g4", "b5b6", "g4g3", "b6c7", "g3h2"}), true);
        EXPECT_EQ(shatranj.PlaySeq({"c7b8", "h2g1"}), true);
        const auto g1p = shatranj.GetBoard()->GetPieces()->GetPiece(shatranj::Position("g1"));
        EXPECT_TRUE((*g1p)->IsVizier());
        const auto b8p = shatranj.GetBoard()->GetPieces()->GetPiece(shatranj::Position("b8"));
        EXPECT_TRUE((*b8p)->IsVizier());

        std::cout << *(shatranj.GetBoard()) << std::endl;
    }
}
TEST(SampleCaptureTest_Rook, Positive)
{
    shatranj::Shatranj shatranj(std::make_shared<shatranj::Player>(std::string("player1"), shatranj::Color::kWhite),
                                std::make_shared<shatranj::Player>(std::string("player2"), shatranj::Color::kBlack));
    EXPECT_EQ(shatranj.PlaySeq({"a2a3", "b7b6", "a3a4", "b6b5", "a4b5", "f7f6", "a1a7", "d7d6"}), true);
    std::cout << *(shatranj.GetBoard()) << std::endl;
    EXPECT_EQ(shatranj.GetBoard()->GetCurrentPlayer()->GetColor(), shatranj::Color::kWhite);
    auto lastcapturedplace = shatranj.GetBoard()->GetPieces()->GetPiece(shatranj::Position("a7"));
    EXPECT_EQ(lastcapturedplace.has_value(), true);
    auto lastcapturedpiece = *lastcapturedplace;
    EXPECT_EQ(lastcapturedpiece->GetSymbol() == 'R', true);
    EXPECT_TRUE(lastcapturedpiece->GetPlayer().lock()->GetColor() == shatranj::Color::kWhite);
    CheckPossibleMoves(shatranj, "a7", {"a7a8", "a7a6", "a7a5", "a7a4", "a7a3", "a7a2", "a7a1", "a7b7", "a7c7"});
}

TEST(SampleCaptureTest_Horse, Positive)
{
    {
        shatranj::Shatranj shatranj(
            std::make_shared<shatranj::Player>(std::string("player1"), shatranj::Color::kWhite),
            std::make_shared<shatranj::Player>(std::string("player2"), shatranj::Color::kBlack));
        CheckPossibleMoves(shatranj, "b1", {"b1a3", "b1c3"});
        EXPECT_EQ(shatranj.PlaySeq({"b1c3"}), true);
        CheckPossibleMoves(shatranj, "c3", {"c3b5", "c3a4", "c3d5", "c3e4", "c3b1"});
        EXPECT_EQ(shatranj.PlaySeq({"b8c5"}), false);
        EXPECT_EQ(shatranj.PlaySeq({"b8c6"}), true);
        CheckPossibleMoves(shatranj, "c6", {"c6b4", "c6a5", "c6e5", "c6d4", "c6b8"});
    }
    {
        shatranj::Shatranj shatranj(
            std::make_shared<shatranj::Player>(std::string("player1"), shatranj::Color::kWhite),
            std::make_shared<shatranj::Player>(std::string("player2"), shatranj::Color::kBlack));
        CheckPossibleMoves(shatranj, "b1", {"b1a3", "b1c3"});
        EXPECT_EQ(shatranj.PlaySeq({"b1c3", "g8f6", "c3d5"}), true);
        CheckPossibleMoves(shatranj, "f6", {"f6d5", "f6e4", "f6g4", "f6h5", "f6g8"});
        EXPECT_EQ(shatranj.PlaySeq({"f6h3"}), false);
        EXPECT_EQ(shatranj.PlaySeq({"f6d5"}), true);
        CheckPossibleMoves(shatranj, "d5", {"d5b6", "d5b4", "d5c3", "d5e3", "d5f4", "d5f6"});
    }
}

} // namespace
