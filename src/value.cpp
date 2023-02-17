#include "./value.h"

#include <cmath>
#include <iomanip>
#include <iostream>

#include "./eval_env.h"

std::optional<std::string> Value::getSymbolName() const {
    if (isSymbol()) {
        auto symbol = static_cast<const IdentifierValue*>(this);
        return symbol->getName();
    } else {
        return std::nullopt;
    }
}

bool Value::isSymbol() const {
    return typeid(*this) == typeid(IdentifierValue);
}

bool Value::isNil() const {
    return typeid(*this) == typeid(NilValue);
}

bool Value::isBoolean() const {
    return typeid(*this) == typeid(BooleanValue);
}

bool Value::isString() const {
    return typeid(*this) == typeid(StringValue);
}

bool Value::isNumber() const {
    return typeid(*this) == typeid(NumberValue);
}

bool Value::isPair() const {
    return typeid(*this) == typeid(PairValue);
}

bool Value::isAtom() const {
    return isNil() || isSymbol() || isBoolean() || isNumber() || isString();
}

bool Value::isSelfEvaluating() const {
    return isAtom() && !isSymbol();
}

bool Value::isList() const {
    auto current = this;
    while (!current->isNil()) {
        if (!current->isPair()) {
            return false;
        }
        auto pair = static_cast<const PairValue*>(current);
        current = pair->getCdr().get();
    }
    return true;
}

bool Value::isProcedure() const {
    return typeid(*this) == typeid(BuiltinProcValue) || typeid(*this) == typeid(LambdaValue);
}

bool Value::isTrue() const {
    if (!isBoolean()) return true;
    auto boolean = static_cast<const BooleanValue*>(this);
    return boolean->getValue();
}

std::string PairValue::toString() const {
    std::stringstream ss;
    ss << "(" << *this->car;
    const Value* cdr = this->cdr.get();
    while (cdr->isPair()) {
        auto pair = static_cast<const PairValue*>(cdr);
        ss << " " << *pair->getCar();
        cdr = pair->getCdr().get();
    }
    if (cdr->isNil()) {
        ss << ")";
    } else {
        ss << " . " << *cdr << ")";
    }
    return ss.str();
}

std::string NumberValue::toString() const {
    return value == std::floor(value) && std::isfinite(value) ? std::to_string(std::int64_t(value))
                                                              : std::to_string(value);
}

std::string StringValue::toString() const {
    std::stringstream ss;
    ss << std::quoted(value);
    return ss.str();
}

std::string BuiltinProcValue::toString() const {
    return "#<procedure:builtin>";
}

std::string LambdaValue::toString() const {
    return "#<procedure>";
}

ValuePtr LambdaValue::apply(const std::vector<ValuePtr>& args) {
    auto childEnv = env->createChildEnv(params, args);
    auto result = childEnv->evalList(body);
    return std::move(result.back());
}

std::ostream& Value::print() const {
    if (isSymbol() || isPair() || isNil()) {
        return std::cout << '\'' << toString() << std::endl;
    } else {
        return std::cout << toString() << std::endl;
    }
}

std::ostream& operator<<(std::ostream& os, const Value& value) {
    return os << value.toString();
}