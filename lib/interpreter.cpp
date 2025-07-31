#include "interpreter.h"

Interpret *Interpret::current_ = nullptr;

void Interpret::Run() {
  Eval(root_.get());
}

Value Interpret::Eval(AST::BaseNode *node) {
  if (auto num = dynamic_cast<AST::NumberNode *>(node)) {
    return Value(std::stod(num->number.GetValue()));
  }

  if (auto var = dynamic_cast<AST::VariableNode *>(node)) {
    return Value(global_->LookUp(var->variable.GetValue()));
  }

  if (auto str = dynamic_cast<AST::StringNode *>(node)) {
    return Value(str->string.GetValue());
  }

  if (auto str = dynamic_cast<AST::NilNode *>(node)) {
    return Value(nullptr);
  }

  if (auto bool_node = dynamic_cast<AST::BoolNode *>(node)) {
    std::string value = bool_node->_bool.GetValue();
    if (value == "true") {
      return Value(true);
    } else {
      return Value(false);
    }
  }

  if (auto list_node = dynamic_cast<AST::ListNode *>(node)) {
    std::vector<Value> list;
    for (const auto &elem : list_node->list) {
      list.push_back(Eval(elem.get()));
    }
    return Value::MakeList(std::move(list));
  }

  if (auto index_node = dynamic_cast<AST::IndexNode *>(node)) {
    return ProcessingIndexNode(index_node);
  }

  if (auto slice_node = dynamic_cast<AST::SliceNode *>(node)) {
    return ProcessingSliceNode(slice_node);
  }

  if (auto unary_node = dynamic_cast<AST::UnaryOperationNode *>(node)) {
    Value value = Eval(unary_node->node.get());
    switch (unary_node->operation.GetType()) {
    case TokenType::MINUS:
      if (!std::holds_alternative<double>(value)) {
        throw std::runtime_error("Unary operations are only for numbers");
      }
      return -std::get<double>(value);
    }
  }

  if (auto block = dynamic_cast<AST::BlockNode *>(node)) {
    return ProcessingBlockNode(block);
  }

  if (auto assignment_node = dynamic_cast<AST::AssignmentNode *>(node)) {
    return ParseAssignmentNode(assignment_node);
  }

  if (auto if_node = dynamic_cast<AST::IfNode *>(node)) {
    return ProcessingIfNode(if_node);
  }

  if (auto while_node = dynamic_cast<AST::WhileNode *>(node)) {
    return ProcessingWhileNode(while_node);
  }

  if (auto for_node = dynamic_cast<AST::ForNode *>(node)) {
    Value iterable = Eval(for_node->conditional.get());

    if (!std::holds_alternative<std::vector<Value>>(iterable)) {
      throw std::runtime_error("Error in for");
    }

    const std::string &var_name = for_node->iterator.GetValue();
    const auto &list = std::get<std::vector<Value>>(iterable);

    for (const Value &item : list) {
      global_->Assign(var_name, item);
      Eval(for_node->then.get());
    }

    return Value();
  }

  if (auto str = dynamic_cast<AST::BreakNode *>(node)) {
    throw BreakException{};
  }

  if (auto str = dynamic_cast<AST::ContinueNode *>(node)) {
    throw ContinueException{};
  }

  if (auto func = dynamic_cast<AST::FunctionNode *>(node)) {
    return ProcessingFunctionNode(func);
  }

  if (auto return_node = dynamic_cast<AST::ReturnNode *>(node)) {
    Value value = Eval((return_node->value).get());
    throw ReturnException(value);
  }

  if (auto call = dynamic_cast<AST::CallNode *>(node)) {
    return ProcessingCallNode(call);
  }

  if (auto bin = dynamic_cast<AST::BinOperationNode *>(node)) {
    return ProcessingBinOperationNode(bin);
  }

  throw std::runtime_error("Unknown AST Node");
}

Value Interpret::ProcessingCallNode(AST::CallNode *call) {
  stack_.push_back(call->func);
  auto funcVal = global_->LookUp(call->func);
  std::vector<Value> args;
  for (const auto &elem : call->args) {
    args.push_back(Eval(elem.get()));
  }
  if (auto fn = std::get_if<std::function<Value(const std::vector<Value> &)>>(
          &funcVal)) {
    stack_.pop_back();
    return (*fn)(args);
  }
  if (auto fn = std::get_if<std::shared_ptr<Function>>(&funcVal)) {
    auto result = CallUserFunction(*fn, args);
    stack_.pop_back();
    return result;
  }

  throw std::runtime_error(call->func + " is not a function");
}

