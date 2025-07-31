#include "parser.h"

std::unique_ptr<AST::BlockNode> Parser::ParseCode() {
  std::unique_ptr<AST::BlockNode> root = std::make_unique<AST::BlockNode>();
  while (pos_ < tokens_.size() - 1) {
    root->AddNode(ParseStatement());
  }

  return root;
}

Token Parser::Match(std::vector<TokenType>&& expected_types) {
  if (pos_ < tokens_.size()) {
    TokenType current_token_type = tokens_[pos_].GetType();
    for (TokenType expected_type : expected_types) {
      if (current_token_type == expected_type) {
        ++pos_;

        return tokens_[pos_ - 1];
      }
    }
  }

  return Token(TokenType::NIL, "nil");
}

Token Parser::Require(std::vector<TokenType>&& expected_types) {
  Token current_token = Match(std::move(expected_types));

  if (current_token.GetType() == TokenType::NIL) {
    throw std::runtime_error("Expected another token " +
                             std::to_string(pos_) + "\n");
  }
  
  return current_token;
}

std::unique_ptr<AST::BaseNode> Parser::ParsePrimary() {
  Token token = tokens_[pos_];

  if (token.GetType() == TokenType::NIL) {
    ++pos_;

    return std::make_unique<AST::NilNode>(token);
  }

  if (token.GetType() == TokenType::NUMBER) {
    ++pos_;
    
    return std::make_unique<AST::NumberNode>(token);
  }

  if (token.GetType() == TokenType::STRING) {
    ++pos_;

    std::unique_ptr<AST::StringNode> string = std::make_unique<AST::StringNode>(token);
    if (pos_ < tokens_.size() &&
        tokens_[pos_].GetType() == TokenType::L_BRACKET) {

      return ParseIndexAndSlice(std::move(string));
    }

    return std::move(string);
  }

  if (token.GetType() == TokenType::BOOL) {
    ++pos_;

    return std::make_unique<AST::BoolNode>(token);
  }

  if (token.GetType() == TokenType::IDENTIFIER) {
    ++pos_;
    if (pos_ < tokens_.size() &&
        tokens_[pos_].GetType() == TokenType::L_S_BRACKET) {
          Require({TokenType::L_S_BRACKET});
      std::vector<std::unique_ptr<AST::BaseNode>> args;
      while (pos_ < tokens_.size() &&
             tokens_[pos_].GetType() != TokenType::R_S_BRACKET) {
        if (tokens_[pos_].GetType() == TokenType::COMMA) {
          ++pos_;
        }
        args.emplace_back(ParsePrimary());
      }
      Require({TokenType::R_S_BRACKET});
      return std::make_unique<AST::CallNode>(token.GetValue(),
                                        std::make_unique<AST::VariableNode>(token),
                                        std::move(args));
    } else if (pos_ < tokens_.size() &&
               tokens_[pos_].GetType() == TokenType::L_BRACKET) {
      std::unique_ptr<AST::VariableNode> string =
          std::make_unique<AST::VariableNode>(token);
      return ParseIndexAndSlice(std::move(string));
    } else {
      return std::make_unique<AST::VariableNode>(token);
    }
  }

  if (token.GetType() == TokenType::L_S_BRACKET) {
    ++pos_;
    auto expr = ParseBin();
    Require({TokenType::R_S_BRACKET});
    return expr;
  }

  if (token.GetType() == TokenType::L_BRACKET) {
    if ((tokens_[pos_ - 1].GetType() != TokenType::IDENTIFIER) &&
        (tokens_[pos_ - 1].GetType() != TokenType::STRING)) { // list
          Require({TokenType::L_BRACKET});
      std::vector<std::unique_ptr<AST::BaseNode>> elements;
      while (pos_ < tokens_.size() &&
             tokens_[pos_].GetType() != TokenType::R_BRACKET) {
        if (tokens_[pos_].GetType() == TokenType::COMMA) {
          ++pos_;
        }
        elements.emplace_back(ParseBin());
      }
      Require({TokenType::R_BRACKET});

      return std::make_unique<AST::ListNode>(std::move(elements));
    }
  }

  if (token.GetType() == TokenType::FUNCTION) {
    return ParseFunction();
  }

  throw std::runtime_error("Expected number, string, logical statement, method or variable at " +
                           std::to_string(pos_));
}

