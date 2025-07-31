#pragma once

#include "ast.h"
#include "function.h"
#include "lexer.h"
#include "parser.h"
#include "scope.h"
#include "standart_library_func.h"
#include "value.h"
#include <cmath>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <variant>

class Interpret {
public:
  Interpret(std::unique_ptr<AST::BlockNode> root, std::shared_ptr<Scope> global,
            std::ostream &out)
      : root_(std::move(root)), global_(std::move(global)), out_(out) {}

  std::vector<std::string> GetStack();
  static void Print(const Value &v, std::ostream &output);
  static std::vector<std::string> GetStackTrace();

  void Run();

private:
  std::unique_ptr<AST::BlockNode> root_;
  std::ostream &out_;
  std::shared_ptr<Scope> global_;
  static Interpret *current_;
  std::vector<std::string> stack_;

  Value Eval(AST::BaseNode *node);
  Value ProcessingCallNode(AST::CallNode *call);
  Value ProcessingBinOperationNode(AST::BinOperationNode *bin);
  Value ProcessingBlockNode(AST::BlockNode *block);
  Value ProcessingIfNode(AST::IfNode *if_node);
  Value ProcessingWhileNode(AST::WhileNode *while_node);
  Value ProcessingNumber(AST::BinOperationNode *bin, double l, double r);
  Value ProcessingString(AST::BinOperationNode *bin, std::string l,
                         std::string r);
  Value ProcessingNumberString(AST::BinOperationNode *bin, std::string l,
                               double r);
  Value ProcessingListList(AST::BinOperationNode *bin, std::vector<Value> &l,
                           std::vector<Value> &r);
  Value ProcessingListNumber(AST::BinOperationNode *bin, std::vector<Value> &l,
                             double r);
  Value ProcessingIndexNode(AST::IndexNode *index_node);
  Value ProcessingSliceNode(AST::SliceNode *slice_node);
  Value ParseAssignmentNode(AST::AssignmentNode *assignment_node);
  Value ProcessingFunctionNode(AST::FunctionNode *func);
  Value CallUserFunction(std::shared_ptr<Function> func,
                         const std::vector<Value> &args);
};

struct BreakException : std::exception {};
struct ContinueException : std::exception {};
struct ReturnException : std::exception {
  Value value;
  ReturnException(Value value_) : value(value_) {}
};

bool interpret(std::istream &input, std::ostream &output);
