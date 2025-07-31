#include <lib/interpreter.h>
#include <gtest/gtest.h>

TEST(TypesTestSuite, IntTest) {
    std::string code = R"(
        x = 1
        y = 2
        z = 3 * x + y
        print(z)
    )";

    std::string expected = "5";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(TypesTestSuite, StringTest) {
    std::string code = R"(
        x = "2"*2
        y = x + "3"*2 + "9"*2
        print(y)
    )";

    std::string expected = "223399";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
