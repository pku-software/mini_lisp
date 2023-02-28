#include "./wasm_env.h"

#ifdef WASM
#include <emscripten/bind.h>
#endif

#include "./reader.h"
#include "./tokenizer.h"

WasmEnv::WasmEnv(): env{EvaluateEnv::create()} {}

std::string WasmEnv::eval(const std::string& code) {
    auto tokens = Tokenizer::tokenize(code);
    Reader reader(tokens);
    return env->eval(reader.read())->toString();
}

#ifdef WASM

using namespace emscripten;

EMSCRIPTEN_BINDINGS(mini_lisp) {
    class_<WasmEnv>("WasmEnv")
        .constructor()
        .function("eval", &WasmEnv::eval);
}

#endif