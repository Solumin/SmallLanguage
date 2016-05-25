#ifndef SMALL_VISITOR_HPP
#define SMALL_VISITOR_HPP

#include "ast/small_expr.hpp"
#include "ast/small_stmt.hpp"

class Visitor {
    public:
        // Visitor functions for Expressions
        virtual void visit(EId *e) = 0;
        virtual void visit(EInt *e) = 0;
        virtual void visit(EFloat *e) = 0;
        virtual void visit(EBool *e) = 0;
        virtual void visit(EChar *e) = 0;
        virtual void visit(EString *e) = 0;
        virtual void visit(EList *e) = 0;
        virtual void visit(ETuple *e) = 0;
        virtual void visit(EOp2 *e) = 0;
        virtual void visit(EOp1 *e) = 0;
        virtual void visit(ELambda *e) = 0;
        virtual void visit(EApp *e) = 0;
        virtual void visit(EIf *e) = 0;
        // Visitor functions for Statements
        virtual void visit(Seq *s) = 0;
        virtual void visit(Assign *s) = 0;
        virtual void visit(Return *s) = 0;
};
#endif
