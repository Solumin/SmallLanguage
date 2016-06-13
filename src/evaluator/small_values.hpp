#ifndef SMALL_VALUES_H
#define SMALL_VALUES_H

#include <string>
#include <sstream>
#include <vector>

#include "small_env.hpp"
#include "small_lang_forwards.hpp"
#include "ast/small_expr.hpp"

class Value {
    public:
        virtual ~Value() {}
        virtual Value *clone() const = 0;
        virtual std::string toString() const = 0;
        virtual Value *add(const Value *rhs) const = 0;

        Value *operator+(const Value *rhs) const {
            return add(rhs);
        }
};

class VInt : public Value {
    int value;
    public:
    VInt (int v);
    VInt (const VInt &other);
    virtual ~VInt();
    virtual Value *clone() const;
    virtual std::string toString() const;
    virtual Value *add(const Value *rhs) const;
    int getValue() const;
};

class VFloat : public Value {
    float value;
    public:
    VFloat (float v);
    VFloat (const VFloat &other);
    virtual ~VFloat();
    virtual Value *clone() const;
    virtual std::string toString() const;
    virtual Value *add(const Value *rhs) const;
    float getValue() const;
};

class VBool : public Value {
    bool value;
    public:
    VBool (bool b);
    VBool (const VBool &other);
    virtual ~VBool();
    virtual Value *clone() const;
    virtual std::string toString() const;
    virtual Value *add(const Value *rhs) const;
    bool getValue() const;
};

class VChar : public Value {
    char value;
    public:
    VChar (char c);
    VChar (const VChar &other);
    virtual ~VChar();
    virtual Value *clone() const;
    virtual std::string toString() const;
    virtual Value *add(const Value *rhs) const;
    char getValue() const;
};

class VString : public Value {
    std::string value;
    public:
    VString (std::string v);
    VString (const char* str);
    VString (const VString &other);
    virtual ~VString();
    virtual Value *clone() const;
    virtual std::string toString() const;
    virtual Value *add(const Value *rhs) const;
    std::string getValue() const;
};

class VList : public Value {
    std::vector<Value*> value;
    public:
    VList ();
    VList (std::vector<Value*> l);
    VList (Value *e);
    VList (const VList &other);
    virtual ~VList();
    virtual Value *clone() const;
    virtual std::string toString() const;
    virtual Value *add(const Value *rhs) const;
    std::vector<Value*> getValue() const;
};

class VTuple : public Value {
    std::vector<Value*> value;
    int size;
    public:
    VTuple();
    VTuple (std::vector<Value*> l);
    VTuple (const VTuple &other);
    virtual ~VTuple();
    virtual Value *clone() const;
    virtual std::string toString() const;
    virtual Value *add(const Value *rhs) const;
    std::vector<Value*> getValue() const;
    int getSize() const;
};

class VClos : public Value {
    ELambda *lambda;
    Env env;
    public:
    VClos (ELambda *l, Env e);
    VClos (const VClos &other);
    virtual ~VClos();
    virtual Value *clone() const;
    virtual std::string toString() const;
    virtual Value *add(const Value *rhs) const;
    ELambda *getLambda() const;
    Env getEnv() const;
};

#endif
