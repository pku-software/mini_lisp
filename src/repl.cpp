#include <string>
#include <iostream>
#include <iomanip>

#include "./tokenizer.h"
#include "./error.h"

void readEvalPrintLoop() {
    std::string line;
    while (true) {
        std::cout << ">>> ";
        std::getline(std::cin, line);
        if (std::cin.eof()) {
            break;
        }
        try {
            auto tokens = Tokenizer(line).tokenize();
            for (auto& token : tokens) {
                std::cout << *token << " ";
            }
        } catch (EOFError& e) {
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what();
        }
        std::cout << std::endl;
    }
}