#ifndef SMALL_OPS_HPP
#define SMALL_OPS_HPP

// TODO: Simplify this:
// - Doesn't involve cast to access Op name
// - Ensures Op2 and Op2Strings are always in sync (macro?)
// - Same with Op1
enum class Op2 {
    // Arithmetic
    Add
    ,Sub
    ,Mul
    ,Div
    ,Mod

    // Boolean
    ,LAnd
    ,LOr

    // Comparative
    ,Lt
    ,Lte
    ,Gt
    ,Gte
    ,Eq
};

static const std::string Op2Strings[] = {
    "+"
    ,"-"
    ,"*"
    ,"/"
    ,"%"
    ,"&&"
    ,"||"
    ,"<"
    ,"<="
    ,">"
    ,">="
    ,"=="
};

enum class Op1 {
    // Arithmetic
    Neg

    // Boolean
    ,LNot
};

static const std::string Op1Strings[] = {
    "-"
    ,"!"
};

#endif
