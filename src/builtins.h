#ifndef BUILTINS_H
#define BUILTINS_H

#include <memory>
#include <string>
#include <unordered_map>
#include <limits>

#include "./value.h"


void checkArgsCount(const std::vector<ValuePtr>& args, std::size_t min,
                    std::size_t max = std::numeric_limits<std::size_t>::max());

extern const std::unordered_map<std::string, BuiltinFuncType*> BUILTINS;

#endif