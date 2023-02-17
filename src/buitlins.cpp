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
    return std::tuple{(ptrs->isNumber() ? static_cast<const NumberValue&>(*ptrs).getValue()
                                        : throw LispError(ptrs->toString() + " is not number"))...};
}

ValuePtr procedureQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    return std::make_shared<BooleanValue>(args[0]->isProcedure());
}
ValuePtr listQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    return std::make_shared<BooleanValue>(args[0]->isList());
}
ValuePtr booleanQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    return std::make_shared<BooleanValue>(args[0]->isBoolean());
}
ValuePtr numberQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    return std::make_shared<BooleanValue>(args[0]->isNumber());
}
ValuePtr symbolQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    return std::make_shared<BooleanValue>(args[0]->isSymbol());
}
ValuePtr stringQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    return std::make_shared<BooleanValue>(args[0]->isString());
}
ValuePtr nullQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    return std::make_shared<BooleanValue>(args[0]->isNil());
}

ValuePtr not_(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    return std::make_shared<BooleanValue>(!args[0]->isTrue());
}
ValuePtr eqQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 2);
    if (args[0]->isNil() && args[1]->isNil()) {
        return std::make_shared<BooleanValue>(true);
    }
    return std::make_shared<BooleanValue>(args[0] == args[1]);
}
ValuePtr equalQ(const std::vector<ValuePtr>& args, EvaluateEnv& env) {
    checkArgsCount(args, 2);
    const auto& a = args[0];
    const auto& b = args[1];
    const auto& rawA = *a;
    const auto& rawB = *b;
    if (typeid(rawA) != typeid(rawB)) {
        return std::make_shared<BooleanValue>(false);
    }
    if (a->isNumber()) {
        auto aNum = static_cast<const NumberValue*>(a.get());
        auto bNum = static_cast<const NumberValue*>(b.get());
        return std::make_shared<BooleanValue>(*aNum == *bNum);
    } else if (a->isString()) {
        auto aStr = static_cast<const StringValue*>(a.get());
        auto bStr = static_cast<const StringValue*>(b.get());
        return std::make_shared<BooleanValue>(*aStr == *bStr);
    } else if (a->isBoolean()) {
        auto aBool = static_cast<const BooleanValue*>(a.get());
        auto bBool = static_cast<const BooleanValue*>(b.get());
        return std::make_shared<BooleanValue>(*aBool == *bBool);
    } else if (a->isSymbol()) {
        auto aSym = static_cast<const IdentifierValue*>(a.get());
        auto bSym = static_cast<const IdentifierValue*>(b.get());
        return std::make_shared<BooleanValue>(*aSym == *bSym);
    } else if (a->isNil()) {
        return std::make_shared<BooleanValue>(true);
    } else if (a->isPair()) {
        auto aPair = static_cast<const PairValue*>(a.get());
        auto bPair = static_cast<const PairValue*>(b.get());
        auto carResult = equalQ({aPair->getCar(), bPair->getCar()}, env);
        auto cdrResult = equalQ({aPair->getCdr(), bPair->getCdr()}, env);
        return std::make_shared<BooleanValue>(carResult->isTrue() && cdrResult->isTrue());
    } else {
        return eqQ(args, env);
    }
}
ValuePtr pairQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    return std::make_shared<BooleanValue>(args[0]->isPair());
}

ValuePtr length(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    auto list = args[0];
    if (!list->isList()) {
        throw LispError("length: argument is not a list");
    }
    std::size_t len = 0;
    while (!list->isNil()) {
        auto pair = static_cast<const PairValue*>(list.get());
        list = pair->getCdr();
        len++;
    }
    return std::make_shared<NumberValue>(double(len));
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
    auto pair = static_cast<const PairValue*>(list.get());
    return pair->getCar();
}
ValuePtr cdr(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    auto list = args[0];
    if (!list->isPair()) {
        throw LispError("cdr: argument is not a pair");
    }
    auto pair = static_cast<const PairValue*>(list.get());
    return pair->getCdr();
}

