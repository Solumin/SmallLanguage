#ifndef SMALL_AST_H
#define SMALL_AST_H

#include <string>
#include <sstream>
#include <vector>

#include <iostream>

// TODO: Separate files
class Expr {
public:
    virtual ~Expr() {}

    virtual Expr *clone() = 0;

    virtual std::string toString() = 0;

    // Should also have the value of the expr
    // virtual Value evaluate(env) = 0;
};

class Statement {
public:
    virtual ~Statement() {}

    virtual Statement *clone() = 0;

    virtual std::string toString() = 0;
};

// TODO: Simplify this:
// - Doesn't involve cast to access Op name
// - Ensures Op2 and Op2Strings are always in sync (macro?)
// - Same with Op1
enum class Op2 {
    // Arithmetic
    Add
    ,Sub
    ,Mul
    ,Div
    ,Mod

    // Boolean
    ,LAnd
    ,LOr

    // Comparative
    ,Lt
    ,Lte
    ,Gt
    ,Gte
    ,Eq
};

static const std::string Op2Strings[] = {
    "+"
    ,"-"
    ,"*"
    ,"/"
    ,"%"
    ,"&&"
    ,"||"
    ,"<"
    ,"<="
    ,">"
    ,">="
    ,"=="
};

enum class Op1 {
    // Arithmetic
    Neg

    // Boolean
    ,LNot
};

static const std::string Op1Strings[] = {
    "-"
    ,"!"
};


class EId : public Expr {
    std::string id;

public:
    EId (std::string name) {
        id = name;
    }

    EId (const char* name) {
        id = std::string(name);
    }

    // Copy constructor
    EId (const EId &other) {
        id = other.id;
    }

    virtual ~EId() {}

    virtual Expr *clone() {
        return new EId(*this);
    }

    virtual std::string toString() {
        return id;
    }
};

class EInt : public Expr {
    int value;

public:
    EInt (int v) {
        value = v;
    }

    EInt (const EInt &other) {
        value = other.value;
    }

    virtual ~EInt() {}

    virtual Expr *clone() {
        return new EInt(*this);
    }

    virtual std::string toString() {
        return std::to_string(value);
    }
};

class EFloat : public Expr {
    float value;

public:
    EFloat (float v) {
        value = v;
    }

    EFloat (const EFloat &other) {
        value = other.value;
    }

    virtual ~EFloat() {}

    virtual Expr *clone() {
        return new EFloat(*this);
    }

    virtual std::string toString() {
        return std::to_string(value);
    }
};

class EBool : public Expr {
    bool value;

public:
    EBool (bool b) {
        value = b;
    }

    EBool (const EBool &other) {
        value = other.value;
    }

    virtual ~EBool() {}

    virtual Expr *clone() {
        return new EBool(*this);
    }

    virtual std::string toString() {
        return value ? "true" : "false";
    }
};

class EChar : public Expr {
    char value;

public:
    EChar (char c) {
        value = c;
    }

    EChar (const EChar &other) {
        value = other.value;
    }

    virtual ~EChar() {}

    virtual Expr *clone() {
        return new EChar(*this);
    }

    // TODO: Print 'c', '\xNN', '\'' depending!
    virtual std::string toString() {
        return std::string(1, value);
    }
};

class EString : public Expr {
    std::string value;

public:
    EString (std::string v) {
        value = v;
    }

    EString (const char* str) {
        value = std::string(str);
    }

    EString (const EString &other) {
        value = other.value;
    }

    virtual ~EString() {}

    virtual Expr *clone() {
        return new EString(*this);
    }

    virtual std::string toString() {
        return value;
    }
};

class EList : public Expr {
    std::vector<Expr*> value;
public:
    EList () {}

    EList (std::vector<Expr*> l) {
        value = std::vector<Expr*>(l);
    }

    EList (Expr *e) {
        value.push_back(e);
    }

    EList (const EList &other) {
        value = other.value;
    }

    virtual ~EList() {
        delete &value;
    }

    virtual Expr *clone() {
        return new EList(*this);
    }

