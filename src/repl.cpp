#include <string>
#include <iostream>
#include <iomanip>

#include "./tokenizer.h"
#include "./reader.h"
#include "./eval_env.h"
#include "./error.h"

void readEvalPrintLoop() {
    std::string line;
    EvaluateEnv evalEnv;
    while (true) {
        std::cout << ">>> ";
        std::getline(std::cin, line);
        if (std::cin.eof()) {
            break;
        }
        try {
            auto tokens = Tokenizer(line).tokenize();
            Reader reader(std::move(tokens));
            std::shared_ptr value = reader.read();
            auto result = evalEnv.eval(value);
            if (result->isSymbol() || result->isPair() || result->isNil()) {
                std::cout << "'";
            }
            std::cout << *result;
        } catch (EOFError& e) {
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what();
        }
        std::cout << std::endl;
    }
}