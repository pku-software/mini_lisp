#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "./value.h"

class EvaluateEnv : public std::enable_shared_from_this<EvaluateEnv> {
private:
    std::shared_ptr<EvaluateEnv> parent;
    std::unordered_map<std::string, ValuePtr> bindings;

    EvaluateEnv() = default;

public:
    EvaluateEnv(const EvaluateEnv&) = delete;

    static std::shared_ptr<EvaluateEnv> createGlobal();
    std::shared_ptr<EvaluateEnv> createChild(const std::vector<std::string>& params,
                                             const std::vector<ValuePtr>& args);

    ValuePtr eval(ValuePtr expr);
    std::vector<ValuePtr> evalList(ValuePtr expr);
    ValuePtr apply(ValuePtr operator_, const std::vector<ValuePtr>& operands);

    void defineBinding(const std::string& name, ValuePtr value);
    ValuePtr lookupBinding(const std::string& name) const;
};

#endif