Value Interpret::ProcessingBinOperationNode(AST::BinOperationNode *bin) {
  auto left = Eval(bin->left.get());
  auto right = Eval(bin->right.get());
  if (auto l = std::get_if<std::string>(&left)) {
    if (auto r = std::get_if<std::string>(&right)) {
      return ProcessingString(bin, *l, *r);
    }
  }

  if (auto l = std::get_if<double>(&left)) {
    if (auto r = std::get_if<double>(&right)) {
      return ProcessingNumber(bin, *l, *r);
    }
  }

  if (auto l = std::get_if<std::string>(&left)) {
    if (auto r = std::get_if<double>(&right)) {
      return ProcessingNumberString(bin, *l, *r);
    }
  }

  if (auto l = std::get_if<std::string>(&left)) {
    if (auto r = std::get_if<bool>(&right)) {
      return ProcessingNumberString(bin, *l, *r);
    }
  }

  if (auto l = std::get_if<std::vector<Value>>(&left)) {
    if (auto r = std::get_if<std::vector<Value>>(&right)) {
      return ProcessingListList(bin, *l, *r);
    }
  }

  if (auto l = std::get_if<std::vector<Value>>(&left)) {
    if (auto r = std::get_if<double>(&right)) {
      return ProcessingListNumber(bin, *l, *r);
    }
  }

  throw std::runtime_error("Unknown operation");
}

Value Interpret::ProcessingBlockNode(AST::BlockNode *block) {
  Value last = nullptr;
  for (const auto &stmt : block->nodes) {
    last = Eval(stmt.get());
  }
  return last;
}

Value Interpret::ProcessingIfNode(AST::IfNode *if_node) {
  Value cond = Eval(if_node->conditional.get());
  bool conditional = std::get<bool>(cond);

  if (conditional) {
    return ProcessingBlockNode(if_node->then.get());
  }

  for (auto &[if_, then] : if_node->else_if) {
    Value cond = Eval(if_.get());
    bool conditional = std::get<bool>(cond);
    if (conditional) {
      return ProcessingBlockNode(then.get());
    }
  }

  if (if_node->eelse) {
    return ProcessingBlockNode(if_node->eelse.get());
  }

  return nullptr;
}

Value Interpret::ProcessingWhileNode(AST::WhileNode *while_node) {
  while (std::get<bool>(Eval(while_node->conditional.get()))) {
    try {
      ProcessingBlockNode(while_node->then.get());
    } catch (BreakException) {
      break;
    } catch (ContinueException) {
      continue;
    }
  }
  return nullptr;
}

Value Interpret::ProcessingNumber(AST::BinOperationNode *bin, double l,
                                  double r) {
  switch (bin->operation.GetType()) {
  case TokenType::PLUS:
    return l + r;
  case TokenType::MINUS:
    return l - r;
  case TokenType::MULTIPLY:
    return l * r;
  case TokenType::POW:
    return pow(l, r);
  case TokenType::MOD:
    return std::fmod(l, r);
  case TokenType::DIVIDE:
    if (r == 0)
      throw std::runtime_error("Division by 0");
    return l / r;
  case TokenType::EQ:
    return (l == r);
  case TokenType::N_EQ:
    return (l != r);
  case TokenType::GREATER:
    return (l > r);
  case TokenType::LESS:
    return (l < r);
  case TokenType::GREATER_EQ:
    return (l >= r);
  case TokenType::LESS_EQ:
    return (l <= r);
  default:
    throw std::runtime_error("Unknown operator");
  }
}

Value Interpret::ProcessingString(AST::BinOperationNode *bin, std::string l,
                                  std::string r) {
  switch (bin->operation.GetType()) {
  case TokenType::PLUS:
    return l + r;
  case TokenType::MINUS:
    if ((l.length() >= r.length()) &&
        (l.substr(l.length() - r.length(), r.length()) == r)) {
      return l.substr(0, l.length() - r.length());
    } else {
      throw std::runtime_error("Left string is not a substring of the right");
    }
  case TokenType::EQ:
    return (l == r);
  case TokenType::GREATER:
    return (l > r);
  case TokenType::LESS:
    return (l < r);
  case TokenType::GREATER_EQ:
    return (l >= r);
  case TokenType::LESS_EQ:
    return (l <= r);
  default:
    throw std::runtime_error("Unknown operator");
  }
}

Value Interpret::ProcessingNumberString(AST::BinOperationNode *bin,
                                        std::string l, double r) {
  switch (bin->operation.GetType()) {
  case TokenType::MULTIPLY: {
    if (r < 0) {
      throw std::runtime_error(
          "String can not be multiply by a negative number");
    }
    int full = static_cast<int>(r);
    double frac = r - full;
    std::string res = "";
    for (int i = 0; i < full; ++i) {
      res += l;
    }
    for (int i = 0; i < frac * l.length(); ++i) {
      res += l[i];
    }
    return res;
  }
  default:
    throw std::runtime_error("Unknown operator");
  }
}

