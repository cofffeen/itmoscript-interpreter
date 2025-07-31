#include <gtest/gtest.h>
#include <lib/interpreter.h>

TEST(TypesTestSuite, OrderTest1) {
    std::istringstream input("x = 1 + 2 * 3 \n print(x) \n");
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), "7");
}

TEST(TypesTestSuite, OrderTest2) {
    std::istringstream input("x = (1 + 2) * 3 \n print(x) \n");
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), "9");
}

