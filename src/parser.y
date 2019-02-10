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
  KWRETURN "return"
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
  | func_statement
  | body_statement
  | return_statement
  | error TERM
;

return_statement
  : KWRETURN value TERM
;

body_statement
  : KWWHILE body
;

func_statement
  : KWFUNCTION IDENT func_decl_arguments body

body
  : LBRACE statementlist RBRACE
;

function_call
  : value LPAREN func_arguments RPAREN
;

call_statement
  : function_call TERM
;

declare_statement
  : KWVAR IDENT ASSIGN value TERM { delete $2; }
  | KWVAR IDENT TERM { delete $2; }
;

assign_statement
  : var ASSIGN value TERM
;

var
  : var_dot
  | var_dot LBRACK STRING RBRACK { delete $3; }
  | var_dot LBRACK INT RBRACK
;

var_dot
  : IDENT { delete $1; }
  | var_dot DOT IDENT { delete $3; }
;

func_arguments
  :
  | func_arguments_p
;

func_arguments_p
  : func_argument_value
  | func_arguments_p COMMA func_argument_value
;

func_argument_value
  : value
  | IDENT ASSIGN value { delete $1; }
;

func_decl_arguments
  : LPAREN func_decl_argument_list RPAREN
;

func_decl_argument_list
  :
  | func_decl_argument_list_p
;

func_decl_argument_list_p
  : func_decl_argument_list_p COMMA func_decl_argument
  | func_decl_argument
;

func_decl_argument
  : IDENT { delete $1; }
  | IDENT ASSIGN value { delete $1; }
;

object_argument_list
  :
  | object_argument_list_p
;

object_argument_list_p
  : object_argument
  | object_argument COMMA object_argument
  | object_argument COMMA DOTDOT IDENT { delete $4; }
;

object_argument
  : IDENT ASSIGN value { delete $1; }
;

array_argument_list
  :
  | array_argument_list_p
;

array_argument_list_p
  : array_argument
  | array_argument COMMA array_argument_list_p
;

array_argument
  : value
  | INT DOTDOT INT
;



value
  : STRING { delete $1; }
  | var
  | function_call
  | INT
  | FLOAT
  | KWNULL
  | LBRACE object_argument_list RBRACE
  | LBRACK array_argument_list RBRACK
  | value OPEQUAL value
  | value OPNOTEQUAL value
  | value OPLSEQUAL value
  | value OPGREQUAL value
  | value OPLESS value
  | value OPGREATER value
  | value OPPLUS value
  | value OPMINUS value
  | value OPDIV value
  | value OPMULT value
  | OPNOT value
  | KWFUNCTION func_decl_arguments body
;


%%
void yyerror(YYLTYPE* loc, yyscan_t scanner, FelState* state, const char* err)
{
    std::cerr << state->file << ":" << loc->first_line << ":" << loc->first_column << ": " << err << "\n";
}