    virtual std::string toString() {
        std::stringstream str;
        str << "[";
        for (std::vector<Expr*>::iterator it = value.begin(); it != value.end(); ++it) {
            str << (*it)->toString();
            if (++it != value.end())
                str << ", ";
            it--;
        }
        str << "]";
        return str.str();
    }
};

class ETuple : public Expr {
    std::vector<Expr*> value;
    int size;
public:
    ETuple() { size = 0; }

    ETuple (std::vector<Expr*> l) {
        value = std::vector<Expr*>(l);
        size = l.size();
    }

    ETuple (const ETuple &other) {
        value = other.value;
        size = other.size;
    }

    virtual ~ETuple() {
        delete &value;
    }

    virtual Expr *clone() {
        return new ETuple(*this);
    }

    virtual std::string toString() {
        std::stringstream str;
        str << "(";
        for (std::vector<Expr*>::iterator it = value.begin(); it != value.end(); ++it) {
            str << (*it)->toString();
            if (it + 1 != value.end())
                str << ", ";
        }
        str << ")";
        return str.str();
    }
};

class EOp2 : public Expr {
    Expr *left, *right;
    Op2 op;
public:
    EOp2 (Op2 o, Expr *l, Expr *r) {
        op = o;
        left = l->clone();
        right = r->clone();
    }

    EOp2 (const EOp2 &other) {
        op = other.op;
        left = other.left->clone();
        right = other.right->clone();
    }

    virtual ~EOp2() {
        delete left;
        delete right;
    }

    virtual Expr *clone() {
        return new EOp2(*this);
    }

    virtual std::string toString() {
        return left->toString() + Op2Strings[(int)op] + right->toString();
    }
};

class EOp1 : public Expr {
    Expr *e;
    Op1 op;
public:
    EOp1 (Op1 o, Expr *x) {
        op = o;
        e = x->clone();
    }

    EOp1 (const EOp1 &other) {
        op = other.op;
        e = other.e->clone();
    }

    virtual ~EOp1() {
        delete e;
    }

    virtual Expr *clone() {
        return new EOp1(*this);
    }

    virtual std::string toString() {
        return Op1Strings[(int)op] + e->toString();
    }
};

class ELambda : public Expr {
    std::vector<std::string> params;
    Statement *body;
public:
    ELambda (std::vector<char*> ids, Statement *b) {
        for (std::vector<char*>::iterator it = ids.begin(); it != ids.end(); ++it) {
            params.push_back(std::string(*it));
        }
        body = b->clone();
    }

    ELambda (const ELambda &other) {
        params = std::vector<std::string>(other.params);
        body = other.body->clone();
    }

    virtual ~ELambda() {
        delete &params;
        delete body;
    }

    virtual Expr *clone() {
        return new ELambda(*this);
    }

    virtual std::string toString() {
        std::stringstream str;
        str << "(\\ ";
        for (std::vector<std::string>::iterator it = params.begin(); it != params.end(); ++it) {
            str << *it << " ";
        }
        str << "-> " << body->toString() << ")";
        return str.str();
    }
};

class EApp : public Expr {
    Expr *func;
    std::vector<Expr*> args;
public:
    EApp (Expr *f, std::vector<Expr*> as) {
        func = f->clone();
        args = std::vector<Expr*>(as);
    }

    EApp (const EApp &other) {
        func = other.func->clone();
        args = std::vector<Expr*>(other.args);
    }

    virtual ~EApp() {
        delete func;
        delete &args;
    }

    virtual Expr *clone() {
        return new EApp(*this);
    }

    virtual std::string toString() {
        std::stringstream str;
        str << func->toString() << '(';
        for (std::vector<Expr*>::iterator it = args.begin(); it != args.end(); ++it) {
            str << (*it)->toString();
            if (it + 1 != args.end())
                str << ", ";
        }
        str << ')';
        return str.str();
    }
};

class EIf : public Expr {
    Expr *cond;
    Expr *true_body;
    Expr *false_body;
public:
    EIf (Expr *c, Expr *t, Expr *f) {
        cond = c->clone();
        true_body = t->clone();
        false_body = f->clone();
    }

