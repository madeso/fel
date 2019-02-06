%{
#include <string>
#include "parser.hh"
extern int yylex();
%}

%option noyywrap never-interactive nounistd

%%

[a-zA-Z][a-zA-Z0-9]* {
  yylval.ident = new std::string(yytext);
  return IDENT;
}

\n {
}

[ \r\t\v\f] {
  continue;
}

. {
}
%%
