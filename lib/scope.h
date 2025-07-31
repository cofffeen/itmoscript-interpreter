#pragma once

#include <iostream>
#include <map>
#include "value.h"
#include "function.h"

struct Scope {
  std::map<std::string, Value> vars;
  std::shared_ptr<Scope> parent;
  std::map<std::string, Function> functions;

  Value LookUp(const std::string &var);
  void Assign(const std::string &var, const Value &value);

  Scope(std::shared_ptr<Scope> parentScope) : parent(std::move(parentScope)) {}

  Scope() = default;
};
