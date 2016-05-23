#ifndef SMALL_VALUES_H
#define SMALL_VALUES_H

#include <string>
#include <sstream>
#include <vector>

#include "small_lang_forwards.h"

class Value {
    public:
        virtual ~Value() {}

        virtual Value *clone() = 0;

        virtual std::string toString() = 0;
};

class VInt : public Value {
    int value;

    public:
    VInt (int v) {
        value = v;
    }

    VInt (const VInt &other) {
        value = other.value;
    }

    virtual ~VInt() {}

    virtual Value *clone() {
        return new VInt(*this);
    }

    virtual std::string toString() {
        return std::to_string(value);
    }

    int getValue() {
        return value;
    }
};

class VFloat : public Value {
    float value;

    public:
    VFloat (float v) {
        value = v;
    }

    VFloat (const VFloat &other) {
        value = other.value;
    }

    virtual ~VFloat() {}

    virtual Value *clone() {
        return new VFloat(*this);
    }

    virtual std::string toString() {
        return std::to_string(value);
    }

    float getValue() {
        return value;
    }
};

class VBool : public Value {
    bool value;

    public:
    VBool (bool b) {
        value = b;
    }

    VBool (const VBool &other) {
        value = other.value;
    }

    virtual ~VBool() {}

    virtual Value *clone() {
        return new VBool(*this);
    }

    virtual std::string toString() {
        return value ? "true" : "false";
    }

    bool getValue() {
        return value;
    }
};

class VChar : public Value {
    char value;

    public:
    VChar (char c) {
        value = c;
    }

    VChar (const VChar &other) {
        value = other.value;
    }

    virtual ~VChar() {}

    virtual Value *clone() {
        return new VChar(*this);
    }

    // TODO: Print 'c', '\xNN', '\'' depending!
    virtual std::string toString() {
        return std::string(1, value);
    }

    char getValue() {
        return value;
    }
};

class VString : public Value {
    std::string value;

    public:
    VString (std::string v) {
        value = v;
    }

    VString (const char* str) {
        value = std::string(str);
    }

    VString (const VString &other) {
        value = other.value;
    }

    virtual ~VString() {}

    virtual Value *clone() {
        return new VString(*this);
    }

    virtual std::string toString() {
        return value;
    }

    string getValue() {
        return value;
    }
};

class VList : public Value {
    std::vector<Value*> value;
    public:
    VList () {}

    VList (std::vector<Value*> l) {
        value = std::vector<Value*>(l);
    }

    VList (Value *e) {
        value.push_back(e);
    }

    VList (const VList &other) {
        value = other.value;
    }

    virtual ~VList() {
        delete &value;
    }

    virtual Value *clone() {
        return new VList(*this);
    }

    virtual std::string toString() {
        std::stringstream str;
        str << "[";
        for (std::vector<Value*>::iterator it = value.begin(); it != value.end(); ++it) {
            str << (*it)->toString();
            if (++it != value.end())
                str << ", ";
            it--;
        }
        str << "]";
        return str.str();
    }

    std::vector<Value*> getValue() {
        return value;
    }
};

class VTuple : public Value {
    std::vector<Value*> value;
    int size;
    public:
    VTuple() { size = 0; }

    VTuple (std::vector<Value*> l) {
        value = std::vector<Value*>(l);
        size = l.size();
    }

    VTuple (const VTuple &other) {
        value = other.value;
        size = other.size;
    }

    virtual ~VTuple() {
        delete &value;
    }

    virtual Value *clone() {
        return new VTuple(*this);
    }

    virtual std::string toString() {
        std::stringstream str;
        str << "(";
        for (std::vector<Value*>::iterator it = value.begin(); it != value.end(); ++it) {
            str << (*it)->toString();
            if (it + 1 != value.end())
                str << ", ";
        }
        str << ")";
        return str.str();
    }

    std::vector<Value*> getValue() {
        return value;
    }
};

class VClos : public Value {
    ELambda *lambda;
    Env env;
    public:
    VClos (Expr *l, Env e) {
        lambda = l->clone();
        env = e;
    }

    VClos (const VClos &other) {
        lambda = other.lambda->clone();
        env = other.env;
    }

    virtual ~VClos() {
        delete lambda;
        delete env;
    }

    virtual Value *clone() {
        return new VClos(*this);
    }

    virtual std::string toString() {
        return lambda->toString();
    }

    ELambda *getLambda() {
        return lambda;
    }

    Env getEnv() {
        return env;
    }
};

#endif
