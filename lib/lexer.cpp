#include "lexer.h"

std::vector<Token> Lexer::GetTokens() {
  while (tokens_.empty() || tokens_.back().GetType() != TokenType::EOFF) {
    tokens_.push_back(NextToken());
  }

  return tokens_;
}

void Lexer::UpdateSymbol() {
  if (code_.get(symbol_)) {
    if (symbol_ == '\n') {
      ++line_;
    }
  } else {
    symbol_ = '\0';
  }
}

void Lexer::SkipWhitespace() {
  while (isspace(symbol_)) {
    UpdateSymbol();
  }
}

Token Lexer::ReadNumber() {
  SkipWhitespace();

  std::string number = "";
  while (isdigit(symbol_) || symbol_ == '-' || symbol_ == 'e' ||
         symbol_ == 'E' || symbol_ == '.' || symbol_ == '+') {
    number.push_back(symbol_);
    UpdateSymbol();
  }

  return Token(TokenType::NUMBER, number);
}

Token Lexer::ReadString() {
  SkipWhitespace();

  std::string str = "";
  while (isalpha(symbol_) || isdigit(symbol_) || symbol_ == '_') {
    str.push_back(symbol_);
    UpdateSymbol();
  }

  if (keywords.find(str) != keywords.end()) {
    return Token(keywords[str], str);
  }

  return Token(TokenType::IDENTIFIER, str);
}

Token Lexer::ReadStringValue() {
  std::string str = "";
  while (symbol_ != '\"') {
    str += symbol_;
    UpdateSymbol();
  }
  UpdateSymbol();
  return Token(TokenType::STRING, str);
}

Token Lexer::NextToken() {
  SkipWhitespace();

  if (symbol_ == '\0') {
    UpdateSymbol();
    return Token(TokenType::EOFF, "\0");
  }

  if (symbol_ == '\"') {
    UpdateSymbol();
    return ReadStringValue();
  }

  if (isdigit(symbol_)) {
    return ReadNumber();
  }
  
  if (isalpha(symbol_)) {
    return ReadString();
  }

  std::string word = "";
  word.push_back(symbol_);
  UpdateSymbol();
  word.push_back(symbol_);

  if (word == "//") {
    while (symbol_ != '\n' && symbol_ != '\0') {
      UpdateSymbol();
    }
    return NextToken();
  }

  if (keywords.find(word) != keywords.end()) {
    UpdateSymbol();
    return Token(keywords[word], word);
  }
  word.pop_back();
  if (keywords.find(word) != keywords.end()) {
    return Token(keywords[word], word);
  }

  throw std::runtime_error("Unknown symbol '" + std::to_string(word.front()) +
                           "' at line " + std::to_string(line_) + "\n");
}
