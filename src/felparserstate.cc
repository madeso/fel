#include "felparserstate.h"

void FelParserState::AddLog(const std::string& file, int line, int col, const std::string& message)
{
  log->AddLog(file, line, col, message);
}

