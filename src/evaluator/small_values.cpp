#include <cmath>

#include "evaluator/small_values.hpp"
#include "evaluator/small_eval_errors.hpp"

// Helpful macros
#define ARITH_OP_UNSUPPORTED(type, op)          \
    Value *type::op(const Value *rhs) const {   \
        throw new ArithUnsupportedError();      \
    }

#define ARITH_UNSUPPORTED(type) \
    ARITH_OP_UNSUPPORTED(type, add) \
    ARITH_OP_UNSUPPORTED(type, sub) \
    ARITH_OP_UNSUPPORTED(type, mul) \
    ARITH_OP_UNSUPPORTED(type, div) \
    ARITH_OP_UNSUPPORTED(type, mod)

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

Value *VInt::arith_op(Op2 op, const Value *rhs) const {
    const VInt *ri = dynamic_cast<const VInt*>(rhs);
    if (ri) {
        switch (op) {
            case Op2::Add: return new VInt(getValue() + ri->getValue());
            case Op2::Sub: return new VInt(getValue() - ri->getValue());
            case Op2::Mul: return new VInt(getValue() * ri->getValue());
            case Op2::Div: return new VInt(getValue() / ri->getValue());
            case Op2::Mod: return new VInt(getValue() % ri->getValue());
        }
    }

    const VFloat *rf = dynamic_cast<const VFloat*>(rhs);
    if (rf) {
        VFloat lf = VFloat(getValue());
        switch (op) {
            case Op2::Add: return new VFloat(lf.getValue() + rf->getValue());
            case Op2::Sub: return new VFloat(lf.getValue() - rf->getValue());
            case Op2::Mul: return new VFloat(lf.getValue() * rf->getValue());
            case Op2::Div: return new VFloat(lf.getValue() / rf->getValue());
            case Op2::Mod: return new VFloat(fmod(lf.getValue(), rf->getValue()));
        }
    }

    throw new ArithArgMismatch();
}

Value *VInt::add(const Value *rhs) const {
    return arith_op(Op2::Add, rhs);
}

Value *VInt::sub(const Value *rhs) const {
    return arith_op(Op2::Sub, rhs);
}

Value *VInt::mul(const Value *rhs) const {
    return arith_op(Op2::Mul, rhs);
}

Value *VInt::div(const Value *rhs) const {
    return arith_op(Op2::Div, rhs);
}

Value *VInt::mod(const Value *rhs) const {
    return arith_op(Op2::Mod, rhs);
}

bool VInt::lt(const Value *rhs) const {
    const VInt *ri = dynamic_cast<const VInt*>(rhs);
    if (ri)
        return getValue() < ri->getValue();

    const VFloat *rf = dynamic_cast<const VFloat*>(rhs);
    if (rf) {
        VFloat lf = VFloat(getValue());
        return lf.lt(rhs);
    }

    throw new CompArgMismatch();
}

