# A Small Language

Small (`.smol`) is a functional language with a minimal syntax. The main purpose
of this project is work with language implementation tools that I've never used
before, namely Bison and Flex, and also to work with C/C++.

My eventual goal is to compile Small into LLVM IR. My previous language
implementations (namely ScriptNScribe) are all implemented in Haskell with the
`parsec` library for parsing.

Current status: Lexing and parsing, building a basic AST. Working on
implementing a basic evaluator.
