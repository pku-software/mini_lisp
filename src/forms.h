#ifndef FORMS_H
#define FORMS_H

#include <memory>
#include <string>
#include <unordered_map>

#include "./eval_env.h"
#include "./value.h"


using SpecialFormType = ValuePtr(ValuePtr, EvaluateEnv&);

extern const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS;

#endif