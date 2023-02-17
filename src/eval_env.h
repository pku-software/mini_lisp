#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "./value.h"


class EvaluateEnv : public std::enable_shared_from_this<EvaluateEnv> {
private:
    std::unordered_map<std::string, ValuePtr> bindings;

    void bindGlobals();
    std::shared_ptr<EvaluateEnv> createChildEnv(const std::vector<std::string>& params,
                               const std::vector<ValuePtr>& args) const;
    friend class LambdaValue;

    EvaluateEnv() {
        bindGlobals();
    }
    EvaluateEnv(const EvaluateEnv&) = default;

public:

    static std::shared_ptr<EvaluateEnv> create() {
        return std::shared_ptr<EvaluateEnv>(new EvaluateEnv());
    }
    std::shared_ptr<EvaluateEnv> clone() const {
        return std::shared_ptr<EvaluateEnv>(new EvaluateEnv(*this));
    }

    ValuePtr eval(ValuePtr expr);
    std::vector<ValuePtr> evalList(ValuePtr expr);
    ValuePtr apply(ValuePtr operator_, const std::vector<ValuePtr>& operands);

    void defineBinding(const std::string& name, ValuePtr value);
    ValuePtr lookupBinding(const std::string& name) const;
};

#endif