std::unique_ptr<AST::BaseNode>
Parser::ParseIndexAndSlice(std::unique_ptr<AST::BaseNode> var) {
  Require({TokenType::L_BRACKET});

  std::unique_ptr<AST::BaseNode> start =
      std::make_unique<AST::NilNode>(Token(TokenType::NIL, "nil"));
  if (tokens_[pos_].GetType() != TokenType::COLON) {
    start = ParseBin();
  }
  if (tokens_[pos_].GetType() == TokenType::COLON) {
    Require({TokenType::COLON});
    std::unique_ptr<AST::BaseNode> end =
        std::make_unique<AST::NilNode>(Token(TokenType::NIL, "nil"));
    if (tokens_[pos_].GetType() != TokenType::R_BRACKET) {
      end = ParseBin();
    }
    Require({TokenType::R_BRACKET});
    return std::make_unique<AST::SliceNode>(std::move(var), std::move(start),
                                       std::move(end));
  }
  Require({TokenType::R_BRACKET});
  return std::make_unique<AST::IndexNode>(std::move(var), std::move(start));
}

std::unique_ptr<AST::BaseNode> Parser::ParseUnary() {
  Token token = tokens_[pos_];
  if (token.GetType() == TokenType::MINUS ||
      token.GetType() == TokenType::PLUS) {
    ++pos_;
    std::unique_ptr<AST::BaseNode> node = ParseUnary();
    return std::make_unique<AST::UnaryOperationNode>(token, std::move(node));
  }

  if (token.GetType() == TokenType::NOT) {
    ++pos_;
    std::unique_ptr<AST::BaseNode> node = ParseUnary();
    return std::make_unique<AST::UnaryOperationNode>(token, std::move(node));
  }
  
  return ParsePrimary();
}

int Parser::GetPriority(TokenType type) {
  if(priority.find(type) != priority.end()){
    return priority[type];
  }

  return 0;
}

std::unique_ptr<AST::BaseNode> Parser::ParseBin(int max_priority) {
  std::unique_ptr<AST::BaseNode> left = ParseUnary();

  while (pos_ < tokens_.size()) {
    Token token = tokens_[pos_];
    int priority = GetPriority(token.GetType());

    if ((priority == 0) || (priority < max_priority)) {
      break;
    }

    ++pos_;

    std::unique_ptr<AST::BaseNode> right = ParseBin(priority + 1);

    left = std::make_unique<AST::BinOperationNode>(std::move(left), token,
                                              std::move(right));
  }

  return left;
}

std::unique_ptr<AST::BaseNode> Parser::ParseStatement() {
  Token token = tokens_[pos_];
  switch (token.GetType()) {
  case TokenType::IF:
    return ParseIf();
  case TokenType::FOR:
    return ParseFor();
  case TokenType::WHILE:
    return ParseWhile();
  case TokenType::RETURN:
    return ParseReturn();
  case TokenType::BREAK:
    return ParseBreak();
  case TokenType::CONTINUE:
    return ParseContinue();
  case TokenType::FUNCTION:
    return ParseFunction();
  case TokenType::IDENTIFIER:
    if (NextTokenIsOperation()) {
      return ParseAssignment();
    } else {
      std::unique_ptr<AST::BaseNode> expr = ParseBin();
      return std::move(expr);
    }
  default:
    std::unique_ptr<AST::BaseNode> expr = ParseBin();
    return expr;
  }
}

bool Parser::NextTokenIsOperation() {
  size_t save_pos = pos_;
  if ((pos_ + 1) >= tokens_.size()) {
    return false;
  }

  ++pos_;
  Token nextToken =
  Match({TokenType::ASSIGN, TokenType::DIVIDE_A, TokenType::MINUS_A,
             TokenType::MOD_A, TokenType::MULTIPLY_A, TokenType::PLUS_A,
             TokenType::POW_A, TokenType::EOFF});
  pos_ = save_pos;
  return (nextToken.GetType() != TokenType::NIL);
}

std::unique_ptr<AST::BaseNode> Parser::ParseAssignment() {
  Token variable = Require({TokenType::IDENTIFIER});
  Token operation =
  Require({TokenType::ASSIGN, TokenType::DIVIDE_A, TokenType::MINUS_A,
               TokenType::MOD_A, TokenType::MULTIPLY_A, TokenType::PLUS_A,
               TokenType::POW_A});
  std::unique_ptr<AST::BaseNode> value = ParseBin();

  return std::make_unique<AST::AssignmentNode>(operation, variable,
                                          std::move(value));
}

std::unique_ptr<AST::ReturnNode> Parser::ParseReturn() {
  Require({TokenType::RETURN});
  std::unique_ptr<AST::BaseNode> res;
  if ((pos_ < tokens_.size() - 1)) {
    res = ParseBin();
  }
  if (res == nullptr) {
    return std::make_unique<AST::ReturnNode>();
  }
  return std::make_unique<AST::ReturnNode>(std::move(res));
}

