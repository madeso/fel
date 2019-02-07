%{
#include <iostream>
#include <string>

#include "lexer.hh"

void yyerror(yyscan_t scanner, const char* err);
%}

%code requires {
  typedef void* yyscan_t;
}

%define parse.error verbose

%lex-param   { yyscan_t scanner }
%parse-param { yyscan_t scanner }

%union {
  std::string* string;
  std::string* ident;
  int ival;
}

%token <ident> IDENT 
%token <string> STRING
%token LET "let"
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
void yyerror(yyscan_t scanner, const char* err)
{
   std::cerr << "Error: " << err << "\n";
}
