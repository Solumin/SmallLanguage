#ifndef SMALL_EXPR_HPP
#define SMALL_EXPR_HPP

#include <string>
#include <vector>

#include "ast/small_ops.hpp"

#include "small_lang_forwards.h"

class Expr {
    public:
        virtual ~Expr() {}

        virtual Expr *clone() = 0;

        virtual std::string toString() = 0;

        virtual void accept(Visitor &v) = 0;
};

class EId : public Expr {
    std::string id;

    public:
    EId (std::string);

    EId (const char*);

    EId (const EId &);

    virtual ~EId();

    virtual Expr *clone();

    virtual std::string toString();

    virtual void accept(Visitor&);

    std::string getID();
};

class EInt : public Expr {
    int value;

    public:
    EInt (int);

    EInt (const EInt &);

    virtual ~EInt();

    virtual Expr *clone();

    virtual std::string toString();

    virtual void accept(Visitor&);

    int getValue();
};

class EFloat : public Expr {
    float value;

    public:
    EFloat (float);

    EFloat (const EFloat &);

    virtual ~EFloat();

    virtual Expr *clone();

    virtual std::string toString();

    virtual void accept(Visitor&);

    float getValue();
};

class EBool : public Expr {
    bool value;

    public:
    EBool (bool);

    EBool (const EBool &);

    virtual ~EBool();

    virtual Expr *clone();

    virtual std::string toString();

    virtual void accept(Visitor&);

    bool getValue();
};

class EChar : public Expr {
    char value;

    public:
    EChar (char);

    EChar (const EChar &);

    virtual ~EChar();

    virtual Expr *clone();

    virtual std::string toString();

    virtual void accept(Visitor&);

    char getValue();
};

class EString : public Expr {
    std::string value;

    public:
    EString (std::string);

    EString (const char*);

    EString (const EString &);

    virtual ~EString();

    virtual Expr *clone();

    virtual std::string toString();

    virtual void accept(Visitor&);

    std::string getValue();
};

class EList : public Expr {
    std::vector<Expr*> value;

    public:
    EList ();

    EList (std::vector<Expr*>);

    EList (Expr *e);

    EList (const EList &);

    virtual ~EList();

    virtual Expr *clone();

    virtual std::string toString();

    virtual void accept(Visitor&);

    std::vector<Expr*> getValue();
};

class ETuple : public Expr {
    std::vector<Expr*> value;
    int size;

    public:
    ETuple();

    ETuple (std::vector<Expr*>);

    ETuple (const ETuple &);

    virtual ~ETuple();

    virtual Expr *clone();

    virtual std::string toString();

    virtual void accept(Visitor&);

    std::vector<Expr*> getValue();
};

class EOp2 : public Expr {
    Expr *left, *right;
    Op2 op;

    public:
    EOp2 (Op2, Expr *l, Expr *r);

    EOp2 (const EOp2 &);

    virtual ~EOp2();

    virtual Expr *clone();

    virtual std::string toString();

    virtual void accept(Visitor&);

    Op2 getOp();

    Expr *getLeft();

    Expr *getRight();
};

class EOp1 : public Expr {
    Expr *e;
    Op1 op;

    public:
    EOp1 (Op1, Expr *x);

    EOp1 (const EOp1 &);

    virtual ~EOp1();

    virtual Expr *clone();

    virtual std::string toString();

    virtual void accept(Visitor&);

    Op1 getOp();

    Expr *getExpr();
};

class ELambda : public Expr {
    std::vector<std::string> params;
    Statement *body;

    public:
    ELambda (std::vector<char*>, Statement *);

    ELambda (const ELambda &);

    virtual ~ELambda();

    virtual Expr *clone();

    virtual std::string toString();

    virtual void accept(Visitor&);

    std::vector<std::string> getParams();

    Statement *getBody();
};

class EApp : public Expr {
    Expr *func;
    std::vector<Expr*> args;

    public:
    EApp (Expr *f, std::vector<Expr*>);

    EApp (const EApp &);

    virtual ~EApp();

    virtual Expr *clone();

    virtual std::string toString();

    virtual void accept(Visitor&);

    Expr *getFunc();

    std::vector<Expr*> getArgs();
};

class EIf : public Expr {
    Expr *cond;
    Expr *true_body;
    Expr *false_body;

    public:
    EIf (Expr *c, Expr *t, Expr *f);

    EIf (const EIf &);

    virtual ~EIf();

    virtual Expr *clone();

    virtual std::string toString();

    virtual void accept(Visitor&);

    Expr *getCond();

    Expr *getTrueBody();

    Expr *getFalseBody();
};

#endif
