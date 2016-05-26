#include <cstdio>
#include <string>
#include <cmath>

#include "evaluator/small_eval_visitor.hpp"

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
    } else if (op <= Op2::Gte) { // comparative op
        tmp_value = new VBool(eval_comp(op, lhs, rhs));
    } else if (op == Op2::Eq) { // special case
        tmp_value = new VBool(eval_eq(lhs, rhs));
    } else {
        throw "EOp2: Unknown op";
    }
}

// Integers are automatically converted to floats, never vice versa
Value *EvalVisitor::eval_arith(Op2 op, Value *lhs, Value *rhs) {
    // If we have two ints, proceed with int-int ops (first if block)
    // If at least one of the args is a float (conversion to VInt failed) then
    // make sure we have two floats.
    VInt *li = dynamic_cast<VInt*>(lhs);
    VInt *ri = dynamic_cast<VInt*>(rhs);
    VFloat *lf = dynamic_cast<VFloat*>(lhs);
    VFloat *rf = dynamic_cast<VFloat*>(rhs);
    if (li && !ri) // li needs to be converted to float
        lf = new VFloat(li->getValue());
    if (!li && ri) // ri needs to be converted to float
        rf = new VFloat(ri->getValue());
    // It's possible that li or ri isn't a VInt OR a VFloat. That's handled in
    // the else.

    if (li && ri) { // two integers
        switch (op) {
            case Op2::Add: return new VInt(li->getValue() + ri->getValue());
            case Op2::Sub: return new VInt(li->getValue() - ri->getValue());
            case Op2::Mul: return new VInt(li->getValue() * ri->getValue());
            case Op2::Div: return new VInt(li->getValue() / ri->getValue());
            case Op2::Mod: return new VInt(li->getValue() % ri->getValue());
        }
    } else if (lf && rf) { // two floats
        switch (op) {
            case Op2::Add: return new VFloat(lf->getValue() + rf->getValue());
            case Op2::Sub: return new VFloat(lf->getValue() - rf->getValue());
            case Op2::Mul: return new VFloat(lf->getValue() * rf->getValue());
            case Op2::Div: return new VFloat(lf->getValue() / rf->getValue());
            case Op2::Mod: return new VFloat(fmod(lf->getValue(), rf->getValue()));
        }
    } else {
        throw ("Op2: Expected numeric LHS and RHS for arithmetic op `" + Op2Strings[(int)op] + "'");
    }
}

bool EvalVisitor::eval_bool(Op2 op, Value *lhs, Value *rhs) {
    VBool *l = dynamic_cast<VBool*>(lhs);
    VBool *r = dynamic_cast<VBool*>(rhs);
    if (!l || !r)
        throw ("Op2: Expected boolean LHs and RHS for boolean op `" + Op2Strings[(int)op] + "'");
    switch (op) {
        case Op2::LAnd: return l->getValue() && r->getValue();
        case Op2::LOr: return l->getValue() || r->getValue();
    }
}

