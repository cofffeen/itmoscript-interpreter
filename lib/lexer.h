#pragma once
#include "token.h"
#include <iostream>
#include <map>
#include <vector>

class Lexer {
public:
  Lexer(std::istream &code) : code_(code), line_(1) { UpdateSymbol(); }
  
  std::vector<Token> GetTokens();

private:
  std::istream &code_;
  size_t line_;
  std::vector<Token> tokens_;
  char symbol_;

  void UpdateSymbol();
  void SkipWhitespace();
  Token ReadNumber();
  Token ReadString();
  Token ReadStringValue();
  Token NextToken();
};

static std::map<std::string, TokenType> keywords{
    {"+", TokenType::PLUS},
    {"-", TokenType::MINUS},
    {"*", TokenType::MULTIPLY},
    {"/", TokenType::DIVIDE},
    {"%", TokenType::MOD},
    {"^", TokenType::POW},
    {"==", TokenType::EQ},
    {"!=", TokenType::N_EQ},
    {"<", TokenType::LESS},
    {">", TokenType::GREATER},
    {"<=", TokenType::LESS_EQ},
    {">=", TokenType::GREATER_EQ},
    {"and", TokenType::AND},
    {"or", TokenType::OR},
    {"not", TokenType::NOT},
    {"=", TokenType::ASSIGN},
    {"+=", TokenType::PLUS_A},
    {"-=", TokenType::MINUS_A},
    {"*=", TokenType::MULTIPLY_A},
    {"/=", TokenType::DIVIDE_A},
    {"%=", TokenType::MOD_A},
    {"^=", TokenType::POW_A},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"end", TokenType::END},
    {"then", TokenType::THEN},
    {"while", TokenType::WHILE},
    {"for", TokenType::FOR},
    {"in", TokenType::IN},
    {"break", TokenType::BREAK},
    {"continue", TokenType::CONTINUE},
    {"return", TokenType::RETURN},
    {"nil", TokenType::NIL},
    {"true", TokenType::BOOL},
    {"false", TokenType::BOOL},
    {"function", TokenType::FUNCTION},
    {"(", TokenType::L_S_BRACKET},
    {")", TokenType::R_S_BRACKET},
    {"[", TokenType::L_BRACKET},
    {"]", TokenType::R_BRACKET},
    {":", TokenType::COLON},
    {",", TokenType::COMMA},
    {"\0", TokenType::EOFF}};
