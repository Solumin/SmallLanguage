#include <string>

#include "small_visitor.hpp"
#include "small_expr.hpp"
#include "small_stmt.hpp"

#include "small_values.hpp"
#include "small_env.hpp"

class EvalVisitor : public Visitor {
    Env eval_env;
    Value *tmp_value;
    Value *final_value;

    public:
        EvalVisitor() {};
        // Visitor functions for Expressions
        void visit(EId *e) {
            tmp_value = eval_env.at(e->getID());
        }

        void visit(EInt *e) {
            tmp_value = new VInt(e->getValue());
        }

        void visit(EFloat *e) {
            tmp_value = new VFloat(e->getValue());
        }

        void visit(EBool *e) {
            tmp_value = new VBool(e->getValue());
        }

        void visit(EChar *e) {
            tmp_value = new VChar(e->getValue());
        }

        void visit(EString *e) {
            tmp_value = new VString(e->getValue());
        }

        void visit(EList *e) {
            std::vector<Value*> vlist;
            for (Expr *ex : e->getValue()) {
                ex->accept(*this);
                vlist.push_back(tmp_value);
            }
            tmp_value = new VList(vlist);
        }

        void visit(ETuple *e) {
            std::vector<Value*> vlist;
            for (Expr *ex : e->getValue()) {
                ex->accept(*this);
                vlist.push_back(tmp_value);
            }
            tmp_value = new VList(vlist);
        }

        void visit(EOp2 *e) {

        }

        void visit(EOp1 *e) {

        }

        void visit(ELambda *e) {
            tmp_value = new VClos(e, eval_env);
        }

        // TODO: Clean up env after the function call!!
        void visit(EApp *e) {
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

        void visit(EIf *e) {
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
        void visit(Seq *s) {
            s->getFirst()->accept(*this);
            s->getSecond()->accept(*this);
            final_value = tmp_value;
        }

        void visit(Assign *s) {
            s->getRHS()->accept(*this);
            if (eval_env.count(s->getID()) > 0) {
                throw ("Variable `" + s->getID() + "' already exists");
            } else {
                eval_env.insert({s->getID(), tmp_value});
            }
            final_value = tmp_value;
        }

        void visit(Return *s) {
            s->getRHS()->accept(*this);
            final_value = tmp_value;
        }
};
