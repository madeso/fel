%{
#include <string>
#include "parser.hh"
%}

%option reentrant noyywrap never-interactive nounistd

%%

"//"[^\n]* {
  // ignore single line comments
}

"let" { return LET; }
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
  yylval.ident = new std::string(yytext);
  return IDENT;
}

\"(\\.|[^\\"\n])*\"	{
  yylval.string = new std::string(yytext);
  return STRING;
}


\n {
}

[ \r\t\v\f] {
  continue;
}

. {
}
%%

