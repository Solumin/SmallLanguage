#include <iostream>

#include "ast/small_stmt.hpp"

extern "C" Statement *runparser(FILE *source);

int main( int argc, char** argv) {
    FILE *source = fopen(argv[1], "r");
    if (!source) {
        std::cout << "Failed to open " << argv[1] << std::endl;
        return 1;
    }

    Statement *ast = runparser(source);
    fclose(source);

    if (ast == NULL) {
        std::cout << "Parsing failed." << std::endl;
        return 2;
    } else {
        std::cout << "Parsing completed." << std::endl;
        std::cout << "The program:\n" << ast->toString() << std::endl;
    }
    return 0;
}
