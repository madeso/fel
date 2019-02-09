%{
#include <iostream>
#include <string>

#include "felparser.h"
#include "parser.hh"
#include "lexer.hh"

void yyerror(YYLTYPE* loc, yyscan_t scanner, FelState* fel, const char* err);
%}

%code requires {
  typedef void* yyscan_t;
}

%define api.pure full
%define parse.error verbose
%locations

%lex-param   { yyscan_t scanner }
%parse-param { yyscan_t scanner }
%parse-param { FelState* state }

%union {
  std::string* string;
  std::string* ident;
  int ival;
}

%token END 0 "end of file"
%token <ident> IDENT 
%printer { frpintf(yyo, "%s", $$->c_str()); } IDENT
%token <string> STRING
%token VAR "var"
  WHILE "while"
  FUNCTION "function"
  DOT "."
  DOTDOT ".."
  ASSIGN "assign ="
  EQUAL "=="
  TERM ";"
  LPAREN "("
  RPAREN ")"

%start input

%%

input
  : statement input
  | 
;

statement
  : call_statement
  | assign_statement
  | body_statement
;

body_statement
  : WHILE
;

call_statement
  : IDENT LPAREN function_arguments RPAREN TERM
  {std::cout << "Calling function " << *$1 << "\n"; delete $1; }
;

assign_statement
  : IDENT ASSIGN value TERM
  { std::cout << "Assign " << *$1 << "\n"; delete $1; }
;

function_arguments
  : value function_arguments
  |
;

value
  : STRING
  { std::cout << "string value " << *$1 << "\n"; delete $1; }
  | IDENT
  { std::cout << "ident" << *$1 << "\n"; delete $1; }
  | call_statement
  | value EQUAL value
;


%%
void yyerror(YYLTYPE* loc, yyscan_t scanner, FelState* state, const char* err)
{
    std::cerr << state->file << ":" << loc->first_line << ":" << loc->first_column << ": syntax error: " << err << "\n";
}
