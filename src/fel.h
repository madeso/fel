#ifndef FEL_H
#define FEL_H

#include "stdio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  FEL_NO_ERROR,
  FEL_FAILED_TO_OPEN_FILE
} fel_error;

typedef union {
  FILE* file;
  void* data;
} fel_file;

typedef struct {
  void (*error_callback)(void* data, char* error);
  void* error_callback_data;

  int (*file_open)(void* data, fel_file* file, char* path);
  void (*file_close)(void* data, fel_file* file);
  char (*file_read)(void* data, fel_file* file);
  void* file_data;
} fel_context;

typedef struct {
  char* arg_begin;
  char* arg_end;
  char* arg_sep;
} fel_syntax;

void fel_syntax_default(fel_syntax* s);

fel_context* fel_new();
void fel_delete(fel_context* ctx);

fel_error fel_load_and_run_file(fel_context* ctx, fel_syntax* s, char* file);

#ifdef __cplusplus
}
#endif

#endif // FEL_H
