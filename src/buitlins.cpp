#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>

#include "./builtins.h"
#include "./error.h"
#include "./eval_env.h"

void checkArgsCount(const std::vector<ValuePtr>& args, std::size_t min, std::size_t max) {
    if (args.size() < min) {
        throw LispError("Too few arguments: " + std::to_string(args.size()) + " < " +
                        std::to_string(min));
    } else if (args.size() > max) {
        throw LispError("Too many arguments: " + std::to_string(args.size()) + " > " +
                        std::to_string(max));
    }
}

template <typename... ValPtrs>
auto extractNumbers(ValPtrs... ptrs) {
    static_assert((... && std::is_same_v<ValPtrs, ValuePtr>), "Not ValuePtr");
    return std::tuple{(ptrs->isNumber() ? ptrs->asNumber()
                                        : throw LispError(ptrs->toString() + " is not number"))...};
}

ValuePtr procedureQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    return Value::fromBoolean(args[0]->isProcedure());
}
ValuePtr listQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    return Value::fromBoolean(args[0]->isList());
}
ValuePtr booleanQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    return Value::fromBoolean(args[0]->isBoolean());
}
ValuePtr numberQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    return Value::fromBoolean(args[0]->isNumber());
}
ValuePtr symbolQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    return Value::fromBoolean(args[0]->isSymbol());
}
ValuePtr stringQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    return Value::fromBoolean(args[0]->isString());
}
ValuePtr nullQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    return Value::fromBoolean(args[0]->isNil());
}

ValuePtr not_(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    return Value::fromBoolean(!args[0]->isTrue());
}
ValuePtr eqQ(const std::vector<ValuePtr>& args, EvaluateEnv& env) {
    checkArgsCount(args, 2);
    auto a = std::move(args[0]);
    auto b = std::move(args[1]);
    const auto& rawA = *a;
    const auto& rawB = *b;
    if (typeid(rawA) != typeid(rawB)) {
        return Value::fromBoolean(false);
    }
    if (a->isNumber()) {
        auto aNum = a->asNumber();
        auto bNum = b->asNumber();
        return Value::fromBoolean(aNum == bNum);
    }  else if (a->isBoolean()) {
        auto aBool = a->asBool();
        auto bBool = b->asBool();
        return Value::fromBoolean(aBool == bBool);
    } else if (a->isSymbol()) {
        auto aSym = a->getSymbolName();
        auto bSym = b->getSymbolName();
        return Value::fromBoolean(*aSym == *bSym);
    } else if (a->isNil()) {
        return Value::fromBoolean(true);
    } else {
        return Value::fromBoolean(a == b);
    }
}
ValuePtr equalQ(const std::vector<ValuePtr>& args, EvaluateEnv& env) {
    checkArgsCount(args, 2);
    auto a = std::move(args[0]);
    auto b = std::move(args[1]);
    if (a->isPair() && b->isPair()) {
        auto&& [aCar, aCdr] = a->asPair();
        auto&& [bCar, bCdr] = b->asPair();
        auto carResult = equalQ({aCar, bCar}, env);
        auto cdrResult = equalQ({aCdr, bCdr}, env);
        return Value::fromBoolean(carResult->isTrue() && cdrResult->isTrue());
    } else if (a->isString() && b->isString()) {
        auto&& aStr = a->asString();
        auto&& bStr = b->asString();
        return Value::fromBoolean(aStr == bStr);
    }else {
        return eqQ(args, env);
    }
}
ValuePtr pairQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    return Value::fromBoolean(args[0]->isPair());
}

ValuePtr length(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    auto list = args[0];
    auto vec = list->toVector();
    return Value::fromNumber(double(vec.size()));
}
ValuePtr cons(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 2);
    return std::make_shared<PairValue>(args[0], args[1]);
}
ValuePtr car(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    auto list = args[0];
    if (!list->isPair()) {
        throw LispError("car: argument is not a pair");
    }
    auto&& [car, cdr] = list->asPair();
    return car;
}
ValuePtr cdr(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    auto list = args[0];
    if (!list->isPair()) {
        throw LispError("cdr: argument is not a pair");
    }
    auto&& [car, cdr] = list->asPair();
    return cdr;
}

