#ifndef SMALL_STMTS_HPP
#define SMALL_STMTS_HPP

#include <string>

#include "small_expr.hpp"

class Expr;

class Statement {
    public:
    virtual ~Statement() {}

    virtual Statement *clone() = 0;

    virtual std::string toString() = 0;
};

class Seq : public Statement {
    Statement *s1, *s2;
    public:
    Seq (Statement*, Statement*);

    Seq (const Seq&);

    virtual ~Seq();

    virtual Statement *clone();

    virtual std::string toString();
};

class Assign : public Statement {
    std::string id;
    Expr *e;
    public:
    Assign (std::string, Expr*);

    Assign (const Assign&);

    virtual ~Assign();

    virtual Statement *clone();

    virtual std::string toString();
};

class Return : public Statement {
    Expr *e;
    public:
    Return (Expr*);

    Return (const Return&);

    virtual ~Return();

    virtual Statement *clone();

    virtual std::string toString();
};

#endif
