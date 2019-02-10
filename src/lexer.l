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

"var" { return KWVAR; }
"while" { return KWWHILE; }
"function" { return KWFUNCTION; }
"null" { return KWNULL; }
"return" { return KWRETURN; }

"." { return DOT; }
".." { return DOTDOT; }
"=" { return ASSIGN; }
";" { return TERM; }
"(" { return LPAREN; }
")" { return RPAREN; }
"{" { return LBRACE; }
"}" { return RBRACE; }
"[" { return LBRACK; }
"]" { return RBRACK; }
":" { return COLON; }
"," { return COMMA; }

"+" { return OPPLUS; }
"-" { return OPMINUS; }
"/" { return OPDIV; }
"*" { return OPMULT; }

"==" { return OPEQUAL; }
"!=" { return OPNOTEQUAL; }
"<=" { return OPLSEQUAL; }
">=" { return OPGREQUAL; }
"<" { return OPLESS; }
">" { return OPGREATER; }
"!" { return OPNOT; }

[0-9]+ {
  return INT;
}

[0-9]+\.[0-9]+ {
  return FLOAT;
}

[a-zA-Z_][a-zA-Z_0-9]* {
  
  yylval->ident = new std::string(yytext);
  return IDENT;
}

\"(\\.|[^\\"\n])*\"	{
  yylval->string = new std::string(yytext);
  return STRING;
}

\'(\\.|[^\\'\n])*\'	{
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

