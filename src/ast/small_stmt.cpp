#include <string>

#include "ast/small_stmt.hpp"
#include "ast/small_expr.hpp"
#include "visitor/small_visitor.hpp"

// ================
// ***** Seq ******
// ================

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

void Seq::accept(Visitor &v) {
    v.visit(this);
}

Statement *Seq::getFirst() {
    return s1;
}

Statement *Seq::getSecond() {
    return s2;
}

// ================
// *** Assign *****
// ================

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

void Assign::accept(Visitor &v) {
    v.visit(this);
}

std::string Assign::getID() {
    return id;
}

Expr *Assign::getRHS() {
    return e;
}

// ================
// *** Return *****
// ================

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

void Return::accept(Visitor &v) {
    v.visit(this);
}

Expr *Return::getRHS() {
    return e;
}