#ifndef WASM_ENV
#define WASM_ENV

#include <string>

#include "./eval_env.h"

class WasmEnv {
private:
    std::shared_ptr<EvaluateEnv> env;
public:
    WasmEnv();

    std::string eval(const std::string& code);
};

#endif