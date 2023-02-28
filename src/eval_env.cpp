#include "./eval_env.h"

#include <algorithm>
#include <iterator>
#include <memory>
#include <string>

#include "./builtins.h"
#include "./error.h"
#include "./forms.h"

namespace rg = std::ranges;

void EvaluateEnv::bindGlobals() {
    for (auto&& [name, func] : BUILTINS) {
        defineBinding(name, std::make_shared<BuiltinProcValue>(func));
    }
}

std::shared_ptr<EvaluateEnv> EvaluateEnv::createChildEnv(const std::vector<std::string>& params,
                                                         const std::vector<ValuePtr>& args) const {
    if (params.size() != args.size()) {
        throw LispError("Procedure expected " + std::to_string(params.size()) +
                        " parameters, got " + std::to_string(args.size()));
    }
    auto childEnv = clone();
    for (std::size_t i = 0; i < params.size(); i++) {
        childEnv->defineBinding(params[i], args[i]);
    }
    return childEnv;
}

ValuePtr EvaluateEnv::apply(ValuePtr operator_, const std::vector<ValuePtr>& operands) {
    if (!operator_->isProcedure()) {
        throw LispError("Not a procedure " + operator_->toString());
    }
    auto raw = operator_.get();
    if (typeid(*raw) == typeid(BuiltinProcValue)) {
        auto proc = std::static_pointer_cast<BuiltinProcValue>(std::move(operator_));
        return proc->apply(operands, *this);
    }
    auto lambda = std::static_pointer_cast<LambdaValue>(std::move(operator_));
    return lambda->apply(operands);
}

ValuePtr EvaluateEnv::eval(ValuePtr expr) {
    if (auto name = expr->getSymbolName()) {
        auto v = lookupBinding(*name);
        if (!v) {
            throw LispError("Unbound variable " + *name);
        }
        return v;
    } else if (expr->isSelfEvaluating()) {
        return expr;
    }
    if (!expr->isList()) {
        throw LispError("Malformed list " + expr->toString());
    }
    auto&& [car, cdr] = expr->asPair();
    if (auto name = car->getSymbolName()) {
        if (auto it = SPECIAL_FORMS.find(*name); it != SPECIAL_FORMS.end()) {
            return it->second(std::move(cdr), *this);
        }
    }
    auto operator_ = eval(std::move(car));
    auto operands = evalList(std::move(cdr));
    return apply(std::move(operator_), std::move(operands));
}

std::vector<ValuePtr> EvaluateEnv::evalList(ValuePtr expr) {
    std::vector<ValuePtr> result;
    auto list = expr->toVector();
    std::transform(list.begin(), list.end(), std::back_inserter(result),
                  [this](ValuePtr v) { return eval(std::move(v)); });
    return result;
}

void EvaluateEnv::defineBinding(const std::string& name, ValuePtr value) {
    bindings[name] = std::move(value);
}

ValuePtr EvaluateEnv::lookupBinding(const std::string& name) const {
    auto it = bindings.find(name);
    if (it == bindings.end()) {
        return nullptr;
    }
    return it->second;
}
