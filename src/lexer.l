%{
#include <string>
#include "felparserstate.h"
#include "parser.hh"
#include <iostream>
#include <sstream>

#define YY_USER_ACTION \
  yyextra->first_column = yyextra->last_column; \
  for(char* c=yytext; *c; ++c) { \
    yyextra->last_column = *c=='\n' ? 0 : yyextra->last_column + 1; \
  } \
  yylloc->first_line = yylloc->last_line = yylineno;\
  yylloc->first_column = yyextra->first_column; \
  yylloc->last_column = yyextra->last_column;

%}

%option extra-type="FelParserState*"
%option yylineno bison-locations
%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

%x str_state
%x chr_state
%%

"//"[^\n]* {
  // ignore single line comments
}

"var" { return KWVAR; }
"if" { return KWIF; }
"in" { return KWIN; }
"break" { return KWBREAK; }
"else" { return KWELSE; }
"scope" { return KWSCOPE; }
"while" { return KWWHILE; }
"for" { return KWFOR; }
"loop" { return KWLOOP; }
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

\"      { yyextra->str.resize(0); BEGIN(str_state); }
'       { yyextra->str.resize(0); BEGIN(chr_state); }

<chr_state>'        {
        BEGIN(INITIAL);
        yyextra->str.push_back(0);
        yylval->string = new std::string(&yyextra->str[0]);
        return STRING;
        }
<str_state>\"        {
        BEGIN(INITIAL);
        yyextra->str.push_back(0);
        yylval->string = new std::string(&yyextra->str[0]);
        return STRING;
        }

<str_state,chr_state>\n        {
        yyextra->AddLog(yyextra->file, yylineno, yyextra->first_column, "Newline in progress inside string");
        }
<str_state,chr_state>\\[0-7]{1,3} {
        int result;
        (void) sscanf( yytext + 1, "%o", &result );
        if ( result > 0xff ) {
          yyextra->AddLog(yyextra->file, yylineno, yyextra->first_column, "Invalid octal escape sequence");
        }
        yyextra->str.push_back(result);
        }
<str_state,chr_state>\\[0-9]+ {
          yyextra->AddLog(yyextra->file, yylineno, yyextra->first_column, "Bad escape sequence");
        }
<str_state,chr_state>\\n  yyextra->str.push_back('\n');
<str_state,chr_state>\\t  yyextra->str.push_back('\t');
<str_state,chr_state>\\r  yyextra->str.push_back('\r');
<str_state,chr_state>\\b  yyextra->str.push_back('\b');
<str_state,chr_state>\\f  yyextra->str.push_back('\f');
<str_state,chr_state>\\(.|\n)  yyextra->str.push_back(yytext[1]);

<chr_state>[^\\\n']+        {
        char *yptr = yytext;
        while ( *yptr )
                yyextra->str.push_back(*yptr++);
        }
<str_state>[^\\\n\"]+        {
        char *yptr = yytext;
        while ( *yptr )
                yyextra->str.push_back(*yptr++);
        }

\n {
}

[ \r\t\v\f] {
}

. {
  std::stringstream ss;
  ss << "Invalid character: " << yytext;
  yyextra->AddLog(yyextra->file, yylineno, yyextra->first_column, ss.str());
}
%%

