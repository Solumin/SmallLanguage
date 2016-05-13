#ifndef SMALL_AST_H
#define SMALL_AST_H

#include <string>
#include <sstream>
#include <list>
#include <vector>

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

class Expr {
public:
    virtual ~Expr() {}

    virtual Expr *clone() = 0;

    virtual std::string toString() = 0;

    // Should also have the value of the expr
    // virtual Value evaluate(env) = 0;
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
    std::list<Expr*> value;
public:
    EList () {}

    EList (std::list<Expr*> l) {
        value = l;
    }

    EList (Expr *e) {
        value.push_front(e);
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
        for (std::list<Expr*>::iterator it = value.begin(); it != value.end(); ++it) {
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
    ETuple() {}

    ETuple (std::vector<Expr*> l) {
        value = l;
        size = l.size();
    }

    ETuple (std::list<Expr*> l) {
        for (std::list<Expr*>::iterator it = l.begin(); it != l.end(); ++it) {
            value.push_back(*it);
        }
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

class Statement {
public:
    virtual ~Statement() {}

    virtual Statement *clone() = 0;

    virtual std::string toString() = 0;
};

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
        return id + " = " + e->toString();
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
       return "RETURN " + e->toString();
    }
};


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