ValuePtr list(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    ValuePtr list = std::make_shared<NilValue>();
    for (auto it = args.rbegin(); it != args.rend(); ++it) {
        list = std::make_shared<PairValue>(*it, list);
    }
    return list;
}
ValuePtr append(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    ValuePtr result = std::make_shared<NilValue>();
    for (auto it = args.rbegin(); it != args.rend(); ++it) {
        auto arg = *it;
        if (!arg->isList()) {
            throw LispError("append: argument is not a list");
        }
        if (arg->isNil()) continue;
        auto [car, cdr] = static_cast<const PairValue&>(*arg);
        auto r = std::make_shared<PairValue>(car, result);
        auto p = r;
        arg = cdr;
        while (arg->isPair()) {
            auto [car, cdr] = static_cast<const PairValue&>(*arg);
            auto newCdr = std::make_shared<PairValue>(car, result);
            p->setCdr(newCdr);
            p = newCdr;
            arg = cdr;
        }
        result = r;
    }
    return result;
}

ValuePtr integerQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    auto [number] = extractNumbers(args[0]);
    return std::make_shared<BooleanValue>(number == std::floor(number) && std::isfinite(number));
}
ValuePtr add(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    double result = 0;
    for (auto arg : args) {
        auto [number] = extractNumbers(arg);
        result += number;
    }
    return std::make_shared<NumberValue>(result);
}
ValuePtr sub(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1, 2);
    if (args.size() == 1) {
        auto [number] = extractNumbers(args[0]);
        return std::make_shared<NumberValue>(-number);
    } else {
        auto [lhs, rhs] = extractNumbers(args[0], args[1]);
        return std::make_shared<NumberValue>(lhs - rhs);
    }
}
ValuePtr mult(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    double result = 1;
    for (auto arg : args) {
        auto [number] = extractNumbers(arg);
        result *= number;
    }
    return std::make_shared<NumberValue>(result);
}
ValuePtr div(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1, 2);
    if (args.size() == 1) {
        auto [number] = extractNumbers(args[0]);
        return std::make_shared<NumberValue>(1 / number);
    } else {
        auto [lhs, rhs] = extractNumbers(args[0], args[1]);
        return std::make_shared<NumberValue>(lhs / rhs);
    }
}
ValuePtr expt(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 2);
    auto [lhs, rhs] = extractNumbers(args[0], args[1]);
    return std::make_shared<NumberValue>(std::pow(lhs, rhs));
}
ValuePtr abs(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    auto [number] = extractNumbers(args[0]);
    return std::make_shared<NumberValue>(std::abs(number));
}
ValuePtr quotient(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 2);
    auto [lhs, rhs] = extractNumbers(args[0], args[1]);
    return std::make_shared<NumberValue>(std::floor(lhs / rhs));
}
ValuePtr modulo(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 2);
    auto [lhs, rhs] = extractNumbers(args[0], args[1]);
    return std::make_shared<NumberValue>(std::fmod(lhs, rhs));
}
ValuePtr remainder(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 2);
    auto [lhs, rhs] = extractNumbers(args[0], args[1]);
    return std::make_shared<NumberValue>(std::remainder(lhs, rhs));
}
ValuePtr eq(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 2);
    auto [lhs, rhs] = extractNumbers(args[0], args[1]);
    return std::make_shared<BooleanValue>(lhs == rhs);
}
ValuePtr lt(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 2);
    auto [lhs, rhs] = extractNumbers(args[0], args[1]);
    return std::make_shared<BooleanValue>(lhs < rhs);
}
ValuePtr gt(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 2);
    auto [lhs, rhs] = extractNumbers(args[0], args[1]);
    return std::make_shared<BooleanValue>(lhs > rhs);
}
ValuePtr lteq(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 2);
    auto [lhs, rhs] = extractNumbers(args[0], args[1]);
    return std::make_shared<BooleanValue>(lhs <= rhs);
}
ValuePtr gteq(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 2);
    auto [lhs, rhs] = extractNumbers(args[0], args[1]);
    return std::make_shared<BooleanValue>(lhs >= rhs);
}
ValuePtr evenQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    auto [number] = extractNumbers(args[0]);
    return std::make_shared<BooleanValue>(std::fmod(number, 2) == 0);
}
ValuePtr oddQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    auto [number] = extractNumbers(args[0]);
    return std::make_shared<BooleanValue>(std::fmod(number, 2) != 0);
}
ValuePtr zeroQ(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    auto [number] = extractNumbers(args[0]);
    return std::make_shared<BooleanValue>(number == 0);
}

