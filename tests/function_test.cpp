#include <gtest/gtest.h>
#include <lib/interpreter.h>

TEST(FunctionTestSuite, Fibonacci) {
  std::string code = R"(
          fib = function(n)
              if n == 0 then
                  return 0
              end if
  
              a = 0
              b = 1
  
              for i in range(1,n,1) then
                  c = a + b
                  a = b
                  b = c
              end for
  
              return b
          end function
  
          print(fib(10))
      )";

  std::string expected = "55";

  std::istringstream input(code);
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), expected);
}

TEST(FunctionTestSuite, FizzBuzz) {
  std::string code = R"(
      fizzBuzz = function(n)
          for i in range(1, n, 1) then
              s = "Fizz" * (i % 3 == 0) + "Buzz" * (i % 5 == 0)
              if s == "" then
                  print(i)
              else
                  print(s)
              end if
          end for
      end function
  
      fizzBuzz(100)
        )";

  std::string expected =
      "12Fizz4BuzzFizz78FizzBuzz11Fizz1314FizzBuzz1617Fizz19BuzzFizz2223FizzBuz"
      "z26Fizz2829FizzBuzz3132Fizz34BuzzFizz3738FizzBuzz41Fizz4344FizzBuzz4647F"
      "izz49BuzzFizz5253FizzBuzz56Fizz5859FizzBuzz6162Fizz64BuzzFizz6768FizzBuz"
      "z71Fizz7374FizzBuzz7677Fizz79BuzzFizz8283FizzBuzz86Fizz8889FizzBuzz9192F"
      "izz94BuzzFizz9798Fizz";

  std::istringstream input(code);
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), expected);
}

TEST(FunctionTestSuite, Maximum) {
  std::string code = R"(
      max = function(arr)
          if len(arr) == 0 then
              return nil
          end if
  
          m = arr[0]
  
          for i in arr then
              if i > m then m = i end if
          end for
  
          return m
      end function
  
      print(max([10, -1, 0, 2, 2025, 239]))
      )";

  std::string expected = "2025";

  std::istringstream input(code);
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), expected);
}

TEST(FunctionTestSuite, SimpleFunctionTest) {
  std::string code = R"(
        incr = function(value)
            return value + 1
        end function

        x = incr(2)
        print(x)
    )";

  std::string expected = "3";

  std::istringstream input(code);
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), expected);
}

TEST(FunctionTestSuite, FunctionAsArgTest) {
  std::string code = R"(
        incr = function(value)
            return value + 1
        end function

        printresult = function(value, func)
            result = func(value)
            print(result)
        end function

        printresult(2, incr)
    )";

  std::string expected = "3";

  std::istringstream input(code);
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), expected);
}

TEST(FunctionTestSuite, NestedFunctionTest) {
  std::string code = R"(
        // NB: inner and outer `value` are different symbols.
        // You are not required to implement closures (aka lambdas).

        incr_and_print = function(value)
            incr = function(value)
                return value + 1
            end function

            print(incr(value))
        end function

        incr_and_print(2)
    )";

  std::string expected = "3";

  std::istringstream input(code);
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), expected);
}

TEST(FunctionTestSuite, FunnySyntaxTest) {
  std::string code = R"(
        func1 = function() return 1 end function
        func2 = function() return 2 end function
        func3 = function() return 3 end function
        
        funcs = [func1(), func2(), func3()]

        print(funcs[0])
        print(funcs[1])
        print(funcs[2])
    )";

  std::string expected = "123";

  std::istringstream input(code);
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), expected);
}

TEST(FunctionTestSuite, Abs) {
  std::istringstream input("a = -239 \n d = abs(a) print(d)");
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), "239");
}

TEST(FunctionTestSuite, Ceil) {
  std::istringstream input("a = 2.39 \n d = ceil(a) print(d)");
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), "3");
}

TEST(FunctionTestSuite, Floor) {
  std::istringstream input("a = 2.39 \n d = floor(a) print(d)");
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), "2");
}

TEST(FunctionTestSuite, Round) {
  std::istringstream input("a = 2.39 \n d = round(a) print(d)");
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), "2");
}

TEST(FunctionTestSuite, Sqrt) {
  std::istringstream input("a = 239.239 \n d = sqrt(a) print(d)");
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), "15.4674");
}

TEST(FunctionTestSuite, ParseNum) {
  std::istringstream input("a = \"239\" \n d = parse_num(a) print(d)");
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), "239");
}

TEST(FunctionTestSuite, Len) {
  std::istringstream input("a = \"239abc\" \n d = len(a) print(d)");
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), "6");
}

TEST(FunctionTestSuite, Lower) {
  std::istringstream input("a = \"AaAaA\" \n d = lower(a) print(d)");
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), "aaaaa");
}

TEST(FunctionTestSuite, Upper) {
  std::istringstream input("a = \"AaAaA\" \n d = upper(a) print(d)");
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), "AAAAA");
}

TEST(FunctionTestSuite, Split) {
  std::istringstream input(
      "a = \"A a A a A\" \n b = \" \" \n d = split(a, b) print(d)");
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), "[A, a, A, a, A]");
}

TEST(FunctionTestSuite, Join) {
  std::istringstream input(
      "a = [\"2\", \"3\", \"9\"] \n b = \" \" \n d = join(a, b) print(d)");
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), "2 3 9");
}

TEST(FunctionTestSuite, Replace) {
  std::istringstream input("a = \"AAAbbBB\" \n b = \"bbBB\" \n c = \"aaa\" \n "
                           "d = replace(a, b, c) print(d)");
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), "AAAaaa");
}

TEST(FunctionTestSuite, Range) {
  std::istringstream input(
      "a = 2 \n b = 9 \n c = 3 \n d = range(a, b, c) print(d)");
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), "[2, 5, 8]");
}

TEST(FunctionTestSuite, LenList) {
  std::istringstream input("a = [2, 23, 239] d = len(a) print(d)");
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), "3");
}

TEST(FunctionTestSuite, Push) {
  std::istringstream input(
      "a = [2, 23, 239] \n b = 239239 \n d = push(a, b) print(d)");
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), "[2, 23, 239, 239239]");
}

TEST(FunctionTestSuite, Pop) {
  std::istringstream input("a = [2, 23, 239] \n d = pop(a) print(d)");
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), "239");
}

TEST(FunctionTestSuite, Insert) {
  std::istringstream input(
      "a = [2, 23, 239] \n b = 2 \n c = 3 \n d = insert(a, b, c) print(d)");
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), "[2, 23, 3, 239]");
}

TEST(FunctionTestSuite, Remove) {
  std::istringstream input(
      "a = [2, 23, 239] \n b = 2 \n d = remove(a, b) print(d)");
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), "[2, 23]");
}

TEST(FunctionTestSuite, Sort) {
  std::istringstream input("a = [2, 239, 23] \n d = sort(a) print(d)");
  std::ostringstream output;

  ASSERT_TRUE(interpret(input, output));
  ASSERT_EQ(output.str(), "[2, 23, 239]");
}
