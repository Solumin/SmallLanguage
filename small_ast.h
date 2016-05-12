#ifndef SMALL_AST_H
#define SMALL_AST_H

#include <string>
#include <list>

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
        return "[fuck you]";
    }
};

class Statement {
protected:
    const std::string semiend = ";\n";

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
        return s1->toString() + semiend + s2->toString();
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
