#include "../solution.h"

#include <limits.h>

#include "gtest/gtest.h"
namespace
{

TEST(SolutionSampleTest, Negative)
{
    struct deneme {
        int a;
        bool b;
    };
    struct deneme2 {
        int a;
        char b;
        char c;
        char d;
        char e;
    };
    struct deneme3 {
        char b;
        char c;
        int a;
        char d;
        char e;
    };
    struct alignas(4) deneme4 {
        unsigned int a : 3;
        unsigned int b : 3;
        unsigned int c : 2;
    };
    struct alignas(1) deneme5 {
        unsigned char a : 2;
        unsigned char b : 2;
        unsigned char c : 2;
    };

    EXPECT_EQ(8, sizeof(deneme));
    EXPECT_EQ(8, sizeof(deneme2));
    EXPECT_EQ(12, sizeof(deneme3));
    EXPECT_EQ(4, sizeof(deneme4));
    EXPECT_EQ(1, sizeof(deneme5));

    deneme4 instance4 = {};
    for (int i = 0; i < 10; i++) {
        EXPECT_LE(instance4.a, 7);
        instance4.a++;
    }
}
} // namespace
