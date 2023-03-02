#ifdef __EMSCRIPTEN__

#include <emscripten/bind.h>

#include "./eval_env.h"
#include "./reader.h"
#include "./tokenizer.h"

class WasmEnv {
private:
    std::shared_ptr<EvaluateEnv> env;

public:
    WasmEnv() : env{EvaluateEnv::create()} {}

    std::string eval(const std::string& code) {
        auto tokens = Tokenizer::tokenize(code);
        Reader reader(tokens);
        auto result = env->eval(reader.read());
        if (result->isSymbol() || result->isPair() || result->isNil()) {
            return "'" + result->toString();
        } else {
            return result->toString();
        }
    }
};

EMSCRIPTEN_BINDINGS(mini_lisp) {
    emscripten::class_<WasmEnv>("WasmEnv")
        .constructor()
        .function("eval", &WasmEnv::eval);
}

#endif