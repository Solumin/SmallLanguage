#include <string>
#include <sstream>
#include <vector>

#include "small_expr.hpp"
#include "small_values.hpp"
#include "small_env.hpp"

EId::EId (std::string name) {
    id = name;
}

EId::EId (const char* name) {
    id = std::string(name);
}

EId::EId (const EId &other) {
    id = other.id;
}

EId::~EId() {}

Expr *EId::clone() {
    return new EId(*this);
}

std::string EId::toString() {
    return id;
}

Value *EId::evaluate(Env env) {
    return env.at(id);
}


EInt::EInt (int v) {
    value = v;
}

EInt::EInt (const EInt &other) {
    value = other.value;
}

EInt::~EInt() {}

Expr *EInt::clone() {
    return new EInt(*this);
}

std::string EInt::toString() {
    return std::to_string(value);
}

Value *EInt::evaluate(Env env) {
    return new VInt(value);
}


EFloat::EFloat (float v) {
    value = v;
}

EFloat::EFloat (const EFloat &other) {
    value = other.value;
}

EFloat::~EFloat() {}

Expr *EFloat::clone() {
    return new EFloat(*this);
}

std::string EFloat::toString() {
    return std::to_string(value);
}

Value *EFloat::evaluate(Env env) {
    return new VFloat(value);
}


EBool::EBool (bool b) {
    value = b;
}

EBool::EBool (const EBool &other) {
    value = other.value;
}

EBool::~EBool() {}

Expr *EBool::clone() {
    return new EBool(*this);
}

std::string EBool::toString() {
    return value ? "true" : "false";
}

Value *EBool::evaluate(Env env) {
    return new VBool(value);
}

EChar::EChar (char c) {
    value = c;
}

EChar::EChar (const EChar &other) {
    value = other.value;
}

EChar::~EChar() {}

Expr *EChar::clone() {
    return new EChar(*this);
}

// TODO: Print 'c', '\xNN', '\'' depending!
std::string EChar::toString() {
    return std::string(1, value);
}

Value *EChar::evaluate(Env env) {
    return new VChar(value);
}


EString::EString (std::string v) {
    value = v;
}

EString::EString (const char* str) {
    value = std::string(str);
}

EString::EString (const EString &other) {
    value = other.value;
}

EString::~EString() {}

Expr *EString::clone() {
    return new EString(*this);
}

std::string EString::toString() {
    return value;
}

Value *EString::evaluate(Env env) {
    return new VString(value);
}


EList::EList () {}

EList::EList (std::vector<Expr*> l) {
    value = std::vector<Expr*>(l);
}

EList::EList (Expr *e) {
    value.push_back(e);
}

EList::EList (const EList &other) {
    value = std::vector<Expr*>(other.value);
}

EList::~EList() {
    delete &value;
}

Expr *EList::clone() {
    return new EList(*this);
}

