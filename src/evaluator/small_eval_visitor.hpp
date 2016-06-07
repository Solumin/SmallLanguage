#ifndef SMALL_EVAL_VISITOR_HPP
#define SMALL_EVAL_VISITOR_HPP

#include "visitor/small_visitor.hpp"

#include "ast/small_expr.hpp"
#include "ast/small_stmt.hpp"
#include "evaluator/small_values.hpp"
#include "small_env.hpp"

class EvalVisitor : public Visitor {
    Env eval_env;
    Value *tmp_value;
    Value *final_value;

    public:
        EvalVisitor() {};
        // Visitor functions for Expressions
        virtual void visit(EId *e);
        virtual void visit(EInt *e);
        virtual void visit(EFloat *e);
        virtual void visit(EBool *e);
        virtual void visit(EChar *e);
        virtual void visit(EString *e);
        virtual void visit(EList *e);
        virtual void visit(ETuple *e);
        virtual void visit(EOp2 *e);
        virtual void visit(EOp1 *e);
        virtual void visit(ELambda *e);
        virtual void visit(EApp *e);
        virtual void visit(EIf *e);
        // Visitor functions for Statements
        virtual void visit(Seq *s);
        virtual void visit(Assign *s);
        virtual void visit(Return *s);

        // Functions for handling evaluator
        Value *getResult();
        void reset();
    private:
        // Helper functions
        Value *eval_arith(Op2 op, Value *lhs, Value *rhs);
        bool eval_bool(Op2 op, Value *lhs, Value *rhs);
        bool eval_comp(Op2 op, Value *lhs, Value *rhs);
        bool eval_eq(Value *lhs, Value *rhs);
};
#endif