    EIf (const EIf &other) {
        cond = other.cond->clone();
        true_body = other.true_body->clone();
        false_body = other.false_body->clone();
    }

    virtual ~EIf() {
        delete cond;
        delete true_body;
        delete false_body;
    }

    virtual Expr *clone() {
        return new EIf(*this);
    }

    virtual std::string toString() {
        return "if " + cond->toString() +
               " then " + true_body->toString() +
               " else " + false_body->toString();
    }
};

// ==========
// STATEMENTS
// ==========

class Skip : public Statement {
public:
    Skip () {}
    virtual ~Skip() {}
    virtual Statement *clone() { return this; }
    virtual std::string toString() { return ""; }
};

class Seq : public Statement {
    Statement *s1, *s2;
public:
    Seq (Statement *a, Statement *b) {
        s1 = a->clone();
        s2 = b->clone();
    }

    Seq (Statement *a) : s1(a) {
        s2 = new ::Skip();
    }

    Seq (const Seq &other) {
        s1 = other.s1->clone();
        s2 = other.s2->clone();
    }

    virtual ~Seq() {
        delete s1;
        delete s2;
    }

    virtual Statement *clone() {
        return new Seq(*this);
    }

    virtual std::string toString() {
        return s1->toString() + "\n" + s2->toString();
    }
};

class Assign : public Statement {
    std::string id;
    Expr *e;
public:
    Assign (std::string name, Expr *lhs) {
        id = name;
        e = lhs->clone();
    }

    Assign (const Assign &other) {
        id = other.id;
        e = other.e->clone();
    }

    virtual ~Assign() {
        delete e;
    }

    virtual Statement *clone() {
        return new Assign(*this);
    }

    virtual std::string toString() {
        return id + " = " + e->toString() + ";";
    }
};

class Return : public Statement {
    Expr *e;
public:
    Return (Expr *any) {
        e = any->clone();
    }

    Return (const Return &other) {
        e = other.e->clone();
    }

    virtual ~Return() {
        delete e;
    }

    virtual Statement *clone() {
        return new Return(*this);
    }

    virtual std::string toString() {
       return "return " + e->toString() + ";";
    }
};

/* class Function : public Statement { */
/*     std::string name; */
/*     std::vector<std::string> params; */
/*     Statement *body; */
/* public: */
/*     Function (std::string n, std::vector<char*> ids, Statement *b) { */
/*         name = n; */
/*         body = b->clone(); */
/*         for (std::vector<char*>::iterator it = ids.begin(); it != ids.end(); ++it) { */
/*             params.push_back(std::string(*it)); */
/*         } */
/*     } */

/*     Function (char* n, std::vector<char*> ids, Statement *b) { */
/*         name = std::string(n); */
/*         body = b->clone(); */
/*         for (std::vector<char*>::iterator it = ids.begin(); it != ids.end(); ++it) { */
/*             params.push_back(std::string(*it)); */
/*         } */
/*     } */

/*     Function (const Function &other) { */
/*         name = other.name; */
/*         params = std::vector<std::string>(other.params); */
/*         body = other.body->clone(); */
/*     } */

/*     virtual ~Function() { */
/*         delete &params; */
/*         delete body; */
/*     } */

/*     virtual Statement *clone() { */
/*         return new Function(*this); */
/*     } */

/*     virtual std::string toString() { */
/*         std::stringstream str; */
/*         str << name << " "; */
/*         for (std::vector<std::string>::iterator it = params.begin(); it != params.end(); ++it) { */
/*             str << *it << " "; */
/*         } */
/*         str << "= (" << body->toString() << ")"; */
/*         return str.str(); */
/*     } */
/* }; */



class Any : public Statement {
    Expr *e;
public:
    Any (Expr *any) {
        e = any->clone();
    }

    Any (const Any &other) {
        e = other.e->clone();
    }

    virtual ~Any() {
        delete e;
    }

    virtual Statement *clone() {
        return new Any(*this);
    }

    virtual std::string toString() {
       return e->toString();
    }
};
#endif
