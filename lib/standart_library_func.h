#pragma once
#include <iostream>
#include <variant>
#include <memory>
#include <string>
#include <functional>
#include <map>
#include <cmath>
#include <random>
#include <algorithm>
#include "interpreter.h"

class Value;

struct Scope;

class Interpret;

void AddSystemFunction(std::shared_ptr<Scope> global, std::ostream& output);
