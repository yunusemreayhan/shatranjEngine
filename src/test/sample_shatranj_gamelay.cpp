#include <algorithm>
#include <limits.h>

#include "../position.h"
#include "../shatranj.h"
#include "gtest/gtest.h"

namespace
{

TEST(SampleCaptureTest, Positive)
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
    auto possiblemovesoflastcapturedpiece = lastcapturedpiece->GetPossibleMoves(shatranj.GetBoard());
    EXPECT_TRUE(lastcapturedpiece->GetPlayer().lock()->GetColor() == shatranj::Color::kWhite);
    std::vector<std::string> expectedmoves = {"a7a8", "a7a6", "a7a5", "a7a4", "a7a3", "a7a2", "a7a1", "a7b7", "a7c7"};
    EXPECT_EQ(possiblemovesoflastcapturedpiece.size(), expectedmoves.size());
    for (const auto &move : possiblemovesoflastcapturedpiece)
    {
        const std::string movestring = move.first.ToString() + move.second.ToString();
        std::cout << "expected movestring " << movestring << std::endl;
        EXPECT_TRUE(std::find(expectedmoves.begin(), expectedmoves.end(),
                              movestring) != expectedmoves.end());
    }
    for (const auto &move : expectedmoves)
    {
        std::cout << "checking move " << move << std::endl;
        auto found =
            std::find_if(possiblemovesoflastcapturedpiece.begin(), possiblemovesoflastcapturedpiece.end(),
                         [&move](const std::pair<shatranj::Position, shatranj::Position> &movefromcaptured) -> bool {
                             return movefromcaptured.first.ToString() + movefromcaptured.second.ToString() == move;
                         }) != possiblemovesoflastcapturedpiece.end();
        EXPECT_TRUE(found);
        if (!found)
        {
            std::cout << "not found " << move << std::endl;
        }
    }
}
} // namespace
