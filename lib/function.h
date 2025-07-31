#pragma once

#include "ast.h"
#include <functional>
#include <iostream>
#include <map>

struct Scope;

struct Function {
  std::string name;
  std::vector<std::string> args;
  std::unique_ptr<AST::BlockNode> body;
  std::shared_ptr<Scope> closure;
};
