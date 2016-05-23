#include <iostream>

#include "ast/small_stmt.hpp"
#include "ast/small_expr.hpp"
#include "ast/small_values.hpp"
#include "evaluator/small_eval_visitor.hpp"

extern "C" Statement *runparser(FILE *source);

int main(int argc, const char** argv) {
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

    EvalVisitor *evaluator = new EvalVisitor();

    ast->accept(*evaluator);

    std::cout << "Final value: " << evaluator->getResult()->toString() << std::endl;
}
