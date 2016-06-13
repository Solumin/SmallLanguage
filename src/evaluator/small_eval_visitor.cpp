#include <cstdio>
#include <string>
#include <cmath>

#include "evaluator/small_eval_visitor.hpp"
#include "evaluator/small_eval_errors.hpp"

// Visitor functions for Expressions
void EvalVisitor::visit(EId *e) {
    tmp_value = eval_env.at(e->getID());
}

void EvalVisitor::visit(EInt *e) {
    tmp_value = new VInt(e->getValue());
}

void EvalVisitor::visit(EFloat *e) {
    tmp_value = new VFloat(e->getValue());
}

void EvalVisitor::visit(EBool *e) {
    tmp_value = new VBool(e->getValue());
}

void EvalVisitor::visit(EChar *e) {
    tmp_value = new VChar(e->getValue());
}

void EvalVisitor::visit(EString *e) {
    tmp_value = new VString(e->getValue());
}

void EvalVisitor::visit(EList *e) {
    std::vector<Value*> vlist;
    for (Expr *ex : e->getValue()) {
        ex->accept(*this);
        vlist.push_back(tmp_value);
    }
    tmp_value = new VList(vlist);
}

void EvalVisitor::visit(ETuple *e) {
    std::vector<Value*> vlist;
    for (Expr *ex : e->getValue()) {
        ex->accept(*this);
        vlist.push_back(tmp_value);
    }
    tmp_value = new VList(vlist);
}

void EvalVisitor::visit(EOp2 *e) {
    // Get op, left hand side and right hand side
    Op2 op = e->getOp();
    e->getLeft()->accept(*this);
    Value *lhs = tmp_value;
    e->getRight()->accept(*this);
    Value *rhs = tmp_value;

    if (op <= Op2::Mod) { // arithmetic op
        tmp_value = eval_arith(op, lhs, rhs);
    } else if (op <= Op2::LOr) { // boolean op
        tmp_value = new VBool(eval_bool(op, lhs, rhs));
    } else if (op <= Op2::Eq) { // comparative op
        tmp_value = new VBool(eval_comp(op, lhs, rhs));
    } else {
        throw new EvalUnknownOp();
    }
}

// Integers are automatically promoted to floats
Value *EvalVisitor::eval_arith(Op2 op, Value *lhs, Value *rhs) {
    switch (op) {
        case Op2::Add: return lhs->add(rhs);
        case Op2::Sub: return lhs->sub(rhs);
        case Op2::Mul: return lhs->mul(rhs);
        case Op2::Div: return lhs->div(rhs);
        case Op2::Mod: return lhs->mod(rhs);
    }
}

bool EvalVisitor::eval_bool(Op2 op, Value *lhs, Value *rhs) {
    VBool *l = dynamic_cast<VBool*>(lhs);
    VBool *r = dynamic_cast<VBool*>(rhs);
    if (!l || !r)
        throw ("Op2: Expected boolean LHS and RHS for boolean op `" + Op2Strings[(int)op] + "'");
    switch (op) {
        case Op2::LAnd: return l->getValue() && r->getValue();
        case Op2::LOr: return l->getValue() || r->getValue();
    }
}

bool EvalVisitor::eval_comp(Op2 op, Value *lhs, Value *rhs) {
    switch (op) {
        case Op2::Lt:  return lhs->lt(rhs);
        case Op2::Lte: return lhs->lte(rhs);
        case Op2::Gt:  return lhs->gt(rhs);
        case Op2::Gte: return lhs->gte(rhs);
        case Op2::Eq:  return lhs->eq(rhs);
    }
}

void EvalVisitor::visit(EOp1 *e) {
    e->getExpr()->accept(*this);
    Value *v = tmp_value;
    switch(e->getOp()) {
        case Op1::Neg: tmp_value = eval_op1_neg(v); break;
        case Op1::LNot: tmp_value = eval_op1_not(v); break;
        default: throw "Unknown unary op";
    }
}

// Can negate ints and floats
Value *EvalVisitor::eval_op1_neg(Value *v) {
    VInt *vi = dynamic_cast<VInt*>(v);
    if (vi)
        return new VInt(-(vi->getValue()));

    VFloat *vf = dynamic_cast<VFloat*>(v);
    if (vf)
        return new VFloat(-(vf->getValue()));

    throw "Expected an Int or Float";
}

// Van ONLY negate Bool
Value *EvalVisitor::eval_op1_not(Value *v) {
    VBool *vb = dynamic_cast<VBool*>(v);
    if (vb)
        return new VBool(!vb->getValue());

    throw "Expected a Bool";
}

void EvalVisitor::visit(ELambda *e) {
    tmp_value = new VClos(e, eval_env);
}

// TODO: Clean up env after the function call!!
void EvalVisitor::visit(EApp *e) {
    // Get the evaluated lambda/closure:
    e->getFunc()->accept(*this);
    VClos *clos = dynamic_cast<VClos*>(tmp_value);
    if (clos == NULL)
        throw "App: LHS did not eval to function";

    ELambda *lambda = clos->getLambda();
    std::vector<std::string> params = lambda->getParams();
    std::vector<Expr*> args = e->getArgs();

    // Add te param => arg mapping to the env
    if (params.size() != args.size())
        throw "App: params and args length mismatch";

    std::vector<std::string>::iterator params_iter = params.begin();
    std::vector<Expr*>::iterator args_iter = args.begin();
    Env env_copy = eval_env;
    while (args_iter != args.end() && params_iter != params.end()) {
        (*args_iter)->accept(*this);
        env_copy.insert({*params_iter, tmp_value});
        args_iter++;
        params_iter++;
    }

    lambda->getBody()->accept(*this);
    // tmp_value is set to the return value, nothing more to do here.
}

void EvalVisitor::visit(EIf *e) {
    e->getCond()->accept(*this);
    VBool *cond = dynamic_cast<VBool*>(tmp_value);
    if (cond == NULL)
        throw ("This language is NOT \"truthy\", and If-cond did not evaluate to bool: " + e->getCond()->toString());

    if (cond->getValue())
        e->getTrueBody()->accept(*this);
    else
        e->getFalseBody()->accept(*this);
    // tmp_value is set to the return value, nothing more to do here.
}

// Visitor functions for Statements
void EvalVisitor::visit(Seq *s) {
    s->getFirst()->accept(*this);
    s->getSecond()->accept(*this);
    final_value = tmp_value;
}

void EvalVisitor::visit(Assign *s) {
    s->getRHS()->accept(*this);
    if (eval_env.count(s->getID()) > 0) {
        throw ("Variable `" + s->getID() + "' already exists");
    } else {
        eval_env.insert({s->getID(), tmp_value});
    }
    final_value = tmp_value;
}

void EvalVisitor::visit(Return *s) {
    s->getRHS()->accept(*this);
    final_value = tmp_value;
}

// Functions for handling evaluator
Value *EvalVisitor::getResult() {
    return final_value;
}

void EvalVisitor::reset() {
    eval_env.clear();
    final_value = NULL;
    tmp_value = NULL;
}
