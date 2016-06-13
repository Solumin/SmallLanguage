#include "evaluator/small_values.hpp"
#include "evaluator/small_eval_errors.hpp"

//=========
// VInt
//=========

VInt::VInt (int v) {
    value = v;
}

VInt::VInt (const VInt &other) {
    value = other.value;
}

VInt::~VInt() {}

Value *VInt::clone() const {
    return new VInt(*this);
}

std::string VInt::toString() const {
    return std::to_string(value);
}

int VInt::getValue() const {
    return value;
}

Value *VInt::add(const Value *rhs) const {
    const VInt *ri = dynamic_cast<const VInt*>(rhs);
    if (ri)
        return new VInt(getValue() + ri->getValue());

    const VFloat *rf = dynamic_cast<const VFloat*>(rhs);
    if (rf)
        return new VFloat(getValue() + rf->getValue());

    throw new ArithArgMismatch();
}

//=========
// VFloat
//=========

VFloat::VFloat (float v) {
    value = v;
}

VFloat::VFloat (const VFloat &other) {
    value = other.value;
}

VFloat::~VFloat() {}

Value *VFloat::clone() const {
    return new VFloat(*this);
}

std::string VFloat::toString() const {
    return std::to_string(value);
}

float VFloat::getValue() const {
    return value;
}

Value *VFloat::add(const Value *rhs) const {
    const VFloat *rf = dynamic_cast<const VFloat*>(rhs);
    if (rf)
        return new VFloat(getValue() + rf->getValue());

    const VInt *ri = dynamic_cast<const VInt*>(rhs);
    if (ri)
        return new VFloat(getValue() + ri->getValue());

    throw new ArithArgMismatch();
}

//=========
// VBool
//=========

VBool::VBool (bool b) {
    value = b;
}

VBool::VBool (const VBool &other) {
    value = other.value;
}

VBool::~VBool() {}

Value *VBool::clone() const {
    return new VBool(*this);
}

std::string VBool::toString() const {
    return value ? "true" : "false";
}

bool VBool::getValue() const {
    return value;
}

Value *VBool::add(const Value *rhs) const {
    throw new ArithUnsupportedError();
}

// ===========
// VChar
// ==========

VChar::VChar (char c) {
    value = c;
}

VChar::VChar (const VChar &other) {
    value = other.value;
}

VChar::~VChar() {}

Value *VChar::clone() const {
    return new VChar(*this);
}

// TODO: Print 'c', '\xNN', '\'' depending!
std::string VChar::toString() const {
    return std::string(1, value);
}

char VChar::getValue() const {
    return value;
}

Value *VChar::add(const Value *rhs) const {
    throw new ArithUnsupportedError();
}

//=========
// VString
//=========

VString::VString (std::string v) {
    value = v;
}

VString::VString (const char* str) {
    value = std::string(str);
}

VString::VString (const VString &other) {
    value = other.value;
}

VString::~VString() {}

Value *VString::clone() const {
    return new VString(*this);
}

std::string VString::toString() const {
    return value;
}

std::string VString::getValue() const {
    return value;
}

Value *VString::add(const Value *rhs) const {
    throw new ArithUnsupportedError();
}

//=========
// VList
//=========

VList::VList () {}

VList::VList (std::vector<Value*> l) {
    value = std::vector<Value*>(l);
}

VList::VList (Value *e) {
    value.push_back(e);
}

VList::VList (const VList &other) {
    value = other.value;
}

VList::~VList() {
    delete &value;
}

Value *VList::clone() const {
    return new VList(*this);
}

std::string VList::toString() const {
    std::stringstream str;
    str << "[";
    for (std::vector<Value*>::const_iterator it = value.begin(); it != value.end(); ++it) {
        str << (*it)->toString();
        if (++it != value.end())
            str << ", ";
        it--;
    }
    str << "]";
    return str.str();
}

std::vector<Value*> VList::getValue() const {
    return value;
}

Value *VList::add(const Value *rhs) const {
    throw new ArithUnsupportedError();
}

//=========
// VTuple
//=========

VTuple::VTuple() { size = 0; }

VTuple::VTuple (std::vector<Value*> l) {
    value = std::vector<Value*>(l);
    size = l.size();
}

VTuple::VTuple (const VTuple &other) {
    value = other.value;
    size = other.size;
}

VTuple::~VTuple() {
    delete &value;
}

Value *VTuple::clone() const {
    return new VTuple(*this);
}

std::string VTuple::toString() const {
    std::stringstream str;
    str << "(";
    for (std::vector<Value*>::const_iterator it = value.begin(); it != value.end(); ++it) {
        str << (*it)->toString();
        if (it + 1 != value.end())
            str << ", ";
    }
    str << ")";
    return str.str();
}

Value *VTuple::add(const Value *rhs) const {
    throw new ArithUnsupportedError();
}

std::vector<Value*> VTuple::getValue() const {
    return value;
}

int VTuple::getSize() const {
    return size;
}

//=========
// VClos
//=========

// TODO: Is this the correct way to copy the env?
VClos::VClos (ELambda *l, Env e) {
    lambda = dynamic_cast<ELambda*>(l->clone());
    env = e;
}

VClos::VClos (const VClos &other) {
    lambda = dynamic_cast<ELambda*>(other.lambda->clone());
    env = other.env;
}

VClos::~VClos() {
    delete lambda;
}

Value *VClos::clone() const {
    return new VClos(*this);
}

std::string VClos::toString() const {
    return lambda->toString();
}

Value *VClos::add(const Value *rhs) const {
    throw new ArithUnsupportedError();
}

ELambda *VClos::getLambda() const {
    return lambda;
}

Env VClos::getEnv() const {
    return env;
}
