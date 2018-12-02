#ifndef FEL_H
#define FEL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
} fel_context;

typedef struct {
} fel_syntax;

void fel_syntax_default(fel_syntax* s);

fel_context* fel_new();
void fel_delete(fel_context* ctx);

void fel_load_and_run_file(fel_context* ctx, fel_syntax* s, char* file);

#ifdef __cplusplus
}
#endif

#endif // FEL_H