bool VInt::eq(const Value *rhs) const {
    const VInt *ri = dynamic_cast<const VInt*>(rhs);
    if (ri)
        return getValue() == ri->getValue();

    const VFloat *rf = dynamic_cast<const VFloat*>(rhs);
    if (rf) {
        VFloat lf = VFloat(getValue());
        return lf.eq(rhs);
    }

    throw new CompArgMismatch();
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

Value *VFloat::arith_op(Op2 op, const Value *rhs) const {
    const VFloat *rf = dynamic_cast<const VFloat*>(rhs);
    if (rf) {
        switch (op) {
            case Op2::Add: return new VFloat(getValue() + rf->getValue());
            case Op2::Sub: return new VFloat(getValue() - rf->getValue());
            case Op2::Mul: return new VFloat(getValue() * rf->getValue());
            case Op2::Div: return new VFloat(getValue() / rf->getValue());
            case Op2::Mod: return new VFloat(fmod(getValue(), rf->getValue()));
        }
    }

    const VInt *ri = dynamic_cast<const VInt*>(rhs);
    if (ri) {
        VFloat rif = VFloat(ri->getValue());
        switch (op) {
            case Op2::Add: return new VFloat(getValue() + rif.getValue());
            case Op2::Sub: return new VFloat(getValue() - rif.getValue());
            case Op2::Mul: return new VFloat(getValue() * rif.getValue());
            case Op2::Div: return new VFloat(getValue() / rif.getValue());
            case Op2::Mod: return new VFloat(fmod(getValue(), rif.getValue()));
        }
    }

    throw new ArithArgMismatch();
}

Value *VFloat::add(const Value *rhs) const {
    return arith_op(Op2::Add, rhs);
}

Value *VFloat::sub(const Value *rhs) const {
    return arith_op(Op2::Sub, rhs);
}

Value *VFloat::mul(const Value *rhs) const {
    return arith_op(Op2::Mul, rhs);
}

Value *VFloat::div(const Value *rhs) const {
    return arith_op(Op2::Div, rhs);
}

Value *VFloat::mod(const Value *rhs) const {
    return arith_op(Op2::Mod, rhs);
}

bool VFloat::lt(const Value *rhs) const {
    const VFloat *rf = dynamic_cast<const VFloat*>(rhs);
    if (rf)
        return getValue() == rf->getValue();

    const VInt *ri = dynamic_cast<const VInt*>(rhs);
    if (ri) {
        VFloat rif = VFloat(ri->getValue());
        return getValue() < rif.getValue();
    }

    throw new CompArgMismatch();
}

bool VFloat::eq(const Value *rhs) const {
    const VInt *ri = dynamic_cast<const VInt*>(rhs);
    if (ri)
        return getValue() == ri->getValue();

    const VFloat *rf = dynamic_cast<const VFloat*>(rhs);
    if (rf) {
        VFloat rif = VFloat(ri->getValue());
        return getValue() == rif.getValue();
    }

    throw new CompArgMismatch();
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

ARITH_UNSUPPORTED(VBool)

bool VBool::eq(const Value *rhs) const {
    const VBool *rb = dynamic_cast<const VBool*>(rhs);
    if (rb)
        return getValue() == rb->getValue();

    return new ArithArgMismatch();
}

bool VBool::lt(const Value *rhs) const {
    throw new CompUnsupportedError();
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

ARITH_UNSUPPORTED(VChar)

bool VChar::eq(const Value *rhs) const {
    const VChar *rc = dynamic_cast<const VChar*>(rhs);
    if (rc)
        return getValue() == rc->getValue();

    throw new CompArgMismatch();
}

bool VChar::lt(const Value *rhs) const {
    const VChar *rc = dynamic_cast<const VChar*>(rhs);
    if (rc)
        return getValue() < rc->getValue();

    throw new CompArgMismatch();
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

ARITH_UNSUPPORTED(VString)

bool VString::eq(const Value *rhs) const {
    const VString *rstr = dynamic_cast<const VString*>(rhs);
    if (rstr)
        return getValue().compare(rstr->getValue()) < 0;

    throw new CompArgMismatch();
}

bool VString::lt(const Value *rhs) const {
    const VString *rstr = dynamic_cast<const VString*>(rhs);
    if (rstr)
        return getValue().compare(rstr->getValue()) == 0;

    throw new CompArgMismatch();
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

ARITH_UNSUPPORTED(VList)

bool VList::eq(const Value *rhs) const {
    const VList *rls = dynamic_cast<const VList*>(rhs);
    if (!rls)
        throw new CompArgMismatch();

    // Equal lists have the same size
    if (getValue().size() != rls->getValue().size())
        return false;

    // Empty lists are equal
    if (getValue().size() == 0 && rls->getValue().size() == 0)
        return true;

    // Compare by iterating over elements
    std::vector<Value*>::const_iterator lit;
    std::vector<Value*>::const_iterator rit;

    for (lit = getValue().begin(), rit = rls->getValue().begin();
            lit != getValue().end() && rit != rls->getValue().end();
            lit++, rit++) {
        if (!(*lit)->eq(*rit))
            return false;
    }
    return true;
}

// Lists are ordered lexicographically
bool VList::lt(const Value *rhs) const {
    const VList *rls = dynamic_cast<const VList*>(rhs);
    if (!rls)
        throw new CompArgMismatch();

    // An empty list is always < a non-empty list
    if (getValue().size() == 0)
        return rls->getValue().size() > 0;

    // Compare by iterating over elements
    std::vector<Value*>::const_iterator lit;
    std::vector<Value*>::const_iterator rit;

    for (lit = getValue().begin(), rit = rls->getValue().begin();
         lit != getValue().end() && rit != rls->getValue().end();
         lit++, rit++) {
        if (!(*lit)->lt(*rit))
            return false;
    }
    return true;
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

ARITH_UNSUPPORTED(VTuple)

bool VTuple::eq(const Value *rhs) const {
    const VTuple *rts = dynamic_cast<const VTuple*>(rhs);
    if (!rts)
        throw new CompArgMismatch();

    // Cannot compare differently-sized tuples!
    if (getSize() != rts->getSize())
        throw new TupleSizeMismatch();

    // Compare by iterating over elements
    std::vector<Value*>::const_iterator lit;
    std::vector<Value*>::const_iterator rit;

    for (lit = getValue().begin(), rit = rts->getValue().begin();
            lit != getValue().end() && rit != rts->getValue().end();
            lit++, rit++) {
        if (!(*lit)->eq(*rit))
            return false;
    }
    return true;
}

// Lists are ordered lexicographically
bool VTuple::lt(const Value *rhs) const {
    const VTuple *rts = dynamic_cast<const VTuple*>(rhs);
    if (!rts)
        throw new CompArgMismatch();

    // Cannot compare differently-sized tuples!
    if (getSize() != rts->getSize())
        throw new TupleSizeMismatch();

    // Compare by iterating over elements
    std::vector<Value*>::const_iterator lit;
    std::vector<Value*>::const_iterator rit;

    for (lit = getValue().begin(), rit = rts->getValue().begin();
            lit != getValue().end() && rit != rts->getValue().end();
            lit++, rit++) {
        if (!(*lit)->lt(*rit))
            return false;
    }
    return true;
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

ARITH_UNSUPPORTED(VClos)

bool VClos::eq(const Value *rhs) const {
    throw new CompUnsupportedError();
}

bool VClos::lt(const Value *rhs) const {
    throw new CompUnsupportedError();
}

ELambda *VClos::getLambda() const {
    return lambda;
}

Env VClos::getEnv() const {
    return env;
}
