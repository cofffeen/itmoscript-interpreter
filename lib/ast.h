#pragma once

#include "lexer.h"

namespace AST {

struct BaseNode {
    BaseNode() = default;
  virtual ~BaseNode() = default;
};

struct VariableNode : public BaseNode {
  VariableNode(Token var) : variable(var) {}

  Token variable;
};

struct NumberNode : public BaseNode {
  NumberNode(Token num) : number(num) {}

  Token number;
};

struct StringNode : public BaseNode {
  StringNode(Token str) : string(str) {}

  Token string;
};

struct NilNode : public BaseNode {
  NilNode(Token nil_token) : nil(nil_token) {};

  Token nil;
};

struct BoolNode : public BaseNode {
  BoolNode(Token bool_token) : _bool(bool_token) {}

  Token _bool;
};

struct CallNode : public BaseNode {
  CallNode(std::string function, std::unique_ptr<BaseNode> obj,
           std::vector<std::unique_ptr<BaseNode>> arguments)
      : func(function), object(std::move(obj)),
        args(std::move(arguments)) {}

  std::string func;
  std::unique_ptr<BaseNode> object;
  std::vector<std::unique_ptr<BaseNode>> args;
};

struct ListNode : public BaseNode {
  ListNode(std::vector<std::unique_ptr<BaseNode>> list_arg)
      : list(std::move(list_arg)) {}

  std::vector<std::unique_ptr<BaseNode>> list;
};

struct IndexNode : public BaseNode {
  IndexNode(std::unique_ptr<BaseNode> obj, std::unique_ptr<BaseNode> ind)
      : object(std::move(obj)), index(std::move(ind)) {}

  std::unique_ptr<BaseNode> object;
  std::unique_ptr<BaseNode> index;
};

struct SliceNode : public BaseNode {
  SliceNode(std::unique_ptr<BaseNode> obj, std::unique_ptr<BaseNode> start_arg,
            std::unique_ptr<BaseNode> end_arg)
      : object(std::move(obj)), start(std::move(start_arg)),
        end(std::move(end_arg)) {}

  std::unique_ptr<BaseNode> object;
  std::unique_ptr<BaseNode> start;
  std::unique_ptr<BaseNode> end;
};

struct BinOperationNode : public BaseNode {
  BinOperationNode(std::unique_ptr<BaseNode> left_node, Token &operation_arg,
                   std::unique_ptr<BaseNode> right_node)
      : operation(operation_arg), left(std::move(left_node)),
        right(std::move(right_node)) {}

  Token operation;
  std::unique_ptr<BaseNode> left;
  std::unique_ptr<BaseNode> right;
};

struct UnaryOperationNode : public BaseNode {
  UnaryOperationNode(Token &operation_arg, std::unique_ptr<BaseNode> node_arg)
      : operation(operation_arg), node(std::move(node_arg)) {}

  Token operation;
  std::unique_ptr<BaseNode> node;
};

struct BlockNode : public BaseNode {
  BlockNode() = default;

  void AddNode(std::unique_ptr<BaseNode> node) {
    nodes.emplace_back(std::move(node));
  }

  std::vector<std::unique_ptr<BaseNode>> nodes;
};

struct AssignmentNode : public BaseNode {
  AssignmentNode(Token operation_arg, Token var,
                 std::unique_ptr<BaseNode> val)
      : operation(operation_arg), variable(var), value(std::move(val)) {}

  Token operation;
  Token variable;
  std::unique_ptr<BaseNode> value;
};

struct IfNode : public BaseNode {
  IfNode(std::unique_ptr<BaseNode> conditional_arg,
         std::unique_ptr<BlockNode> then_arg,
         std::vector<
             std::pair<std::unique_ptr<BaseNode>, std::unique_ptr<BlockNode>>>
             else_if_arg,
         std::unique_ptr<BlockNode> else_arg)
      : conditional(std::move(conditional_arg)), then(std::move(then_arg)),
        else_if(std::move(else_if_arg)), eelse(std::move(else_arg)) {}

  std::unique_ptr<BaseNode> conditional;
  std::unique_ptr<BlockNode> then;
  std::vector<std::pair<std::unique_ptr<BaseNode>, std::unique_ptr<BlockNode>>>
      else_if;
  std::unique_ptr<BlockNode> eelse;
};

struct WhileNode : public BaseNode {
  WhileNode(std::unique_ptr<BaseNode> conditional_arg,
            std::unique_ptr<BlockNode> then_arg)
      : conditional(std::move(conditional_arg)), then(std::move(then_arg)) {}

  std::unique_ptr<BaseNode> conditional;
  std::unique_ptr<BlockNode> then;
};

struct ForNode : public BaseNode {
  ForNode(Token it, std::unique_ptr<BaseNode> conditional_arg,
          std::unique_ptr<BlockNode> then_arg)
      : iterator(it), conditional(std::move(conditional_arg)),
        then(std::move(then_arg)) {}

  Token iterator;
  std::unique_ptr<BaseNode> conditional;
  std::unique_ptr<BlockNode> then;
};

struct FunctionNode : public BaseNode {
  FunctionNode(std::vector<std::unique_ptr<BaseNode>> args_arg,
               std::unique_ptr<BlockNode> then_arg)
      : args(std::move(args_arg)), then(std::move(then_arg)) {}

  std::vector<std::unique_ptr<BaseNode>> args;
  std::unique_ptr<BlockNode> then;
};

struct BreakNode : public BaseNode {
  BreakNode() = default;
};

struct ContinueNode : public BaseNode {
  ContinueNode() = default;
};

struct ReturnNode : public BaseNode {
  ReturnNode(std::unique_ptr<BaseNode> val) : value(std::move(val)) {}
  ReturnNode() : value(nullptr) {};

  std::unique_ptr<BaseNode> value;
};

} // namespace AST
