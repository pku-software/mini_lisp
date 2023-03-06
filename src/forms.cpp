#include "./forms.h"

#include <limits>
#include <memory>
#include <unordered_set>

#include "./error.h"

std::vector<ValuePtr> checkOperandsCount(
    ValuePtr operands, std::size_t min = 0,
    std::size_t max = std::numeric_limits<std::size_t>::max()) {
    auto vec = operands->toVector();
    if (vec.size() < min) {
        throw LispError("Too few operands: " + std::to_string(vec.size()) + " < " +
                        std::to_string(min));
    } else if (vec.size() > max) {
        throw LispError("Too many operands: " + std::to_string(vec.size()) + " > " +
                        std::to_string(max));
    }
    return vec;
}

ValuePtr lambdaForm(ValuePtr operands, EvaluateEnv& env) {
    checkOperandsCount(operands, 2);
    auto [formals, body] = std::move(operands->asPair());
    std::vector<std::string> params;
    std::unordered_set<std::string> paramSet;
    while (formals->isPair()) {
        auto&& [car, cdr] = formals->asPair();
        if (auto name = car->getSymbolName()) {
            if (paramSet.count(*name)) {
                throw LispError("Duplicate parameter name: " + *name);
            }
            params.push_back(*name);
            paramSet.insert(*name);
        } else {
            throw LispError("Expect symbol in Lambda parameter, found " + car->toString());
        }
        formals = std::move(cdr);
    }
    return std::make_shared<LambdaValue>(params, std::move(body), env.shared_from_this());
}

ValuePtr defineForm(ValuePtr operands, EvaluateEnv& env) {
    auto args = checkOperandsCount(operands, 2);
    if (auto name = args[0]->getSymbolName()) {
        if (args.size() > 2) {
            throw LispError("Too many operands: " + std::to_string(args.size()) + " < 2");
        }
        env.defineBinding(*name, env.eval(args[1]));
        return args[0];
    } else if (args[0]->isPair()) {
        auto&& [decl, body] = operands->asPair();
        auto&& [car, cdr] = decl->asPair();
        if (auto name = car->getSymbolName()) {
            auto proc = lambdaForm(std::make_shared<PairValue>(cdr, body), env);
            env.defineBinding(*name, proc);
            return car;
        } else {
            throw LispError("In lambda definition, " + car->toString() + " is not a symbol name");
        }
    } else {
        throw LispError("Malformed define form: " + args[0]->toString());
    }
}

ValuePtr quasiquoteItem(ValuePtr val, EvaluateEnv& env, std::size_t level) {
    if (!val->isPair()) {
        return val;
    }
    auto&& [car, cdr] = val->asPair();
    if (auto name = car->getSymbolName()) {
        if (*name == "unquote") {
            level--;
            if (level == 0) {
                auto args = checkOperandsCount(cdr, 1, 1);
                return env.eval(args[0]);
            }
        } else if (*name == "quasiquote") {
            level++;
        }
    }
    auto car_ = quasiquoteItem(car, env, level);
    auto cdr_ = quasiquoteItem(cdr, env, level);
    return std::make_shared<PairValue>(std::move(car_), std::move(cdr_));
}

ValuePtr quoteForm(ValuePtr operands, EvaluateEnv& env) {
    auto args = checkOperandsCount(std::move(operands), 1, 1);
    return args[0];
}

ValuePtr quasiquoteForm(ValuePtr operands, EvaluateEnv& env) {
    auto args = checkOperandsCount(std::move(operands), 1, 1);
    return quasiquoteItem(std::move(args[0]), env, 1);
}

ValuePtr beginForm(ValuePtr operands, EvaluateEnv& env) {
    checkOperandsCount(operands, 1);
    auto result = env.evalList(std::move(operands));
    return std::move(result.back());
}

ValuePtr ifForm(ValuePtr operands, EvaluateEnv& env) {
    auto args = checkOperandsCount(operands, 2, 3);
    if (env.eval(std::move(args[0]))->isTrue()) {
        return env.eval(std::move(args[1]));
    } else if (args.size() == 3) {
        return env.eval(std::move(args[2]));
    } else {
        return Value::nil();
    }
}

ValuePtr andForm(ValuePtr operands, EvaluateEnv& env) {
    if (operands->isPair()) {
        auto&& [car, cdr] = operands->asPair();
        auto val = env.eval(std::move(car));
        if (!val->isTrue()) {
            return Value::fromBoolean(false);
        } else if (cdr->isNil()) {
            return val;
        } else {
            return andForm(std::move(cdr), env);
        }
    }
    return Value::fromBoolean(true);
}

ValuePtr orForm(ValuePtr operands, EvaluateEnv& env) {
    if (operands->isPair()) {
        auto&& [car, cdr] = operands->asPair();
        auto val = env.eval(std::move(car));
        if (val->isTrue()) {
            return val;
        } else {
            return orForm(std::move(cdr), env);
        }
    }
    return Value::fromBoolean(false);
}

ValuePtr condForm(ValuePtr operands, EvaluateEnv& env) {
    auto vec = checkOperandsCount(operands);
    for (auto clause : vec) {
        auto form = checkOperandsCount(clause, 1);
        ValuePtr test;
        if (auto name = form[0]->getSymbolName(); name && *name == "else") {
            test = Value::fromBoolean(true);
            if (clause != vec.back()) {
                throw LispError("else clause must be the last one");
            }
        } else {
            test = env.eval(form[0]);
        }
        if (test->isTrue()) {
            if (form.size() > 1) {
                return env.eval(form[1]);
            } else {
                return test;
            }
        }
    }
    return Value::nil();
}

ValuePtr letForm(ValuePtr operands, EvaluateEnv& env) {
    checkOperandsCount(operands, 2);
    auto&& [car, cdr] = operands->asPair();
    auto bindings = checkOperandsCount(std::move(car));
    std::vector<std::string> names;
    std::vector<ValuePtr> values;
    for (auto binding : bindings) {
        auto vec = checkOperandsCount(std::move(binding), 2, 2);
        if (auto name = vec[0]->getSymbolName()) {
            auto val = env.eval(std::move(vec[1]));
            names.push_back(*name);
            values.push_back(std::move(val));
        } else {
            throw LispError("Expect let binding name, found " + vec[0]->toString());
        }
    }
    auto newEnv = env.createChild(names, values);
    auto results = newEnv->evalList(std::move(cdr));
    return std::move(results.back());
}

const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS{
    {"define", defineForm}, {"quote", quoteForm}, {"quasiquote", quasiquoteForm},
    {"lambda", lambdaForm}, {"begin", beginForm}, {"if", ifForm},
    {"and", andForm},       {"or", orForm},       {"cond", condForm},
    {"let", letForm}};