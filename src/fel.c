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

typedef struct {
  FILE file;
} fel_default_file;

int fel_default_file_open(void* data, fel_file* file, char* path)
{
  file->file = fopen(path, "rb");
  return file->file != 0;
}

void fel_default_file_close(void* data, fel_file* file)
{
  fclose(file->file);
}

char fel_default_file_read(void* data, fel_file* file)
{
  char r = fgetc(file->file);
  if(r == EOF) return 0;
  else return r;
}

fel_context* fel_new()
{
  fel_context* ctx = (fel_context*)malloc(sizeof(fel_context));
  ctx->error_callback = 0;
  ctx->error_callback_data = 0;
  ctx->file_open = &fel_default_file_open;
  ctx->file_close = &fel_default_file_close;
  ctx->file_read = &fel_default_file_read;
  ctx->file_data = 0;
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

typedef struct {
  fel_file file;
  char peek;
} fel_parse_status;

char fel_parse_read(fel_context* ctx, fel_parse_status* f)
{
  if(f->peek != 0)
  {
    char r = f->peek;
    f->peek = 0;
    return r;
  }
  return ctx->file_read(ctx->file_data, &f->file);
}

char fel_parse_peek(fel_context* ctx, fel_parse_status* f)
{
  if(f->peek == 0)
  {
    f->peek = fel_parse_read(ctx, f);
  }
  return f->peek;
}

void fel_parse_skip_spaces(fel_context* ctx, fel_parse_status* f)
{
  while(1)
  {
    switch(fel_parse_peek(ctx, f))
    {
      case ' ':
      case '\t':
      case '\n':
      case '\r':
        // skip space...
        fel_parse_read(ctx, f);
        break;
      default:
        return;
    }
  }
}

fel_error fel_parse_and_run(fel_context* ctx, fel_parse_status* f)
{
  fel_parse_skip_spaces(ctx, f);
  if(fel_parse_peek(ctx, f) != 0)
  {
    fel_report_error(ctx, "File is not empty...");
  }
  return FEL_NO_ERROR;
}


fel_error fel_load_and_run_file(fel_context* ctx, fel_syntax* s, char* file)
{
  fel_parse_status f;
  f.peek = 0;
  if(!ctx->file_open(ctx->file_data, &f.file, file))
  {
    fel_report_error(ctx, "Failed to open file: %s", file);
    return FEL_FAILED_TO_OPEN_FILE;
  }
  fel_error err = fel_parse_and_run(ctx, &f);
  ctx->file_close(ctx->file_data, &f.file);
  return err;
}

