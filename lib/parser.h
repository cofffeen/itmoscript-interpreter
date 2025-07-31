#pragma once

#include "ast.h"
#include <map>

class Parser {
public:
  Parser(std::vector<Token> tokens) : tokens_(tokens), pos_(0) {}

  std::unique_ptr<AST::BlockNode> ParseCode();

private:
  std::vector<Token> tokens_;
  size_t pos_;
  std::map<std::string, std::string> scope_;
  std::stack<TokenType> expected_;

  Token Match(std::vector<TokenType>&& expected_types);
  Token Require(std::vector<TokenType>&& expected_types);

  std::unique_ptr<AST::BaseNode> ParsePrimary();
  std::unique_ptr<AST::BaseNode> ParseUnary();
  int GetPriority(TokenType type);
  std::unique_ptr<AST::BaseNode> ParseBin(int max_priority = 0);
  std::unique_ptr<AST::BaseNode> ParseStatement();
  bool NextTokenIsOperation();

  std::unique_ptr<AST::BaseNode> ParseAssignment();
  std::unique_ptr<AST::ReturnNode> ParseReturn();
  std::unique_ptr<AST::BreakNode> ParseBreak();
  std::unique_ptr<AST::ContinueNode> ParseContinue();
  std::unique_ptr<AST::BlockNode>
  ParseCodeUntil(std::vector<TokenType>&& end_token_types);
  std::unique_ptr<AST::WhileNode> ParseWhile();
  std::unique_ptr<AST::ForNode> ParseFor();
  std::unique_ptr<AST::IfNode> ParseIf();
  std::unique_ptr<AST::FunctionNode> ParseFunction();
  std::unique_ptr<AST::BaseNode> ParseLogic();
  std::unique_ptr<AST::BaseNode>
  ParseIndexAndSlice(std::unique_ptr<AST::BaseNode> var);

  bool Accept(std::vector<TokenType>&& valid_types);
};

static std::map<TokenType, int> priority{
  {TokenType::OR, 1},         {TokenType::AND, 2},
  {TokenType::EQ, 3},         {TokenType::GREATER, 3},
  {TokenType::GREATER_EQ, 3}, {TokenType::LESS, 3},
  {TokenType::LESS_EQ, 3},    {TokenType::N_EQ, 3},
  {TokenType::PLUS, 4},       {TokenType::PLUS_A, 4},
  {TokenType::MINUS, 4},      {TokenType::MINUS_A, 4},
  {TokenType::MULTIPLY, 5},   {TokenType::MULTIPLY_A, 5},
  {TokenType::DIVIDE, 5},     {TokenType::DIVIDE_A, 5},
  {TokenType::MOD, 5},        {TokenType::MOD_A, 5},
  {TokenType::POW, 6},        {TokenType::POW_A, 6},
};