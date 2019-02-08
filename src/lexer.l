%{
#include <string>
#include "felparser.h"
#include "parser.hh"
#include <iostream>

%}

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

[a-zA-Z][a-zA-Z0-9]* {
  
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
  std::cerr << "file" << ":" << yylineno << ":" << yycolumn << ": Invalid character : " << yytext << "\n";
}
%%