std::unique_ptr<AST::BreakNode> Parser::ParseBreak() {
  Require({TokenType::BREAK});
  return std::make_unique<AST::BreakNode>();
}

std::unique_ptr<AST::ContinueNode> Parser::ParseContinue() {
  Require({TokenType::CONTINUE});
  return std::make_unique<AST::ContinueNode>();
}

std::unique_ptr<AST::BlockNode>
Parser::ParseCodeUntil(std::vector<TokenType>&&  end_token_types) {
  std::unique_ptr<AST::BlockNode> block_node = std::make_unique<AST::BlockNode>();

  while ((pos_ < tokens_.size() - 1) &&
         std::find(end_token_types.begin(), end_token_types.end(),
                   tokens_[pos_].GetType()) == end_token_types.end()) {
    block_node->AddNode(ParseStatement());
  }

  return block_node;
}

std::unique_ptr<AST::WhileNode> Parser::ParseWhile() {
  Require({TokenType::WHILE});
  std::unique_ptr<AST::BaseNode> conditional = ParseCodeUntil({TokenType::THEN});
  Require({TokenType::THEN});
  std::unique_ptr<AST::BlockNode> then = ParseCodeUntil({TokenType::END});
  Require({TokenType::END});
  Require({TokenType::WHILE});

  return std::make_unique<AST::WhileNode>(std::move(conditional), std::move(then));
}

std::unique_ptr<AST::ForNode> Parser::ParseFor() {
  Require({TokenType::FOR});
  Token iter = Require({TokenType::IDENTIFIER});
  Require({TokenType::IN});
  std::unique_ptr<AST::BaseNode> conditional = ParseCodeUntil({TokenType::THEN});
  Require({TokenType::THEN});
  std::unique_ptr<AST::BlockNode> then = ParseCodeUntil({TokenType::END});
  Require({TokenType::END});
  Require({TokenType::FOR});

  return std::make_unique<AST::ForNode>(iter, std::move(conditional),
                                   std::move(then));
}

std::unique_ptr<AST::IfNode> Parser::ParseIf() {
  Require({TokenType::IF});
  std::unique_ptr<AST::BlockNode> conditional = ParseCodeUntil({TokenType::THEN});
  Require({TokenType::THEN});

  std::unique_ptr<AST::BlockNode> then_block =
      ParseCodeUntil({TokenType::END, TokenType::ELSE});
  std::vector<std::pair<std::unique_ptr<AST::BaseNode>, std::unique_ptr<AST::BlockNode>>>
      else_if;
  size_t save_pos = pos_;

  while ((Match({TokenType::ELSE}).GetType() != TokenType::NIL) &&
         (Match({TokenType::IF}).GetType() != TokenType::NIL)) {
    save_pos = pos_;
    std::unique_ptr<AST::BaseNode> new_conditional =
        ParseCodeUntil({TokenType::THEN});
        Require({TokenType::THEN});

    std::unique_ptr<AST::BlockNode> new_then_block =
        ParseCodeUntil({TokenType::END, TokenType::ELSE});
    else_if.emplace_back(std::move(new_conditional), std::move(new_then_block));
    save_pos = pos_;
  }

  std::unique_ptr<AST::BlockNode> else_block = nullptr;
  pos_ = save_pos;
  if (Match({TokenType::ELSE}).GetType() != TokenType::NIL) {
    else_block = ParseCodeUntil({TokenType::END});
  }

  Require({TokenType::END});
  Require({TokenType::IF});

  return (std::make_unique<AST::IfNode>(std::move(conditional),
                                   std::move(then_block), std::move(else_if),
                                   std::move(else_block)));
}

std::unique_ptr<AST::FunctionNode> Parser::ParseFunction() {
  Require({TokenType::FUNCTION});
  Require({TokenType::L_S_BRACKET});
  std::vector<std::unique_ptr<AST::BaseNode>> args;
  if (tokens_[pos_].GetType() != TokenType::R_S_BRACKET) {
    do {
      args.push_back(ParseBin());
    } while (Accept({TokenType::COMMA}));
  }
  Require({TokenType::R_S_BRACKET});

  std::unique_ptr<AST::BlockNode> then = ParseCodeUntil({TokenType::END});
  Require({TokenType::END});
  Require({TokenType::FUNCTION});

  return std::make_unique<AST::FunctionNode>(std::move(args), std::move(then));
}

bool Parser::Accept(std::vector<TokenType>&&  valid_types) {
  for (int i = 0; i < valid_types.size(); ++i) {
    if (tokens_[pos_].GetType() == valid_types[i]) {
      ++pos_;

      return true;
    }
  }

  return false;
}
