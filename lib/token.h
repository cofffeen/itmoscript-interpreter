#pragma once

#include <iostream>
#include "token_type.h"

class Token {
public:
  Token(TokenType type, std::string value)
      : type_(type), value_(value) {}

  TokenType GetType();
  std::string GetValue();

private:
  TokenType type_;
  std::string value_;
};
