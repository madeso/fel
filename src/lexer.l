%{
#include <string>
#include "felparser.h"
#include "parser.hh"
#include <iostream>
#define YY_USER_ACTION \
          yylloc->first_line = yylloc->last_line = yylineno;\
          yylloc->first_column = yylloc->last_column = yycolumn;
%}

%option yylineno bison-locations
%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

%%

"//"[^\n]* {
  // ignore single line comments
std::cout << "single line comment\n";
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
std::cout << "newline\n";
}

[ \r\t\v\f] {
std::cout << "space\n";
}

. {
  std::cerr << "file" << ":" << yylineno << ":" << yycolumn << ": Invalid character : " << yytext << "\n";
}
%%

