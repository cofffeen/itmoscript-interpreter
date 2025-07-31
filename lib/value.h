#pragma once

#include "function.h"
#include <iostream>
#include <variant>

class Value
    : public std::variant<double, std::string, bool, std::nullptr_t,
                          std::function<Value(const std::vector<Value> &)>,
                          std::vector<Value>, std::shared_ptr<Function>> {
                            
  using base = std::variant<double, std::string, bool, std::nullptr_t,
                            std::function<Value(const std::vector<Value> &)>,
                            std::vector<Value>, std::shared_ptr<Function>>;

public:
  using base::base;
  using base::operator=;

  static Value MakeList(std::vector<Value> &&elems) {
    return Value(std::move(elems));
  }
};
