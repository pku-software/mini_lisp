#include "./repl.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        readEvalPrintLoop();
    }
}