Value Interpret::ProcessingIndexNode(AST::IndexNode *index_node) {
  Value index = Eval(index_node->index.get());
  Value object = Eval(index_node->object.get());

  if (!std::holds_alternative<double>(index)) {
    throw std::runtime_error("Index must be a number");
  }

  double dindex = std::get<double>(index);
  int iindex = static_cast<int>(dindex);

  if (dindex != iindex) {
    throw std::runtime_error("Index must be a number");
  }

  if (std::holds_alternative<std::string>(object)) {
    std::string s = std::get<std::string>(object);

    if (iindex < 0 || iindex >= static_cast<int>(s.size())) {
      throw std::runtime_error("Index is out of range");
    }

    return Value(std::string(1, s[iindex]));
  }

  if (std::holds_alternative<std::vector<Value>>(object)) {
    std::vector<Value> v = std::get<std::vector<Value>>(object);

    if (iindex < 0 || iindex >= static_cast<int>(v.size())) {
      throw std::runtime_error("Index is out of range");
    }

    return v[iindex];
  }

  return nullptr;
}

Value Interpret::ProcessingSliceNode(AST::SliceNode *slice_node) {
  Value start = Eval(slice_node->start.get());
  Value end = Eval(slice_node->end.get());
  if (std::holds_alternative<nullptr_t>(start)) {
    start = double(0);
  }
  Value object = Eval(slice_node->object.get());
  if (std::holds_alternative<std::string>(object)) {
    if (std::holds_alternative<nullptr_t>(end)) {
      end = double(std::get<std::string>(object).length());
    }
    if (!std::holds_alternative<double>(start)) {
      throw std::runtime_error("Index must be an integer number");
    }
    if (!std::holds_alternative<double>(end)) {
      throw std::runtime_error("Index must be an integer number");
    }
    double dstart = std::get<double>(start);
    double dend = std::get<double>(end);
    int istart = static_cast<int>(dstart);
    int iend = static_cast<int>(dend);
    if (dstart != istart || dend != iend) {
      throw std::runtime_error("Index must be an integer number");
    }
    std::string s = std::get<std::string>(object);

    if (istart < 0 || iend <= istart || iend > static_cast<int>(s.length())) {
      throw std::runtime_error("Index is out of range");
    }

    return Value(s.substr(istart, iend - istart + 1));
  }

  if (std::holds_alternative<std::vector<Value>>(object)) {
    if (std::holds_alternative<nullptr_t>(end)) {
      end = double(std::get<std::vector<Value>>(object).size());
    }
    if (!std::holds_alternative<double>(start)) {
      throw std::runtime_error("Index must be an integer number");
    }
    if (!std::holds_alternative<double>(end)) {
      throw std::runtime_error("Index must be an integer number");
    }
    double dstart = std::get<double>(start);
    double dend = std::get<double>(end);
    int istart = static_cast<int>(dstart);
    int iend = static_cast<int>(dend);
    if (dstart != istart || dend != iend) {
      throw std::runtime_error("Index must be a number");
    }
    std::vector<Value> v = std::get<std::vector<Value>>(object);

    if (istart < 0 || iend <= istart || iend > static_cast<int>(v.size())) {
      throw std::runtime_error("Index is out of range");
    }

    std::vector<Value> res;
    for (int i = istart; i < iend; ++i) {
      res.push_back(v[i]);
    }

    return Value(res);
  }
  return nullptr;
}

Value Interpret::ParseAssignmentNode(AST::AssignmentNode *assignment_node) {
  switch (assignment_node->operation.GetType()) {
  case TokenType::ASSIGN: {
    Value val = Eval(assignment_node->value.get());

    if (auto funcVal = std::get_if<std::shared_ptr<Function>>(&val)) {
      (*funcVal)->name = assignment_node->variable.GetValue();
    }

    global_->Assign(assignment_node->variable.GetValue(), val);

    return val;
  }
  case TokenType::PLUS_A: {
    double val = std::get<double>(Eval(assignment_node->value.get()));
    double current =
        std::get<double>(global_->LookUp(assignment_node->variable.GetValue()));
    global_->Assign(assignment_node->variable.GetValue(), val + current);
    return val;
  }
  case TokenType::MINUS_A: {
    double val = std::get<double>(Eval(assignment_node->value.get()));
    double current =
        std::get<double>(global_->LookUp(assignment_node->variable.GetValue()));
    global_->Assign(assignment_node->variable.GetValue(), current - val);
    return val;
  }
  case TokenType::DIVIDE_A: {
    double val = std::get<double>(Eval(assignment_node->value.get()));
    double current =
        std::get<double>(global_->LookUp(assignment_node->variable.GetValue()));
    global_->Assign(assignment_node->variable.GetValue(), current / val);
    return val;
  }
  case TokenType::MULTIPLY_A: {
    double val = std::get<double>(Eval(assignment_node->value.get()));
    double current =
        std::get<double>(global_->LookUp(assignment_node->variable.GetValue()));
    global_->Assign(assignment_node->variable.GetValue(), val * current);
    return val;
  }
  case TokenType::POW_A: {
    double val = std::get<double>(Eval(assignment_node->value.get()));
    double current =
        std::get<double>(global_->LookUp(assignment_node->variable.GetValue()));
    global_->Assign(assignment_node->variable.GetValue(), pow(current, val));
    return val;
  }
  case TokenType::MOD_A: {
    double val = std::get<double>(Eval(assignment_node->value.get()));
    double current =
        std::get<double>(global_->LookUp(assignment_node->variable.GetValue()));
    global_->Assign(assignment_node->variable.GetValue(),
                    std::fmod(current, val));
    return val;
  }
  }
  return nullptr;
}

