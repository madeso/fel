%{
#include <string>
#include "felparser.h"
#include "parser.hh"
#include <iostream>

void count(yyscan_t scanner);
#define COUNT_CHARS count(yyscanner)

%}

%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

%%

"//"[^\n]* {
  // ignore single line comments
  COUNT_CHARS;
}

"let" { COUNT_CHARS; return LET; }
"while" { COUNT_CHARS; return WHILE; }
"function" { COUNT_CHARS; return FUNCTION; }
"." { COUNT_CHARS; return DOT; }
".." { COUNT_CHARS; return DOTDOT; }
"=" { COUNT_CHARS; return ASSIGN; }
"==" { COUNT_CHARS; return EQUAL; }
";" { COUNT_CHARS; return TERM; }
"(" { COUNT_CHARS; return LPAREN; }
")" { COUNT_CHARS; return RPAREN; }

[a-zA-Z][a-zA-Z0-9]* {
  COUNT_CHARS;
  yylval->ident = new std::string(yytext);
  return IDENT;
}

\"(\\.|[^\\"\n])*\"	{
  COUNT_CHARS;
  yylval->string = new std::string(yytext);
  return STRING;
}


\n {
  COUNT_CHARS;
}

[ \r\t\v\f] {
  COUNT_CHARS;
  continue;
}

. {
  COUNT_CHARS;
  std::cerr << "Invalid character : " << yytext << "\n";
}
%%

void count(yyscan_t scanner)
{
  int column = 0;
	int i;

	for (i = 0;  yyget_text(scanner)[i] != '\0'; i++)
  {
    char c = yyget_text(scanner)[i];
		if (c == '\n')
    {
			column = 0;
    }
		else if (c == '\t')
    {
			column += 8 - (column % 8);
    }
		else
    {
			column++;
    }
  }
}