ValuePtr list(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    ValuePtr list = Value::nil();
    for (auto it = args.rbegin(); it != args.rend(); ++it) {
        list = std::make_shared<PairValue>(*it, list);
    }
    return list;
}
ValuePtr append(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    std::vector<ValuePtr> result;
    for (auto arg : args) {
        auto vec = arg->toVector();
        result.insert(result.end(), vec.begin(), vec.end());
    }
    return Value::fromVector(result);
}

ValuePtr integerQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    auto [number] = extractNumbers(std::move(args[0]));
    return Value::fromBoolean(number == std::floor(number) && std::isfinite(number));
}
ValuePtr add(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    double result = 0;
    for (auto arg : args) {
        auto [number] = extractNumbers(std::move(arg));
        result += number;
    }
    return Value::fromNumber(result);
}
ValuePtr sub(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1, 2);
    if (args.size() == 1) {
        auto [number] = extractNumbers(std::move(args[0]));
        return Value::fromNumber(-number);
    } else {
        auto [lhs, rhs] = extractNumbers(std::move(args[0]), std::move(args[1]));
        return Value::fromNumber(lhs - rhs);
    }
}
ValuePtr mult(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    double result = 1;
    for (auto arg : args) {
        auto [number] = extractNumbers(std::move(arg));
        result *= number;
    }
    return Value::fromNumber(result);
}
ValuePtr div(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1, 2);
    if (args.size() == 1) {
        auto [number] = extractNumbers(std::move(args[0]));
        return Value::fromNumber(1 / number);
    } else {
        auto [lhs, rhs] = extractNumbers(std::move(args[0]), std::move(args[1]));
        return Value::fromNumber(lhs / rhs);
    }
}
ValuePtr expt(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 2);
    auto [lhs, rhs] = extractNumbers(std::move(args[0]), std::move(args[1]));
    return Value::fromNumber(std::pow(lhs, rhs));
}
ValuePtr abs(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    auto [number] = extractNumbers(std::move(args[0]));
    return Value::fromNumber(std::abs(number));
}
ValuePtr quotient(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 2);
    auto [lhs, rhs] = extractNumbers(std::move(args[0]), std::move(args[1]));
    return Value::fromNumber(int(lhs / rhs));
}
double lispModulo(double n, double m) {
    double r = n - m * int(n / m);
    // If m and r has different sign, adjust r
    // Assume that double is IEEE 754 64-bit floating point
    if ((*reinterpret_cast<std::int64_t*>(&m) ^ *reinterpret_cast<std::int64_t*>(&r)) < 0) {
        r += m;
    }
    return r;
}
ValuePtr modulo(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 2);
    auto [lhs, rhs] = extractNumbers(std::move(args[0]), std::move(args[1]));
    return Value::fromNumber(lispModulo(lhs, rhs)); 
}
ValuePtr remainder(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 2);
    auto [lhs, rhs] = extractNumbers(std::move(args[0]), std::move(args[1]));
    return Value::fromNumber(std::fmod(lhs, rhs));
}
ValuePtr eq(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 2);
    auto [lhs, rhs] = extractNumbers(std::move(args[0]), std::move(args[1]));
    return Value::fromBoolean(lhs == rhs);
}
ValuePtr lt(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 2);
    auto [lhs, rhs] = extractNumbers(std::move(args[0]), std::move(args[1]));
    return Value::fromBoolean(lhs < rhs);
}
ValuePtr gt(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 2);
    auto [lhs, rhs] = extractNumbers(std::move(args[0]), std::move(args[1]));
    return Value::fromBoolean(lhs > rhs);
}
ValuePtr lteq(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 2);
    auto [lhs, rhs] = extractNumbers(std::move(args[0]), std::move(args[1]));
    return Value::fromBoolean(lhs <= rhs);
}
ValuePtr gteq(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 2);
    auto [lhs, rhs] = extractNumbers(std::move(args[0]), std::move(args[1]));
    return Value::fromBoolean(lhs >= rhs);
}
ValuePtr evenQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    auto [number] = extractNumbers(std::move(args[0]));
    return Value::fromBoolean(std::fmod(number, 2) == 0);
}
ValuePtr oddQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    auto [number] = extractNumbers(std::move(args[0]));
    return Value::fromBoolean(std::fmod(number, 2) != 0);
}
ValuePtr zeroQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    auto [number] = extractNumbers(std::move(args[0]));
    return Value::fromBoolean(number == 0);
}

