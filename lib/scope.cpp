#include "scope.h"

Value Scope::LookUp(const std::string& var) {
    for (const auto& [key, val] : vars) {
        if (key == var) return val;
    }
    if (parent) return parent->LookUp(var);
    throw std::runtime_error("No variable " + var);
}

void Scope::Assign(const std::string& var, const Value& value) {
    if (vars.count(var) || !parent) {
        vars[var] = value;
    } else {
        parent->Assign(var, value);
    }
}
