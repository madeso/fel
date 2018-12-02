#include "fel.h"
#include "stdio.h"
#include "stdarg.h"
#include "stdlib.h"

void fel_syntax_default(fel_syntax* s)
{
  s->arg_begin = "(";
  s->arg_end = ")";
  s->arg_sep = ",";
}

fel_context* fel_new()
{
  fel_context* ctx = (fel_context*)malloc(sizeof(fel_context));
  ctx->error_callback = 0;
  return ctx;
}

void fel_report_error(fel_context* ctx, char* format, ...)
{
  if(ctx && ctx->error_callback)
  {
    char buffer[1024] = {0,};
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    ctx->error_callback(0, buffer);
    va_end(args);
  }
}

void fel_delete(fel_context* ctx)
{
  if(ctx)
  {
    free(ctx);
  }
}

fel_error fel_load_and_run_file(fel_context* ctx, fel_syntax* s, char* file)
{
  FILE* f = fopen(file, "rb");
  if(f == 0)
  {
    fel_report_error(ctx, "Failed to open file: %s", file);
    return FEL_FAILED_TO_OPEN_FILE;
  }
  fclose(f);
  return FEL_NO_ERROR;
}