ValuePtr display(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    for (auto arg : args) {
        if (arg->isString()) {
            auto string = static_cast<const StringValue&>(*arg);
            std::cout << string.getValue();
        } else {
            std::cout << arg->toString();
        }
    }
    return std::make_shared<NilValue>();
}
ValuePtr print(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    for (auto arg : args) {
        arg->print();
    }
    return std::make_shared<NilValue>();
}
ValuePtr displayln(const std::vector<ValuePtr>& args, EvaluateEnv& env) {
    auto r = display(args, env);
    std::cout << "\n";
    return r;
}
ValuePtr newline(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    std::cout << std::endl;
    return std::make_shared<NilValue>();
}
ValuePtr error(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 1);
    throw LispError(args[0]->toString());
}
ValuePtr exit(const std::vector<ValuePtr>& args, EvaluateEnv&) {
    checkArgsCount(args, 0, 1);
    if (args.empty()) {
        std::exit(0);
    } else {
        auto [number] = extractNumbers(args[0]);
        std::exit(number);
    }
}

ValuePtr map(const std::vector<ValuePtr>& args, EvaluateEnv& env) {
    checkArgsCount(args, 2, 2);
    if (!args[0]->isProcedure()) {
        throw LispError("map: first argument must be a procedure");
    }
    if (!args[1]->isList()) {
        throw LispError("map: second argument must be a list");
    }
    auto proc = args[0];
    std::function<ValuePtr(ValuePtr)> mapImpl = [&](ValuePtr list) -> ValuePtr {
        if (list->isPair()) {
            auto [car, cdr] = static_cast<const PairValue&>(*list);
            auto r = env.apply(proc, {car});
            return std::make_shared<PairValue>(r, mapImpl(std::move(cdr)));
        } else {
            return std::make_shared<NilValue>();
        }
    };
    return mapImpl(std::move(args[1]));
}

ValuePtr filter(const std::vector<ValuePtr>& args, EvaluateEnv& env) {
    checkArgsCount(args, 2, 2);
    if (!args[0]->isProcedure()) {
        throw LispError("filter: first argument must be a procedure");
    }
    if (!args[1]->isList()) {
        throw LispError("filter: second argument must be a list");
    }
    auto proc = args[0];
    std::function<ValuePtr(ValuePtr)> filterImpl = [&](ValuePtr list) -> ValuePtr {
        if (list->isPair()) {
            auto [car, cdr] = static_cast<const PairValue&>(*list);
            auto r = env.apply(proc, {car});
            if (r->isTrue()) {
                return std::make_shared<PairValue>(car, filterImpl(std::move(cdr)));
            } else {
                return filterImpl(std::move(cdr));
            }
        } else {
            return std::make_shared<NilValue>();
        }
    };
    return filterImpl(std::move(args[1]));
}
ValuePtr reduce(const std::vector<ValuePtr>& args, EvaluateEnv& env) {
    checkArgsCount(args, 2, 2);
    if (!args[0]->isProcedure()) {
        throw LispError("reduce: first argument must be a procedure");
    }
    if (!args[1]->isList()) {
        throw LispError("reduce: second argument must be a list");
    }
    if (args[1]->isNil()) {
        throw LispError("reduce list must has at least 1 element");
    }
    auto [init, rest] = static_cast<const PairValue&>(*args[1]);
    auto proc = args[0];
    while (rest->isPair()) {
        auto [car, cdr] = static_cast<const PairValue&>(*rest);
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
    if (!args[1]->isList()) {
        throw LispError("apply: second argument must be a list");
    }
    auto callArgs = env.evalList(args[1]);
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