void Interpret::Print(const Value &v, std::ostream &output) {
  if (auto d = std::get_if<double>(&v)) {
    output << *d;
  } else if (auto s = std::get_if<std::string>(&v)) {
    output << *s;
  } else if (auto b = std::get_if<bool>(&v)) {
    output << (*b ? "true" : "false");
  } else if (std::get_if<std::nullptr_t>(&v)) {
    output << "nil";
  } else if (auto vec = std::get_if<std::vector<Value>>(&v)) {
    output << "[";
    for (int i = 0; i < (*vec).size(); ++i) {
      Print((*vec)[i], output);
      if (i != (*vec).size() - 1) {
        output << ", ";
      }
    }
    output << "]";
  }
}

Value Interpret::ProcessingListList(AST::BinOperationNode *bin,
                                    std::vector<Value> &l,
                                    std::vector<Value> &r) {
  switch (bin->operation.GetType()) {
  case TokenType::PLUS: {
    std::vector<Value> res;
    for (int i = 0; i < l.size(); ++i) {
      res.push_back(l[i]);
    }

    for (int i = 0; i < r.size(); ++i) {
      res.push_back(r[i]);
    }

    return Value(res);
  }
  default:
    throw std::runtime_error("Unknown operation");
  }
}

Value Interpret::ProcessingListNumber(AST::BinOperationNode *bin,
                                      std::vector<Value> &l, double r) {
  switch (bin->operation.GetType()) {
  case TokenType::MULTIPLY: {
    std::vector<Value> res;
    int ir = static_cast<int>(r);
    double dr = r - ir;
    for (int i = 0; i < ir; ++i) {
      for (int j = 0; j < l.size(); ++j) {
        res.push_back(l[j]);
      }
    }

    for (int i = 0; i < ((l.size()) * dr); ++i) {
      res.push_back(l[i]);
    }

    return Value(res);
  }
  default:
    throw std::runtime_error("Unknown operation");
  }
}

std::vector<std::string> Interpret::GetStackTrace() {
  if (current_ == nullptr)
    return {};
  return current_->stack_;
}

std::vector<std::string> Interpret::GetStack() { return stack_; }

Value Interpret::ProcessingFunctionNode(AST::FunctionNode *func) {
  Function function;
  function.closure = global_;
  function.args.reserve(func->args.size());
  for (const auto &arg : func->args) {
    if (auto varArg = dynamic_cast<AST::VariableNode *>(arg.get())) {
      function.args.push_back(varArg->variable.GetValue());
    } else {
      throw std::runtime_error("Argument must be a variable");
    }
  }

  function.body = std::make_unique<AST::BlockNode>(std::move(*(func->then)));

  return Value(std::make_shared<Function>(std::move(function)));
}

Value Interpret::CallUserFunction(std::shared_ptr<Function> func,
                                  const std::vector<Value> &args) {
  if (args.size() != func->args.size()) {
    throw std::runtime_error("Error in number of arguments");
  }

  auto local = std::make_shared<Scope>(func->closure);
  for (size_t i = 0; i < args.size(); ++i) {
    local->Assign(func->args[i], args[i]);
  }

  auto old_env = global_;
  global_ = local;

  Value result;
  try {
    result = Eval(func->body.get());
  } catch (const ReturnException &r) {
    result = r.value;
  }

  global_ = old_env;
  return result;
}

bool interpret(std::istream &input, std::ostream &output) {
  try {
    Lexer lexer(input);
    auto tokens = lexer.GetTokens();

    Parser parser(tokens);

    auto global = std::make_shared<Scope>();
    AddSystemFunction(global, output);

    Interpret interpreter(parser.ParseCode(), global, output);
    interpreter.Run();
  } catch (const std::exception &e) {
    output << e.what() << std::endl;
    return false;
  }
  return true;
}