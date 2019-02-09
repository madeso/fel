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
%token INT
%token FLOAT
%token KWVAR "var"
  KWWHILE "while"
  KWNULL "null"
  KWFUNCTION "function"
  DOT "."
  DOTDOT ".."
  ASSIGN "="
  TERM ";"
  LPAREN "("
  RPAREN ")"
  LBRACE "{"
  RBRACE "}"
  LBRACK "["
  RBRACK "]"
  COLON ":"
  COMMA ","
  OPPLUS "+"
  OPMINUS "-"
  OPDIV "/"
  OPMULT "*"
  OPEQUAL "=="
  OPNOTEQUAL "!="
  OPLSEQUAL "<="
  OPGREQUAL ">="
  OPLESS "<"
  OPGREATER ">"
  OPNOT "!"


%start statementlist

%%

statementlist
  : statement statementlist
  | 
;

statement
  : call_statement
  | declare_statement
  | assign_statement
  | function_statement
  | body_statement
  | error TERM
;

body_statement
  : KWWHILE body
;

function_statement
  : KWFUNCTION IDENT function_declared_arguments body

body
  : LBRACE statementlist RBRACE
;

call_statement
  : IDENT LPAREN function_arguments RPAREN TERM {delete $1; }
;

declare_statement
  : KWVAR IDENT ASSIGN value TERM { delete $2; }
  | KWVAR IDENT TERM { delete $2; }
;

assign_statement
  : IDENT ASSIGN value TERM { delete $1; }
;

function_arguments
  : value function_arguments
  |
;

function_declared_arguments
  : LPAREN RPAREN
;

value
  : STRING { delete $1; }
  | IDENT { delete $1; }
  | call_statement
  | INT
  | FLOAT
  | KWNULL
  | value OPEQUAL value
  | KWFUNCTION function_declared_arguments body
;


%%
void yyerror(YYLTYPE* loc, yyscan_t scanner, FelState* state, const char* err)
{
    std::cerr << state->file << ":" << loc->first_line << ":" << loc->first_column << ": " << err << "\n";
}
