#include "./repl.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>

#include "./error.h"
#include "./eval_env.h"
#include "./reader.h"
#include "./tokenizer.h"

namespace rg = std::ranges;

void readEvalPrintLoop() {
    std::string line;
    std::deque<TokenPtr> tokens;
    Reader reader(tokens, [&](bool topLevel) {
        std::cout << (topLevel ? ">>> " : " .. ");
        std::getline(std::cin, line);
        if (std::cin.eof()) {
            return false;
        }
        rg::move(Tokenizer::tokenize(line), std::back_inserter(tokens));
        return true;
    });
    auto env = EvaluateEnv::create();
    while (true) {
        try {
            auto result = env->eval(reader.read());
            result->print();
        } catch (EOFError&) {
            break;
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

void loadFile(const char* filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        std::exit(1);
    }
    std::string line;
    std::deque<TokenPtr> tokens;
    while (std::getline(file, line)) {
        rg::move(Tokenizer::tokenize(line), std::back_inserter(tokens));
    }
    auto env = EvaluateEnv::create();
    Reader reader(tokens);
    try {
        while (true) {
            env->eval(reader.read());
        }
    } catch (EOFError&) {
    } catch (std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}