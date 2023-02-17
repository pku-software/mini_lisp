#ifndef VALUE_H
#define VALUE_H

#include <concepts>
#include <functional>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

class Value;
using ValuePtr = std::shared_ptr<Value>;
class PairValue;

class Value {
public:
    virtual std::string toString() const = 0;
    virtual ~Value() = default;
    auto operator<=>(const Value&) const = default;

    std::ostream& print() const;

    bool isSymbol() const;
    bool isNil() const;
    bool isBoolean() const;
    bool isNumber() const;
    bool isString() const;
    bool isPair() const;
    bool isAtom() const;
    bool isSelfEvaluating() const;
    bool isProcedure() const;

    bool isList() const;
    bool isTrue() const;

    const std::string* getSymbolName() const;

    bool asBool() const;
    double asNumber() const;
    const std::string& asString() const;
    const PairValue& asPair() const;
    std::vector<ValuePtr> toVector() const;

    static ValuePtr nil();
    static ValuePtr fromBoolean(bool);
    static ValuePtr fromNumber(double);
    static ValuePtr fromVector(const std::vector<ValuePtr>&);
};

class NilValue final : public Value {
public:
    NilValue() = default;
    bool operator==(const NilValue&) const = default;
    std::string toString() const override {
        return "()";
    }
};

class IdentifierValue final : public Value {
private:
    std::string name;

public:
    IdentifierValue(const std::string& name) : name{name} {}
    bool operator==(const IdentifierValue&) const = default;

    const std::string& getName() const {
        return name;
    }

    std::string toString() const override {
        return name;
    }
};

class BooleanValue final : public Value {
private:
    bool value;

public:
    BooleanValue(bool value) : value{value} {}
    bool operator==(const BooleanValue&) const = default;

    bool getValue() const {
        return value;
    }

    std::string toString() const override {
        return value ? "#t" : "#f";
    }
};

class NumberValue final : public Value {
private:
    double value;

public:
    NumberValue(double value) : value{value} {}
    auto operator<=>(const NumberValue&) const = default;

    double getValue() const {
        return value;
    }

    std::string toString() const override;
};

class StringValue final : public Value {
private:
    std::string value;

public:
    StringValue(const std::string& value) : value{value} {}
    auto operator<=>(const StringValue&) const = default;

    const std::string& getValue() const {
        return value;
    }

    std::string toString() const override;
};

class PairValue final : public Value {
private:
    ValuePtr car;
    ValuePtr cdr;

public:
    PairValue(ValuePtr car, ValuePtr cdr) : car{std::move(car)}, cdr{std::move(cdr)} {}

    ValuePtr getCar() const {
        return car;
    }
    ValuePtr getCdr() const {
        return cdr;
    }
    void setCar(ValuePtr v) {
        car = v;
    }
    void setCdr(ValuePtr v) {
        cdr = v;
    }
    std::string toString() const override;

    template <std::size_t I>
    std::tuple_element_t<I, PairValue> get() const {
        if constexpr (I == 0) {
            return car;
        } else if constexpr (I == 1) {
            return cdr;
        } else {
            static_assert(I < 2, "Index out of bounds");
        }
    }
};

template <>
struct std::tuple_size<PairValue> {
    static constexpr std::size_t value{2};
};

template <std::size_t I>
struct std::tuple_element<I, PairValue> {
    using type = ValuePtr;
};

class EvaluateEnv;

using BuiltinFuncTypeNoEnv = ValuePtr(const std::vector<ValuePtr>&);
using BuiltinFuncType = ValuePtr(const std::vector<ValuePtr>&, EvaluateEnv&);

class BuiltinProcValue final : public Value {
private:
    BuiltinFuncType* func;

public:
    BuiltinProcValue(BuiltinFuncType* func) : func{std::move(func)} {}

    ValuePtr apply(const std::vector<ValuePtr>& args, EvaluateEnv& env) const {
        return func(args, env);
    }
    std::string toString() const override;
};

class LambdaValue final : public Value {
private:
    std::vector<std::string> params;
    ValuePtr body;
    std::shared_ptr<EvaluateEnv> env;

public:
    LambdaValue(const std::vector<std::string>& params, ValuePtr body,
                std::shared_ptr<EvaluateEnv> env)
        : params{params}, body{std::move(body)}, env{std::move(env)} {}

    ValuePtr apply(const std::vector<ValuePtr>& args);

    std::string toString() const override;
};

std::ostream& operator<<(std::ostream& os, const Value& value);

#endif