#include "stdio.h"
#include "fel.h"

void error_callback(void* data, char* error)
{
  fprintf(stderr, "fel error: %s\n", error);
}

int main(int argc, char* argv[])
{
  if(argc != 2 ) {
    fprintf(stderr, "Missing file argument.\n");
    fprintf(stdout, "Usage: %s FILE\n", argv[0]);
    return -2;
  }
  fel_context* f = fel_new();
  if(f == 0) {
    fprintf(stderr, "Failed to start fel engine.\n");
    return -1;
  }
  f->error_callback = &error_callback;
  fel_syntax syntax;
  fel_syntax_default(&syntax);
  fel_error err = fel_load_and_run_file(f, &syntax, argv[1]);
  if(err != FEL_NO_ERROR)
  {
    fprintf(stderr, "Failed to open file and run file.\n");
  }
  fel_delete(f);
  return 0;
}

