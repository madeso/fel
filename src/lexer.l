%{
#include <string>
#include "felparser.h"
#include "parser.hh"
#include <iostream>

#define YY_USER_ACTION \
  yyextra->first_column = yyextra->last_column; \
  for(char* c=yytext; *c; ++c) { \
    yyextra->last_column = *c=='\n' ? 0 : yyextra->last_column + 1; \
  } \
  yylloc->first_line = yylloc->last_line = yylineno;\
  yylloc->first_column = yyextra->first_column; \
  yylloc->last_column = yyextra->last_column;

%}

%option extra-type="FelState*"
%option yylineno bison-locations
%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

%%

"//"[^\n]* {
  // ignore single line comments
}

"var" { return VAR; }
"while" { return WHILE; }
"function" { return FUNCTION; }
"." { return DOT; }
".." { return DOTDOT; }
"=" { return ASSIGN; }
"==" { return EQUAL; }
";" { return TERM; }
"(" { return LPAREN; }
")" { return RPAREN; }

[a-zA-Z_][a-zA-Z_0-9]* {
  
  yylval->ident = new std::string(yytext);
  return IDENT;
}

\"(\\.|[^\\"\n])*\"	{
  
  yylval->string = new std::string(yytext);
  return STRING;
}


\n {
}

[ \r\t\v\f] {
}

. {
  std::cerr << yyextra->file << ":" << yylineno << ":" << yyextra->first_column << ": Invalid character: " << yytext << "\n";
}
%%