// Can compare:
// - Numeric types (float, int)
// - Characters
// - Strings
// - Lists: Lexicographic (and type)
// - Tuples: Size (and type)
bool EvalVisitor::eval_comp(Op2 op, Value *lhs, Value *rhs) {
    // Numeric Comparisons
    VInt *li = dynamic_cast<VInt*>(lhs);
    VInt *ri = dynamic_cast<VInt*>(rhs);
    VFloat *lf = dynamic_cast<VFloat*>(lhs);
    VFloat *rf = dynamic_cast<VFloat*>(rhs);
    if (li && !ri) // li needs to be converted to float
        lf = new VFloat(li->getValue());
    if (!li && ri) // ri needs to be converted to float
        rf = new VFloat(ri->getValue());
    if (li && ri) {
        switch (op) {
            case Op2::Lt:  return li->getValue() < ri->getValue();
            case Op2::Lte: return li->getValue() <= ri->getValue();
            case Op2::Gt:  return li->getValue() > ri->getValue();
            case Op2::Gte: return li->getValue() >= ri->getValue();
        }
    }
    if (lf && rf) {
        switch (op) {
            case Op2::Lt:  return lf->getValue() < rf->getValue();
            case Op2::Lte: return lf->getValue() <= rf->getValue();
            case Op2::Gt:  return lf->getValue() > rf->getValue();
            case Op2::Gte: return lf->getValue() >= rf->getValue();
        }
    }

    // Character comparison
    VChar *lc = dynamic_cast<VChar*>(lhs);
    VChar *rc = dynamic_cast<VChar*>(rhs);
    if (lc && rc) {
        switch (op) {
            case Op2::Lt:  return lc->getValue() < rc->getValue();
            case Op2::Lte: return lc->getValue() <= rc->getValue();
            case Op2::Gt:  return lc->getValue() > rc->getValue();
            case Op2::Gte: return lc->getValue() >= rc->getValue();
        }
    }

    // String comparison
    VString *lstr = dynamic_cast<VString*>(lhs);
    VString *rstr = dynamic_cast<VString*>(rhs);
    if (lstr && rstr) {
        int cmp = lstr->getValue().compare(rstr->getValue());
        if (cmp < 0 && op == Op2::Lt)
            return true;
        if (cmp <= 0 && op == Op2::Lte)
            return true;
        if (cmp > 0 && op == Op2::Gt)
            return true;
        if (cmp >= 0 && op == Op2::Gte)
            return true;
        else
            return false;
    }

    // TODO: Make list and tuple comparison type aware
    // List comparison
    VList *lls = dynamic_cast<VList*>(lhs);
    VList *rls = dynamic_cast<VList*>(rhs);
    if (lls && rls) {
        // Is either list empty?
        // Two empty lists are ==
        if (lls->getValue().size() == 0 && rls->getValue().size() == 0) {
            return (op == Op2::Lte || op == Op2::Gte);
        }
        // Empty list is always < and <= another list
        if (lls->getValue().size() == 0) {
            return (op == Op2::Lt || op == Op2::Lte);
        }
        // Any list is always > and >= the empty list
        if (rls->getValue().size() == 0) {
            return (op == Op2::Gt || op == Op2::Gte);
        }

        std::vector<Value*>::iterator lit = lls->getValue().begin();
        std::vector<Value*>::iterator rit = rls->getValue().begin();

        for (; lit != lls->getValue().end() && rit != rls->getValue().end();
                lit++, rit++) {
            if (*lit == NULL) {
                printf("LIT* NULL");
                throw "";
            }
            if (*rit == NULL) {
                printf("RIT* NULL");
                throw "";
            }
            if (!eval_eq(*lit, *rit)) {
                return eval_comp(op, *lit, *rit);
            }
        }
        // If we've reached this point, the two lists are equal.
        return (op == Op2::Lte || op == Op2::Gte);
    }

    // Tuple comparison
    VTuple *ltup = dynamic_cast<VTuple*>(lhs);
    VTuple *rtup = dynamic_cast<VTuple*>(rhs);
    if (ltup && rtup) {
        if (ltup->getSize() != rtup->getSize())
            throw "Op2: Cannot compare different-size tuples";
        std::vector<Value*>::iterator lit = ltup->getValue().begin();
        std::vector<Value*>::iterator rit = rtup->getValue().begin();

        for (; lit != ltup->getValue().end() && rit != rtup->getValue().end();
               lit++, rit++) {
            if (!eval_eq(*lit, *rit)) {
                return eval_comp(op, *lit, *rit);
            }
        }
        // If we've reached this point, the two tuples are equal.
        return (op == Op2::Lte || op == Op2::Gte);
    }

    throw "Could not eval";
}

bool EvalVisitor::eval_eq(Value *lhs, Value *rhs) {
    // Numeric Comparisons
    VInt *li = dynamic_cast<VInt*>(lhs);
    VInt *ri = dynamic_cast<VInt*>(rhs);
    VFloat *lf = dynamic_cast<VFloat*>(lhs);
    VFloat *rf = dynamic_cast<VFloat*>(rhs);
    if (li && !ri) // li needs to be converted to float
        lf = new VFloat(li->getValue());
    if (!li && ri) // ri needs to be converted to float
        rf = new VFloat(ri->getValue());
    if (li && ri) {
        return li->getValue() == ri->getValue();
    }

    if (lf && rf) {
        return lf->getValue() == rf->getValue();
    }

    // Character comparison
    VChar *lc = dynamic_cast<VChar*>(lhs);
    VChar *rc = dynamic_cast<VChar*>(rhs);
    if (lc && rc) {
        return lc->getValue() == rc->getValue();
    }

    // String comparison
    VString *lstr = dynamic_cast<VString*>(lhs);
    VString *rstr = dynamic_cast<VString*>(rhs);
    if (lstr && rstr) {
        return lstr->getValue().compare(rstr->getValue()) == 0;
    }

    // TODO: Make list and tuple comparison type aware
    // List comparison
    VList *lls = dynamic_cast<VList*>(lhs);
    VList *rls = dynamic_cast<VList*>(rhs);
    if (lls && rls) {
        if (lls->getValue().size() != rls->getValue().size())
            return false;
        std::vector<Value*>::iterator lit;
        std::vector<Value*>::iterator rit;

        for (lit = lls->getValue().begin(), rit = rls->getValue().begin();
                lit != lls->getValue().end() && rit != rls->getValue().end();
                lit++, rit++) {
            if (!eval_eq(*lit, *rit)) {
                return false;
            }
        }
        return true;
    }

    // Tuple comparison
    VTuple *ltup = dynamic_cast<VTuple*>(lhs);
    VTuple *rtup = dynamic_cast<VTuple*>(rhs);
    if (ltup && rtup) {
        if (ltup->getSize() != rtup->getSize())
            throw "Op2: Cannot compare different-size tuples";
        std::vector<Value*>::iterator lit;
        std::vector<Value*>::iterator rit;

        for (lit = ltup->getValue().begin(), rit = rtup->getValue().begin();
                lit != ltup->getValue().end() && rit != rtup->getValue().end();
                lit++, rit++) {
            if (!eval_eq(*lit, *rit)) {
                return false;
            }
        }
        return true;
    }
}

void EvalVisitor::visit(EOp1 *e) {

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
