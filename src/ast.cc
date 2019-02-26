#include "ast.h"
#include "felparserstate.h"

#include "parser.hh"
#include "lexer.hh"

void StatementList::VisitAll(StatementVisitor* vis)
{
  for(auto s : statements)
  {
    s->Visit(vis);
  }
}

void FunctionCall::Visit(StatementVisitor* vis) const
{
  vis->OnFunctionCall(*this);
}

void FunctionCall::Visit(ValueVisitor* vis) const
{
  vis->OnFunctionCall(*this);
}


void StringValue::Visit(ValueVisitor* vis) const
{
  vis->OnString(*this);
}

void StringToAst(const std::string& str, const std::string& filename, fel::Log* log, StatementList* program)
{
  yyscan_t scanner;
  YY_BUFFER_STATE state;

  if (yylex_init(&scanner)) {
      /* could not initialize */
      return;
  }

  FelParserState fel;
  fel.file = filename;
  fel.log = log;
  yyset_extra(&fel, scanner);

  state = yy_scan_string(str.c_str(), scanner);
  yyset_lineno(1, scanner);

  if (yyparse(scanner, &fel)) {
      /* error parsing */
      return;
  }

  yy_delete_buffer(state, scanner);
  yylex_destroy(scanner);

  *program = fel.program;
}