std::string EList::toString() {
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

Value *EList::evaluate(Env env) {
    std::vector<Value*> vlist;
    for (std::vector<Expr*>::iterator it = value.begin(); it != value.end(); ++it) {
        vlist.push_back((*it)->evaluate(env));
    }
    return new VList(vlist);
}


ETuple::ETuple() { size = 0; }

ETuple::ETuple (std::vector<Expr*> l) {
    value = std::vector<Expr*>(l);
    size = l.size();
}

ETuple::ETuple (const ETuple &other) {
    value = other.value;
    size = other.size;
}

ETuple::~ETuple() {
    delete &value;
}

Expr *ETuple::clone() {
    return new ETuple(*this);
}

std::string ETuple::toString() {
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

Value *ETuple::evaluate(Env env) {
    std::vector<Value*> vlist;
    for (std::vector<Expr*>::iterator it = value.begin(); it != value.end(); ++it) {
        vlist.push_back((*it)->evaluate(env));
    }
    return new VList(vlist);
}


EOp2::EOp2 (Op2 o, Expr *l, Expr *r) {
    op = o;
    left = l->clone();
    right = r->clone();
}

EOp2::EOp2 (const EOp2 &other) {
    op = other.op;
    left = other.left->clone();
    right = other.right->clone();
}

EOp2::~EOp2() {
    delete left;
    delete right;
}

Expr *EOp2::clone() {
    return new EOp2(*this);
}

std::string EOp2::toString() {
    return left->toString() + Op2Strings[(int)op] + right->toString();
}


EOp1::EOp1 (Op1 o, Expr *x) {
    op = o;
    e = x->clone();
}

EOp1::EOp1 (const EOp1 &other) {
    op = other.op;
    e = other.e->clone();
}

EOp1::~EOp1() {
    delete e;
}

Expr *EOp1::clone() {
    return new EOp1(*this);
}

std::string EOp1::toString() {
    return Op1Strings[(int)op] + e->toString();
}

Value *EOp1::evaluate(Env env) {

}


ELambda::ELambda (std::vector<char*> ids, Statement *b) {
    for (std::vector<char*>::iterator it = ids.begin(); it != ids.end(); ++it) {
        params.push_back(std::string(*it));
    }
    body = b->clone();
}

ELambda::ELambda (const ELambda &other) {
    params = std::vector<std::string>(other.params);
    body = other.body->clone();
}

ELambda::~ELambda() {
    delete &params;
    delete body;
}

Expr *ELambda::clone() {
    return new ELambda(*this);
}

std::string ELambda::toString() {
    std::stringstream str;
    str << "(\\ ";
    for (std::vector<std::string>::iterator it = params.begin(); it != params.end(); ++it) {
        str << *it << " ";
    }
    str << "-> " << body->toString() << ")";
    return str.str();
}

Value *ELambda::evaluate(Env env) {
    return new VClos(*this, env);
}


EApp::EApp (Expr *f, std::vector<Expr*> as) {
    func = f->clone();
    args = std::vector<Expr*>(as);
}

EApp::EApp (const EApp &other) {
    func = other.func->clone();
    args = std::vector<Expr*>(other.args);
}

EApp::~EApp() {
    delete func;
    delete &args;
}

Expr *EApp::clone() {
    return new EApp(*this);
}

std::string EApp::toString() {
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

Value *EApp::evaluate(Env env) {
    // Get the evaluated lambda
    VClos *clos = dynamic_cast<VClos*>(func->evaluate(env));

    if (clos == NULL)
        throw "App: LHS did not eval to function";

    std::vector<std::string> params = clos->getLambda()->getParams();

    // Add the param => arg mapping to the env
    if (params.length != args.length)
        throw "App: params and args length mismatch";

    std::vector<std::string>::iterator params_iter = params.begin();
    std::vector<Expr*>::iterator args_iter = args.begin();

    Env env_copy = env;

    while(args_iter != args.end() && params_iter != params.end()) {
        env_copy.insert({*paramsiter, (*args_iter)->evaluate(env)});
        ++args_iter;
        ++params_iter;
    }

    Env res_env = body->evaluate(env_copy);

    if (res_env.count("return") == 0)
        throw "App: Function had no return statement";

    return res_env["return"];
}

EIf::EIf (Expr *c, Expr *t, Expr *f) {
    cond = c->clone();
    true_body = t->clone();
    false_body = f->clone();
}

EIf::EIf (const EIf &other) {
    cond = other.cond->clone();
    true_body = other.true_body->clone();
    false_body = other.false_body->clone();
}

EIf::~EIf() {
    delete cond;
    delete true_body;
    delete false_body;
}

Expr *EIf::clone() {
    return new EIf(*this);
}

std::string EIf::toString() {
    return "if " + cond->toString() +
        " then " + true_body->toString() +
        " else " + false_body->toString();
}

Value *EIf::evaluate(Env env) {
    VBool *c = dynamic_cast<VBool*>(cond->evaluate(env));

    if (c == NULL)
        throw ("This language is NOT \"truthy\", and If-cond did not evaluate to bool: " + cond->toString());

    if (c->getValue())
        return true_body->evaluate(env);
    else
        return false_body->evaluate(env);
}
