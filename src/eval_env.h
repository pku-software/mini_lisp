#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

#include "./value.h"

class EvaluateEnv {
private:
    std::unordered_map<std::string, ValuePtr> bindings;

    void bindGlobals();
    EvaluateEnv createChildEnv(const std::vector<std::string>& params, const std::vector<ValuePtr>& args) const;

public:
    EvaluateEnv() {
        bindGlobals();
    };

    ValuePtr eval(ValuePtr expr);
    std::vector<ValuePtr> evalList(ValuePtr expr);
    ValuePtr apply(ValuePtr operator_, const std::vector<ValuePtr>& operands);

    void defineBinding(const std::string& name, ValuePtr value);
    ValuePtr lookupBinding(const std::string& name) const;
};

#endif