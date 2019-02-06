%{
#include <iostream>
#include <string>

extern int yylex();
extern int yyparse();
extern FILE* yyin;

void yyerror(const char* err);
%}

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
void yyerror(const char* err)
{
   std::cerr << "Error: " << err << "\n";
}
