%{
#include <iostream>
#include <string>

#include "lexer.hh"

void yyerror(yyscan_t scanner, const char* err);
%}

%code requires {
  typedef void* yyscan_t;
}

%lex-param   { yyscan_t scanner }
%parse-param { yyscan_t scanner }

%union {
  std::string* ident;
  int ival;
}

%token <ident> IDENT 
%start input

%%

input
  : IDENT input
  { std::cout << "ident: " << *$1 << "\n"; delete $1; }
  | IDENT
  { std::cout << "ident: " << *$1 << "\n"; delete $1; }
;

%%
void yyerror(yyscan_t scanner, const char* err)
{
   std::cerr << "Error: " << err << "\n";
}
