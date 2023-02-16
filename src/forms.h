#ifndef FORMS_H
#define FORMS_H

#include <unordered_map>
#include <string>
#include <memory>

#include "./value.h"
#include "./eval_env.h"

using SpecialFormType = ValuePtr(ValuePtr, EvaluateEnv&);

extern const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS;

#endif