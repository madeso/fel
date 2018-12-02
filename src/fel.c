#include "fel.h"
#include "stdlib.h"

void fel_syntax_default(fel_syntax* s)
{
}

fel_context* fel_new()
{
  return (fel_context*)malloc(sizeof(fel_context));
}

void fel_delete(fel_context* ctx)
{
  if(ctx)
  {
    free(ctx);
  }
}

void fel_load_and_run_file(fel_context* ctx, fel_syntax* s, char* file)
{
}

