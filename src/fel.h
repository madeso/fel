#ifndef FEL_H
#define FEL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  FEL_NO_ERROR,
  FEL_FAILED_TO_OPEN_FILE
} fel_error;

typedef void (*fel_error_callback)(void* data, char* error);

typedef struct {
  fel_error_callback error_callback;
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