ValuePtr display(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    for (auto arg : args) {
        if (arg->isString()) {
            std::cout << arg->asString();
        } else {
            std::cout << arg->toString();
        }
    }
    return Value::nil();
}
ValuePtr print(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    for (auto arg : args) {
        arg->print();
    }
    return Value::nil();
}
ValuePtr displayln(const std::vector<ValuePtr>& args, EvaluateEnv& env) {
    auto r = display(args, env);
    std::cout << "\n";
    return r;
}
ValuePtr newline(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    std::cout << std::endl;
    return Value::nil();
}
ValuePtr error(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    throw LispError(args[0]->toString());
}
[[noreturn]] ValuePtr exit(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 0, 1);
    if (args.empty()) {
        std::exit(0);
    } else {
        auto [number] = extractNumbers(std::move(args[0]));
        std::exit(number);
    }
}

ValuePtr map(const std::vector<ValuePtr>& args, EvaluateEnv& env) {
    checkArgsCount(args, 2, 2);
    auto proc = args[0];
    auto list = args[1]->toVector();
    std::vector<ValuePtr> mapped;
    std::transform(list.begin(), list.end(), std::back_inserter(mapped),
                  [&](ValuePtr v) { return env.apply(proc, {std::move(v)}); });
    return Value::fromVector(mapped);
}

ValuePtr filter(const std::vector<ValuePtr>& args, EvaluateEnv& env) {
    checkArgsCount(args, 2, 2);
    auto proc = args[0];
    auto list = args[1]->toVector();
    std::vector<ValuePtr> filtered;
    std::copy_if(list.begin(), list.end(), std::back_inserter(filtered),
                [&](ValuePtr v) { return env.apply(proc, {std::move(v)})->isTrue(); });
    return Value::fromVector(filtered);
}
ValuePtr reduce(const std::vector<ValuePtr>& args, EvaluateEnv& env) {
    checkArgsCount(args, 2, 2);
    if (!args[1]->isList()) {
        throw LispError("reduce: second argument must be a list");
    }
    if (args[1]->isNil()) {
        throw LispError("reduce list must has at least 1 element");
    }
    auto [init, rest] = std::move(args[1]->asPair());
    auto proc = std::move(args[0]);
    while (rest->isPair()) {
        auto&& [car, cdr] = rest->asPair();
        init = env.apply(proc, {init, car});
        rest = std::move(cdr);
    }
    return init;
}

ValuePtr eval(const std::vector<ValuePtr>& args, EvaluateEnv& env) {
    checkArgsCount(args, 1, 1);
    return env.eval(args[0]);
}
ValuePtr apply(const std::vector<ValuePtr>& args, EvaluateEnv& env) {
    checkArgsCount(args, 2, 2);
    auto callArgs = args[1]->toVector();
    return env.apply(std::move(args[0]), std::move(callArgs));
}

const std::unordered_map<std::string, BuiltinFuncType*> BUILTINS{{"procedure?", procedureQ},
                                                                 {"list?", listQ},
                                                                 {"boolean?", booleanQ},
                                                                 {"number?", numberQ},
                                                                 {"symbol?", symbolQ},
                                                                 {"string?", stringQ},
                                                                 {"null?", nullQ},
                                                                 {"not", not_},
                                                                 {"equal?", equalQ},
                                                                 {"eq?", eqQ},
                                                                 {"pair?", pairQ},
                                                                 {"length", length},
                                                                 {"cons", cons},
                                                                 {"car", car},
                                                                 {"cdr", cdr},
                                                                 {"list", list},
                                                                 {"append", append},
                                                                 {"integer?", integerQ},
                                                                 {"+", add},
                                                                 {"-", sub},
                                                                 {"*", mult},
                                                                 {"/", div},
                                                                 {"expt", expt},
                                                                 {"abs", abs},
                                                                 {"quotient", quotient},
                                                                 {"modulo", modulo},
                                                                 {"remainder", remainder},
                                                                 {"=", eq},
                                                                 {"<", lt},
                                                                 {">", gt},
                                                                 {"<=", lteq},
                                                                 {">=", gteq},
                                                                 {"even?", evenQ},
                                                                 {"odd?", oddQ},
                                                                 {"zero?", zeroQ},
                                                                 {"display", display},
                                                                 {"print", print},
                                                                 {"displayln", displayln},
                                                                 {"newline", newline},
                                                                 {"error", error},
                                                                 {"map", map},
                                                                 {"filter", filter},
                                                                 {"reduce", reduce},
                                                                 {"exit", exit},
                                                                 {"eval", eval},
                                                                 {"apply", apply}};