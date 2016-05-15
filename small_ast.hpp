#ifndef SMALL_AST_H
#define SMALL_AST_H

#include<string>
#include<map>

#include "small_expr.hpp"
#include "small_stmt.hpp"
#include "small_ops.hpp"
#include "small_values.hpp"
#include "small_env.hpp"

class AST {
    Statement *root;

    Env env_eval(Env env) {
        return root->evaluate(env);
    }

    public:
        AST(Statement *r) {
            root = r->clone();
        }

        AST(const AST &other) {
            root = other.root->clone();
        }

        ~AST() {
            delete root;
        }

        AST *clone() {
            return new AST(*this);
        }

        std::string toString() {
            return root->toString();
        }

        Env eval() {
            Env env;
            return env_eval(env);
        }
};
#endif
