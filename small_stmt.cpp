#include <string>

#include "small_stmt.hpp"
#include "small_expr.hpp"
/* #include "small_lang_forwards.h" */

Seq::Seq (Statement *a, Statement *b) {
    s1 = a->clone();
    s2 = b->clone();
}

Seq::Seq (const Seq &other) {
    s1 = other.s1->clone();
    s2 = other.s2->clone();
}

Seq::~Seq() {
    delete s1;
    delete s2;
}

Statement *Seq::clone() {
    return new Seq(*this);
}

std::string Seq::toString() {
    return s1->toString() + "\n" + s2->toString();
}

Env Seq::evaluate(Env env) {
    return s2->evaluate(s1->evaluate(env));
}


Assign::Assign (std::string name, Expr *lhs) {
    id = name;
    e = lhs->clone();
}

Assign::Assign (const Assign &other) {
    id = other.id;
    e = other.e->clone();
}

Assign::~Assign() {
    delete e;
}

Statement *Assign::clone() {
    return new Assign(*this);
}

std::string Assign::toString() {
    return id + " = " + e->toString() + ";";
}

Env Assign::evaluate(Env env) {
    Value *res = e->evaluate(env);
    if (env.count(id) > 0) {
        throw "Variable already exists";
    } else {
        env.insert({id, res});
    }
    return env;
}


Return::Return (Expr *any) {
    e = any->clone();
}

Return::Return (const Return &other) {
    e = other.e->clone();
}

Return::~Return() {
    delete e;
}

Statement *Return::clone() {
    return new Return(*this);
}

std::string Return::toString() {
    return "return " + e->toString() + ";";
}

Env Return::evaluate(Env env) {
    Value *res = e->evaluate(env);
    env.insert({"return", res});
    return env;
}
