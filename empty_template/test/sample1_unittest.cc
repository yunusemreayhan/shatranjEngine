#include "../solution.h"

#include <limits.h>

#include "gtest/gtest.h"
namespace
{

TEST(SolutionSampleTest, Negative)
{
    EXPECT_EQ(-1, solution(1));
}
} // namespace
